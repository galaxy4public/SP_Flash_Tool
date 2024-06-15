#ifndef CONSOLEMODENETWORKINTF_H
#define CONSOLEMODENETWORKINTF_H

#include <QSharedPointer>
#include "../../Inc/Flash.Mode.Struct.h"

class NetworkThread;
class Connection;

class ConsoleModeNetworkIntf: public QObject
{
    Q_OBJECT

public:
    ConsoleModeNetworkIntf();
    virtual ~ConsoleModeNetworkIntf();

public:
    void postData(std::shared_ptr<Connection> conn, callback_transfer_t cb_trans) const;

    void setFlashXMLFile(const QString &flash_xml_file);

    void startNetworkThread();

private:
    void waitNetworkThreadFinished(bool force_terminate) const;
    bool network_online() const;

private:
    QString m_flash_xml_file;
    QSharedPointer<NetworkThread> m_network_thread;
};

#endif // CONSOLEMODENETWORKINTF_H
