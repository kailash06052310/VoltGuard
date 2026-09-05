#include "trend_widget.h"

#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QBrush>
#include <QtMath>

PressureTrendWidget::PressureTrendWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumHeight(240);
    setMinimumWidth(600);

    setStyleSheet(
        "background-color: #1e1e1e;"
        "border: 1px solid #666666;"
        "border-radius: 8px;"
    );
}

void PressureTrendWidget::setData(
    const QVector<double> &pressures,
    const QVector<bool> &safeStates
)
{
    pressureHistory = pressures;
    safeHistory = safeStates;

    if (pressureHistory.size() > MAX_POINTS)
    {
        pressureHistory =
            pressureHistory.mid(
                pressureHistory.size() - MAX_POINTS
            );
    }

    if (safeHistory.size() > MAX_POINTS)
    {
        safeHistory =
            safeHistory.mid(
                safeHistory.size() - MAX_POINTS
            );
    }

    update();
}

void PressureTrendWidget::paintEvent(
    QPaintEvent *event
)
{
    Q_UNUSED(event);

    QPainter painter(this);

    painter.setRenderHint(
        QPainter::Antialiasing
    );

    const int leftMargin = 55;
    const int rightMargin = 20;
    const int topMargin = 35;
    const int bottomMargin = 35;

    const QRect plotArea(
        leftMargin,
        topMargin,
        width() - leftMargin - rightMargin,
        height() - topMargin - bottomMargin
    );

    // --------------------------------------------------
    // Background
    // --------------------------------------------------

    painter.fillRect(
        rect(),
        QColor("#1e1e1e")
    );

    // --------------------------------------------------
    // Title
    // --------------------------------------------------

    painter.setPen(
        QColor("#ffffff")
    );

    painter.setFont(
        QFont(
            "Arial",
            12,
            QFont::Bold
        )
    );

    painter.drawText(
        15,
        22,
        "PHYSICS PRESSURE TREND"
    );

    // --------------------------------------------------
    // Empty state
    // --------------------------------------------------

    if (pressureHistory.isEmpty())
    {
        painter.setFont(
            QFont(
                "Arial",
                10
            )
        );

        painter.drawText(
            plotArea,
            Qt::AlignCenter,
            "Waiting for physics data..."
        );

        return;
    }

    // --------------------------------------------------
    // Determine graph scale
    // --------------------------------------------------

    double maximumPressure =
        SAFE_PRESSURE_LIMIT;

    for (double pressure : pressureHistory)
    {
        if (pressure > maximumPressure)
        {
            maximumPressure = pressure;
        }
    }

    // Add headroom above the maximum value.
    maximumPressure *= 1.15;

    if (maximumPressure < 110.0)
    {
        maximumPressure = 110.0;
    }

    // --------------------------------------------------
    // Grid
    // --------------------------------------------------

    painter.setPen(
        QPen(
            QColor("#3f3f3f"),
            1
        )
    );

    const int horizontalLines = 5;

    for (int i = 0; i <= horizontalLines; ++i)
    {
        const double ratio =
            static_cast<double>(i)
            / horizontalLines;

        const int y =
            plotArea.bottom()
            - static_cast<int>(
                ratio * plotArea.height()
            );

        painter.drawLine(
            plotArea.left(),
            y,
            plotArea.right(),
            y
        );

        const double value =
            ratio * maximumPressure;

        painter.setPen(
            QColor("#aaaaaa")
        );

        painter.setFont(
            QFont(
                "Arial",
                8
            )
        );

        painter.drawText(
            5,
            y + 4,
            QString::number(
                value,
                'f',
                0
            )
        );

        painter.setPen(
            QPen(
                QColor("#3f3f3f"),
                1
            )
        );
    }

    // --------------------------------------------------
    // Safe pressure limit line
    // --------------------------------------------------

    const double safeRatio =
        SAFE_PRESSURE_LIMIT
        / maximumPressure;

    const int safeY =
        plotArea.bottom()
        - static_cast<int>(
            safeRatio * plotArea.height()
        );

    QPen safeLimitPen(
        QColor("#f0c040"),
        2,
        Qt::DashLine
    );

    painter.setPen(
        safeLimitPen
    );

    painter.drawLine(
        plotArea.left(),
        safeY,
        plotArea.right(),
        safeY
    );

    painter.setPen(
        QColor("#f0c040")
    );

    painter.setFont(
        QFont(
            "Arial",
            8,
            QFont::Bold
        )
    );

    painter.drawText(
        plotArea.right() - 115,
        safeY - 6,
        "SAFE LIMIT: 100 bar"
    );

    // --------------------------------------------------
    // Pressure graph
    // --------------------------------------------------

    if (pressureHistory.size() == 1)
    {
        const double pressure =
            pressureHistory.first();

        const double ratio =
            pressure / maximumPressure;

        const int x =
            plotArea.center().x();

        const int y =
            plotArea.bottom()
            - static_cast<int>(
                ratio * plotArea.height()
            );

        painter.setBrush(
            safeHistory.first()
                ? QColor("#6ee7a0")
                : QColor("#ff5c5c")
        );

        painter.setPen(
            Qt::NoPen
        );

        painter.drawEllipse(
            QPoint(x, y),
            5,
            5
        );

        return;
    }

    const int count =
        pressureHistory.size();

    // Draw connecting line.
    for (int i = 1; i < count; ++i)
    {
        const double previousPressure =
            pressureHistory[i - 1];

        const double currentPressure =
            pressureHistory[i];

        const double previousRatio =
            previousPressure / maximumPressure;

        const double currentRatio =
            currentPressure / maximumPressure;

        const int previousX =
            plotArea.left()
            + static_cast<int>(
                static_cast<double>(i - 1)
                / (count - 1)
                * plotArea.width()
            );

        const int currentX =
            plotArea.left()
            + static_cast<int>(
                static_cast<double>(i)
                / (count - 1)
                * plotArea.width()
            );

        const int previousY =
            plotArea.bottom()
            - static_cast<int>(
                previousRatio
                * plotArea.height()
            );

        const int currentY =
            plotArea.bottom()
            - static_cast<int>(
                currentRatio
                * plotArea.height()
            );

        QPen linePen;

        if (
            i < safeHistory.size()
            && !safeHistory[i]
        )
        {
            linePen = QPen(
                QColor("#ff5c5c"),
                3
            );
        }
        else
        {
            linePen = QPen(
                QColor("#6ee7a0"),
                3
            );
        }

        painter.setPen(
            linePen
        );

        painter.drawLine(
            previousX,
            previousY,
            currentX,
            currentY
        );
    }

    // --------------------------------------------------
    // Data points
    // --------------------------------------------------

    painter.setPen(
        Qt::NoPen
    );

    for (int i = 0; i < count; ++i)
    {
        const double pressure =
            pressureHistory[i];

        const double ratio =
            pressure / maximumPressure;

        const int x =
            plotArea.left()
            + static_cast<int>(
                static_cast<double>(i)
                / (count - 1)
                * plotArea.width()
            );

        const int y =
            plotArea.bottom()
            - static_cast<int>(
                ratio * plotArea.height()
            );

        const bool safe =
            i < safeHistory.size()
            ? safeHistory[i]
            : true;

        painter.setBrush(
            safe
                ? QColor("#6ee7a0")
                : QColor("#ff5c5c")
        );

        painter.drawEllipse(
            QPoint(x, y),
            4,
            4
        );
    }

    // --------------------------------------------------
    // Latest value
    // --------------------------------------------------

    const double latestPressure =
        pressureHistory.last();

    painter.setPen(
        latestPressure > SAFE_PRESSURE_LIMIT
            ? QColor("#ff5c5c")
            : QColor("#6ee7a0")
    );

    painter.setFont(
        QFont(
            "Arial",
            10,
            QFont::Bold
        )
    );

    painter.drawText(
        plotArea.left(),
        height() - 10,
        QString(
            "Latest pressure: %1 bar"
        ).arg(
            latestPressure,
            0,
            'f',
            2
        )
    );
}