#ifndef SIGNATUREVALIDATION_H
#define SIGNATUREVALIDATION_H

#include <QObject>
#include <QDebug>
#include <stdio.h>
#include <QString>
#include <iostream>
#include <string>
#include <algorithm>

#include <vector>
#include <QByteArray>
#include <QBitArray>

#include "../ed25519-donna/ed25519.h"

class SignatureValidation : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE bool verify(QString message, QString signature, QString pulicKey);
    Q_INVOKABLE QString base64_encode(QString message);

};

#endif // SIGNATUREVALIDATION_H
