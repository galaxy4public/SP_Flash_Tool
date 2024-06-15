#ifndef SECDEVFWINFOPARSER_H
#define SECDEVFWINFOPARSER_H

#include "IXMLParser.h"

class SecDevFWInfoParser: public IXMLParser
{
public:
    SecDevFWInfoParser() {}
    virtual ~SecDevFWInfoParser() {}

    const QString & getRandomData() const;
    const QString & getHRID() const;
    const QString & getSOCID() const;

protected:
    virtual bool parseDocument(const QDomElement &root_node) override;

private:
    bool parseNodeValue(QString &tag_value, const QDomElement &root_node, const QString &tag_name);

private:
    QString m_random_data;
    QString m_hrid;
    QString m_soc_id;
};

#endif // SECDEVFWINFOPARSER_H
