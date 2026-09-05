#include "trend_widget.h"

#include <QBrush>
#include <QColor>
#include <QFont>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>

PressureTrendWidget::PressureTrendWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumHeight(300);
    setMinimumWidth(600);

    setStyleSheet(
        "background-color: #1e1e1e;"
        "border: 1px solid #666666;"
        "border-radius: 8px;"
    );
}

void PressureTrendWidget::setData(
    const QVector<double> &predictedPressures,
    const QVector<double> &actualPressures,
    const QVector<bool> &safeStates
)
{
    predictedPressureHistory = predictedPressures;
    actualPressureHistory = actualPressures;
    safeHistory = safeStates;

    if (predictedPressureHistory.size() > MAX_POINTS)
    {
        predictedPressureHistory =
            predictedPressureHistory.mid(
                predictedPressureHistory.size() - MAX_POINTS
            );
    }

    if (actualPressureHistory.size() > MAX_POINTS)
    {
        actualPressureHistory =
            actualPressureHistory.mid(
                actualPressureHistory.size() - MAX_POINTS
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
    const int topMargin = 55;
    const int bottomMargin = 45;

    const QRect plotArea(
        leftMargin,
        topMargin,
        width() - leftMargin - rightMargin,
        height() - topMargin - bottomMargin
    );

    painter.fillRect(
        rect(),
        QColor("#1e1e1e")
    );

    // ==================================================
    // Title
    // ==================================================

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
        "PHYSICS PRESSURE: PREDICTED vs ACTUAL"
    );

    // ==================================================
    // Legend
    // ==================================================

    painter.setFont(
        QFont(
            "Arial",
            9,
            QFont::Bold
        )
    );

    painter.setPen(
        QColor("#5dade2")
    );

    painter.drawText(
        20,
        43,
        "PREDICTED"
    );

    painter.setPen(
        QColor("#ffffff")
    );

    painter.drawText(
        105,
        43,
        "ACTUAL"
    );

    painter.setPen(
        QColor("#f0c040")
    );

    painter.drawText(
        175,
        43,
        "SAFE LIMIT"
    );

    // ==================================================
    // Empty state
    // ==================================================

    if (
        predictedPressureHistory.isEmpty()
        &&
        actualPressureHistory.isEmpty()
    )
    {
        painter.setPen(
            QColor("#aaaaaa")
        );

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

    // ==================================================
    // Determine graph count
    // ==================================================

    int count =
        predictedPressureHistory.size();

    if (
        actualPressureHistory.size()
        > count
    )
    {
        count =
            actualPressureHistory.size();
    }

    if (count <= 0)
    {
        return;
    }

    // ==================================================
    // Determine graph scale
    // ==================================================

    double maximumPressure =
        SAFE_PRESSURE_LIMIT;

    for (
        double pressure :
        predictedPressureHistory
    )
    {
        if (pressure > maximumPressure)
        {
            maximumPressure = pressure;
        }
    }

    for (
        double pressure :
        actualPressureHistory
    )
    {
        if (pressure > maximumPressure)
        {
            maximumPressure = pressure;
        }
    }

    maximumPressure *= 1.15;

    if (maximumPressure < 110.0)
    {
        maximumPressure = 110.0;
    }

    // ==================================================
    // Grid
    // ==================================================

    const int horizontalLines = 5;

    painter.setFont(
        QFont(
            "Arial",
            8
        )
    );

    for (
        int i = 0;
        i <= horizontalLines;
        ++i
    )
    {
        const double ratio =
            static_cast<double>(i)
            / horizontalLines;

        const int y =
            plotArea.bottom()
            -
            static_cast<int>(
                ratio * plotArea.height()
            );

        painter.setPen(
            QPen(
                QColor("#3f3f3f"),
                1
            )
        );

        painter.drawLine(
            plotArea.left(),
            y,
            plotArea.right(),
            y
        );

        painter.setPen(
            QColor("#aaaaaa")
        );

        const double value =
            ratio * maximumPressure;

        painter.drawText(
            5,
            y + 4,
            QString::number(
                value,
                'f',
                0
            )
        );
    }

    // ==================================================
    // Safe pressure limit
    // ==================================================

    const double safeRatio =
        SAFE_PRESSURE_LIMIT
        / maximumPressure;

    const int safeY =
        plotArea.bottom()
        -
        static_cast<int>(
            safeRatio * plotArea.height()
        );

    painter.setPen(
        QPen(
            QColor("#f0c040"),
            2,
            Qt::DashLine
        )
    );

    painter.drawLine(
        plotArea.left(),
        safeY,
        plotArea.right(),
        safeY
    );

    // Keep label safely inside the graph.
    const int safeLabelY =
        qMax(
            plotArea.top() + 14,
            safeY - 7
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
        plotArea.right() - 155,
        safeLabelY,
        "100 bar SAFE LIMIT"
    );

    // ==================================================
    // Helper for Y coordinate
    // ==================================================

    auto calculateY =
        [&](double pressure)
        {
            const double ratio =
                pressure / maximumPressure;

            return plotArea.bottom()
                -
                static_cast<int>(
                    ratio * plotArea.height()
                );
        };

    // ==================================================
    // Draw predicted pressure line
    // ==================================================

    if (
        predictedPressureHistory.size() == 1
    )
    {
        const int x =
            plotArea.center().x();

        const int y =
            calculateY(
                predictedPressureHistory.first()
            );

        painter.setPen(
            Qt::NoPen
        );

        painter.setBrush(
            QColor("#5dade2")
        );

        painter.drawEllipse(
            QPoint(x, y),
            5,
            5
        );
    }
    else if (
        predictedPressureHistory.size() > 1
    )
    {
        painter.setPen(
            QPen(
                QColor("#5dade2"),
                3
            )
        );

        for (
            int i = 1;
            i < predictedPressureHistory.size();
            ++i
        )
        {
            const int previousX =
                plotArea.left()
                +
                static_cast<int>(
                    static_cast<double>(i - 1)
                    /
                    (predictedPressureHistory.size() - 1)
                    *
                    plotArea.width()
                );

            const int currentX =
                plotArea.left()
                +
                static_cast<int>(
                    static_cast<double>(i)
                    /
                    (predictedPressureHistory.size() - 1)
                    *
                    plotArea.width()
                );

            const int previousY =
                calculateY(
                    predictedPressureHistory[i - 1]
                );

            const int currentY =
                calculateY(
                    predictedPressureHistory[i]
                );

            painter.drawLine(
                previousX,
                previousY,
                currentX,
                currentY
            );
        }
    }

    // ==================================================
    // Predicted data points
    // ==================================================

    painter.setPen(
        Qt::NoPen
    );

    for (
        int i = 0;
        i < predictedPressureHistory.size();
        ++i
    )
    {
        const int x =
            predictedPressureHistory.size() == 1
            ?
            plotArea.center().x()
            :
            plotArea.left()
            +
            static_cast<int>(
                static_cast<double>(i)
                /
                (predictedPressureHistory.size() - 1)
                *
                plotArea.width()
            );

        const int y =
            calculateY(
                predictedPressureHistory[i]
            );

        painter.setBrush(
            QColor("#5dade2")
        );

        painter.drawEllipse(
            QPoint(x, y),
            4,
            4
        );
    }

    // ==================================================
    // Draw actual pressure line
    // ==================================================

    if (
        actualPressureHistory.size() == 1
    )
    {
        const int x =
            plotArea.center().x();

        const int y =
            calculateY(
                actualPressureHistory.first()
            );

        painter.setPen(
            Qt::NoPen
        );

        painter.setBrush(
            QColor("#ffffff")
        );

        painter.drawEllipse(
            QPoint(x, y),
            5,
            5
        );
    }
    else if (
        actualPressureHistory.size() > 1
    )
    {
        painter.setPen(
            QPen(
                QColor("#ffffff"),
                2
            )
        );

        for (
            int i = 1;
            i < actualPressureHistory.size();
            ++i
        )
        {
            const int previousX =
                plotArea.left()
                +
                static_cast<int>(
                    static_cast<double>(i - 1)
                    /
                    (actualPressureHistory.size() - 1)
                    *
                    plotArea.width()
                );

            const int currentX =
                plotArea.left()
                +
                static_cast<int>(
                    static_cast<double>(i)
                    /
                    (actualPressureHistory.size() - 1)
                    *
                    plotArea.width()
                );

            const int previousY =
                calculateY(
                    actualPressureHistory[i - 1]
                );

            const int currentY =
                calculateY(
                    actualPressureHistory[i]
                );

            painter.drawLine(
                previousX,
                previousY,
                currentX,
                currentY
            );
        }
    }

    // ==================================================
    // Actual data points
    // ==================================================

    painter.setPen(
        Qt::NoPen
    );

    for (
        int i = 0;
        i < actualPressureHistory.size();
        ++i
    )
    {
        const int x =
            actualPressureHistory.size() == 1
            ?
            plotArea.center().x()
            :
            plotArea.left()
            +
            static_cast<int>(
                static_cast<double>(i)
                /
                (actualPressureHistory.size() - 1)
                *
                plotArea.width()
            );

        const int y =
            calculateY(
                actualPressureHistory[i]
            );

        painter.setBrush(
            QColor("#ffffff")
        );

        painter.drawEllipse(
            QPoint(x, y),
            3,
            3
        );
    }

    // ==================================================
    // Latest values
    // ==================================================

    painter.setFont(
        QFont(
            "Arial",
            9,
            QFont::Bold
        )
    );

    if (!predictedPressureHistory.isEmpty())
    {
        const double latestPredicted =
            predictedPressureHistory.last();

        painter.setPen(
            QColor("#5dade2")
        );

        painter.drawText(
            plotArea.left(),
            height() - 10,
            QString(
                "Predicted: %1 bar"
            ).arg(
                latestPredicted,
                0,
                'f',
                2
            )
        );
    }

    if (!actualPressureHistory.isEmpty())
    {
        const double latestActual =
            actualPressureHistory.last();

        painter.setPen(
            latestActual > SAFE_PRESSURE_LIMIT
                ? QColor("#ff5c5c")
                : QColor("#ffffff")
        );

        painter.drawText(
            plotArea.left() + 150,
            height() - 10,
            QString(
                "Actual: %1 bar"
            ).arg(
                latestActual,
                0,
                'f',
                2
            )
        );
    }
}