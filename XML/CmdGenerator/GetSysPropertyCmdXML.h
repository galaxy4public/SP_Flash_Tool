#ifndef GETSYSPROPERTYCMDXML_H
#define GETSYSPROPERTYCMDXML_H

#include <QSharedPointer>
#include "ICmdXMLGenerator.h"
#include "XMLFileAssistant.h"

class GetSysPropertyCmdXML : public ICmdXMLGenerator
{
public:
    GetSysPropertyCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    QString getCmdValue() const;

    virtual QString getCmdName() const override;

    inline QString getKey() const {return m_key; }
    void setKey(const QString &key);

private:
    QSharedPointer<XMLTargetFileAssistant> m_target_file_assist;
    QString m_key;
};

#endif // GETSYSPROPERTYCMDXML_H
