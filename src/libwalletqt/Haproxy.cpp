#include "Haproxy.h"
#include "Thread.h"
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

bool Haproxy::haproxy(const QString &host, const QString &ip, const QString &port, const QString &endpoint, const QString &endpointport, const QString &fixedHost, const QString &auth, const QString &provider, const QString &plan, const QString &serviceName) {
    qDebug() << "Starting haproxy";

    QFile::remove(host + "/provider.http");
    QFile fileProvider(host + "/provider.http");

    // delete old file and create new one
    QFile::remove(host + "/haproxy.cfg");
    QFile file(host + "/haproxy.cfg");

    //create provider.http
    if(fileProvider.open(QIODevice::ReadOnly | QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream txtStream(&fileProvider);

        txtStream << "HTTP/1.0 200 PROVIDER\n";
        txtStream << "Access-Control-Allow-Origin: *\n";
        txtStream << "Access-Control-Allow-Methods: GET\n";
        txtStream << "Cache-Control: no-cache\n";
        txtStream << "Content-Type: text/html\n\n";

        txtStream << "{\"provider\":\""+serviceName+"\",\"service\":\""+plan+"\"}";


        txtStream.seek(0);
        /*
        while(!txtStream.atEnd()) {
            qDebug() << txtStream.readLine();
        }
        */
        fileProvider.close();
    }


    //create config file
    if (file.open(QIODevice::ReadOnly | QIODevice::WriteOnly | QIODevice::Text)) {

        QTextStream txtStream(&file);
        txtStream << "global\n";
        txtStream << "maxconn         2000\n";
        txtStream << "daemon\n";
        txtStream << "ssl-default-bind-ciphers ECDH+AESGCM\n";
        txtStream << "ssl-default-bind-options force-tlsv12\n";
        txtStream << "no log\n";


        txtStream << "frontend icproxy\n";
        txtStream << "bind            "+ip+":"+port+"\n";
        txtStream << "mode            http\n";
        txtStream << "log             global\n";
        txtStream << "option          dontlognull\n";
        txtStream << "option          nolinger\n";
        txtStream << "option          http_proxy\n";
        txtStream << "option          contstats\n";
        txtStream << "maxconn         8000\n";
        txtStream << "timeout client  30s\n";

        txtStream << "acl is_mgmt_host url_dom _local_\n";
        txtStream << "acl is_mgmt_path path_beg /status\n";
        txtStream << "acl is_stats_path path_beg /stats\n";
        txtStream << "acl is_mgmt_id hdr_reg(X-ITNS-MgmtID) ^"+provider+"$\n";
        txtStream << "acl is_proxy_request url_reg '.*://.*'\n";
        txtStream << "acl is_connect method CONNECT\n";
        txtStream << "acl is_options method OPTIONS\n";

        txtStream << "# If this is local request with right authid /stats, forward to stats backend\n";
        txtStream << "use_backend b-stats if !is_options !is_proxy_request is_stats_path is_mgmt_id\n";

        txtStream << "#  If this is local request with authid /status, forward to status backend\n";
        txtStream << "use_backend b-status if !is_proxy_request is_mgmt_path is_mgmt_id\n";

        txtStream << "# If this is proxy request with right id\n";
        txtStream << "use_backend b-status if is_mgmt_host is_mgmt_path is_mgmt_id\n";

        txtStream << "# If this is proxy request with right id\n";
        txtStream << "use_backend b-stats if is_mgmt_host is_stats_path is_mgmt_id\n";

        txtStream << "# Wrong mgmtid\n";
        txtStream << "use_backend b-err if is_mgmt_host is_mgmt_path !is_mgmt_id\n";

        txtStream << "# Forward OPTIONS to status\n";
        txtStream << "use_backend b-status if is_options !is_proxy_request is_mgmt_path is_mgmt_id\n";
        txtStream << "use_backend b-status if is_options !is_proxy_request is_stats_path\n";
        txtStream << "use_backend http-proxy if is_proxy_request || is_connect\n";

        txtStream << "backend http-proxy\n";
        txtStream << "mode            http\n";
        txtStream << "timeout connect 5s\n";
        txtStream << "timeout server  30s\n";
        //txtStream << "timeout client  30s\n";
        txtStream << "retries         2\n";
        txtStream << "option          nolinger\n";
        txtStream << "option          httplog\n";
        txtStream << "http-request add-header X-ITNS-PaymentID "+auth+"\n";
        #ifdef Q_OS_WIN
        txtStream << "server hatls " + endpoint + ":" + endpointport + " force-tlsv12 ssl ca-file 'ca.cert.pem'\n";
        #else
        txtStream << "server hatls " + endpoint + ":" + endpointport + " force-tlsv12 ssl ca-file '"+host+"/ca.cert.pem'\n";
        #endif

        #ifdef Q_OS_WIN
        txtStream << "errorfile 503 ha_err_connect.http\n";
        #else
        txtStream << "errorfile 503 "+host+"/ha_err_connect.http\n";
        #endif

        txtStream << "backend b-err\n";
        txtStream << "mode            http\n";
        txtStream << "timeout server  30s\n";
        txtStream << "timeout connect 5s\n";
        //txtStream << "timeout client  30s\n";
        #ifdef Q_OS_WIN
        txtStream << "errorfile 503 ha_err_badid.http\n";
        #else
        txtStream << "errorfile 503 "+host+"/ha_err_badid.http\n";
        #endif

        txtStream << "backend b-status\n";
        txtStream << "mode            http\n";
        txtStream << "timeout server  30s\n";
        txtStream << "timeout connect 5s\n";
        //txtStream << "timeout client  30s\n";
        #ifdef Q_OS_WIN
        txtStream << "errorfile 503 ha_info.http\n";
        #else
        txtStream << "errorfile 503 "+host+"/ha_info.http\n";
        #endif

        txtStream << "backend b-stats\n";
        txtStream << "mode            http\n";
        txtStream << "timeout server  30s\n";
        txtStream << "timeout connect 5s\n";
        txtStream << "server Local 127.0.0.1:8181\n";

        txtStream << "listen  stats\n";
        txtStream << "timeout client  30s\n";
        txtStream << "timeout server  30s\n";
        txtStream << "timeout connect 5s\n";
        txtStream << "bind  127.0.0.1:8181\n";
        txtStream << "mode            http\n";
        txtStream << "stats enable\n";
        txtStream << "stats hide-version\n";
        txtStream << "stats refresh 30s\n";
        txtStream << "stats show-node\n";
        txtStream << "stats uri  /stats\n";

        txtStream << "listen provider\n";
        txtStream << "timeout client  30s\n";
        txtStream << "mode            http\n";
        txtStream << "timeout server  30s\n";
        txtStream << "timeout connect 5s\n";
        #ifdef Q_OS_WIN
        txtStream << "errorfile 503 provider.http\n";
        #else
        txtStream << "errorfile 503 "+host+"/provider.http\n";
        #endif

        txtStream << "bind 127.0.0.1:8182\n";

        txtStream.seek(0);
        /*
        while(!txtStream.atEnd()){
            qDebug() << txtStream.readLine();
        }
        */
        file.close();

        QString command = "";
        #ifdef Q_OS_WIN
            command = "haproxy.exe -f haproxy.cfg";
            WinExec(qPrintable(command),SW_HIDE);
        #else
            // try to find haproxy correctly
            QProcess shellProcess;
            shellProcess.start("/bin/sh");
            shellProcess.write("which haproxy || whereis haproxy | cut -d ' ' -f 2");
            shellProcess.closeWriteChannel();
            shellProcess.waitForFinished(-1);
            QString haProxyPath = shellProcess.readAllStandardOutput().trimmed();

            qDebug() << "HAProxy Path " << haProxyPath;

            // ha proxy location not found if output from command is empty or just the first word from whereis
            if (haProxyPath.isEmpty() || haProxyPath == "haproxy:") {
                qDebug() << "HAProxy not found!";
                return false;
            }

            //system("trap 'pkill -f haproxy; echo teste haproxy; exit;' INT TERM");
            command = haProxyPath + " -f " + host +"/haproxy.cfg";
            system(qPrintable(command));
        #endif

        qDebug() << "Starting Haproxy: " << command;

    }
    else {
        qDebug() << "could not open the file";
        return false;
    }
    return true;
}

void Haproxy::haproxyCert(const QString &host, const QString &certificate){
    QFile::remove(host+"/ca.cert.pem");
    QFile file (host+"/ca.cert.pem");

    if(file.open(QIODevice::ReadOnly | QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream txtStream(&file);
        txtStream << certificate;
        txtStream.seek(0);
        /*
        while(!txtStream.atEnd()){
            qDebug() << txtStream.readLine();
        }
        */
        file.close();
    }else{
        qDebug() << "could not open the file";
        return;
    }
}



void Haproxy::killHAproxy(){
	qDebug() << "kill proxy";
    #ifdef Q_OS_WIN
        WinExec("taskkill /f /im haproxy.exe",SW_HIDE);
    #else
        system("pkill -f haproxy");
    #endif
}

// returns true if proxy is online and accepting connections, false otherwise
QString Haproxy::verifyHaproxy(const QString &host, const QString &port, const QString &provider) {
    Thread t;
    return t.start(host.toStdString(), port.toStdString(), provider.toStdString());
}
