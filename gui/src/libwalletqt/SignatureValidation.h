#ifndef SIGNATUREVALIDATION_H
#define SIGNATUREVALIDATION_H

#include <QObject>
#include <QDebug>
#include <QString>
#include <string>

#include "../ed25519-donna/ed25519.h"

// validate signature from SDP - json
class SignatureValidation : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE bool verify(QString message, QString signature, QString pulicKey, int msgLength);

};

#endif // SIGNATUREVALIDATION_H
