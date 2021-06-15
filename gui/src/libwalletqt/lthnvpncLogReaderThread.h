#ifndef LTHNVPNCLOGREADERTHREAD_H
#define LTHNVPNCLOGREADERTHREAD_H

#include <QDebug>
#include <QProcess>
#include <QThread>
#include <QtCore>
#include <QObject>

#include <iostream>
#include <string>
#include <fstream>

#ifdef Q_OS_WIN
    #include <windows.h>
#endif

class lthnvpncLogReaderThread : public QThread {

	Q_OBJECT

public:

	void run() override;
	void setLogFile(const std::string file);
#ifdef Q_OS_WIN
	void setLogFileHandle(HANDLE handle) { m_logFileHandle = handle; }
#endif

private:
	std::string m_logFile;
#ifdef Q_OS_WIN
	HANDLE m_logFileHandle;
#endif

signals:
	void resultReady(const QString &s);
};

#endif