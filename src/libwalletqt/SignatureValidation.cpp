#include "SignatureValidation.h"

bool SignatureValidation::verify(QString message, QVariantList signature, QString publicKey)
{

    qDebug() << publicKey.length();
    qDebug() << publicKey;
    qDebug() << "enter signature verify";

    // convert QString into std:string
    std::string msgUTF8 = message.toUtf8().constData();
    std::vector<char> msgToChars{ begin(msgUTF8), end(msgUTF8) };

    std::vector<unsigned char> msgUChars;
    transform(begin(msgUTF8), end(msgUTF8), back_inserter(msgUChars),
    [](char msgC) { return static_cast<unsigned char>(msgC); });

    std::string pkeyUTF = publicKey.toUtf8().constData();

    std::vector<char> pekyToChars{ begin(pkeyUTF), end(pkeyUTF) };

    std::vector<unsigned char> pkeyUChars;
    transform(begin(pkeyUTF), end(pkeyUTF), back_inserter(pkeyUChars),
    [](char pkeyC) { return static_cast<unsigned char>(pkeyC); });

    // oficial keys
    ed25519_public_key pk = {0x46,0x9e,0x78,0x72,0x70,0xaf,0x46,0x4c,0xc7,0xd6,0x41,0xcf,0x48,0xe3,0xa0,0xf4,0x85,0x71,0xff,0x2c,0xc3,0xfa,0x45,0x79,0x32,0x96,0xc0,0x9c,0x1e,0xd1,0x3e,0xbc,};
    ed25519_signature sig = {0xD3,0x6C,0xE4,0x24,0xE3,0xCE,0xE6,0x86,0x25,0x18,0xDF,0xFE,0x36,0x62,0x28,0xC5,0xD0,0x77,0xFC,0xD5,0xF6,0x5F,0x33,0x0F,0x04,0x41,0xDE,0xCB,0x3F,0xDC,0x3F,0xC9,0xFD,0x21,0xBA,0x33,0xD5,0x8B,0xB9,0x8D,0xCA,0x76,0xF8,0x3F,0x17,0x14,0xCD,0xA0,0x8B,0x5A,0x3F,0x01,0x45,0x9B,0x90,0x5F,0xCC,0x29,0x10,0xFC,0xFD,0xD2,0x7A,0x0F,};

    qDebug() << pk[0];


    //if(ed25519_sign_open(msgUChars.data(), message.length(), ppk, sig) == 0) {

    //ed25519_public_key pk = {(unsigned char)*pekyToChars.data()};
    //pkeyUChars.data();
    //strcpy((unsigned char*)pk, pkeyUChars.data());

    // its close the wallet
    //std::copy(begin(pkeyUChars), end(pkeyUChars), pk);

    //memcpy( ppk, pkeyString.c_str() ,pkeyString.size());

    qDebug() << pk[0];

    if(ed25519_sign_open(msgUChars.data(), message.length(), pk, sig) == 0) {
        qDebug() << "valid signature";
        return true;
    };

    qDebug() << "end false";
    return false;
}
