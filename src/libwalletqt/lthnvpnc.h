#ifndef LTHNVPNC_H
#define LTHNVPNC_H

#include <QDir>
#include <QFile>
#include <QDebug>
#include <QProcess>
#include <QThread>
#include <QtCore>
#include <QObject>

#include <vector>
#include <mutex>
#include "lthnvpncLogReaderThread.h"

#ifdef Q_OS_WIN
	#include <QTimer>
    #include <windows.h>
#endif

class lthnvpnc : public QObject {

	Q_OBJECT	
public:

	Q_INVOKABLE bool killLthnvpnc();
	Q_INVOKABLE bool initializeLthnvpnc(const QString& workingDir, const QString& authId, const QString& providerId, const QString& serviceId);
	Q_INVOKABLE bool isMessageAvailable();
	Q_INVOKABLE QString getLastMessage();
	Q_INVOKABLE void cleanupThreads();

    lthnvpnc() = default;
    ~lthnvpnc() {
    	cleanupThreads();
    }

public slots:
	void handleThreadLogReaderResult(const QString& result);

private:
	lthnvpncLogReaderThread* threadLogReader;
	// Stores status messages read from the lthnvpnc log file output
	// Most recent messages are listed last
	std::vector<QString> m_statusMsg;
	std::mutex m_statusMsg_mutex;
#ifdef Q_OS_WIN
	QTimer* timerCheckLog;
	// Pipes used to redirect lthnvpnc output
	HANDLE m_logFileHandle_OUT_Rd = NULL;
	HANDLE m_logFileHandle_OUT_Wr = NULL;
#endif

};

#endif