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
    // --------------------------------------------------
    // Dashboard telemetry
    // --------------------------------------------------

    QLabel *systemStatusLabel;
    QLabel *pumpRpmLabel;
    QLabel *pressureLabel;
    QLabel *flowRateLabel;
    QLabel *valveLabel;

    // --------------------------------------------------
    // Security decision
    // --------------------------------------------------

    QLabel *decisionLabel;
    QLabel *actionLabel;

    // --------------------------------------------------
    // Security event table
    // --------------------------------------------------

    QTableWidget *eventTable;

    // --------------------------------------------------
    // Physics trend visualization
    // --------------------------------------------------

    PressureTrendWidget *pressureTrendWidget;

    QVector<double> pressureHistory;
    QVector<bool> safeHistory;

    // --------------------------------------------------
    // Automatic dashboard refresh
    // --------------------------------------------------

    QTimer *refreshTimer;

    // --------------------------------------------------
    // Data loading
    // --------------------------------------------------

    void loadSecurityEvents();
};

#endif