#ifndef THREAD_H
#define THREAD_H

#include "HTTPResponse.h"

#include <QThread>
#include <QtCore>
#include <QObject>

#include <thread>
#include <string>

class Thread : public QThread
{    
public:
    QString start(std::string host, std::string port, std::string provider);

};

#endif // THREAD_H
