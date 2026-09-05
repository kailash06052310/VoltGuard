#ifndef VOLTGUARD_DASHBOARD_H
#define VOLTGUARD_DASHBOARD_H

#include <QLabel>
#include <QMainWindow>
#include <QTableWidget>
#include <QTimer>
#include <QVector>

class PressureTrendWidget;

class VoltGuardDashboard : public QMainWindow
{
public:
    explicit VoltGuardDashboard(
        QWidget *parent = nullptr
    );

private:
    // System Monitor page
    QLabel *systemStatusLabel;
    QLabel *pumpRpmLabel;
    QLabel *pressureLabel;
    QLabel *flowRateLabel;
    QLabel *valveLabel;

    PressureTrendWidget *pressureTrendWidget;

    // Security Center page
    QLabel *decisionLabel;
    QLabel *actionLabel;
    QLabel *reasonLabel;

    QTableWidget *eventTable;

    // Physics history
    QVector<double> predictedPressureHistory;
    QVector<double> actualPressureHistory;
    QVector<bool> safeHistory;

    QTimer *refreshTimer;

    void loadSecurityEvents();
};

#endif