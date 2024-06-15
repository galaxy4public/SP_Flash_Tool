#include "SecDevFWInfoParser.h"
#include "../XMLConst.h"

const QString &SecDevFWInfoParser::getRandomData() const
{
    return m_random_data;
}

const QString & SecDevFWInfoParser::getHRID() const
{
    return m_hrid;
}

const QString & SecDevFWInfoParser::getSOCID() const
{
    return m_soc_id;
}

bool SecDevFWInfoParser::parseDocument(const QDomElement &root_node)
{
    QString value;
    bool parse_result = parseNodeValue(value, root_node, RND_NODE_NAME);
    if (!parse_result) return parse_result;
    m_random_data = value;
    parse_result = parseNodeValue(value, root_node, HRID_NODE_NAME);
    if (!parse_result) return parse_result;
    m_hrid = value;
    parse_result = parseNodeValue(value, root_node, SOCID_NODE_NAME);
    if (!parse_result) return parse_result;
    m_soc_id = value;
    return parse_result;
}

bool SecDevFWInfoParser::parseNodeValue(QString &tag_value, const QDomElement &root_node, const QString &tag_name)
{
    QDomElement node = root_node.firstChildElement(tag_name);
    if (node.isNull()) {
        m_error_msg = QString("%1 node is not exist.").arg(tag_name);
        return false;
    }
     tag_value = node.text().trimmed();
     return true;
}
