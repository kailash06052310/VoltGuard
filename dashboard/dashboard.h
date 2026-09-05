#ifndef VOLTGUARD_DASHBOARD_H
#define VOLTGUARD_DASHBOARD_H

#include <QLabel>
#include <QMainWindow>
#include <QTableWidget>
#include <QTimer>

class VoltGuardDashboard : public QMainWindow
{
public:
    explicit VoltGuardDashboard(QWidget *parent = nullptr);

private:
    QLabel *systemStatusLabel;
    QLabel *pumpRpmLabel;
    QLabel *pressureLabel;
    QLabel *flowRateLabel;
    QLabel *valveLabel;
    QLabel *decisionLabel;
    QLabel *actionLabel;

    QTableWidget *eventTable;

    QTimer *refreshTimer;

    void loadSecurityEvents();
};

#endif