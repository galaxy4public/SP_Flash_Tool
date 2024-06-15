#include "DebugUFSCmdXML.h"
#include <QDir>
#include <QHash>
#include "../../Utility/Utils.h"

const QHash<uint16_t, QString> g_Lu3_Type_Hash = {
    {0, "NORMAL"},
    {1, "SYSTEM"},
    {2, "NON-PERSISTENT"},
    {3, "ENHANCED1"},
    {4, "ENHANCED2"},
    {5, "ENHANCED3"},
    {6, "ENHANCED4"}
};

DebugUFSCmdXML::DebugUFSCmdXML(const QString &cmd_version_value):
    ICmdXMLGenerator(cmd_version_value, DEBUG_UFS_CMD_NAME_VAL, DA_NODE_NAME)
{

}

void DebugUFSCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);
    this->createXMLSubNode(arg_node, FUNCTION_NODE_NAME, this->getFunctionFieldVal());
}

UFSUpdateFirmwareCmdXML::UFSUpdateFirmwareCmdXML():
    DebugUFSCmdXML(DEBUG_UFS_UPDATE_FIRMWARE_CMD_VER_VAL),
    m_source_file_assist(new XMLSourceFileAssistant())
{
    m_source_file_assist->setFileType(FILE_TYPE::LOCAL_FILE);
}

QString UFSUpdateFirmwareCmdXML::getFunctionFieldVal() const
{
    return UPDATE_FIRMWARE;
}

void UFSUpdateFirmwareCmdXML::fillArgNode(QDomElement * const arg_node)
{
    DebugUFSCmdXML::fillArgNode(arg_node);
    this->createXMLSubNode(arg_node, SOURCE_FILE_NODE_NAME, QDir::toNativeSeparators(m_source_file_assist->getFileDesc()));
}

void UFSUpdateFirmwareCmdXML::setFileType(FILE_TYPE file_type)
{
    m_source_file_assist->setFileType(file_type);
}

void UFSUpdateFirmwareCmdXML::setFileName(const QString &file_name)
{
    m_source_file_assist->setFileName(file_name);
}

void UFSUpdateFirmwareCmdXML::setMemInfo(const QString &start_address, const QString &length)
{
    m_source_file_assist->setStartAddress(start_address);
    m_source_file_assist->setLength(length);
}

UFSConfigSettingCmdXML::UFSConfigSettingCmdXML():
    DebugUFSCmdXML(DEBUG_UFS_SETTING_CMD_VER_VAL),
    m_wt_size_mb(0),
    m_wt_size_enabled(false),
    m_wt_reduction(true),
    m_hpb_size_mb(0),
    m_hpb_size_enabled(false),
    m_hpb_pinned_index(0),
    m_hpb_pinned_cnt(0),
    m_hpb_pinned_region_enabled(false),
    m_hpb_ctrl_mode("AUTO"),
    m_hpb_ctrl_mode_enabled(false),
    m_lu3_size_mb(0),
    m_lu3_type_index(0),
    m_lu3_enabled(false)
{

}

void UFSConfigSettingCmdXML::setWTSize(uint32_t tw_size)
{
    m_wt_size_mb = tw_size;
}

void UFSConfigSettingCmdXML::setWTSizeEnabled(bool enabled)
{
    m_wt_size_enabled = enabled;
}

void UFSConfigSettingCmdXML::setWTReduction(bool need_reduction)
{
    m_wt_reduction = need_reduction;
}

void UFSConfigSettingCmdXML::setHPBSize(uint32_t hpb_size)
{
    m_hpb_size_mb = hpb_size;
}

void UFSConfigSettingCmdXML::setHPBSizeEnabled(bool enabled)
{
    m_hpb_size_enabled = enabled;
}

void UFSConfigSettingCmdXML::setHPBPinnedIndex(uint16_t hpb_index)
{
    m_hpb_pinned_index = hpb_index;
}

void UFSConfigSettingCmdXML::setHPBPinnedCnt(uint16_t hpb_cnt)
{
    m_hpb_pinned_cnt = hpb_cnt;
}

void UFSConfigSettingCmdXML::setHPBPinnedEnabled(bool enabled)
{
    m_hpb_pinned_region_enabled = enabled;
}

void UFSConfigSettingCmdXML::setHPBCtrlMode(const QString &hbp_ctrl_mode)
{
    m_hpb_ctrl_mode = hbp_ctrl_mode;
}

void UFSConfigSettingCmdXML::setHPBCtrlModeEnabled(bool enabled)
{
    m_hpb_ctrl_mode_enabled = enabled;
}

QString UFSConfigSettingCmdXML::getFunctionFieldVal() const
{
    return SETTING;
}

void UFSConfigSettingCmdXML::setLu3Size(uint32_t lu3_size)
{
    m_lu3_size_mb = lu3_size;
}

void UFSConfigSettingCmdXML::setLu3TypeIndex(uint16_t lu3_type_index)
{
    m_lu3_type_index = lu3_type_index;
}

void UFSConfigSettingCmdXML::setLu3Enabled(bool enabled)
{
    m_lu3_enabled = enabled;
}

void UFSConfigSettingCmdXML::fillArgNode(QDomElement * const arg_node)
{
    DebugUFSCmdXML::fillArgNode(arg_node);
    QDomElement tw_node = this->createXMLSubNode(arg_node, TW_NODE_NAME, QStringLiteral(""));
    QDomElement hpb_node = this->createXMLSubNode(arg_node, HPB_NODE_NAME, QStringLiteral(""));
    QDomElement split_last_lua_node = this->createXMLSubNode(arg_node, SPLIT_LAST_LUA_NODE_NAME, QStringLiteral(""));
    this->createXMLSubNode(&tw_node, SIZE_NODE_NAME, QString("0x%1").arg(m_wt_size_mb, 0, 16));
    this->createXMLSubNode(&tw_node, REDUCTION_NODE_NAME, Utils::transBoolToYESNO(m_wt_reduction));
    this->createXMLSubNode(&hpb_node, SIZE_NODE_NAME, QString("0x%1").arg(m_hpb_size_mb, 0, 16));
    this->createXMLSubNode(&hpb_node, PRSI_NODE_NAME, QString("0x%1").arg(m_hpb_pinned_index, 0, 16));
    this->createXMLSubNode(&hpb_node, PRC_NODE_NAME, QString("0x%1").arg(m_hpb_pinned_cnt, 0, 16));
    this->createXMLSubNode(&hpb_node, HPB_CTRL_MODE_NODE_NAME, m_hpb_ctrl_mode.trimmed().toUpper());
    this->createXMLSubNode(&split_last_lua_node, SIZE_NODE_NAME, QString("0x%1").arg(m_lu3_size_mb, 0, 16));
    this->createXMLSubNode(&split_last_lua_node, TYPE_NODE_NAME, g_Lu3_Type_Hash.value(m_lu3_type_index));
    QMap<QString, QString> switch_attr_map = {
        {WT_SIZE_ATTR_NAME, Utils::transBoolToOnOff(m_wt_size_enabled)},
        {WT_REDUCTION_ATTR_NAME, Utils::transBoolToOnOff(true)},
        {HPB_SIZE_ATTR_NAME, Utils::transBoolToOnOff(m_hpb_size_enabled)},
        {HPB_PINNED_INDEX_ATTR_NAME, Utils::transBoolToOnOff(m_hpb_pinned_region_enabled)},
        {HPB_PINNED_CNT_ATTR_NAME, Utils::transBoolToOnOff(m_hpb_pinned_region_enabled)},
        {HPB_CTRL_MODE_ATTR_NAME, Utils::transBoolToOnOff(m_hpb_ctrl_mode_enabled)},
        {SLL_SIZE_ATTR_NAME, Utils::transBoolToOnOff(m_lu3_enabled)},
        {SLL_TYPE_ATTR_NAME, Utils::transBoolToOnOff(m_lu3_enabled)}
    };
    this->createNodeWithAttrs(arg_node, SWITCH_NODE_NAME, switch_attr_map, QStringLiteral(""));
}
