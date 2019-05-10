#include "languageLink.hpp"

#include <QJsonArray>

#ifdef DEBUGJOUVEN
#include <QDebug>
#endif

languageLink_c::languageLink_c(
        const QString& fromLanguage_pri_con
        , const QString& toLanguage_par_con
        , const QHash<QString, QString>& keyValues_par_con)
    : fromLanguage_pri(fromLanguage_pri_con)
    , toLanguage_pri(toLanguage_par_con)
    , keyValues_pri(keyValues_par_con)
{}

void languageLink_c::write_f(QJsonObject& json) const
{
    json["fromLanguage"] = fromLanguage_pri;
    json["toLanguage"] = toLanguage_pri;
    QJsonObject itemsJSONObjectTmp;
    QHash<QString, QString>::const_iterator iteTemp = keyValues_pri.constBegin();
    while (iteTemp != keyValues_pri.constEnd())
    {
        itemsJSONObjectTmp[iteTemp.key()] = iteTemp.value();
        ++iteTemp;
    }
    json["items"] = itemsJSONObjectTmp;
}

void languageLink_c::read_f(const QJsonObject& json)
{
    fromLanguage_pri = json["fromLanguage"].toString();
    toLanguage_pri = json["toLanguage"].toString();
    QJsonObject itemsJSONObjectTmp(json["items"].toObject());
    for (const QString& key_ite_con : itemsJSONObjectTmp.keys())
    {
        if (key_ite_con.isEmpty())
        {
        }
        else
        {
            keyValues_pri.insert(key_ite_con, itemsJSONObjectTmp.value(key_ite_con).toString());
        }
    }
}

void languageLink_c::addUpdatePair_f(
        const QString& key_par_con
        , const QString& value_par_con)
{
    if (not key_par_con.isEmpty())
    {
        keyValues_pri.insert(key_par_con, value_par_con);
    }

}

bool languageLink_c::removeKey_f(const QString& key_par_con)
{
    bool resultTmp(false);
    if (key_par_con.isEmpty())
    {
    }
    else
    {
        resultTmp = keyValues_pri.remove(key_par_con) > 0;
    }
    return resultTmp;
}

int_fast32_t languageLink_c::removeValues_f(const QString& value_par_con)
{
    int_fast32_t countTmp(0);
    QString keyTmp(keyValues_pri.key(value_par_con));
    if (keyTmp.isEmpty())
    {

    }
    else
    {
        countTmp = keyValues_pri.remove(keyTmp);
    }
    return countTmp;
}

QString languageLink_c::keyToValue_f(
        const QString& key_par_con
        , bool* found_par_ptr) const
{
    QString valueTmp;
    QHash<QString, QString>::const_iterator findResultTmp(keyValues_pri.find(key_par_con));
    bool foundTmp(findResultTmp not_eq keyValues_pri.constEnd());
    if (foundTmp)
    {
        valueTmp = findResultTmp.value();
    }
    else
    {

    }
    if (found_par_ptr not_eq Q_NULLPTR)
    {
        *found_par_ptr = foundTmp;
    }
    return valueTmp;
}

std::vector<QString> languageLink_c::valueToKeys_f(const QString& value_par_con) const
{
    std::vector<QString> resultTmp;
    while (true)
    {
        QString keyTmp(keyValues_pri.key(value_par_con));
        if (keyTmp.isEmpty())
        {
            break;
        }
        else
        {
            resultTmp.emplace_back(keyTmp);
        }
        //no need for break
    }
    return resultTmp;
}

QList<QString> languageLink_c::keys_f() const
{
    return keyValues_pri.keys();
}

QString languageLink_c::fromLanguage_f() const
{
    return fromLanguage_pri;
}

QString languageLink_c::toLanguage_f() const
{
    return toLanguage_pri;
}
