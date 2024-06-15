#ifndef NETWORKINTF_H
#define NETWORKINTF_H

#include <QObject>
#include <QSharedPointer>
#include <atomic>

class NetworkInft: public QObject
{
    Q_OBJECT

public:
    enum network_con_changed_src: unsigned char {
      nccs_try_connect = 0,
      nccs_try_connect_show,
      nccs_post_data
    };

public:
    static QSharedPointer<NetworkInft> instance();

    bool network_online() const;
    void tryConnect();
    void tryConnectShow();
    void postData(const QString &device_id);
    void setNetOnline(bool online);

signals:
    void signal_net_connection_changed(network_con_changed_src con_changed_src);

private slots:
    void slot_network_connected(bool connected);
    void slot_net_connected_show(bool connected);
    void slot_net_posted(bool posted);

private:
    NetworkInft();
    NetworkInft(const NetworkInft &) = delete;
    NetworkInft(const NetworkInft &&) = delete;
    NetworkInft & operator=(const NetworkInft &) = delete;
    NetworkInft & operator=(const NetworkInft &&) = delete;

private:
    static QSharedPointer<NetworkInft> m_instance;

    std::atomic<bool> m_network_online;
};

#endif // NETWORKINTF_H
