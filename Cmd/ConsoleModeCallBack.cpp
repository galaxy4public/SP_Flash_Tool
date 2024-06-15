#include "ConsoleModeCallBack.h"
#include "../Logger/Log.h"
#include "../Utility/Utils.h"
#include <cassert>

ConsoleMode::ConsoleModeEntry * ConsoleModeCallBack::m_console_mode_entry = nullptr;

void ConsoleModeCallBack::setupBromCallbackStruct(callbacks_struct_t *cbs)
{
    assert(nullptr != cbs);
    cbs->trans.cb_op_progress = &ConsoleModeCallBack::cb_conn_brom_operation_process;
    fillCallbackStruct(cbs);
}

void ConsoleModeCallBack::setupDACallbackStruct(callbacks_struct_t *cbs)
{
    assert(nullptr != cbs);
    cbs->trans.cb_op_progress = &ConsoleModeCallBack::cb_conn_da_operation_process;
    fillCallbackStruct(cbs);
}

void ConsoleModeCallBack::setupCmdCallbackStruct(callbacks_struct_t *cbs)
{
    assert(nullptr != cbs);
    fillCallbackStruct(cbs);
}

void ConsoleModeCallBack::setupStopCallbackStruct(callback_stop_t *cb_stop)
{
    assert(nullptr != cb_stop);
    cb_stop->_this = nullptr;
    cb_stop->cb_notify_stop = &ConsoleModeCallBack::cb_notify_stop;
}

void ConsoleModeCallBack::setupChksumCallbackStruct(callback_transfer_t *cb_chksum)
{
    assert(nullptr != cb_chksum);
    cb_chksum->_this = nullptr;
    cb_chksum->cb_notify_stop = &ConsoleModeCallBack::cb_notify_stop;
    cb_chksum->cb_op_progress = &ConsoleModeCallBack::cb_scatter_chksum_operation_process;
}

CB_COM_PORT_STATUS_FUNC ConsoleModeCallBack::getCBComportStatus()
{
    return ConsoleModeCallBack::cb_com_port_status;
}

CB_DA_CONNECTED_FUNC ConsoleModeCallBack::getCBDAConnect()
{
    return ConsoleModeCallBack::cb_da_connected;
}

void ConsoleModeCallBack::setConsoleModeEntry(ConsoleMode::ConsoleModeEntry *entry)
{
    m_console_mode_entry = entry;
}

bool ConsoleModeCallBack::cb_notify_stop(void *_this)
{
    Q_UNUSED(_this)
    return false;
}

void ConsoleModeCallBack::cb_conn_brom_operation_process(void *_this, unsigned int progress, unsigned long long data_xferd, const char *info)
{
    Q_UNUSED(_this)
    Q_UNUSED(info)
    static unsigned int lastReportedProgress = 0;

    if (progress - lastReportedProgress >= 5 && progress != lastReportedProgress)
    {
        LOGI("%d%% of BROM data has been sent(about %s)", progress, Utils::PrettyByteString(data_xferd).c_str());
        lastReportedProgress = progress;
    }
}

void ConsoleModeCallBack::cb_conn_da_operation_process(void *_this, unsigned int progress, unsigned long long data_xferd, const char *info)
{
    Q_UNUSED(_this)
    Q_UNUSED(info)

    static unsigned int lastReportedProgress = 0;

    if (progress - lastReportedProgress >= 5 && progress != lastReportedProgress)
    {
        LOGI("%d%% of DA data has been sent(about %s)", progress, Utils::PrettyByteString(data_xferd).c_str());
        lastReportedProgress = progress;
    }
}

void ConsoleModeCallBack::cb_scatter_chksum_operation_process(void *_this, unsigned int progress, unsigned long long data_xferd, const char *info)
{
    Q_UNUSED(_this)
    static unsigned int lastReportedProgress = 0;

    if (progress - lastReportedProgress >= 5 && progress != lastReportedProgress)
    {
        LOGI("%d%% of %s has been calculate checksum value(about %s)", progress, std::string(info).c_str(), Utils::PrettyByteString(data_xferd).c_str());
        lastReportedProgress = progress;
    }
}

void ConsoleModeCallBack::cb_com_port_status(const std::string &friendly_port_name)
{
    LOG("com port friendly name: %s", friendly_port_name.c_str());
}

void ConsoleModeCallBack::cb_da_connected(const QString &da_conn_xml)
{
    Q_ASSERT(nullptr != m_console_mode_entry);

    LOG("DA HW info is:\n%s", qPrintable(da_conn_xml));
}

void ConsoleModeCallBack::fillCallbackStruct(callbacks_struct_t *cbs)
{
    assert(nullptr != cbs);

    cbs->trans._this = nullptr;
    cbs->trans.cb_notify_stop = &ConsoleModeCallBack::cb_notify_stop;

    setupSLACallbackStruct(&(cbs->cb_sla));
}
