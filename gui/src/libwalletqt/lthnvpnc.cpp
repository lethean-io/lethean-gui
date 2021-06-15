#include "lthnvpnc.h"

bool lthnvpnc::initializeLthnvpnc(const QString& workingDir, const QString& authId, const QString& providerId, const QString& serviceId) {

	if (authId.isEmpty() || providerId.isEmpty() || serviceId.isEmpty()) {
		qDebug() << "lthnvpnc::initializeLthnvpnc bad params! AuthId " + authId + " providerId " + providerId + " serviceId " + serviceId;
		return false;
	}

	//kill any running instances
	if (m_wasLthnvpncStarted) {
		killLthnvpnc();

		int ms = 1000;
		//sleep for 1second as order of killing old lthnvpnc and starting new one is indeterminate
		#ifdef Q_OS_WIN
		    Sleep(uint(ms));
		#else
		    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
		    nanosleep(&ts, NULL);
		#endif
	}

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
  			m_wasLthnvpncStarted = true;
  			CloseHandle(ProcessInfo.hProcess);
  			CloseHandle(ProcessInfo.hThread);
  			CloseHandle(m_logFileHandle_OUT_Wr);
  		}

  		//start log monitor thread
	    m_threadLogReader = new lthnvpncLogReaderThread();
	    m_threadLogReader->setLogFileHandle(m_logFileHandle_OUT_Rd);
	    connect(m_threadLogReader, &lthnvpncLogReaderThread::resultReady, this, &lthnvpnc::handleThreadLogReaderResult);
	    connect(m_threadLogReader, &lthnvpncLogReaderThread::finished, m_threadLogReader, &QObject::deleteLater);
	    m_threadLogReader->start();
	#else
  		QString logPath = workingDir + "lthnvpnc.log";
        QString command = workingDir + "lthnvpnc --authid " + authId + " connect " + providerId + "/" + serviceId + " > " + logPath + " 2>&1 &";
        qDebug() << "Launching lthnvpnc: " + command;
        int result = system(qPrintable(command));
        if (result != 0) {
            qDebug () << "Failed to launch lthnvpnc: " + QString::number(result);
            return false;
        }

        m_wasLthnvpncStarted = true;
        //start log monitor thread
	    m_threadLogReader = new lthnvpncLogReaderThread();
	    m_threadLogReader->setLogFile(logPath.toStdString());
	    connect(m_threadLogReader, &lthnvpncLogReaderThread::resultReady, this, &lthnvpnc::handleThreadLogReaderResult);
	    connect(m_threadLogReader, &lthnvpncLogReaderThread::finished, m_threadLogReader, &QObject::deleteLater);
	    m_threadLogReader->start();
	#endif

    return true;
}

void lthnvpnc::killLthnvpnc() {
	qDebug() << "Killing lthnvpnc";
	#ifdef Q_OS_WIN
        WinExec("taskkill /f /im lthnvpnc.exe", SW_HIDE);        
        restartLthnVpnSvc();
    #else
        system("pkill -f lthnvpnc");
    #endif

    cleanupThreads();
}

void lthnvpnc::restartLthnVpnSvc() const {
	#ifdef Q_OS_WIN
		qDebug() << "Checking Lethean VPN service status...";
		//obtain sc manager handle
		DWORD dwWaitTime;
		DWORD dwBytesNeeded;
		DWORD dwStartTickCount;
		DWORD dwOldCheckPoint;
		SERVICE_STATUS_PROCESS ssp;

		SC_HANDLE hScManager = OpenSCManagerA(NULL, SERVICES_ACTIVE_DATABASEA, 
			SC_MANAGER_CONNECT); // | SC_MANAGER_ENUMERATE_SERVICE | SC_MANAGER_QUERY_LOCK_STATUS | STANDARD_RIGHTS_READ);
		if (hScManager == NULL) {
			DWORD lastError = GetLastError();
			qDebug() << "Failed to obtain SCM handle! " + QString::number(lastError);
			return;
		}

		//obtain handle for our service
		LPCSTR svcName = "OpenVPNServiceInteractive$Lethean";
		SC_HANDLE hSvc = OpenServiceA(hScManager, svcName, SERVICE_QUERY_STATUS | SERVICE_START | SERVICE_STOP);
		if (hSvc == NULL) {
			DWORD lastError = GetLastError();
			qDebug() << "Failed to obtain service handle! " + QString::number(lastError);
			goto stop_cleanup;
		}
		
		qDebug() << "Checking Lethean VPN service status";
		//query service statu to ensure it is not already stopped
		if ( !QueryServiceStatusEx( 
			hSvc, 
			SC_STATUS_PROCESS_INFO,
			(LPBYTE)&ssp, 
			sizeof(SERVICE_STATUS_PROCESS),
			&dwBytesNeeded ) )
		{
			DWORD lastError = GetLastError();
			qDebug() << "QueryServiceStatusEx failed " + QString::number(lastError);
			goto stop_cleanup;
		}

		//if service is not stopped, attempt to stop it
		if ( ssp.dwCurrentState != SERVICE_STOPPED )
		{
			//stop service if a current stop is not pending
			if ( ssp.dwCurrentState != SERVICE_STOP_PENDING ) {
				if(!ControlService( hSvc, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&ssp )) {
					DWORD lastError = GetLastError();
					qDebug() << "Failed to stop service! " + QString::number(lastError);
					goto stop_cleanup;
				}
				qDebug() << "Requesting stop for Lethean VPN service";
			}

			DWORD dwTimeout = 30000; // 30-second time-out
			DWORD dwStartTime = GetTickCount();
			//wait for service to be fully stopped
			while ( ssp.dwCurrentState != SERVICE_STOPPED ) 
			{
				Sleep( ssp.dwWaitHint );
				if ( !QueryServiceStatusEx( 
					hSvc, 
					SC_STATUS_PROCESS_INFO,
					(LPBYTE)&ssp, 
					sizeof(SERVICE_STATUS_PROCESS),
					&dwBytesNeeded ) )
				{
					DWORD lastError = GetLastError();
					qDebug() << "QueryServiceStatusEx failed " + QString::number(lastError);
					goto stop_cleanup;
				}

				if ( ssp.dwCurrentState == SERVICE_STOPPED )
					break;

				if ( GetTickCount() - dwStartTime > dwTimeout )
				{
					qDebug( "Service stop wait timed out" );
					goto stop_cleanup;
				}
			}
		}

		qDebug() << "Lethean VPN service stopped successfully";
		//start service		
		if (!StartService(hSvc, 0, NULL)) {
			DWORD lastError = GetLastError();
			qDebug() << "Failed to start service! " + QString::number(lastError);
			goto stop_cleanup;
		}

		//query status until started
		if ( !QueryServiceStatusEx( 
			hSvc, 
			SC_STATUS_PROCESS_INFO,
			(LPBYTE)&ssp, 
			sizeof(SERVICE_STATUS_PROCESS),
			&dwBytesNeeded ) )
		{
			DWORD lastError = GetLastError();
			qDebug() << "QueryServiceStatusEx failed " + QString::number(lastError);
			goto stop_cleanup;
		}

		dwStartTickCount = GetTickCount();
		dwOldCheckPoint = ssp.dwCheckPoint;

		while(ssp.dwCurrentState == SERVICE_START_PENDING) {
			// Do not wait longer than the wait hint. A good interval is 
			// one-tenth the wait hint, but no less than 1 second and no 
			// more than 10 seconds. 

			dwWaitTime = ssp.dwWaitHint / 10;

			if( dwWaitTime < 1000 )
			    dwWaitTime = 1000;
			else if ( dwWaitTime > 10000 )
			    dwWaitTime = 10000;

			Sleep( dwWaitTime );

			// Check the status again. 

			if (!QueryServiceStatusEx( 
			    hSvc,             // handle to service 
			    SC_STATUS_PROCESS_INFO, // info level
			    (LPBYTE) &ssp,             // address of structure
			    sizeof(SERVICE_STATUS_PROCESS), // size of structure
			    &dwBytesNeeded ) )              // if buffer too small
			{
			    DWORD lastError = GetLastError();
				qDebug() << "QueryServiceStatusEx failed " + QString::number(lastError);
			    break; 
			}

			if ( ssp.dwCheckPoint > dwOldCheckPoint )
			{
			    // Continue to wait and check.

			    dwStartTickCount = GetTickCount();
			    dwOldCheckPoint = ssp.dwCheckPoint;
			}
			else
			{
			    if(GetTickCount()-dwStartTickCount > ssp.dwWaitHint)
			    {
			        // No progress made within the wait hint.
			        break;
			    }
			}
		}

		if (ssp.dwCurrentState == SERVICE_RUNNING) 
		{
			qDebug() << "Lethean VPN service started successfully";
		}
		else 
		{ 
			qDebug("Lethean VPN service not started.");
			qDebug() << "  Current State: " + QString::number(ssp.dwCurrentState); 
			qDebug() << "  Exit Code: " + QString::number(ssp.dwWin32ExitCode); 
			qDebug() << "  Check Point: " + QString::number(ssp.dwCheckPoint); 
			qDebug() << "  Wait Hint: " + QString::number(ssp.dwWaitHint); 
		} 

stop_cleanup:
	if (hScManager != NULL)
		CloseServiceHandle(hScManager);
	if (hSvc != NULL)
		CloseServiceHandle(hSvc);
	#endif
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
	if (m_threadLogReader != Q_NULLPTR) {
		qDebug() << "Cleaning up log monitor thread";
		m_threadLogReader->quit();
	}
}
