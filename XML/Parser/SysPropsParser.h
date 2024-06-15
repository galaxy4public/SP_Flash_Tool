#ifndef SYSPROPSPARSER_H
#define SYSPROPSPARSER_H

#include "IXMLParser.h"

class SysPropsParser: public IXMLParser
{
public:
    SysPropsParser();
    virtual ~SysPropsParser() {}

    bool DASLAEnabled() const;
    inline QString getHRID() const { return m_hrid; }
    inline int getSegmentCode() const { return m_segment_code; }

protected:
    virtual bool parseDocument(const QDomElement &root_node) override;

private:
    bool parseItemElement(const QDomElement &item_node);
    bool parseNodeValue(QString &tag_value, const QDomElement &root_node, const QString &tag_name);

private:
    QString m_da_sla_enabled;
    QString m_hrid;
    int m_segment_code;
};

#endif // SYSPROPSPARSER_H
