#ifndef WRITEPRIVATECERTCMDXMLGENERATOR_H
#define WRITEPRIVATECERTCMDXMLGENERATOR_H

#include <QSharedPointer>
#include "ICmdXMLGenerator.h"
#include "XMLFileAssistant.h"

class WritePrivateCertCmdXML : public ICmdXMLGenerator
{
public:
    WritePrivateCertCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    virtual void loadXML(const QDomElement &xml_node) Q_DECL_OVERRIDE;
    virtual void saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const Q_DECL_OVERRIDE;

    void setFileType(FILE_TYPE file_type);
    void setFileName(const QString &file_name);
    void setMemInfo(const QString &start_address, const QString &length);

private:
    QSharedPointer<XMLSourceFileAssistant> m_source_file_assist;
};

#endif // WRITEPRIVATECERTCMDXMLGENERATOR_H
