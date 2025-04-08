#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QFile>
#include <QDateTime>
#include "trackermemory.h"
#include "trackdata.h"
#include "logger.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_initButton_clicked();
    void on_pingButton_clicked();
    void on_startLoggingButton_clicked();
    void on_stopLoggingButton_clicked();
    void on_automaticPollCheckBox_stateChanged(int state);
    void on_actionStart_Logging_triggered();
    void on_actionStop_Logging_triggered();
    void on_actionExit_triggered();
    void on_actionInitialize_triggered();
    void on_actionPing_triggered();
    void on_actionAutomatic_Poll_toggled(bool checked);

    void pollTracker();
    void handleTrackerError(const QString& errorMsg);
    void handleLoggerError(const QString& errorMsg);

private:
    Ui::MainWindow *ui;
    TrackerMemory *m_trackerMemory;
    Logger *m_logger;
    QTimer *m_pollTimer;

    void updateUI(const TrackData& data);
    void setUIEnabled(bool enabled);
};
#endif // MAINWINDOW_H