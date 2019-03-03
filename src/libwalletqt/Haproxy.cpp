#include "Haproxy.h"

bool Haproxy::haproxy(const QString &host, const QString &ip, const QString &port, const QString &endpoint, const QString &endpointport, const QString &fixedHost, const QString &auth, const QString &provider, const QString &plan, const QString &serviceName) {
    // TODO: this should be set at global scope instead of for these individual users of "host"
    // many files are expected to be at "the same level" as the gui executable
    // - for windows: a relative path is ok
    #if defined(Q_OS_WIN)
    const QString sibling_file_path = "";
    #else
    const QString sibling_file_path = host;
    #endif

    qDebug() << "Starting haproxy";

    QFile::remove(sibling_file_path + "provider.http");
    QFile fileProvider(sibling_file_path + "provider.http");

    QFile::remove(sibling_file_path + "ha_credit.http");
    QFile fileCredit(sibling_file_path + "ha_credit.http");

    QFile::remove(sibling_file_path + "ha_err_badid.http");
    QFile fileBadid(sibling_file_path + "ha_err_badid.http");

    QFile::remove(sibling_file_path + "ha_err_connect.http");
    QFile fileConnect(sibling_file_path + "ha_err_connect.http");

    QFile::remove(sibling_file_path + "ha_err_nopayment.http");
    QFile fileNopayment(sibling_file_path + "ha_err_nopayment.http");

    QFile::remove(sibling_file_path + "ha_err_overlimit.http");
    QFile fileOverlimit(sibling_file_path + "ha_err_overlimit.http");

    QFile::remove(sibling_file_path + "ha_info.http");
    QFile fileInfo(sibling_file_path + "ha_info.http");

    // delete old file and create new one
    QFile::remove(sibling_file_path + "haproxy.cfg");
    QFile file(sibling_file_path + "haproxy.cfg");

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

    //create ha_credit.http
    if(fileCredit.open(QIODevice::ReadOnly | QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream txtStream(&fileCredit);

        txtStream << "HTTP/1.0 200 OK\n";
        txtStream << "Cache-Control: no-cache\n";
        txtStream << "Connection: close\n";
        txtStream << "Content-Type: text/html\n";
        txtStream << "<html>\n";

        txtStream << "<body>\n";
        txtStream << "<h1>ITNSVPN credit unavailable yet</h1>\n";
        txtStream << "</body>\n";
        txtStream << "</html>\n";



        txtStream.seek(0);
        /*
        while(!txtStream.atEnd()) {
            qDebug() << txtStream.readLine();
        }
        */
        fileCredit.close();
    }

    //create ha_err_badid.http
    if(fileBadid.open(QIODevice::ReadOnly | QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream txtStream(&fileBadid);

        txtStream << "HTTP/1.0 503 BAD_ID\n";
        txtStream << "Access-Control-Allow-Origin: *\n";
        txtStream << "Access-Control-Allow-Methods: GET\n";
        txtStream << "Access-Control-Allow-Headers: X-ITNS-Status\n";
        txtStream << "Access-Control-Max-Age: 86400\n";

        txtStream << "Cache-Control: no-cache\n";
        txtStream << "Connection: close\n";
        txtStream << "Content-Type: text/html\n";
        txtStream << "X-ITNS-Status: BAD_ID\n";
        txtStream << "BAD_ID\n";

        txtStream.seek(0);
        /*
        while(!txtStream.atEnd()) {
            qDebug() << txtStream.readLine();
        }
        */
        fileBadid.close();
    }

    //create ha_err_connect.http
    if(fileConnect.open(QIODevice::ReadOnly | QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream txtStream(&fileConnect);

        txtStream << "HTTP/1.0 503 CONNECTION_ERROR\n";
        txtStream << "Access-Control-Allow-Origin: *\n";
        txtStream << "Access-Control-Allow-Methods: GET\n";
        txtStream << "Access-Control-Allow-Headers: X-ITNS-Status\n";
        txtStream << "Access-Control-Max-Age: 86400\n";

        txtStream << "Cache-Control: no-cache\n";
        txtStream << "Connection: close\n";
        txtStream << "Content-Type: text/html\n";
        txtStream << "X-ITNS-Status: CONNECTION_ERROR\n";
        txtStream << "CONNECTION_ERROR\n";

        txtStream.seek(0);
        /*
        while(!txtStream.atEnd()) {
            qDebug() << txtStream.readLine();
        }
        */
        fileConnect.close();
    }

    //create ha_err_nopayment.http
    if(fileNopayment.open(QIODevice::ReadOnly | QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream txtStream(&fileNopayment);

        txtStream << "HTTP/1.0 403 Forbidden\n";
        txtStream << "Access-Control-Allow-Origin: *\n";
        txtStream << "Access-Control-Allow-Methods: GET\n";
        txtStream << "Access-Control-Allow-Headers: X-ITNS-Status\n";
        txtStream << "Access-Control-Max-Age: 86400\n";

        txtStream << "Cache-Control: no-cache\n";
        txtStream << "Connection: close\n";
        txtStream << "Content-Type: text/html\n";
        txtStream << "X-ITNS-Status: NO_PAYMENT\n";
        txtStream << "NO_PAYMENT\n";

        txtStream.seek(0);
        /*
        while(!txtStream.atEnd()) {
            qDebug() << txtStream.readLine();
        }
        */
        fileNopayment.close();
    }

    //create ha_err_overlimit.http
    if(fileOverlimit.open(QIODevice::ReadOnly | QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream txtStream(&fileOverlimit);

        txtStream << "HTTP/1.0 429 Too many requests\n";
        txtStream << "Access-Control-Allow-Origin: *\n";
        txtStream << "Access-Control-Allow-Methods: GET\n";
        txtStream << "Access-Control-Allow-Headers: X-ITNS-Status\n";
        txtStream << "Access-Control-Max-Age: 86400\n";

        txtStream << "Cache-Control: no-cache\n";
        txtStream << "Connection: close\n";
        txtStream << "Content-Type: text/html\n";
        txtStream << "X-ITNS-Status: OVERLIMIT\n";
        txtStream << "OVERLIMIT\n";

        txtStream.seek(0);
        /*
        while(!txtStream.atEnd()) {
            qDebug() << txtStream.readLine();
        }
        */
        fileOverlimit.close();
    }

    //create ha_info.http
    if(fileInfo.open(QIODevice::ReadOnly | QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream txtStream(&fileInfo);

        txtStream << "HTTP/1.0 200 OK\n";
        txtStream << "Access-Control-Allow-Origin: *\n";
        txtStream << "Access-Control-Allow-Methods: GET\n";
        txtStream << "Access-Control-Allow-Headers: X-ITNS-Status\n";
        txtStream << "Access-Control-Max-Age: 86400\n";

        txtStream << "Cache-Control: no-cache\n";
        txtStream << "Connection: close\n";
        txtStream << "Content-Type: text/html\n";
        txtStream << "X-ITNS-Status: OK\n";
        txtStream << "OK\n";

        txtStream.seek(0);
        /*
        while(!txtStream.atEnd()) {
            qDebug() << txtStream.readLine();
        }
        */
        fileInfo.close();
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
        txtStream << "server hatls " + endpoint + ":" + endpointport + " force-tlsv12 ssl ca-file '"+sibling_file_path+"ca.cert.pem'\n";
        //save the host variable to show in dashboard
        Haproxy::m_haproxyConfigPath = sibling_file_path;
        #endif

        txtStream << "errorfile 503 " + sibling_file_path + "ha_err_connect.http\n";

        txtStream << "backend b-err\n";
        txtStream << "mode            http\n";
        txtStream << "timeout server  30s\n";
        txtStream << "timeout connect 5s\n";
        //txtStream << "timeout client  30s\n";
        txtStream << "errorfile 503 " + sibling_file_path + "ha_err_badid.http\n";

        txtStream << "backend b-status\n";
        txtStream << "mode            http\n";
        txtStream << "timeout server  30s\n";
        txtStream << "timeout connect 5s\n";
        //txtStream << "timeout client  30s\n";
        txtStream << "errorfile 503 " + sibling_file_path + "ha_info.http\n";

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
        txtStream << "errorfile 503 " + sibling_file_path + "provider.http\n";

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
            QString haProxyPath = NULL;
            QString hasHaproxyExecutable = NULL;

            #if defined(Q_OS_MAC)
                // verify if the haproxy exist in Mac if not send an alert
                QFileInfo check_haproxy_exist_osx("/usr/local/bin/haproxy");
                check_haproxy_exist_osx.refresh();
                if (check_haproxy_exist_osx.exists()) {
                    haProxyPath = "/usr/local/bin/haproxy";
                } else {
                    m_haproxyStatus = "Failed: " + haProxyPath;
                    return false;
                }
            #else
                // try to find haproxy correctly
                QProcess shellProcess;

                // find for haproxy
                shellProcess.start("/bin/sh");
                shellProcess.write("which haproxy || whereis haproxy | cut -d ' ' -f 2");
                shellProcess.closeWriteChannel();
                shellProcess.waitForFinished(-1);
                haProxyPath = shellProcess.readAllStandardOutput().trimmed();

                // when you remove the haproxy from your computer that path still works
                if (haProxyPath == "/etc/haproxy") {
                    qDebug() << "HAProxy has only uninstall path ";
                    return false;
                }

                // verify if has haproxy executable
                command = "[ ! -e " + haProxyPath + " ]; echo $?";
                shellProcess.start("/bin/sh");
                shellProcess.write(qPrintable(command));
                shellProcess.closeWriteChannel();
                shellProcess.waitForFinished(-1);
                hasHaproxyExecutable = shellProcess.readAllStandardOutput().trimmed();

                if (hasHaproxyExecutable != "1") {
                    qDebug() << "HAProxy has no executable ";
                    return false;
                }
            #endif

            qDebug() << "HAProxy Path " << haProxyPath;

            // save in haproxy variable the path to show in dashboard
            Haproxy::m_haproxyPath = haProxyPath;

            // ha proxy location not found if output from command is empty or just the first word from whereis
            if (haProxyPath.isEmpty() || haProxyPath == "haproxy:") {
                qDebug() << "HAProxy not found!";
                 m_haproxyStatus = "NotFound: " + haProxyPath;
                return false;
            }

            //system("trap 'pkill -f haproxy; echo teste haproxy; exit;' INT TERM");
            command = haProxyPath + " -f " + sibling_file_path + "haproxy.cfg";
            system(qPrintable(command));
        #endif

        qDebug() << "Starting Haproxy: " << command;

    }
    else {
        m_haproxyStatus = "Failed to open (" + QString::number(file.error()) + ") " + sibling_file_path + "haproxy.cfg";
        qDebug() << "could not open the file";
        return false;
    }
    return true;
}

void Haproxy::haproxyCert(const QString &host, const QString &certificate){
    // TODO: this should be set at global scope instead of for these individual users of "host"
    // many files are expected to be at "the same level" as the gui executable
    // - for windows: a relative path is ok
    #if defined(Q_OS_WIN)
    const QString sibling_file_path = "";
    #else
    const QString sibling_file_path = host;
    #endif

    QFile::remove(sibling_file_path + "ca.cert.pem");
    QFile file (sibling_file_path + "ca.cert.pem");

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
    m_haproxyStatus = "";
    #ifdef Q_OS_WIN
        WinExec("taskkill /f /im haproxy.exe",SW_HIDE);
    #else
        system("pkill -f haproxy");
    #endif
}

// returns true if proxy is online and accepting connections, false otherwise
void Haproxy::verifyHaproxy(const QString &host, const QString &port, const QString &provider) {
    ThreadVerifyHaproxy *workerThread = new ThreadVerifyHaproxy();
    workerThread->setup(host.toStdString(), port.toStdString(), provider.toStdString());
    connect(workerThread, &ThreadVerifyHaproxy::resultReady, this, &Haproxy::handleResults);
    connect(workerThread, &ThreadVerifyHaproxy::finished, workerThread, &QObject::deleteLater);
    workerThread->start();
}

void Haproxy::handleResults(const QString haproxyStatus) {
     m_haproxyStatus = haproxyStatus;
}
