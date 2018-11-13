#ifndef THREAD_H
#define THREAD_H

#include "HTTPResponse.h"
#include <Wallet.h>
#include "PendingTransaction.h" // we need to have an access to the PendingTransaction::Priority enum here;
#include <QtConcurrent/QtConcurrent>
#include "wallet/wallet2_api.h" // we need to have an access to the Monero::Wallet::Status enum here;

#include <QThread>
#include <QtCore>
#include <QObject>

#include <thread>
#include <string>

// reference to wallet class otherwise we won't be able to compile due to lack of class name reference
class Wallet;

// Class to make non blocking threaded request
class Thread : public QThread
{    
public:
    // creates a thread for validating haproxy payment status and connectivity
    // TODO - rename this to a more user friendly method name
    QString start(std::string host, std::string port, std::string provider);

    // creates an automatic payment within a thread to avoid UI freeze
    void makeAutomatedThreadedPayment(Wallet *wallet, const QString &dst_addr, const QString &payment_id,
                       quint64 amount, quint32 mixin_count, PendingTransaction::Priority priority);
};

#endif // THREAD_H
