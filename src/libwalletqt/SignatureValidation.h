#ifndef SIGNATUREVALIDATION_H
#define SIGNATUREVALIDATION_H

#include <QObject>
#include <QDebug>
#include <stdio.h>
#include <string.h>
#include "../ed25519-donna/ed25519.h"

class SignatureValidation : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE bool verify();

};

#endif // SIGNATUREVALIDATION_H
