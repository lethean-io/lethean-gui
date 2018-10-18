#ifndef HAPROXY
#define HAPROXY

#include <QObject>
#include <QDebug>
namespace Monero {
    class Haproxy; // forward declaration
}
class Haproxy : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString haproxyPath READ haproxyPath)
    Q_PROPERTY(QString haproxyConfigPath READ haproxyConfigPath)
public:
    //static Haproxy * haproxy();
    //save the path to get the error when is not possible run the haproxy
    QString haproxyPath() const {
        return m_haproxyPath;
    }

    QString haproxyConfigPath() const {
        return m_haproxyConfigPath;
    }

    Q_INVOKABLE bool haproxy(const QString &host, const QString &ip, const QString &port, const QString &endpoint, const QString &endpointport, const QString &fixedHost, const QString &auth, const QString &provider, const QString &plan, const QString &serviceName);
    Q_INVOKABLE void haproxyCert(const QString &host, const QString &certificate);
    Q_INVOKABLE void killHAproxy();

    // returns true if proxy is online and accepting connections, false otherwise
    Q_INVOKABLE QString verifyHaproxy(const QString &host, const QString &ip, const QString &provider);


private:
    QString m_haproxyPath;
    QString m_haproxyConfigPath;
};

#endif // HAPROXY

