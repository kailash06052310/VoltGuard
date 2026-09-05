#ifndef VOLTGUARD_TREND_WIDGET_H
#define VOLTGUARD_TREND_WIDGET_H

#include <QWidget>
#include <QVector>

class QPaintEvent;

class PressureTrendWidget : public QWidget
{
public:
    explicit PressureTrendWidget(QWidget *parent = nullptr);

    void setData(
        const QVector<double> &predictedPressures,
        const QVector<double> &actualPressures,
        const QVector<bool> &safeStates
    );

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<double> predictedPressureHistory;
    QVector<double> actualPressureHistory;
    QVector<bool> safeHistory;

    static constexpr double SAFE_PRESSURE_LIMIT = 100.0;

    // Keep the graph readable by showing
    // only the latest physics samples.
    static constexpr int MAX_POINTS = 20;
};

#endif