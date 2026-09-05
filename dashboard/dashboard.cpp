#include "dashboard.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QTableWidgetItem>
#include <QBrush>
#include <QColor>
#include <QVBoxLayout>
#include <QWidget>

VoltGuardDashboard::VoltGuardDashboard(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("VoltGuard OT Dashboard");
    resize(1000, 700);

    auto *centralWidget = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(centralWidget);

    // --------------------------------------------------
    // Dashboard title
    // --------------------------------------------------

    auto *title = new QLabel("VOLTGUARD OT DASHBOARD");

    title->setAlignment(Qt::AlignCenter);

    title->setStyleSheet(
        "font-size: 28px;"
        "font-weight: bold;"
        "padding: 15px;"
    );

    mainLayout->addWidget(title);

    // --------------------------------------------------
    // System status
    // --------------------------------------------------

    systemStatusLabel =
        new QLabel("SYSTEM STATUS: PROTECTED");

    systemStatusLabel->setAlignment(Qt::AlignCenter);

    systemStatusLabel->setStyleSheet(
        "font-size: 20px;"
        "font-weight: bold;"
        "padding: 10px;"
    );

    mainLayout->addWidget(systemStatusLabel);

    // --------------------------------------------------
    // Telemetry cards
    // --------------------------------------------------

    auto *telemetryLayout = new QHBoxLayout();

    pumpRpmLabel =
        new QLabel("Pump RPM\n--");

    pressureLabel =
        new QLabel("Pressure\n--");

    flowRateLabel =
        new QLabel("Flow Rate\n--");

    valveLabel =
        new QLabel("Valve Opening\n--");

    const QString telemetryStyle =
        "font-size: 18px;"
        "font-weight: bold;"
        "border: 1px solid gray;"
        "border-radius: 8px;"
        "padding: 20px;"
        "min-width: 180px;";

    pumpRpmLabel->setStyleSheet(telemetryStyle);
    pressureLabel->setStyleSheet(telemetryStyle);
    flowRateLabel->setStyleSheet(telemetryStyle);
    valveLabel->setStyleSheet(telemetryStyle);

    telemetryLayout->addWidget(pumpRpmLabel);
    telemetryLayout->addWidget(pressureLabel);
    telemetryLayout->addWidget(flowRateLabel);
    telemetryLayout->addWidget(valveLabel);

    mainLayout->addLayout(telemetryLayout);

    // --------------------------------------------------
    // Decision and action
    // --------------------------------------------------

    decisionLabel =
        new QLabel("Decision: --");

    actionLabel =
        new QLabel("Action: --");

    decisionLabel->setStyleSheet(
        "font-size: 20px;"
        "font-weight: bold;"
        "padding: 10px;"
    );

    actionLabel->setStyleSheet(
        "font-size: 18px;"
        "padding: 10px;"
    );

    mainLayout->addWidget(decisionLabel);
    mainLayout->addWidget(actionLabel);

    // --------------------------------------------------
    // Security events title
    // --------------------------------------------------

    auto *eventsTitle =
        new QLabel("SECURITY EVENTS");

    eventsTitle->setStyleSheet(
        "font-size: 20px;"
        "font-weight: bold;"
        "padding-top: 10px;"
    );

    mainLayout->addWidget(eventsTitle);

    // --------------------------------------------------
    // Security event table
    // --------------------------------------------------

    eventTable =
        new QTableWidget(this);

    eventTable->setColumnCount(4);

    eventTable->setHorizontalHeaderLabels({
        "Timestamp",
        "Pump RPM",
        "Decision",
        "Reason"
    });

    eventTable->horizontalHeader()
        ->setSectionResizeMode(
            0,
            QHeaderView::ResizeToContents
        );

    eventTable->horizontalHeader()
        ->setSectionResizeMode(
            1,
            QHeaderView::ResizeToContents
        );

    eventTable->horizontalHeader()
        ->setSectionResizeMode(
            2,
            QHeaderView::ResizeToContents
        );

    eventTable->horizontalHeader()
        ->setSectionResizeMode(
            3,
            QHeaderView::Stretch
        );

    eventTable->setEditTriggers(
        QAbstractItemView::NoEditTriggers
    );

    eventTable->setSelectionBehavior(
        QAbstractItemView::SelectRows
    );

    mainLayout->addWidget(eventTable);

    setCentralWidget(centralWidget);

    // --------------------------------------------------
    // Initial data load
    // --------------------------------------------------

    loadSecurityEvents();

    // --------------------------------------------------
    // Automatic refresh
    // --------------------------------------------------

    refreshTimer =
        new QTimer(this);

    refreshTimer->setInterval(1000);

    connect(
        refreshTimer,
        &QTimer::timeout,
        this,
        &VoltGuardDashboard::loadSecurityEvents
    );

    refreshTimer->start();
}

// ------------------------------------------------------
// Load security events from JSONL log
// ------------------------------------------------------

void VoltGuardDashboard::loadSecurityEvents()
{
    /*
     * Dashboard executable:
     *
     * VoltGuard/
     *   dashboard/
     *     build/
     *       VoltGuardDashboard.exe
     *
     * Security log:
     *
     * VoltGuard/
     *   logs/
     *     security_events.jsonl
     *
     * Resolve the log relative to the executable.
     */

    const QString logPath =
        QDir(
            QCoreApplication::applicationDirPath()
        ).absoluteFilePath(
            "../../logs/security_events.jsonl"
        );

    QFile file(logPath);

    if (!file.open(
            QIODevice::ReadOnly |
            QIODevice::Text))
    {
        systemStatusLabel->setText(
            "SYSTEM STATUS: NO SECURITY LOG"
        );

        decisionLabel->setText(
            "Decision: NO DATA"
        );

        actionLabel->setText(
            "Action: Waiting for VoltGuard events"
        );

        eventTable->setRowCount(0);

        return;
    }

    eventTable->setRowCount(0);

    QString latestDecision;
    QString latestReason;

    double latestPumpRpm = 0.0;
    double latestPressure = 0.0;
    double latestFlowRate = 0.0;
    double latestValveOpening = 0.0;

    bool foundEvent = false;

    // --------------------------------------------------
    // Read every JSONL event
    // --------------------------------------------------

    while (!file.atEnd())
    {
        const QByteArray line =
            file.readLine().trimmed();

        if (line.isEmpty())
        {
            continue;
        }

        QJsonParseError parseError;

        const QJsonDocument document =
            QJsonDocument::fromJson(
                line,
                &parseError
            );

        if (
            parseError.error !=
            QJsonParseError::NoError
        )
        {
            continue;
        }

        if (!document.isObject())
        {
            continue;
        }

        const QJsonObject event =
            document.object();

        const QJsonObject state =
            event.value(
                "predicted_state"
            ).toObject();

        const QString timestamp =
            event.value(
                "timestamp"
            ).toString();

        const double pumpRpm =
            event.value(
                "pump_rpm"
            ).toDouble();

        /*
         * Security logger stores these fields
         * directly as strings:
         *
         * "decision": "BLOCK"
         * "reason": "Pressure exceeds..."
         */

        const QString decision =
            event.value(
                "decision"
            ).toString();

        const QString reason =
            event.value(
                "reason"
            ).toString();

        // --------------------------------------------------
        // Add event to table
        // --------------------------------------------------

        const int row =
            eventTable->rowCount();

        eventTable->insertRow(row);

        auto *timestampItem =
            new QTableWidgetItem(timestamp);

        auto *rpmItem =
            new QTableWidgetItem(
                QString::number(
                    pumpRpm,
                    'f',
                    0
                )
            );

        auto *decisionItem =
            new QTableWidgetItem(decision);

        auto *reasonItem =
            new QTableWidgetItem(reason);

        eventTable->setItem(
            row,
            0,
            timestampItem
        );

        eventTable->setItem(
            row,
            1,
            rpmItem
        );

        eventTable->setItem(
            row,
            2,
            decisionItem
        );

        eventTable->setItem(
            row,
            3,
            reasonItem
        );

        // --------------------------------------------------
        // Visual security highlighting
        //
        // QTableWidgetItem is not a QWidget, so it cannot
        // use setStyleSheet(). Use background/foreground
        // brushes for individual table cells instead.
        // --------------------------------------------------

        if (decision == "BLOCK")
        {
            const QBrush blockBackground(
                QColor("#5c1f1f")
            );

            const QBrush blockForeground(
                QColor("#ffffff")
            );

            timestampItem->setBackground(
                blockBackground
            );

            rpmItem->setBackground(
                blockBackground
            );

            decisionItem->setBackground(
                blockBackground
            );

            reasonItem->setBackground(
                blockBackground
            );

            timestampItem->setForeground(
                blockForeground
            );

            rpmItem->setForeground(
                blockForeground
            );

            decisionItem->setForeground(
                blockForeground
            );

            reasonItem->setForeground(
                blockForeground
            );

            decisionItem->setFont(
                QFont(
                    "Arial",
                    10,
                    QFont::Bold
                )
            );
        }
        else if (decision == "ALLOW")
        {
            const QBrush allowBackground(
                QColor("#1f4d2b")
            );

            const QBrush allowForeground(
                QColor("#ffffff")
            );

            timestampItem->setBackground(
                allowBackground
            );

            rpmItem->setBackground(
                allowBackground
            );

            decisionItem->setBackground(
                allowBackground
            );

            reasonItem->setBackground(
                allowBackground
            );

            timestampItem->setForeground(
                allowForeground
            );

            rpmItem->setForeground(
                allowForeground
            );

            decisionItem->setForeground(
                allowForeground
            );

            reasonItem->setForeground(
                allowForeground
            );

            decisionItem->setFont(
                QFont(
                    "Arial",
                    10,
                    QFont::Bold
                )
            );
        }

        // --------------------------------------------------
        // Store latest event
        // --------------------------------------------------

        latestDecision = decision;
        latestReason = reason;

        latestPumpRpm =
            state.value(
                "pump_rpm"
            ).toDouble();

        latestPressure =
            state.value(
                "pressure"
            ).toDouble();

        latestFlowRate =
            state.value(
                "flow_rate"
            ).toDouble();

        latestValveOpening =
            state.value(
                "valve_opening"
            ).toDouble();

        foundEvent = true;
    }

    file.close();

    // --------------------------------------------------
    // No valid events
    // --------------------------------------------------

    if (!foundEvent)
    {
        systemStatusLabel->setText(
            "SYSTEM STATUS: NO VALID EVENTS"
        );

        decisionLabel->setText(
            "Decision: NO DATA"
        );

        actionLabel->setText(
            "Action: Waiting for VoltGuard events"
        );

        return;
    }

    // --------------------------------------------------
    // Update latest telemetry
    // --------------------------------------------------

    pumpRpmLabel->setText(
        QString("Pump RPM\n%1")
            .arg(
                latestPumpRpm,
                0,
                'f',
                0
            )
    );

    pressureLabel->setText(
        QString("Pressure\n%1 bar")
            .arg(
                latestPressure,
                0,
                'f',
                2
            )
    );

    flowRateLabel->setText(
        QString("Flow Rate\n%1")
            .arg(
                latestFlowRate,
                0,
                'f',
                2
            )
    );

    valveLabel->setText(
        QString("Valve Opening\n%1%")
            .arg(
                latestValveOpening,
                0,
                'f',
                0
            )
    );

    // --------------------------------------------------
    // Update security decision
    // --------------------------------------------------

    decisionLabel->setText(
        QString("Decision: %1")
            .arg(latestDecision)
    );

    if (latestDecision == "BLOCK")
    {
        systemStatusLabel->setText(
            "SYSTEM STATUS: ALERT"
        );

        actionLabel->setText(
            QString(
                "Action: DROP  |  %1"
            ).arg(latestReason)
        );

        systemStatusLabel->setStyleSheet(
            "font-size: 20px;"
            "font-weight: bold;"
            "padding: 10px;"
            "background-color: #5c1f1f;"
            "color: white;"
            "border-radius: 8px;"
        );

        decisionLabel->setStyleSheet(
            "font-size: 20px;"
            "font-weight: bold;"
            "padding: 10px;"
            "color: #ff6b6b;"
        );
    }
    else if (latestDecision == "ALLOW")
    {
        systemStatusLabel->setText(
            "SYSTEM STATUS: PROTECTED"
        );

        actionLabel->setText(
            "Action: FORWARD"
        );

        systemStatusLabel->setStyleSheet(
            "font-size: 20px;"
            "font-weight: bold;"
            "padding: 10px;"
            "background-color: #1f4d2b;"
            "color: white;"
            "border-radius: 8px;"
        );

        decisionLabel->setStyleSheet(
            "font-size: 20px;"
            "font-weight: bold;"
            "padding: 10px;"
            "color: #6ee7a0;"
        );
    }
    else
    {
        systemStatusLabel->setText(
            "SYSTEM STATUS: UNKNOWN"
        );

        actionLabel->setText(
            "Action: REVIEW REQUIRED"
        );
    }

    // --------------------------------------------------
    // Keep newest event visible
    // --------------------------------------------------

    if (eventTable->rowCount() > 0)
    {
        eventTable->scrollToBottom();
    }
}