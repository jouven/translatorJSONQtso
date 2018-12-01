#ifndef TRANSLATEJSONQT_LANGUAGE_HPP
#define TRANSLATEJSONQT_LANGUAGE_HPP

#include <QString>
#include <QHash>
#include <QJsonObject>

#include <utility>
#include <vector>

class languageLink_c
{

    //"hardcoded"
    //"eng"
    //"descriptiveeng"
    QString fromLanguage_pri;
    QString toLanguage_pri;
    //can be called items too
    QHash<QString, QString> keyValues_pri;

public:
    //required when initializing reading JSON
    languageLink_c() = default;
    languageLink_c(
            const QString& fromLanguage_pri_con
            , const QString& toLanguage_par_con
            , const QHash<QString, QString>& keyValues_par_con = QHash<QString, QString>()
    );

    void write_f(QJsonObject &json) const;
    void read_f(const QJsonObject &json);

    //non empty keys
    //if the key exists, it will replace the value
    void addUpdatePair_f(const QString& key_par_con, const QString& value_par_con);
    //non empty keys, returns true if a key has been removed
    bool removeKey_f(const QString& key_par_con);
    //this can remove several keys if they use the same value
    int_fast32_t removeValues_f(const QString& value_par_con);

    //non empty keys
    QString keyToValue_f(const QString& key_par_con, bool* found_par_ptr = Q_NULLPTR) const;
    //(slow?) since many keys can have the same values, this can return several keys
    std::vector<QString> valueToKeys_f(const QString& value_par_con) const;

    QList<QString> keys_f() const;

    QString fromLanguage_f() const;
    QString toLanguage_f() const;
};

#endif // TRANSLATEJSONQT_LANGUAGE_HPP

