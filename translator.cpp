#include "translator.hpp"

#include "textQtso/text.hpp"

#include "essentialQtso/macros.hpp"

#include "comuso/practicalTemplates.hpp"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMap>

#ifdef DEBUGJOUVEN
#include <QDebug>
#endif


std::vector<QString> translator_c::lastLoadedJSONFilePaths_f() const
{
    return languageJSONFilePaths_pri;
}

//void translator_c::setJSONFilePaths_f(const std::vector<QString>& JSONFilePaths_par_con)
//{
//    languageJSONFilePaths_pri = JSONFilePaths_par_con;
//}

bool translator_c::isConfigSet_f() const
{
    return not translateFromLanguage_pri.isEmpty()
            and not translateToLanguageChain_pri.empty()
            and not languageLinks_pri.empty();
}

std::vector<std::pair<QString, QString> > translator_c::translationFromToPairs_f() const
{
    std::vector<std::pair<QString, QString>> resultTmp;
    resultTmp.reserve(languageLinks_pri.size());
    for (const languageLink_c& languageLink_ite_con : languageLinks_pri)
    {
        resultTmp.emplace_back(std::make_pair(languageLink_ite_con.fromLanguage_f(), languageLink_ite_con.toLanguage_f()));
    }
    return resultTmp;
}

QString translator_c::translateFromLanguage_f() const
{
    return translateFromLanguage_pri;
}

bool translator_c::setTranslateFromLanguage_f(const QString& translateFromLanguage_par_con, textCompilation_c* errorCompilationPtr_par)
{
    bool resultTmp(false);
    while (true)
    {
        if (translateFromLanguage_par_con.isEmpty())
        {
            APPENDTEXTPTR(errorCompilationPtr_par, "Translate from language empty");
            break;
        }

        resultTmp = languageLinksHasLanguageFrom_f(translateFromLanguage_par_con);
        if (resultTmp)
        {
            translateFromLanguage_pri = translateFromLanguage_par_con;
        }
        else
        {
            text_c errorMessageTmp("No language link that can translate from this language {0}", translateFromLanguage_par_con);
            APPENDTEXTPTR(errorCompilationPtr_par, errorMessageTmp);
        }
        break;
    }
    return resultTmp;
}

void translator_c::readLanguagesJSONFiles_f(
        const std::vector<QString>& languageJSONFilePaths_par_con
)
{
    if (not languageJSONFilePaths_par_con.empty())
    {
        std::vector<QString> successfulFilesTmp;
        for (const QString& JSONFile_ite_con : languageJSONFilePaths_par_con)
        {
            QFile configFileLoad(JSONFile_ite_con);
            QByteArray jsonByteArray;
            if (configFileLoad.open(QIODevice::ReadOnly))
            {
                jsonByteArray = configFileLoad.readAll();
            }
            else
            {
                continue;
            }


            QJsonDocument jsonDocObj(QJsonDocument::fromJson(jsonByteArray));
            if (jsonDocObj.isNull())
            {
                continue;
            }
            else
            {
                if (readLanguagesJSON_f(jsonDocObj.object(), true) > 0)
                {
                    successfulFilesTmp.emplace_back(JSONFile_ite_con);
                }
            }
        }
        languageJSONFilePaths_pri = successfulFilesTmp;
    }
}

bool translator_c::writeConfigJSONFile_f(
        const QString& filePath_par_con
        , const bool writeLanguagesToo_par_con
        , QString* errorStrPtr_par)
{
    bool configSavedTmp(false);

    QJsonObject baseJSONTmp;

    writeConfigJSON_f(baseJSONTmp);
    if (writeLanguagesToo_par_con)
    {
        writeLanguagesJSON_f(baseJSONTmp);
    }

    QJsonDocument jsonDocumentTmp(baseJSONTmp);
    QByteArray jsonByteArrayTmp(jsonDocumentTmp.toJson(QJsonDocument::Indented));
    QFile configFileSaveTmp(filePath_par_con);
    if (configFileSaveTmp.open(QIODevice::WriteOnly))
    {
        if (configFileSaveTmp.write(jsonByteArrayTmp) not_eq -1)
        {
            configSavedTmp = true;
        }
    }
    if (not configSavedTmp)
    {
        APPENDSTRPTR(errorStrPtr_par, "Couldn't save config JSON file");
    }

    return configSavedTmp;
}

bool translator_c::writeLanguagesJSONFile_f(
        const QString& languageFilePath_par_con
        , const QString& fromLanguage_par_con
        , const QString& toLanguage_par_con, QString* errorStrPtr_par)
{
    bool configSavedTmp(false);

    QJsonObject baseJSONTmp;

    writeLanguagesJSON_f(baseJSONTmp, fromLanguage_par_con , toLanguage_par_con);

    QJsonDocument jsonDocumentTmp(baseJSONTmp);
    QByteArray jsonByteArrayTmp(jsonDocumentTmp.toJson(QJsonDocument::Indented));
    QFile configFileSaveTmp(languageFilePath_par_con);
    if (configFileSaveTmp.open(QIODevice::WriteOnly))
    {
        configFileSaveTmp.write(jsonByteArrayTmp);
        configSavedTmp = true;
    }
    else
    {
        APPENDSTRPTR(errorStrPtr_par, "Couldn't save languages JSON file");
    }

    return configSavedTmp;
}

translator_c::translator_c(
        const std::vector<languageLink_c>& languageLinks_par_con
        , const QString& translateFromLanguage_par_con
        , const std::vector<QString>& translateToLanguageChain_par_con
        , const bool addNotFoundKeys_par_con
        , const QString& prependNotFoundValue_par_con
)
    : addNotFoundKeys_pri(addNotFoundKeys_par_con)
    , prependNotFoundValue_pri(prependNotFoundValue_par_con)
{
    for (const languageLink_c& language_ite_con : languageLinks_par_con)
    {
        addLanguageLink_f(language_ite_con);
    }
    setTranslateFromLanguage_f(translateFromLanguage_par_con);
    setTranslateToLanguageChain_f(translateToLanguageChain_par_con);
}

void translator_c::readConfigJSONFile_f(const QString& configFilePath_par_con)
{
    while (true)
    {
        QFile configFileLoadTmp(configFilePath_par_con);
        QByteArray JSONByteArrayTmp;
        if (configFileLoadTmp.open(QIODevice::ReadOnly))
        {
            JSONByteArrayTmp = configFileLoadTmp.readAll();
        }
        else
        {
            break;
        }


        QJsonDocument JSONDocObjTmp(QJsonDocument::fromJson(JSONByteArrayTmp));
        if (JSONDocObjTmp.isNull())
        {
            break;
        }
        else
        {
            lastConfigFile_pri = configFilePath_par_con;
            //try to read languages in the config FILE,
            //since there is the option to save them in the translator config FILE (writeLanguagesToo_par_con)
            readLanguagesJSON_f(JSONDocObjTmp.object());
            //read the language JSON FILES array first
            readLanguageFileListJSON_f(JSONDocObjTmp.object());
            //read the vector of FILES
            readLanguagesJSONFiles_f(languageJSONFilePaths_pri);
            //read the config JSON to get languageFrom and languageToChain
            readConfigJSON_f(JSONDocObjTmp.object());
        }
        break;
    }
}

bool translator_c::addNotFoundKeys_f() const
{
    return addNotFoundKeys_pri;
}

void translator_c::setAddNotFoundKeys_f(const bool addNotFoundKeys_par_con)
{
    addNotFoundKeys_pri = addNotFoundKeys_par_con;
}

QString translator_c::prependNotFoundValue_f() const
{
    return prependNotFoundValue_pri;
}

void translator_c::setPrependNotFoundValue_f(const QString& prependNotFoundValue_par_con)
{
    prependNotFoundValue_pri = prependNotFoundValue_par_con;
}

void translator_c::writeLanguagesJSON_f(
        QJsonObject& json_par
        , const QString& fromLanguage_par_con
        , const QString& toLanguage_par_con
        ) const
{
    QJsonArray languagesArrayTmp;
    for (const languageLink_c& languageLink_ite_con : languageLinks_pri)
    {
        if (
            (languageLink_ite_con.fromLanguage_f() == fromLanguage_par_con or fromLanguage_par_con.isEmpty())
            and (languageLink_ite_con.toLanguage_f() == toLanguage_par_con or toLanguage_par_con.isEmpty())
        )
        {
            QJsonObject jsonObjectTmp;
            languageLink_ite_con.write_f(jsonObjectTmp);
            languagesArrayTmp.append(jsonObjectTmp);
        }

    }
    json_par["languages"] = languagesArrayTmp;
}

int_fast32_t translator_c::readLanguagesJSON_f(
        const QJsonObject& json_par_con
        , const bool replaceExisting_par_con
)
{
    int_fast32_t countTmp(0);
    QJsonArray arrayTmp(json_par_con["languages"].toArray());
    languageLinks_pri.reserve(arrayTmp.size());
    for (const auto item_ite_con : arrayTmp)
    {
        QJsonObject actionDataJsonObject(item_ite_con.toObject());
        languageLink_c languageLinkTmp;
        languageLinkTmp.read_f(actionDataJsonObject);
        if (addLanguageLink_f(languageLinkTmp, replaceExisting_par_con))
        {
            countTmp = countTmp + 1;
        }
    }
    return countTmp;
}

void translator_c::writeConfigJSON_f(QJsonObject& json_par) const
{
    json_par["translateFromLanguage"] = translateFromLanguage_pri;
    json_par["addKeysWithoutValue"] = addNotFoundKeys_pri;
    json_par["prependNotFoundValue"] = prependNotFoundValue_pri;
    {
        QJsonArray translateToLancuageChainArrayTmp;
        for (const QString& languageStr_ite_con : translateToLanguageChain_pri)
        {
            translateToLancuageChainArrayTmp.append(languageStr_ite_con);
        }
        json_par["translateToLanguageChain"] = translateToLancuageChainArrayTmp;
    }
    {
        QJsonArray languageJSONFilePathsArrayTmp;
        for (const QString& languageJSONFilePath_ite_con : languageJSONFilePaths_pri)
        {
            languageJSONFilePathsArrayTmp.append(languageJSONFilePath_ite_con);
        }
        json_par["languageJSONFilePaths"] = languageJSONFilePathsArrayTmp;
    }
}

void translator_c::readLanguageFileListJSON_f(const QJsonObject& json_par_con)
{
    QJsonArray arrayTmp(json_par_con["languageJSONFilePaths"].toArray());
    languageJSONFilePaths_pri.reserve(arrayTmp.size());
    for (const auto item_ite_con : arrayTmp)
    {
        languageJSONFilePaths_pri.emplace_back(item_ite_con.toString());
    }
}

void translator_c::readConfigJSON_f(const QJsonObject& json_par_con)
{
    setTranslateFromLanguage_f(json_par_con["translateFromLanguage"].toString());
    if (json_par_con["addNotFoundKeys"].isBool())
    {
        addNotFoundKeys_pri = json_par_con["addNotFoundKeys"].toBool();
    }
    prependNotFoundValue_pri = json_par_con["prependNotFoundValue"].toString("~");

    QJsonArray arrayTmp(json_par_con["translateToLanguageChain"].toArray());
    std::vector<QString> languageChainTmp;
    languageChainTmp.reserve(arrayTmp.size());
    for (const auto item_ite_con : arrayTmp)
    {
        languageChainTmp.emplace_back(item_ite_con.toString());
    }
    setTranslateToLanguageChain_f(languageChainTmp);
}

//const languageLink_c* translator_c::findLanguageLink_f(
//        const QString& fromLanguage_par_con
//        , const QString& toLanguage_par_con) const
//{
//    for (const languageLink_c& languageLink_ite_con : languageLinks_pri)
//    {
//        if (fromLanguage_par_con == languageLink_ite_con.fromLanguage_f()
//            and toLanguage_par_con == languageLink_ite_con.toLanguage_f())
//        {
//            return std::addressof(languageLink_ite_con);
//        }
//    }
//    return nullptr;
//}

languageLink_c* translator_c::findLanguageLink_f(
        const QString& fromLanguage_par_con
        , const QString& toLanguage_par_con)
{
    languageLink_c* languageLinkTmp_ptr(nullptr);
    for (languageLink_c& languageLink_ite_con : languageLinks_pri)
    {
        if (fromLanguage_par_con == languageLink_ite_con.fromLanguage_f()
            and toLanguage_par_con == languageLink_ite_con.toLanguage_f())
        {
            languageLinkTmp_ptr = std::addressof(languageLink_ite_con);
            break;
        }
    }
    return languageLinkTmp_ptr;
}

bool translator_c::addLanguageLink_f(
        const languageLink_c& languageLink_par_con
        , const bool replaceExisting_par_con
)
{
    bool resultTmp(false);
    //to not replace existing from-to combinations
    if (findLanguageLink_f(languageLink_par_con.fromLanguage_f(), languageLink_par_con.toLanguage_f()) == nullptr)
    {
        languageLinks_pri.emplace_back(languageLink_par_con);
        resultTmp = true;
    }
    else
    {
        if (replaceExisting_par_con)
        {
            removeLanguageLinks_f(languageLink_par_con.fromLanguage_f(), languageLink_par_con.toLanguage_f());
            languageLinks_pri.emplace_back(languageLink_par_con);
        }
    }
    return resultTmp;
}

bool translator_c::addEmptyLanguageLink_f(const QString& fromLanguage_par_con, const QString& toLanguage_par_con)
{
    bool resultTmp(false);
    //to not replace existing from-to combinations
    if (findLanguageLink_f(fromLanguage_par_con, toLanguage_par_con) == nullptr)
    {
        languageLinks_pri.emplace_back(languageLink_c(fromLanguage_par_con, toLanguage_par_con));
        resultTmp = true;
    }
    return resultTmp;
}

uint_fast32_t translator_c::removeLanguageLinks_f(
        const QString& fromLanguage_par_con
        , const QString& toLanguage_par_con)
{
    uint_fast32_t countTmp(0);
    if (fromLanguage_par_con.isEmpty() and toLanguage_par_con.isEmpty())
    {
        countTmp = languageLinks_pri.size();
        clearLanguageLinks_f();
    }
    else
    {
        countTmp = removeIfPredicateTrue_ft(languageLinks_pri, [&](const auto& item_par_con)
        {
            return (
                    (item_par_con->fromLanguage_f() == fromLanguage_par_con or fromLanguage_par_con.isEmpty())
                    and (item_par_con->toLanguage_f() == toLanguage_par_con or toLanguage_par_con.isEmpty())
                    );
        });
        //clear translateFromLanguage_pri if it used that languageLink
        bool fromLanguageFoundTmp(false);
        for (const languageLink_c& languageLink_ite_con : languageLinks_pri)
        {
            if (languageLink_ite_con.fromLanguage_f() == translateFromLanguage_pri)
            {
                fromLanguageFoundTmp = true;
            }

        }
        if (not fromLanguageFoundTmp)
        {
            translateFromLanguage_pri.clear();
        }

        //clear translateToLanguageChain_par_con if it used that languageLink
        bool chainLanguageFoundTmp(false);
        for (uint_fast32_t i = 0, l = translateToLanguageChain_pri.size(); (i + 1) < l; ++i)
        {
             chainLanguageFoundTmp = (
                         findLanguageLink_f(
                             translateToLanguageChain_pri.at(i)
                             , translateToLanguageChain_pri.at(i + 1)
                         ) not_eq Q_NULLPTR
             );
             if (not chainLanguageFoundTmp)
             {
                 translateToLanguageChain_pri.clear();
                 break;
             }
        }


    }
    return countTmp;
}

void translator_c::clearLanguageLinks_f()
{
    languageLinks_pri.clear();
    translateFromLanguage_pri.clear();
    translateToLanguageChain_pri.clear();
}

uint_fast32_t translator_c::languageLinkSize_f() const
{
    return languageLinks_pri.size();
}

uint_fast32_t translator_c::languageLinkExists_f(
        const QString& fromLanguage_par_con
        , const QString& toLanguage_par_con)
{
    uint_fast32_t countTmp(0);
    while (true)
    {
        if (fromLanguage_par_con.isEmpty() and toLanguage_par_con.isEmpty())
        {
            countTmp = languageLinkSize_f();
            break;
        }
        for (const languageLink_c& languageLink_ite_con : languageLinks_pri)
        {
            if (
                (languageLink_ite_con.fromLanguage_f() == fromLanguage_par_con or fromLanguage_par_con.isEmpty())
                and (languageLink_ite_con.toLanguage_f() == toLanguage_par_con or toLanguage_par_con.isEmpty())
                )
            {
                countTmp = countTmp + 1;
            }
        }
        break;
    }

    return countTmp;
}

int_fast32_t translator_c::updateLanguageLink_f(
        const languageLink_c& languageLinkSrc_par_con
        , const bool replaceSameKeyValue_par_con
)
{
    int_fast32_t updateCountTmp(0);
    languageLink_c* languageLinkDstTmp_ptr(findLanguageLink_f(languageLinkSrc_par_con.fromLanguage_f(), languageLinkSrc_par_con.toLanguage_f()));
    if (languageLinkDstTmp_ptr not_eq Q_NULLPTR)
    {
        //ignore warning, keys is const
        for (const QString& keySource_ite_con : languageLinkSrc_par_con.keys_f())
        {
            bool foundDstTmp(false);
            QString valueDstTmp(languageLinkDstTmp_ptr->keyToValue_f(keySource_ite_con, std::addressof(foundDstTmp)));
            if (foundDstTmp)
            {
                if (replaceSameKeyValue_par_con)
                {
                    languageLinkDstTmp_ptr->addUpdatePair_f(keySource_ite_con, valueDstTmp);
                    updateCountTmp = updateCountTmp + 1;
                }
                else
                {
                    //no replacing allowed
                }
            }
            else
            {
                QString valueSrcTmp(languageLinkSrc_par_con.keyToValue_f(keySource_ite_con));
                languageLinkDstTmp_ptr->addUpdatePair_f(keySource_ite_con, valueSrcTmp);
                updateCountTmp = updateCountTmp + 1;
            }
        }
    }
    else
    {
        updateCountTmp = languageLinkSrc_par_con.keys_f().size();
        languageLinks_pri.emplace_back(languageLinkSrc_par_con);
    }
    return updateCountTmp;
}

QString translator_c::baseTranslate_f(const QString& key_par_con, bool* found_par)
{
    QString resultTmp;
    bool foundTmp(false);
    while (true)
    {
        uint_fast32_t chainIndexTmp(0);
        QString currentFromLanguageTmp(translateFromLanguage_pri);
        QString currentToLanguageTmp(translateToLanguageChain_pri.at(chainIndexTmp));

        {
            languageLink_c* languageLinkTmp_ptr(findLanguageLink_f(currentFromLanguageTmp, currentToLanguageTmp));
            if (languageLinkTmp_ptr not_eq nullptr)
            {
                resultTmp = languageLinkTmp_ptr->keyToValue_f(key_par_con, std::addressof(foundTmp));
                if (not foundTmp)
                {
                    if (addNotFoundKeys_pri)
                    {
                        resultTmp = prependNotFoundValue_pri + key_par_con;
                        languageLinkTmp_ptr->addUpdatePair_f(key_par_con, resultTmp);
                    }
                    break;
                }
            }
            else
            {
                //no languagelink found for the from-to combination
            }
        }

        chainIndexTmp = chainIndexTmp + 1;
        while (chainIndexTmp < translateToLanguageChain_pri.size())
        {
            currentFromLanguageTmp = currentToLanguageTmp;
            currentToLanguageTmp = translateToLanguageChain_pri.at(chainIndexTmp);
            const languageLink_c* languageLinkTmp_ptr_con(findLanguageLink_f(currentFromLanguageTmp, currentToLanguageTmp));
            if (languageLinkTmp_ptr_con not_eq nullptr)
            {
                resultTmp = languageLinkTmp_ptr_con->keyToValue_f(key_par_con, std::addressof(foundTmp));
                if (not foundTmp)
                {
                    break;
                }
            }
            else
            {
                //this shouldn't happen since chain language should be in the languageLinks_pri
            }

            chainIndexTmp = chainIndexTmp + 1;
        }
        break;
    }
    if (found_par not_eq nullptr)
    {
        *found_par = foundTmp;
    }
    return resultTmp;
}


QString translator_c::configNotSetErrorMessage_f() const
{
    return "Translator configuration not set. TranslateFromLanguage and translateToLanguageChain must be set\n"
           "and LanguageLinks must be set too except when addNotFoundKeys is true";
}

QString translator_c::translate_f(
        const QString& key_par_con
        , bool* found_par
        , QString* errorStrPtr_par)
{
    QString resultTmp;
    while (true)
    {
        if (not isConfigSet_f())
        {
            APPENDSTRPTR(errorStrPtr_par, configNotSetErrorMessage_f());
            break;
        }

        resultTmp = baseTranslate_f(key_par_con, found_par);
        break;
    }
    return resultTmp;
}

QString translator_c::translateAndReplace_f(
        const text_c& text_par_con
        , bool* found_par
        , textCompilation_c* errorCompilationPtr_par)
{
    QString resultTmp;
    while (true)
    {
        if (text_par_con.translated_f())
        {
            //already translated
        }
        else
        {
            QString translateErrorsTmp;
            resultTmp = translate_f(text_par_con.rawText_f(), found_par, std::addressof(translateErrorsTmp));
            if (translateErrorsTmp.isEmpty())
            {
                //no translate errors
            }
            else
            {
                APPENDTEXTPTR(errorCompilationPtr_par, translateErrorsTmp)
            }
#ifdef DEBUGJOUVEN
            //qDebug() << "translate resultTmp1 " << resultTmp << endl;
#endif
        }
        std::vector<size_t> failReplaceIndexesTmp;
        resultTmp = text_par_con.rawReplace_f(std::addressof(resultTmp), errorCompilationPtr_par == nullptr ? nullptr : std::addressof(failReplaceIndexesTmp));
#ifdef DEBUGJOUVEN
        //qDebug() << "translate resultTmp2 " << resultTmp << endl;
        //qDebug() << "text_par_con.rawReplace_f() " << text_par_con.rawReplace_f() << endl;
#endif
        if (errorCompilationPtr_par not_eq nullptr and not failReplaceIndexesTmp.empty())
        {
            errorCompilationPtr_par->append_f({"Translated text: \"{0}\" failed to replace indexes: ", resultTmp});
            QString errorStrTmp;
            for (const size_t failedIndex_ite_con : failReplaceIndexesTmp)
            {
                errorStrTmp.append(QString::number(failedIndex_ite_con) + ",");
            }
            //remove the last comma
            errorStrTmp.chop(1);
            errorCompilationPtr_par->append_f(errorStrTmp);
        }
        break;
    }
    return resultTmp;
}

QMap<QString, QString> translator_c::translate_f(
        const QSet<QString>& keys_par_con
        , QString* errorStrPtr_par)
{
    QMap<QString, QString> resultTmp;
    while (true)
    {
        if (not isConfigSet_f())
        {
            APPENDSTRPTR(errorStrPtr_par, configNotSetErrorMessage_f());
            break;
        }
        for (const QString& key_ite_con : keys_par_con)
        {
            bool valueFoundTmp(false);
            QString valueTmp(baseTranslate_f(key_ite_con, std::addressof(valueFoundTmp)));
            if (valueFoundTmp or (addNotFoundKeys_pri and valueTmp.startsWith(prependNotFoundValue_pri)))
            {
                resultTmp.insert(key_ite_con, valueTmp);
            }
        }
        break;
    }
    return resultTmp;
}

QString translator_c::translateAndReplaceToString_f(const textCompilation_c& textCompilation_par_con, textCompilation_c* errorCompilationPtr_par)
{
    QString resultTmp;
    for (size_t i = 0; i < textCompilation_par_con.size_f(); ++i)
    {
        if (textCompilation_par_con.text_f(i).translated_f())
        {
            //already translated
            resultTmp.append(textCompilation_par_con.text_f(i).rawReplace_f());
        }
        else
        {
            resultTmp.append(translateAndReplace_f(textCompilation_par_con.text_f(i), nullptr, errorCompilationPtr_par));
        }
    }
    return resultTmp;
}

QMap<QString, QString> translator_c::translateAndReplace_f(
        const textCompilation_c& textCompilation_par_con
        , textCompilation_c* errorCompilationPtr_par
)
{
    QSet<QString> textsToTranslateTmp;
    //get the strings into a set
    for (size_t i = 0; i < textCompilation_par_con.size_f(); ++i)
    {
        if (textCompilation_par_con.text_f(i).translated_f())
        {
            //already translated
        }
        else
        {
            textsToTranslateTmp.insert(textCompilation_par_con.text_f(i).rawText_f());
        }
    }
    //translate
    QString translateErrorsTmp;
    QMap<QString, QString> translationsTmp(translate_f(textsToTranslateTmp, std::addressof(translateErrorsTmp)));
    if (translateErrorsTmp.isEmpty())
    {
        //no translate errors
    }
    else
    {
        errorCompilationPtr_par->append_f(translateErrorsTmp);
    }
    //replace the wildcards in translated raw text with the values
    for (size_t i = 0; i < textCompilation_par_con.size_f(); ++i)
    {
        QString translatedTextCopyTmp;
        QString rawTextTmp;
        const text_c textTmp(textCompilation_par_con.text_f(i));
        if (textTmp.translated_f())
        {
            rawTextTmp = textTmp.rawText_f();
            translatedTextCopyTmp = textTmp.rawText_f();
        }
        else
        {
            auto findResultTmp(translationsTmp.find(textTmp.rawText_f()));
            if (findResultTmp not_eq translationsTmp.end())
            {
                rawTextTmp = textTmp.rawText_f();
                translatedTextCopyTmp = findResultTmp.value();
            }
            else
            {
                //wasn't translated, won't be in the results
                continue;
            }
        }

        std::vector<size_t> failReplaceIndexesTmp;
        //replace the translated value in the result with text_c::rawReplace_f version
        translationsTmp.insert(
                    rawTextTmp
                    , textCompilation_par_con.text_f(i).rawReplace_f(std::addressof(translatedTextCopyTmp)
                                                                     , errorCompilationPtr_par == nullptr ? nullptr : std::addressof(failReplaceIndexesTmp))
        );
        if (errorCompilationPtr_par not_eq nullptr and not failReplaceIndexesTmp.empty())
        {
            errorCompilationPtr_par->append_f({"Translated text: \"{0}\" failed to replace indexes: ", translatedTextCopyTmp});
            QString errorStrTmp;
            for (const size_t failedIndex_ite_con : failReplaceIndexesTmp)
            {
                errorStrTmp.append(QString::number(failedIndex_ite_con) + ",");
            }
            //remove the last comma
            errorStrTmp.chop(1);
            errorCompilationPtr_par->append_f(errorStrTmp);
        }
    }
    return translationsTmp;
}

bool translator_c::languageLinksHasLanguageFrom_f(const QString& translateFromLanguage_par_con) const
{
    bool resultTmp(false);
    for (const languageLink_c& languageLink_ite_con : languageLinks_pri)
    {
        if (languageLink_ite_con.fromLanguage_f() == translateFromLanguage_par_con)
        {
            resultTmp = true;
            break;
        }
        else
        {

        }
    }
    return resultTmp;
}

std::vector<QString> translator_c::translateToLanguageChain_f() const
{
    return translateToLanguageChain_pri;
}

bool translator_c::setTranslateToLanguageChain_f(const std::vector<QString>& translateToLanguageChain_par_con)
{
    bool foundTmp(false);
    while (true)
    {
        if (translateToLanguageChain_par_con.empty())
        {
            break;
        }
        if (translateFromLanguage_pri.isEmpty())
        {
            break;
        }

        //from set + only 1 language in translateToLanguageChain_par_con case
        foundTmp = findLanguageLink_f(translateFromLanguage_pri, translateToLanguageChain_par_con.at(0)) not_eq Q_NULLPTR;
        if (translateToLanguageChain_par_con.size() == 1)
        {
            break;
        }

        //more than 1 language in translateToLanguageChain_par_con case
        for (uint_fast32_t i = 0, l = translateToLanguageChain_par_con.size(); (i + 1) < l; ++i)
        {
             foundTmp = (
                         findLanguageLink_f(
                             translateToLanguageChain_par_con.at(i)
                             , translateToLanguageChain_par_con.at(i + 1)
                         ) not_eq Q_NULLPTR
             );
             if (not foundTmp)
             {
                 break;
             }
        }
        break;
    }
    if (foundTmp)
    {
        translateToLanguageChain_pri = translateToLanguageChain_par_con;
    }
    return foundTmp;
}
