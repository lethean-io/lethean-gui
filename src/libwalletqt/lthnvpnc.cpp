#include "lthnvpnc.h"

bool lthnvpnc::initializeLthnvpnc(const QString& workingDir, const QString& authId, const QString& providerId, const QString& serviceId) {

	if (authId.isEmpty() || providerId.isEmpty() || serviceId.isEmpty()) {
		qDebug() << "lthnvpnc::initializeLthnvpnc bad params! AuthId " + authId + " providerId " + providerId + " serviceId " + serviceId;
		return false;
	}

	//kill any running instances
	// ** TEMPORARILY DISABLED ON WINDOWS DUE TO TIMING ISSUES **
#ifndef Q_OS_WIN
	killLthnvpnc();
#endif

	//launch lthnvpnc with args	
	#ifdef Q_OS_WIN
		//{ Set up HANDLES to act as pipes for reading output from lthnvpnc
		// Set the bInheritHandle flag so pipe handles are inherited.
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(sa);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;

		// Create a pipe for the child process's STDOUT.  
		if ( ! CreatePipe(&m_logFileHandle_OUT_Rd, &m_logFileHandle_OUT_Wr, &sa, 0) ) {
			qDebug() << "Failed to create pipe!";
			return false;
		}

		// Ensure the read handle to the pipe for STDOUT is not inherited.
		if ( ! SetHandleInformation(m_logFileHandle_OUT_Rd, HANDLE_FLAG_INHERIT, 0) ) {
				qDebug() << "Failed to set pipe information!";
				return false;
		}

		// Hide window of created process
		DWORD CreationFlags = CREATE_NO_WINDOW;
		// Setup ProcessInfo (OUT) and StartupInfo (IN)
			PROCESS_INFORMATION ProcessInfo;
		STARTUPINFOA StartupInfo;		
		ZeroMemory(&ProcessInfo, sizeof(PROCESS_INFORMATION));
		ZeroMemory(&StartupInfo, sizeof(STARTUPINFOA));
		StartupInfo.cb = sizeof(STARTUPINFOA);
		StartupInfo.dwFlags |= STARTF_USESTDHANDLES;
		StartupInfo.hStdInput = NULL;
		StartupInfo.hStdError = m_logFileHandle_OUT_Wr;
		StartupInfo.hStdOutput = m_logFileHandle_OUT_Wr;
		//}

		LPCSTR appName = "lthnvpnc.exe";

		QString command = "lthnvpnc.exe connect " + authId + "@" + providerId + "/" + serviceId;		
		QByteArray baCmd = command.toLocal8Bit();
		char* lpstrCmd = baCmd.data();

		qDebug() << "Launching lthnvpnc: " + command;

  		if(!CreateProcessA( 
			appName,
			lpstrCmd,
			NULL,
			NULL,
			TRUE,
			CreationFlags,
			NULL,
			NULL,
			&StartupInfo,
			&ProcessInfo)) {
	  			DWORD lastError = GetLastError();
	  			qDebug() << "Failed to launch lthnvpnc (Windows): " + QString::number(lastError);
	  			cleanupThreads();
	            return false;
  		} else {
  			CloseHandle(ProcessInfo.hProcess);
  			CloseHandle(ProcessInfo.hThread);
  			CloseHandle(m_logFileHandle_OUT_Wr);
  		}

  		//start log monitor thread
	    threadLogReader = new lthnvpncLogReaderThread();
	    threadLogReader->setLogFileHandle(m_logFileHandle_OUT_Rd);
	    connect(threadLogReader, &lthnvpncLogReaderThread::resultReady, this, &lthnvpnc::handleThreadLogReaderResult);
	    connect(threadLogReader, &lthnvpncLogReaderThread::finished, threadLogReader, &QObject::deleteLater);
	    threadLogReader->start();
	#else
  		QString logPath = workingDir + "lthnvpnc.log";
        QString command = workingDir + "lthnvpnc --authid " + authId + " connect " + providerId + "/" + serviceId + " > " + logPath + " 2>&1";
        qDebug() << "Launching lthnvpnc: " + command;
        int result = system(qPrintable(command));
        if (result != 0) {
            qDebug () << "Failed to launch lthnvpnc: " + QString::number(result);
            return false;
        }

        //start log monitor thread
	    threadLogReader = new lthnvpncLogReaderThread();
	    threadLogReader->setLogFile(logPath.toStdString());
	    connect(threadLogReader, &lthnvpncLogReaderThread::resultReady, this, &lthnvpnc::handleThreadLogReaderResult);
	    connect(threadLogReader, &lthnvpncLogReaderThread::finished, threadLogReader, &QObject::deleteLater);
	    threadLogReader->start();
	#endif

    return true;
}

bool lthnvpnc::killLthnvpnc() {
	cleanupThreads();

	qDebug() << "Killing lthnvpnc";
	#ifdef Q_OS_WIN
        uint result = WinExec("taskkill /f /im lthnvpnc.exe", SW_HIDE);
        return (result > 31);
    #else
        int result = system("pkill -f lthnvpnc");
        return (result == 0);
    #endif

    return false;
}

bool lthnvpnc::isMessageAvailable() {
	std::lock_guard<std::mutex> lock(m_statusMsg_mutex);

	if (m_statusMsg.empty())
		return false;
	else
		return true;
}

void lthnvpnc::handleThreadLogReaderResult(const QString& result) {
	qDebug() << "Received log message " + result;
	std::lock_guard<std::mutex> lock(m_statusMsg_mutex);

	if (result.contains("\r\n")) {
		//split messages by newline and add each to message queue separately
		QStringList messages = result.split("\r\n");
		for (const auto& i : messages) {
			if (!i.isEmpty())
				m_statusMsg.push_back(i);
		}
	} else {
		if (!result.isEmpty())
			m_statusMsg.push_back(result);
	}
}

QString lthnvpnc::getLastMessage() {
    std::lock_guard<std::mutex> lock(m_statusMsg_mutex);

	if (m_statusMsg.empty())
		return QString::null;
	else {
		QString ret = m_statusMsg.front();
		m_statusMsg.erase(m_statusMsg.begin());
		return ret;
	}
}

void lthnvpnc::cleanupThreads() {
#ifdef Q_OS_WIN
	// Cleanup file handles
	if (m_logFileHandle_OUT_Rd && m_logFileHandle_OUT_Rd != INVALID_HANDLE_VALUE) {
		qDebug() << "Cleaning up m_logFileHandle_OUT_Rd";
		CloseHandle(m_logFileHandle_OUT_Rd);
	}
	if (m_logFileHandle_OUT_Wr && m_logFileHandle_OUT_Wr != INVALID_HANDLE_VALUE) {
		qDebug() << "Cleaning up m_logFileHandle_OUT_Wr";
		CloseHandle(m_logFileHandle_OUT_Wr);
	}
#endif
	if (threadLogReader != Q_NULLPTR) {
		qDebug() << "Cleaning up log monitor thread";
		threadLogReader->quit();
	}
}