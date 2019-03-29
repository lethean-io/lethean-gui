#include "lthnvpncLogReaderThread.h"

#include <QDebug>
#include <QProcess>
#include <QThread>
#include <QtCore>
#include <QObject>

#include <iostream>
#include <string>
#include <fstream>

void lthnvpncLogReaderThread::run() {

#ifdef Q_OS_WIN
	while(true) {
		if (!m_logFileHandle || m_logFileHandle == INVALID_HANDLE_VALUE) {
			QThread::sleep(1);
			continue;
		}
		DWORD bufSize = 4096;
		CHAR chBuf[bufSize];
		DWORD dwRead;

		BOOL success = ReadFile(m_logFileHandle, chBuf, bufSize, &dwRead, NULL);
		if (success && dwRead != 0) {
			// advance file pointer to account for the amount of bytes we just read
			SetFilePointer(m_logFileHandle, dwRead, NULL, FILE_CURRENT);
			// convert char (windows 8-bit ANSI) to QString
			QString str = QString::fromLocal8Bit(chBuf);
			emit resultReady(str);
		}

		QThread::msleep(250);
	}
#else	
	if (m_logFile.empty())
		return;		

	//on first run, wait a few seconds to allow things to load..
	QThread::sleep(3);

	std::ifstream ifs(m_logFile);

	if (!ifs.is_open()) {
		qDebug() << "Error opening log file " << QString::fromStdString(m_logFile);
		return;
	}

	std::ios::streampos gpos = ifs.tellg();
	std::string line;

		
	while(true) {
		if (!std::getline(ifs,line) || ifs.eof()) {
			ifs.clear();
			ifs.seekg(gpos);
			QThread::msleep(500);
			continue;
		}

		gpos = ifs.tellg();
		emit resultReady(QString::fromStdString(line));
	}
#endif	
}

void lthnvpncLogReaderThread::setLogFile(const std::string file) {
	m_logFile = file;
}