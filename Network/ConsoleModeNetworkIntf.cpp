#include "ConsoleModeNetworkIntf.h"
#include "NetworkThread.h"
#include "NetworkIntf.h"
#include "../../Logger/Log.h"
#include "../../XML/Parser/FlashXMLParser.h"
#include "../../Cmd/DeviceTrackingCmdMacro.h"
#include "../../Utility/Utils.h"

ConsoleModeNetworkIntf::ConsoleModeNetworkIntf():
    m_network_thread(new NetworkThread(QString()))
{

}

ConsoleModeNetworkIntf::~ConsoleModeNetworkIntf()
{
    waitNetworkThreadFinished(true);
}

void ConsoleModeNetworkIntf::postData(std::shared_ptr<Connection> conn, callback_transfer_t cb_trans) const
{
    if (Utils::isSupportDevTracking(m_flash_xml_file) && this->network_online()) {
        NetworkInft::instance()->setNetOnline(true);
        DeviceTrackingCmdMacro dev_tracking_cmd;
        dev_tracking_cmd.execute(conn, cb_trans);
    }
}

void ConsoleModeNetworkIntf::setFlashXMLFile(const QString &flash_xml_file)
{
    m_flash_xml_file = flash_xml_file;
}

void ConsoleModeNetworkIntf::waitNetworkThreadFinished(bool force_terminate) const
{
    if (m_network_thread && m_network_thread->isRunning()) {
        if (force_terminate) m_network_thread->terminate();
        m_network_thread->wait();
    }
}

void ConsoleModeNetworkIntf::startNetworkThread()
{
    if(Utils::isSupportDevTracking(m_flash_xml_file)) {
        LOGI("start to connect network!");
        m_network_thread->start();
    }
}

bool ConsoleModeNetworkIntf::network_online() const
{
    waitNetworkThreadFinished(false);
    return m_network_thread->network_online();
}
