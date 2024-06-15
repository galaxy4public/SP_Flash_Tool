#include "DASLACmdHandler.h"
#include <exception>
#include <QLibrary>
#include "../Conn/Connection.h"
#include "../Cmd/CmdExecutor.h"
#include "../Cmd/CmdWrapper.h"
#include "../XML/CmdGenerator/ICmdXMLGenerator.h"
#include "../XML/CmdGenerator/SecGetDevFWInfoCmdXML.h"
#include "../XML/CmdGenerator/SecSetFlashPolicyCmdXML.h"
#include "../XML/CmdGenerator/GetSysPropertyCmdXML.h"
#include "../XML/Parser/SysPropsParser.h"
#include "../XML/Parser/SecDevFWInfoParser.h"
#include "../Inc/SLA_Challenge.h"
#include "../Err/Exception.h"
#include "../Err/CFlashToolErrorCode.h"
#include "../Logger/Log.h"

#define SLA_RANDOM_LENGTH 16
#define SLA_HRID_LENGTH 16
#define SLA_SOCID_LENGTH 32

#ifdef _WIN32
typedef int (__stdcall *CB_SLA_CHALLENGE_CONFIG)();
typedef int (__stdcall *CB_DA_SLA_Challenge)(void *usr_arg, const
                                             unsigned char  *p_challenge_in,
                                             unsigned int  challenge_in_len,
                                             unsigned char  **pp_challenge_out,
                                             unsigned int  *p_challenge_out_len);
typedef int (__stdcall *CB_DA_SLA_Challenge_END)(void *usr_arg, unsigned char  *p_challenge_out);
#else
typedef int (*CB_SLA_CHALLENGE_CONFIG)();
typedef int (*CB_DA_SLA_Challenge)(void *usr_arg, const
                                             unsigned char  *p_challenge_in,
                                             unsigned int  challenge_in_len,
                                             unsigned char  **pp_challenge_out,
                                             unsigned int  *p_challenge_out_len);
typedef int (*CB_DA_SLA_Challenge_END)(void *usr_arg, unsigned char  *p_challenge_out);
#endif

/*
static QString to_string(unsigned char *data, unsigned int length)
{
    QChar a('0');
    QString ss = QString("Jiming: 0x%1 Hex[").arg((unsigned int)length, 8, 16, a);
    for (unsigned int i = 0; i < length; ++i) {
        ss += QString("%1 ").arg((unsigned char)data[i], 2, 16, a);
    }
    ss += QString("]");
    return ss;
}
*/

void DASLACmdHandler::DASLAVerify(const std::shared_ptr<Connection> conn)
{
    try {
        if (!this->checkDASLAEnabled(conn)) {
            LOGW("Warning: DA SLA Disabled, skip it!");
            return ;
        }

        std::shared_ptr<CmdWrapper> get_dev_fw_info_cmd = this->createGetDevFWInfoCmd();
        CmdExecutor::execute(conn, get_dev_fw_info_cmd, nullptr);
        std::shared_ptr<SecGetDevFWInfoCmdXML> secGetDevFWInfoCmd =
                std::dynamic_pointer_cast<SecGetDevFWInfoCmdXML>(get_dev_fw_info_cmd->get_cmd_xml_generator());
        assert(secGetDevFWInfoCmd);
        QString challenge_in_data = secGetDevFWInfoCmd->getCmdXMLValue();
        SecDevFWInfoParser secDevFWInfoParser;
        if (!secDevFWInfoParser.parseXMLString(challenge_in_data)) {
            LOGE("ERROR: parse sla dev fw info error, error msg: %s", qPrintable(secDevFWInfoParser.get_error_msg()));
            THROW_APP_EXCEPTION(-1, secDevFWInfoParser.get_error_msg());
        }

        QString library_name("SLA_Challenge");
#ifndef _WIN32
        library_name = library_name.toLower();
#endif
        SLA_Feature sla_feature = FEATURE_NONE;
        CB_SLA_CHALLENGE_CONFIG sla_feature_func = (CB_SLA_CHALLENGE_CONFIG)QLibrary::resolve(library_name, "SLA_Feature_Config");
        if (sla_feature_func) {
            sla_feature = (SLA_Feature)sla_feature_func();
        } else {
            LOGE("NO SLA API SLA_Feature_Config.");
        }

        unsigned int challenge_in_data_offset = (sla_feature == FEATURE_HRID) ? SLA_HRID_LENGTH :
                                                (sla_feature == FEATURE_SOCID) ? SLA_SOCID_LENGTH : 0;
        unsigned int challenge_in_data_len = SLA_RANDOM_LENGTH + challenge_in_data_offset;
        unsigned char *p_challenge_in_data = (unsigned char *)malloc(challenge_in_data_len);
        if (NULL == p_challenge_in_data) {
            LOGE("ERROR: insufficient memory for sla data.");
            THROW_APP_EXCEPTION(-1, QStringLiteral("insufficient memory for sla data."))
        }
        memset(p_challenge_in_data, 0, challenge_in_data_len);
        if (sla_feature == FEATURE_HRID) {
            QByteArray hrid_data_arr = this->transHexStrToHexByteArr(secDevFWInfoParser.getHRID());
            memcpy(p_challenge_in_data, hrid_data_arr.data(), challenge_in_data_offset);
        } else if (sla_feature == FEATURE_SOCID) {
            QByteArray socid_data_arr = this->transHexStrToHexByteArr(secDevFWInfoParser.getSOCID());
            memcpy(p_challenge_in_data, socid_data_arr.data(), challenge_in_data_offset);
        }
        QByteArray random_data_arr = this->transHexStrToHexByteArr(secDevFWInfoParser.getRandomData());
        memcpy(p_challenge_in_data + challenge_in_data_offset, random_data_arr.data(), SLA_RANDOM_LENGTH);

        unsigned int challenge_data_out_len = 0;
        unsigned char *p_challenge_out_data = nullptr;

        CB_DA_SLA_Challenge da_sla_challenge_func = (CB_DA_SLA_Challenge)QLibrary::resolve(library_name, "DA_SLA_Challenge");
        if (!da_sla_challenge_func) {
            LOGE("NO SLA API DA_SLA_Challenge.");
            free(p_challenge_in_data);
            THROW_APP_EXCEPTION(-1, QStringLiteral("NO SLA API DA_SLA_Challenge."))
        }
        int status = da_sla_challenge_func(NULL, p_challenge_in_data, challenge_in_data_len, &p_challenge_out_data, &challenge_data_out_len);
        free(p_challenge_in_data);
        if(0 != status)
        {
            LOGW("Warning: call SLA_Challenge function failed, error code is %d.", status);
            p_challenge_out_data = (unsigned char *)"SLA";
            challenge_data_out_len = 4;
            //THROW_APP_EXCEPTION(status, QStringLiteral("call SLA_Challenge function failed."));
        }
        CmdExecutor::execute(conn, this->createSetFlashPolicyCmd(p_challenge_out_data, challenge_data_out_len), nullptr);
        CB_DA_SLA_Challenge_END da_sla_challenge_end_func = (CB_DA_SLA_Challenge_END)QLibrary::resolve(library_name, "DA_SLA_Challenge_END");
        if (da_sla_challenge_end_func) {
            da_sla_challenge_end_func(nullptr, 0);
        } else {
            LOGE("NO SLA API DA_SLA_Challenge_END");
            THROW_APP_EXCEPTION(-1, QStringLiteral("NO SLA API DA_SLA_Challenge_END."))
        }
    } catch (BaseException &e) {
        int error_code = e.err_code();
        if (error_code != E_UNSUPPORTED) {
            std::rethrow_exception(std::current_exception());
        }
    } catch (...) {
        std::rethrow_exception(std::current_exception());
    }
}

std::shared_ptr<CmdWrapper> DASLACmdHandler::createGetDevFWInfoCmd() const
{
    std::shared_ptr<SecGetDevFWInfoCmdXML> secGetDevFWInfoCmdXML = std::make_shared<SecGetDevFWInfoCmdXML>();
    secGetDevFWInfoCmdXML->setFileType(FILE_TYPE::MEMORY_FILE);
    std::shared_ptr<CmdWrapper> cmd_wrapper = std::make_shared<CmdWrapper>(secGetDevFWInfoCmdXML);
    return cmd_wrapper;
}

std::shared_ptr<CmdWrapper> DASLACmdHandler::createSetFlashPolicyCmd(unsigned char *p_challenge_out_data,
                                                                     unsigned int challenge_out_data) const
{
    Q_ASSERT(nullptr != p_challenge_out_data);
    std::shared_ptr<SecSetFlashPolicyCmdXML> secSetFlashPolicyCmdXML = std::make_shared<SecSetFlashPolicyCmdXML>();
    secSetFlashPolicyCmdXML->setFileType(FILE_TYPE::MEMORY_FILE);
    quint64 address = reinterpret_cast<quint64>(p_challenge_out_data);
    QString start_address = QString("%1").arg(address, 0, 16);
    QString length = QString("%1").arg(challenge_out_data, 0, 16);
    secSetFlashPolicyCmdXML->setMemInfo(start_address, length);
    std::shared_ptr<CmdWrapper> cmd_wrapper = std::make_shared<CmdWrapper>(secSetFlashPolicyCmdXML);
    return cmd_wrapper;
}

std::shared_ptr<CmdWrapper> DASLACmdHandler::createGetSysPropertyCmd() const
{
    std::shared_ptr<GetSysPropertyCmdXML> getSysPropertyCmdXML = std::make_shared<GetSysPropertyCmdXML>();
    getSysPropertyCmdXML->setKey(DA_SLA_VAL_NAME);
    return std::make_shared<CmdWrapper>(getSysPropertyCmdXML);
}

bool DASLACmdHandler::checkDASLAEnabled(const std::shared_ptr<Connection> conn) const
{
    std::shared_ptr<CmdWrapper> get_sys_prop_cmd_wrapper = this->createGetSysPropertyCmd();
    CmdExecutor::execute(conn, get_sys_prop_cmd_wrapper, nullptr);
    std::shared_ptr<GetSysPropertyCmdXML> get_sys_prop_cmd =
            std::dynamic_pointer_cast<GetSysPropertyCmdXML>(get_sys_prop_cmd_wrapper->get_cmd_xml_generator());
    assert(get_sys_prop_cmd);
    QString sys_prop_values = get_sys_prop_cmd->getCmdValue();
    SysPropsParser sys_prop_parser;
    if (!sys_prop_parser.parseXMLString(sys_prop_values)) {
        LOGE("ERROR: parse device sys properties error, error msg: %s", qPrintable(sys_prop_parser.get_error_msg()));
        THROW_APP_EXCEPTION(-1, sys_prop_parser.get_error_msg());
    }
    bool da_enabled = sys_prop_parser.DASLAEnabled();
    LOGI("DA SLA enabled status: %s.", da_enabled ? "Enabled" : "Disabled");
    return da_enabled;
}

QByteArray DASLACmdHandler::transHexStrToHexByteArr(const QString &hex_str) const
{
    return QByteArray::fromHex(hex_str.toStdString().data());
}
