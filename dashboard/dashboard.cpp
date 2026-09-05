#include "dashboard.h"
#include "trend_widget.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFont>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QBrush>
#include <QColor>
#include <QTableWidgetItem>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

VoltGuardDashboard::VoltGuardDashboard(
    QWidget *parent
)
    : QMainWindow(parent)
{
    setWindowTitle(
        "VoltGuard OT Dashboard"
    );

    resize(1150, 850);

    // ==================================================
    // Main container
    // ==================================================

    auto *centralWidget =
        new QWidget(this);

    auto *mainLayout =
        new QVBoxLayout(
            centralWidget
        );

    mainLayout->setContentsMargins(
        15,
        15,
        15,
        15
    );

    mainLayout->setSpacing(10);

    // ==================================================
    // Dashboard title
    // ==================================================

    auto *title =
        new QLabel(
            "VOLTGUARD OT SECURITY DASHBOARD"
        );

    title->setAlignment(
        Qt::AlignCenter
    );

    title->setStyleSheet(
        "font-size: 28px;"
        "font-weight: bold;"
        "padding: 8px;"
    );

    mainLayout->addWidget(title);

    // ==================================================
    // Tabs
    // ==================================================

    auto *tabs =
        new QTabWidget(this);

    tabs->setDocumentMode(false);

    tabs->setStyleSheet(
        "QTabBar::tab {"
        "    padding: 10px 25px;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "}"
        "QTabBar::tab:selected {"
        "    font-weight: bold;"
        "}"
    );

    // ==================================================
    // PAGE 1 - SYSTEM MONITOR
    // ==================================================

    auto *systemPage =
        new QWidget();

    auto *systemLayout =
        new QVBoxLayout(
            systemPage
        );

    systemLayout->setContentsMargins(
        10,
        10,
        10,
        10
    );

    systemLayout->setSpacing(10);

    // --------------------------------------------------
    // System status
    // --------------------------------------------------

    systemStatusLabel =
        new QLabel(
            "SYSTEM STATUS: PROTECTED"
        );

    systemStatusLabel->setAlignment(
        Qt::AlignCenter
    );

    systemStatusLabel->setStyleSheet(
        "font-size: 20px;"
        "font-weight: bold;"
        "padding: 12px;"
        "background-color: #1f4d2b;"
        "color: white;"
        "border-radius: 8px;"
    );

    systemLayout->addWidget(
        systemStatusLabel
    );

    // --------------------------------------------------
    // Telemetry
    // --------------------------------------------------

    auto *telemetryTitle =
        new QLabel(
            "CURRENT PHYSICAL STATE"
        );

    telemetryTitle->setStyleSheet(
        "font-size: 18px;"
        "font-weight: bold;"
        "padding-top: 5px;"
    );

    systemLayout->addWidget(
        telemetryTitle
    );

    auto *telemetryLayout =
        new QHBoxLayout();

    telemetryLayout->setSpacing(10);

    pumpRpmLabel =
        new QLabel(
            "Pump RPM\n--"
        );

    pressureLabel =
        new QLabel(
            "Predicted Pressure\n--"
        );

    flowRateLabel =
        new QLabel(
            "Flow Rate\n--"
        );

    valveLabel =
        new QLabel(
            "Valve Opening\n--"
        );

    const QString telemetryStyle =
        "font-size: 17px;"
        "font-weight: bold;"
        "border: 1px solid gray;"
        "border-radius: 8px;"
        "padding: 18px;"
        "min-width: 170px;";

    pumpRpmLabel->setStyleSheet(
        telemetryStyle
    );

    pressureLabel->setStyleSheet(
        telemetryStyle
    );

    flowRateLabel->setStyleSheet(
        telemetryStyle
    );

    valveLabel->setStyleSheet(
        telemetryStyle
    );

    telemetryLayout->addWidget(
        pumpRpmLabel
    );

    telemetryLayout->addWidget(
        pressureLabel
    );

    telemetryLayout->addWidget(
        flowRateLabel
    );

    telemetryLayout->addWidget(
        valveLabel
    );

    systemLayout->addLayout(
        telemetryLayout
    );

    // --------------------------------------------------
    // Physics graph
    // --------------------------------------------------

    auto *trendTitle =
        new QLabel(
            "PHYSICS MONITORING"
        );

    trendTitle->setStyleSheet(
        "font-size: 19px;"
        "font-weight: bold;"
        "padding-top: 5px;"
    );

    systemLayout->addWidget(
        trendTitle
    );

    pressureTrendWidget =
        new PressureTrendWidget();

    systemLayout->addWidget(
        pressureTrendWidget,
        1
    );

    tabs->addTab(
        systemPage,
        "SYSTEM MONITOR"
    );

    // ==================================================
    // PAGE 2 - SECURITY CENTER
    // ==================================================

    auto *securityPage =
        new QWidget();

    auto *securityLayout =
        new QVBoxLayout(
            securityPage
        );

    securityLayout->setContentsMargins(
        10,
        10,
        10,
        10
    );

    securityLayout->setSpacing(10);

    auto *securityTitle =
        new QLabel(
            "SECURITY CENTER"
        );

    securityTitle->setStyleSheet(
        "font-size: 20px;"
        "font-weight: bold;"
    );

    securityLayout->addWidget(
        securityTitle
    );

    // --------------------------------------------------
    // Decision and action row
    // --------------------------------------------------

    auto *securitySummary =
        new QHBoxLayout();

    decisionLabel =
        new QLabel(
            "Decision: --"
        );

    actionLabel =
        new QLabel(
            "Action: --"
        );

    decisionLabel->setAlignment(
        Qt::AlignCenter
    );

    actionLabel->setAlignment(
        Qt::AlignCenter
    );

    decisionLabel->setStyleSheet(
        "font-size: 20px;"
        "font-weight: bold;"
        "padding: 15px;"
        "border: 1px solid gray;"
        "border-radius: 8px;"
    );

    actionLabel->setStyleSheet(
        "font-size: 18px;"
        "font-weight: bold;"
        "padding: 15px;"
        "border: 1px solid gray;"
        "border-radius: 8px;"
    );

    securitySummary->addWidget(
        decisionLabel
    );

    securitySummary->addWidget(
        actionLabel
    );

    securityLayout->addLayout(
        securitySummary
    );

    // --------------------------------------------------
    // Reason
    // --------------------------------------------------

    reasonLabel =
        new QLabel(
            "Reason: Waiting for VoltGuard events"
        );

    reasonLabel->setWordWrap(
        true
    );

    reasonLabel->setStyleSheet(
        "font-size: 15px;"
        "padding: 12px;"
        "border: 1px solid gray;"
        "border-radius: 8px;"
    );

    securityLayout->addWidget(
        reasonLabel
    );

    // --------------------------------------------------
    // Events title
    // --------------------------------------------------

    auto *eventsTitle =
        new QLabel(
            "SECURITY EVENT HISTORY"
        );

    eventsTitle->setStyleSheet(
        "font-size: 18px;"
        "font-weight: bold;"
        "padding-top: 5px;"
    );

    securityLayout->addWidget(
        eventsTitle
    );

    // --------------------------------------------------
    // Event table
    // --------------------------------------------------

    eventTable =
        new QTableWidget();

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

    eventTable->setAlternatingRowColors(
        true
    );

    securityLayout->addWidget(
        eventTable,
        1
    );

    tabs->addTab(
        securityPage,
        "SECURITY CENTER"
    );

    mainLayout->addWidget(
        tabs,
        1
    );

    setCentralWidget(
        centralWidget
    );

    // ==================================================
    // Initial data load
    // ==================================================

    loadSecurityEvents();

    // ==================================================
    // Automatic refresh
    // ==================================================

    refreshTimer =
        new QTimer(this);

    refreshTimer->setInterval(
        1000
    );

    connect(
        refreshTimer,
        &QTimer::timeout,
        this,
        &VoltGuardDashboard::loadSecurityEvents
    );

    refreshTimer->start();
}


// ======================================================
// Load security events
// ======================================================

void VoltGuardDashboard::loadSecurityEvents()
{
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

        systemStatusLabel->setStyleSheet(
            "font-size: 20px;"
            "font-weight: bold;"
            "padding: 12px;"
            "background-color: #5c1f1f;"
            "color: white;"
            "border-radius: 8px;"
        );

        decisionLabel->setText(
            "Decision: NO DATA"
        );

        actionLabel->setText(
            "Action: WAITING"
        );

        reasonLabel->setText(
            "Reason: Waiting for VoltGuard events"
        );

        eventTable->setRowCount(0);

        predictedPressureHistory.clear();
        actualPressureHistory.clear();
        safeHistory.clear();

        pressureTrendWidget->setData(
            predictedPressureHistory,
            actualPressureHistory,
            safeHistory
        );

        return;
    }

    eventTable->setRowCount(0);

    predictedPressureHistory.clear();
    actualPressureHistory.clear();
    safeHistory.clear();

    QString latestDecision;
    QString latestReason;

    double latestPumpRpm = 0.0;
    double latestPressure = 0.0;
    double latestFlowRate = 0.0;
    double latestValveOpening = 0.0;

    bool foundEvent = false;

    // ==================================================
    // Read every JSONL event
    // ==================================================

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

        // --------------------------------------------------
        // Predicted state
        // --------------------------------------------------

        const QJsonObject predictedState =
            event.value(
                "predicted_state"
            ).toObject();

        // --------------------------------------------------
        // Actual state
        // --------------------------------------------------

        const QJsonObject actualState =
            event.value(
                "actual_state"
            ).toObject();

        const QString timestamp =
            event.value(
                "timestamp"
            ).toString();

        const double pumpRpm =
            event.value(
                "pump_rpm"
            ).toDouble();

        const QString decision =
            event.value(
                "decision"
            ).toString();

        const QString reason =
            event.value(
                "reason"
            ).toString();

        const double predictedPressure =
            predictedState.value(
                "pressure"
            ).toDouble();

        const double actualPressure =
            actualState.value(
                "pressure"
            ).toDouble(
                    predictedPressure
                );

        const bool safe =
            predictedState.value(
                "safe"
            ).toBool();

        // --------------------------------------------------
        // Store trend data
        // --------------------------------------------------

        predictedPressureHistory.append(
            predictedPressure
        );

        actualPressureHistory.append(
            actualPressure
        );

        safeHistory.append(
            safe
        );

        // --------------------------------------------------
        // Add event to table
        // --------------------------------------------------

        const int row =
            eventTable->rowCount();

        eventTable->insertRow(
            row
        );

        auto *timestampItem =
            new QTableWidgetItem(
                timestamp
            );

        auto *rpmItem =
            new QTableWidgetItem(
                QString::number(
                    pumpRpm,
                    'f',
                    0
                )
            );

        auto *decisionItem =
            new QTableWidgetItem(
                decision
            );

        auto *reasonItem =
            new QTableWidgetItem(
                reason
            );

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

        // ==================================================
        // Visual security highlighting
        // ==================================================

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

        // ==================================================
        // Store latest event
        // ==================================================

        latestDecision =
            decision;

        latestReason =
            reason;

        latestPumpRpm =
            predictedState.value(
                "pump_rpm"
            ).toDouble();

        latestPressure =
            predictedPressure;

        latestFlowRate =
            predictedState.value(
                "flow_rate"
            ).toDouble();

        latestValveOpening =
            predictedState.value(
                "valve_opening"
            ).toDouble();

        foundEvent = true;
    }

    file.close();

    // ==================================================
    // Update physics graph
    // ==================================================

    pressureTrendWidget->setData(
        predictedPressureHistory,
        actualPressureHistory,
        safeHistory
    );

    // ==================================================
    // No valid events
    // ==================================================

    if (!foundEvent)
    {
        systemStatusLabel->setText(
            "SYSTEM STATUS: NO VALID EVENTS"
        );

        systemStatusLabel->setStyleSheet(
            "font-size: 20px;"
            "font-weight: bold;"
            "padding: 12px;"
            "background-color: #444444;"
            "color: white;"
            "border-radius: 8px;"
        );

        decisionLabel->setText(
            "Decision: NO DATA"
        );

        actionLabel->setText(
            "Action: WAITING"
        );

        reasonLabel->setText(
            "Reason: No valid security events found"
        );

        return;
    }

    // ==================================================
    // Update telemetry
    // ==================================================

    pumpRpmLabel->setText(
        QString(
            "Pump RPM\n%1"
        ).arg(
            latestPumpRpm,
            0,
            'f',
            0
        )
    );

    pressureLabel->setText(
        QString(
            "Predicted Pressure\n%1 bar"
        ).arg(
            latestPressure,
            0,
            'f',
            2
        )
    );

    flowRateLabel->setText(
        QString(
            "Flow Rate\n%1"
        ).arg(
            latestFlowRate,
            0,
            'f',
            2
        )
    );

    valveLabel->setText(
        QString(
            "Valve Opening\n%1%"
        ).arg(
            latestValveOpening,
            0,
            'f',
            0
        )
    );

    // ==================================================
    // Update security decision
    // ==================================================

    decisionLabel->setText(
        QString(
            "Decision: %1"
        ).arg(
            latestDecision
        )
    );

    reasonLabel->setText(
        QString(
            "Reason: %1"
        ).arg(
            latestReason
        )
    );

    if (latestDecision == "BLOCK")
    {
        systemStatusLabel->setText(
            "SYSTEM STATUS: ALERT"
        );

        systemStatusLabel->setStyleSheet(
            "font-size: 20px;"
            "font-weight: bold;"
            "padding: 12px;"
            "background-color: #5c1f1f;"
            "color: white;"
            "border-radius: 8px;"
        );

        decisionLabel->setStyleSheet(
            "font-size: 20px;"
            "font-weight: bold;"
            "padding: 15px;"
            "border: 1px solid #ff5c5c;"
            "border-radius: 8px;"
            "color: #ff6b6b;"
        );

        actionLabel->setText(
            "Action: DROP"
        );

        actionLabel->setStyleSheet(
            "font-size: 18px;"
            "font-weight: bold;"
            "padding: 15px;"
            "border: 1px solid #ff5c5c;"
            "border-radius: 8px;"
            "color: #ff6b6b;"
        );

        reasonLabel->setStyleSheet(
            "font-size: 15px;"
            "padding: 12px;"
            "border: 1px solid #ff5c5c;"
            "border-radius: 8px;"
            "color: #ffb3b3;"
        );
    }
    else if (latestDecision == "ALLOW")
    {
        systemStatusLabel->setText(
            "SYSTEM STATUS: PROTECTED"
        );

        systemStatusLabel->setStyleSheet(
            "font-size: 20px;"
            "font-weight: bold;"
            "padding: 12px;"
            "background-color: #1f4d2b;"
            "color: white;"
            "border-radius: 8px;"
        );

        decisionLabel->setStyleSheet(
            "font-size: 20px;"
            "font-weight: bold;"
            "padding: 15px;"
            "border: 1px solid #6ee7a0;"
            "border-radius: 8px;"
            "color: #6ee7a0;"
        );

        actionLabel->setText(
            "Action: FORWARD"
        );

        actionLabel->setStyleSheet(
            "font-size: 18px;"
            "font-weight: bold;"
            "padding: 15px;"
            "border: 1px solid #6ee7a0;"
            "border-radius: 8px;"
            "color: #6ee7a0;"
        );

        reasonLabel->setStyleSheet(
            "font-size: 15px;"
            "padding: 12px;"
            "border: 1px solid #6ee7a0;"
            "border-radius: 8px;"
            "color: #b9ffd0;"
        );
    }
    else
    {
        systemStatusLabel->setText(
            "SYSTEM STATUS: UNKNOWN"
        );

        systemStatusLabel->setStyleSheet(
            "font-size: 20px;"
            "font-weight: bold;"
            "padding: 12px;"
            "background-color: #444444;"
            "color: white;"
            "border-radius: 8px;"
        );

        decisionLabel->setStyleSheet(
            "font-size: 20px;"
            "font-weight: bold;"
            "padding: 15px;"
            "border: 1px solid gray;"
            "border-radius: 8px;"
        );

        actionLabel->setText(
            "Action: REVIEW REQUIRED"
        );

        actionLabel->setStyleSheet(
            "font-size: 18px;"
            "font-weight: bold;"
            "padding: 15px;"
            "border: 1px solid gray;"
            "border-radius: 8px;"
        );

        reasonLabel->setStyleSheet(
            "font-size: 15px;"
            "padding: 12px;"
            "border: 1px solid gray;"
            "border-radius: 8px;"
        );
    }

    // ==================================================
    // Keep newest event visible
    // ==================================================

    if (eventTable->rowCount() > 0)
    {
        eventTable->scrollToBottom();
    }
}