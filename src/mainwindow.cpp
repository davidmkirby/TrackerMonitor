#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_trackerMemory(new TrackerMemory(this))
    , m_logger(new Logger(this))
    , m_pollTimer(new QTimer(this))
{
    ui->setupUi(this);

    // Initialize the UI
    setUIEnabled(false);
    ui->stopLoggingButton->setEnabled(false);
    ui->actionStop_Logging->setEnabled(false);

    // Connect tracker and logger error signals
    connect(m_trackerMemory, &TrackerMemory::errorOccurred, this, &MainWindow::handleTrackerError);
    connect(m_logger, &Logger::errorOccurred, this, &MainWindow::handleLoggerError);

    // Connect poll timer
    connect(m_pollTimer, &QTimer::timeout, this, &MainWindow::pollTracker);

    // Set poll timer interval to 4ms (250Hz)
    m_pollTimer->setInterval(4);
}

MainWindow::~MainWindow()
{
    // Stop polling and logging
    m_pollTimer->stop();
    m_logger->stopLogging();

    delete ui;
}

void MainWindow::on_initButton_clicked()
{
    // Initialize the tracker memory with PCI device location and base address
    if (m_trackerMemory->initialize(0xdba00000, 0x0800, 0x98, 0x00, 0x00)) {
        ui->statusLabel->setText("Initialized");
        setUIEnabled(true);
    } else {
        ui->statusLabel->setText("Initialization failed");
    }
}

void MainWindow::on_pingButton_clicked()
{
    if (m_trackerMemory->sendPing()) {
        ui->statusLabel->setText("Ping sent");
    } else {
        ui->statusLabel->setText("Ping failed");
    }
}

void MainWindow::on_startLoggingButton_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save Log File", "", "CSV Files (*.csv);;All Files (*)");
    if (filename.isEmpty()) {
        return;
    }

    if (m_logger->startLogging(filename)) {
        ui->statusLabel->setText("Logging started: " + filename);
        ui->startLoggingButton->setEnabled(false);
        ui->stopLoggingButton->setEnabled(true);
        ui->actionStart_Logging->setEnabled(false);
        ui->actionStop_Logging->setEnabled(true);
    }
}

void MainWindow::on_stopLoggingButton_clicked()
{
    m_logger->stopLogging();
    ui->statusLabel->setText("Logging stopped");
    ui->startLoggingButton->setEnabled(true);
    ui->stopLoggingButton->setEnabled(false);
    ui->actionStart_Logging->setEnabled(true);
    ui->actionStop_Logging->setEnabled(false);
}

void MainWindow::on_automaticPollCheckBox_stateChanged(int state)
{
    bool checked = (state == Qt::Checked);

    // Start or stop polling
    if (checked) {
        m_pollTimer->start();
        ui->statusLabel->setText("Automatic polling started");
    } else {
        m_pollTimer->stop();
        ui->statusLabel->setText("Automatic polling stopped");
    }

    ui->actionAutomatic_Poll->setChecked(checked);
}

void MainWindow::on_actionStart_Logging_triggered()
{
    on_startLoggingButton_clicked();
}

void MainWindow::on_actionStop_Logging_triggered()
{
    on_stopLoggingButton_clicked();
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionInitialize_triggered()
{
    on_initButton_clicked();
}

void MainWindow::on_actionPing_triggered()
{
    on_pingButton_clicked();
}

void MainWindow::on_actionAutomatic_Poll_toggled(bool checked)
{
    ui->automaticPollCheckBox->setChecked(checked);
}

void MainWindow::pollTracker()
{
    TrackData data;
    if (m_trackerMemory->readStatusData(data)) {
        updateUI(data);

        // Log the data if logging is enabled
        if (m_logger->isLogging()) {
            m_logger->logData(data);
        }
    }
}

void MainWindow::updateUI(const TrackData& data)
{
    // Update track errors
    ui->rawErrorXLineEdit->setText(QString::number(data.rawErrorX, 'f', 3));
    ui->rawErrorYLineEdit->setText(QString::number(data.rawErrorY, 'f', 3));
    ui->filteredErrorXLineEdit->setText(QString::number(data.filteredErrorX, 'f', 3));
    ui->filteredErrorYLineEdit->setText(QString::number(data.filteredErrorY, 'f', 3));

    // Update status information
    ui->trackStateLineEdit->setText(data.getStateString());
    ui->trackModeLineEdit->setText(data.getModeString());
    ui->targetPolarityLineEdit->setText(data.getPolarityString());
    ui->statusLineEdit->setText(data.getStatusString());

    // Update target information
    ui->targetSizeXLineEdit->setText(QString::number(data.targetSizeX));
    ui->targetSizeYLineEdit->setText(QString::number(data.targetSizeY));
    ui->targetLeftLineEdit->setText(QString::number(data.targetLeft));
    ui->targetTopLineEdit->setText(QString::number(data.targetTop));
    ui->pixelCountLineEdit->setText(QString::number(data.targetPixelCount));

    // Update mount position (scaled by 10000)
    ui->azimuthLineEdit->setText(QString::number(static_cast<float>(data.azimuth) / 10000.0f, 'f', 4));
    ui->elevationLineEdit->setText(QString::number(static_cast<float>(data.elevation) / 10000.0f, 'f', 4));
}

void MainWindow::setUIEnabled(bool enabled)
{
    ui->pingButton->setEnabled(enabled);
    ui->startLoggingButton->setEnabled(enabled);
    ui->automaticPollCheckBox->setEnabled(enabled);
    ui->actionPing->setEnabled(enabled);
    ui->actionStart_Logging->setEnabled(enabled);
    ui->actionAutomatic_Poll->setEnabled(enabled);
}

void MainWindow::handleTrackerError(const QString& errorMsg)
{
    ui->statusLabel->setText("Tracker error: " + errorMsg);
    QMessageBox::critical(this, "Tracker Error", errorMsg);
}

void MainWindow::handleLoggerError(const QString& errorMsg)
{
    ui->statusLabel->setText("Logger error: " + errorMsg);
    QMessageBox::critical(this, "Logger Error", errorMsg);
}