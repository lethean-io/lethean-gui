#ifndef SIGNATUREVALIDATION_H
#define SIGNATUREVALIDATION_H

#include <QObject>
#include <QDebug>
#include <stdio.h>
#include <QString>
#include <iostream>
#include <string>
#include <algorithm>

// ned check or remove
#include <vector>
#include <QByteArray>

#include "../ed25519-donna/ed25519.h"

class SignatureValidation : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE bool verify(QString message, QVariantList signature, QString pulicKey);

};

#endif // SIGNATUREVALIDATION_H
