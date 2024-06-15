#ifndef DASLACMDHANDLER_H
#define DASLACMDHANDLER_H

#include <QString>
#include <memory>
#include "../Inc/CFlashToolTypes.h"

class ICmdXMLGenerator;
class Connection;
class CmdWrapper;

class DASLACmdHandler
{
public:
    DASLACmdHandler() {}
    virtual ~DASLACmdHandler() {}

    void DASLAVerify(const std::shared_ptr<Connection> conn);

private:
    std::shared_ptr<CmdWrapper> createGetDevFWInfoCmd() const;
    std::shared_ptr<CmdWrapper> createSetFlashPolicyCmd(unsigned char *p_challenge_out_data,
                                                        unsigned int challenge_out_data) const;
    std::shared_ptr<CmdWrapper> createGetSysPropertyCmd() const;
    bool checkDASLAEnabled(const std::shared_ptr<Connection> conn) const;
    QByteArray transHexStrToHexByteArr(const QString &hex_str) const;
};

#endif // DASLACMDHANDLER_H
