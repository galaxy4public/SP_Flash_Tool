#include "DeviceTrackingCmdMacro.h"
#include "../../XML/Parser/SysPropsParser.h"
#include "../../Cmd/CmdWrapper.h"
#include "../../Cmd/CmdExecutor.h"
#include "../../Logger/Log.h"
#include "../../Network/NetworkIntf.h"
#include "../../Conn/Connection.h"
#include "../../XML/Parser/GetHWInfoXMLParser.h"

static uint16_t itos_high(uint32_t h) { return ((h & 0xffff0000) >> 16); }
static uint16_t itos_low(uint32_t h) { return (h & 0x0000ffff); }

void DeviceTrackingCmdMacro::execute(const std::shared_ptr<Connection> conn, callback_transfer_t cb_trans)
{
    m_da_hw_info = conn->getDAHWInfo();
    cb_trans.cb_op_progress = &CmdWrapper::cb_operation_process;

    std::shared_ptr<CmdWrapper> get_sys_prop_cmd_wrapper = this->createGetSysPropCmdWrapper(DA_SEG_CODE_VAL_NAME);
    QString cmd_name = get_sys_prop_cmd_wrapper->get_cmd_xml_generator()->getCmdName();
    cmd_name = QString("%1@%2").arg(cmd_name).arg(DA_SEG_CODE_VAL_NAME);
    cb_trans._this = static_cast<void *>(&cmd_name);
    CmdExecutor::execute(conn, get_sys_prop_cmd_wrapper, &cb_trans);
    if (this->continueNextCmd(get_sys_prop_cmd_wrapper)) {
        get_sys_prop_cmd_wrapper = this->createGetSysPropCmdWrapper(DA_HRID_VAL_NAME);
        CmdExecutor::execute(conn, get_sys_prop_cmd_wrapper, &cb_trans);
        continueNextCmd(get_sys_prop_cmd_wrapper);
    }
}

std::shared_ptr<CmdWrapper> DeviceTrackingCmdMacro::createGetSysPropCmdWrapper(const QString &prop_key) const
{
    std::shared_ptr<GetSysPropertyCmdXML> getSysPropCmdXML = std::make_shared<GetSysPropertyCmdXML>();
    getSysPropCmdXML->setKey(prop_key);
    return std::make_shared<CmdWrapper>(getSysPropCmdXML);
}

bool DeviceTrackingCmdMacro::continueNextCmd(const std::shared_ptr<CmdWrapper> get_sys_prop_cmd_wrapper) const
{
    const std::shared_ptr<GetSysPropertyCmdXML> sp_get_sys_prop_cmd_xml =
            std::dynamic_pointer_cast<GetSysPropertyCmdXML>(get_sys_prop_cmd_wrapper->get_cmd_xml_generator());
    assert(sp_get_sys_prop_cmd_xml);
    QString cmd_value = sp_get_sys_prop_cmd_xml->getCmdValue();
    QString prop_name = sp_get_sys_prop_cmd_xml->getKey();
    LOG("get system property[%s] successed, XML: %s", qPrintable(prop_name), qPrintable(cmd_value));

    SysPropsParser sys_prop_parser;
    if (!sys_prop_parser.parseXMLString(cmd_value)) {
        LOGE("ERROR: parse device sys properties error, error msg: %s", qPrintable(sys_prop_parser.get_error_msg()));
        return false;
    }

    if (prop_name == DA_SEG_CODE_VAL_NAME) {
        return this->handleSegCode(sys_prop_parser.getSegmentCode());
    } else if (prop_name == DA_HRID_VAL_NAME) {
        return this->handleHRID(sys_prop_parser.getHRID());
    }
    return false;
}

bool DeviceTrackingCmdMacro::handleHRID(const QString &hrid) const
{
    if (!hrid.isEmpty() && (hrid.compare(QString(hrid.length(), '0')) != 0)) {
        NetworkInft::instance()->postData(this->transHRID(hrid));
    } else {
        QString storage_id = this->parseStorageID();
        Q_ASSERT(!storage_id.isEmpty());
        if (!storage_id.isEmpty()) {
            NetworkInft::instance()->postData(storage_id);
        }
    }
    return false;
}

QString DeviceTrackingCmdMacro::parseStorageID() const
{
    QString storage_id;
    std::shared_ptr<GetHWInfoXMLParser> sp_hw_xml_parser = std::make_shared<GetHWInfoXMLParser>();
    if (sp_hw_xml_parser->parseXMLString(m_da_hw_info)) {
        HWInfoTree::HWInfoTree_ptr hw_info_tree = sp_hw_xml_parser->get_hw_info();
        for (int i = 0; i < hw_info_tree->count(); ++i) {
            HWInfoTree::HWInfoTree_ptr tree_node = hw_info_tree->getChildren(i);
            assert(tree_node);
            if (tree_node->hasChildren()) {
                for (int sub_index = 0; sub_index < tree_node->count(); ++sub_index) {
                    HWInfoTree::HWInfoTree_ptr sub_tree = tree_node->getChildren(sub_index);
                    assert(sub_tree);
                    if (sub_tree->hasChildren()) {
                        continue;
                    }
                    if (sub_tree->getName() == QStringLiteral("id"))
                        storage_id = sub_tree->getValue();
                }
            }
        }
    } else {
        LOG(qPrintable(sp_hw_xml_parser->get_error_msg()));
    }
    return storage_id;
}

QString DeviceTrackingCmdMacro::transHRID(const QString &hrid) const
{
    Q_ASSERT(hrid.length() == 32);
    QByteArray byte_arr = QByteArray::fromHex(hrid.toStdString().data());
    char *data_arr = byte_arr.data();
    int *p_int = reinterpret_cast<int *>(data_arr);
    QString trans_hrid = QString("%1 %2 %3 %4")
            .arg(itos_high(p_int[0]))
            .arg(itos_low(p_int[0]))
            .arg(itos_high(p_int[1]))
            .arg(itos_low(p_int[1]));
    return trans_hrid;
}

bool DeviceTrackingCmdMacro::handleSegCode(int seg_code) const
{
    return seg_code == 0; // segment code == 0 means this is internal device.
}
