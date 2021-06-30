#include "SignatureValidation.h"

// validate signature from SDP - json
bool SignatureValidation::verify(QString message, QString signature, QString publicKey, int msgLength)
{
    QStringList queryPK = publicKey.split(',');
    unsigned char pk[32];
    int pkInt = 0;
    for (const QString& p : queryPK){
        // transform to uint
        bool convertedPk = false;
        unsigned char ucPk = static_cast<unsigned char>(p.toUInt(&convertedPk, 0));
        if (convertedPk){
            pk[pkInt] = ucPk;
        }
        pkInt++;
    }

    QStringList querySig = signature.split(',');
    unsigned char sig[64];
    int sigInt = 0;
    for (const QString& s : querySig){
        // transform to uint
        bool convertedSig = false;
        unsigned char ucSig = static_cast<unsigned char>(s.toUInt(&convertedSig, 0));
        if (convertedSig){
            sig[sigInt] = ucSig;
        }
        sigInt++;
    }

    QStringList queryMeg = message.split(',');
    unsigned char msg[msgLength];
    int msgInt = 0;
    for (const QString& m : queryMeg){
        // transform to uint
        bool convertedMsg = false;
        unsigned char ucMsg = static_cast<unsigned char>(m.toUInt(&convertedMsg, 0));
        if (convertedMsg){
            msg[msgInt] = ucMsg;
        }
        msgInt++;
    }

    if(ed25519_sign_open(msg, msgLength, pk, sig) == 0) {
        return true;
    };
    return false;
}
