#include "logger.h"
#include <QDebug>

Logger::Logger(QObject *parent)
    : QObject(parent)
    , m_isLogging(false)
    , m_headerWritten(false)
{
}

Logger::~Logger()
{
    stopLogging();
}

bool Logger::startLogging(const QString& filename)
{
    if (m_isLogging) {
        stopLogging();
    }

    m_logFile.setFileName(filename);
    if (!m_logFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit errorOccurred("Failed to open log file: " + filename);
        return false;
    }

    m_textStream.setDevice(&m_logFile);
    m_isLogging = true;
    m_headerWritten = false;

    return true;
}

void Logger::stopLogging()
{
    if (m_isLogging) {
        m_logFile.close();
        m_isLogging = false;
    }
}

void Logger::writeHeader()
{
    m_textStream << "Timestamp,RawErrorX,RawErrorY,FilteredErrorX,FilteredErrorY,"
                 << "TrackState,TrackMode,TargetPolarity,Status,"
                 << "TargetSizeX,TargetSizeY,TargetLeft,TargetTop,TargetPixelCount,"
                 << "Azimuth,Elevation" << Qt::endl;
    m_headerWritten = true;
}

void Logger::logData(const TrackData& data)
{
    if (!m_isLogging) {
        return;
    }

    if (!m_headerWritten) {
        writeHeader();
    }

    // Write the timestamp and track data
    m_textStream << QDateTime::currentDateTime().toString(Qt::ISODate) << ","
                 << data.rawErrorX << "," << data.rawErrorY << ","
                 << data.filteredErrorX << "," << data.filteredErrorY << ","
                 << data.getStateString() << "," << data.getModeString() << ","
                 << data.getPolarityString() << "," << data.getStatusString() << ","
                 << data.targetSizeX << "," << data.targetSizeY << ","
                 << data.targetLeft << "," << data.targetTop << ","
                 << data.targetPixelCount << ","
                 << (static_cast<float>(data.azimuth) / 10000.0f) << ","
                 << (static_cast<float>(data.elevation) / 10000.0f) << Qt::endl;
}