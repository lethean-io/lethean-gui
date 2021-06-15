#ifndef THREADVERIFYHAPROXY_H
#define THREADVERIFYHAPROXY_H

#include "HTTPResponse.h"
#include "Wallet.h"
#include "PendingTransaction.h"

#include <QFuture>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QProcess>
#include <QTimer>
#include <QThread>
#include <QtCore>
#include <QObject>

#include <string>
#include <iostream>
#include <istream>
#include <ostream>
#include <boost/asio.hpp>

// Class to make non blocking threaded request
class ThreadVerifyHaproxy : public QThread
{    
    Q_OBJECT
public:
    // set variable value to call the startVerifyHaproxy()
    void setup(std::string host, std::string port, std::string provider);

    // run the thread
    void run() override {
        QString result = startVerifyHaproxy();
        emit resultReady(result);
    }
signals:
    // wait for the signal to send the value back to QML
    void resultReady(const QString &s);

private:
    std::string m_haproxyHost;
    std::string m_haproxyPort;
    std::string m_haproxyProvider;

    // method that validates haproxy connection
    QString startVerifyHaproxy();
    std::string buffer_to_string(const boost::asio::streambuf &buffer);
    HttpResponse proxyRequest(std::string proxyHost, std::string proxyPort, std::string requestURL, std::string provider);

};

#endif // THREADVERIFYHAPROXY_H
