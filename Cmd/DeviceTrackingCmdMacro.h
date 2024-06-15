#ifndef DEVICETRACKINGCMDMACRO_H
#define DEVICETRACKINGCMDMACRO_H

#include <memory>
#include <QString>
#include "../Inc/Flash.Mode.Struct.h"
#include "../XML/CmdGenerator/GetSysPropertyCmdXML.h"

class Connection;
class CmdWrapper;

class DeviceTrackingCmdMacro
{
public:
    void execute(const std::shared_ptr<Connection> conn,
             callback_transfer_t cb_trans);

private:
    std::shared_ptr<CmdWrapper> createGetSysPropCmdWrapper(const QString &prop_key) const;
    bool continueNextCmd(const std::shared_ptr<CmdWrapper> get_sys_prop_cmd_wrapper) const;
    bool handleSegCode(int seg_code) const;
    bool handleHRID(const QString &hrid) const;
    QString parseStorageID() const;
    QString transHRID(const QString &hrid) const;

private:
    QString m_da_hw_info;
};

#endif // DEVICETRACKINGCMDMACRO_H
