#ifndef HAPROXY
#define HAPROXY

#include "ThreadVerifyHaproxy.h"

#include <QDir>
#include <QFile>
#include <QDebug>
#include <QProcess>
#include <QTimer>
#include <QThread>
#include <QtCore>
#include <QObject>

#include <thread>
#include <string>
#include <iostream>
#include <istream>
#include <ostream>
#include <boost/asio.hpp>

#ifdef Q_OS_WIN
    #include <windows.h>
#endif

namespace Monero {
    class Haproxy; // forward declaration
}
class Haproxy : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString haproxyPath READ haproxyPath)
    Q_PROPERTY(QString haproxyConfigPath READ haproxyConfigPath)
    // Use in QML to get the return from proxyStatus
    Q_PROPERTY(QString haproxyStatus READ haproxyStatus)
public:
      QString haproxyStatus() const {
          return m_haproxyStatus;
    }
    //save the path to get the error when is not possible run the haproxy
    QString haproxyPath() const {
        return m_haproxyPath;
    }

    QString haproxyConfigPath() const {
        return m_haproxyConfigPath;
    }

    Q_INVOKABLE bool haproxy(const QString &host, const QString &ip, const QString &port, const QString &endpoint, const QString &endpointport, const QString &fixedHost, const QString &auth, const QString &provider, const QString &plan, const QString &serviceName);
    Q_INVOKABLE void haproxyCert(const QString &host, const QString &certificate);
    Q_INVOKABLE void killHAproxy();

    // returns true if proxy is online and accepting connections, false otherwise
    Q_INVOKABLE void verifyHaproxy(const QString &host, const QString &ip, const QString &provider);

private:
    QString m_haproxyStatus;

    // callback to process results from thread
    void handleResults(const QString proxyStatus);

    QString m_haproxyPath;
    QString m_haproxyConfigPath;
};

#endif // HAPROXY

