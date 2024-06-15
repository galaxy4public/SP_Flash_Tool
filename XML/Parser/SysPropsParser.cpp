#include "SysPropsParser.h"
#include "../XMLConst.h"
#include <limits>

SysPropsParser::SysPropsParser():
    m_segment_code(std::numeric_limits<int>::max())
{

}

bool SysPropsParser::DASLAEnabled() const
{
    return m_da_sla_enabled.toUpper() == QStringLiteral("ENABLED");
}

bool SysPropsParser::parseDocument(const QDomElement &root_node)
{
    QDomElement element = root_node.firstChildElement();
    while (!element.isNull()) {
        QString tag_name = element.tagName().trimmed();
        if (tag_name != ITEM_ATTR_NAME) {
            element = element.nextSiblingElement();
            continue;
        }
        this->parseItemElement(element);
        element = element.nextSiblingElement();
    }
    return true;
}

bool SysPropsParser::parseItemElement(const QDomElement &item_node)
{
    QDomElement current_element = item_node;
    while (!current_element.isNull()) {
        QString key_attr_value = current_element.attribute(KEY_ATTR_NAME);
        if (key_attr_value == DA_SLA_VAL_NAME) {
            m_da_sla_enabled = current_element.text().trimmed();
        } else if (key_attr_value == DA_HRID_VAL_NAME) {
            m_hrid = current_element.text().trimmed();
        } else if (key_attr_value == DA_SEG_CODE_VAL_NAME) {
            QString segment_code = current_element.text().trimmed();
            bool ok;
            m_segment_code = segment_code.toInt(&ok, 0);
            if (!ok) m_segment_code = std::numeric_limits<int>::max();
        }
        current_element = current_element.nextSiblingElement();
    }
    return true;
}
