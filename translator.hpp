
#ifndef TRANSLATORJSONQTSO_TRANSLATOR_HPP
#define TRANSLATORJSONQTSO_TRANSLATOR_HPP

#include "languageLink.hpp"

#include "crossPlatformMacros.hpp"

#include <QString>
#include <QJsonObject>
#include <QMap>
#include <QSet>
//#include <QHash>
#ifdef DEBUGJOUVEN
#include <QDebug>
#endif

#include <vector>

class textCompilation_c;
class text_c;

class EXPIMP_TRANSLATORJSONQTSO translator_c
{
    //won't use a hash here with the languageString i.e. eng esp fra... just do a manual check each time
    //there aren't that many languages anyway
    std::vector<languageLink_c> languageLinks_pri;

    //config variables

    //what language the text literals passed to the translate_f functions are from
    QString translateFromLanguage_pri;
    //"translations" can be chained
    //source "eng", first to "esp", second to "fra"
    //mind the order
    std::vector<QString> translateToLanguageChain_pri;
    //last set/"json loaded" languageJSONFilePaths
    std::vector<QString> languageJSONFilePaths_pri;

    //to help creating an initial translation
    //a minimum translation config must be set:
    //1 translateFromLanguage_pri
    //2 one language in translateToLanguageChain_pri
    bool addNotFoundKeys_pri = false;
    //"this + key" will be used on the, not found, values added
    QString prependNotFoundValue_pri = "~";

    //end config variables

    QString lastConfigFile_pri;

    void writeLanguagesJSON_f(
            QJsonObject& json_par
            , const QString& fromLanguage_par_con = QString()
            , const QString& toLanguage_par_con = QString()
    ) const;
    //return the amount of successful loaded languages
    int_fast32_t readLanguagesJSON_f(
            const QJsonObject& json_par_con
            , const bool replaceExisting_par_con = false
    );

    void writeConfigJSON_f(QJsonObject& json_par) const;
    //two read are needed because "order", language loading needs to go first
    //1st read the list of language files, and load them after
    void readLanguageFileListJSON_f(const QJsonObject& json_par_con);
    //2nd read the configuration, which need language loaded values
    void readConfigJSON_f(const QJsonObject& json_par_con);

    //20191018 not in use because translate can't be const
    //because it has the feature of adding non-existing keys
//    const languageLink_c* findLanguageLink_f(
//            const QString& fromLanguage_par_con
//            , const QString& toLanguage_par_con
//    ) const;
    languageLink_c* findLanguageLink_f(
            const QString& fromLanguage_par_con
            , const QString& toLanguage_par_con
    );

    QString baseTranslate_f(const QString& key_par_con, bool* found_par);
    QString configNotSetErrorMessage_f() const;
public:
    //when initializing from JSON
    translator_c() = default;
    translator_c(
            const QString& configFilePath_par_con
    );
    //adds languageLinks (won't add duplicates)
    //then tries to set translateLanguageFrom and translateToLanguageChain
    translator_c(
            const std::vector<languageLink_c>& languageLinks_par_con
            , const QString& translateFromLanguage_par_con
            , const std::vector<QString>& translateToLanguageChain_par_con
            , const bool addNotFoundKeys_par_con = false
            , const QString& prependNotFoundValue_par_con = "~"
    );

    //replaces (without clearing other languages) loaded languageLinks
    void readLanguagesJSONFiles_f(
            const std::vector<QString>& languageJSONFilePaths_par_con
    );

    //language params can be empty, empty are treated as *any*
    //both empty will write all the languages
    //returns true if successful
    bool writeLanguagesJSONFile_f(
            const QString& languageFilePath_par_con
            , const QString& fromLanguage_par_con = QString()
            , const QString& toLanguage_par_con = QString()
            , QString* errorStrPtr_par = nullptr
    );

    //will trigger readJSONLanguageFiles_f if it has "languageJSONFilePaths" field non empty
    //replaces config variables values
    void readConfigJSONFile_f(
            const QString& configFilePath_par_con
    );

    //writeLanguagesToo_par_con = true, dumps all the languages in the same file
    //returns true if successful
    bool writeConfigJSONFile_f(
            const QString& filePath_par_con
            ,  const bool writeLanguagesToo_par_con = false
            //error is appended if the ptr is not nullptr
            , QString* errorStrPtr_par = nullptr
    );

    //won't replace existing one, use updateLanguageLink_f for that, returns true if successful
    bool addLanguageLink_f(const languageLink_c& languageLink_par_con, const bool replaceExisting_par_con = false);
    bool addEmptyLanguageLink_f(
            const QString& fromLanguage_par_con
            , const QString& toLanguage_par_con
    );

    //returns the amount of removed languages
    //any param can be empty, empty are treated as *any*
    //both empty will be the same as clearLanguageLinks_f
    //might unset translateFromLanguage and translateToLanguageChain if the removed languageLinks were used there
    uint_fast32_t removeLanguageLinks_f(
            const QString& fromLanguage_par_con
            , const QString& toLanguage_par_con
    );
    //unsets translateFromLanguage and translateToLanguageChain
    void clearLanguageLinks_f();
    uint_fast32_t languageLinkSize_f() const;

    //returns the number of matches
    //any param can be empty, empty is treated as *any*
    //both empty will return languageLinkSize_f
    uint_fast32_t languageLinkExists_f(
            const QString& fromLanguage_par_con
            , const QString& toLanguage_par_con = QString()
    );

    //replaceDuplicateKeys_par_con == true = source has keys that already exist on the destination will replace values
    //replaceDuplicateKeys_par_con == false = only NEW keys will be added
    //if the language doesn't match any, it will be added
    //returns the number of keys updated/added
    int_fast32_t updateLanguageLink_f(
            const languageLink_c& languageLinkSrc_par_con
            , const bool replaceSameKeyValue_par_con = false
    );

    //for all the translate functions, when "AddNotFoundKeys" is true it will return a string even if not found is true

    QString translate_f(const QString& key_par_con, bool* found_par = nullptr, QString* errorStrPtr_par = nullptr);
    //won't translate already translated text_c object, but will always replace
    QString translateAndReplace_f(const text_c& text_par_con, bool* found_par = nullptr, textCompilation_c* errorCompilationPtr_par = nullptr);

    //empty key = ignored key
    //return value explanation: key is the original value (the one from the set), value is the translation
    //keys that don't get translated won't be in the returned object except if "AddNotFoundKeys" is true
    QMap<QString, QString> translate_f(
            const QSet<QString>& keys_par_con
            , QString* errorStrPtr_par = nullptr
    );
    //key = text_c rawText, value = text translated and replaced
    //like the single translation version function, it won't translate already translated text_c objects, but will always replace
    //not translated keys won't be in the returned object, except if "AddNotFoundKeys" is true
    //WARNING the above function uses a QSet but this one doesn't, duplicate "keys" will cause issues
    QMap<QString, QString> translateAndReplace_f(
            const textCompilation_c& textCompilation_par_con
            //reports the same errors as translate, also
            //if some replace didn't happen
            , textCompilation_c* errorCompilationPtr_par = nullptr
    );
    QString translateAndReplaceToString_f(
            const textCompilation_c& textCompilation_par_con
            //reports the same errors as translate, also
            //if some replace didn't happen
            , textCompilation_c* errorCompilationPtr_par = nullptr
    );

    bool languageLinksHasLanguageFrom_f(const QString& translateFromLanguage_par_con) const;
    QString translateFromLanguage_f() const;
    //returns true if the from language is loaded so it can be used to translate
    bool setTranslateFromLanguage_f(const QString& translateFromLanguage_par_con);

    std::vector<QString> translateToLanguageChain_f() const;
    //returns true if the chain can be done, if the required languages are loaded
    bool setTranslateToLanguageChain_f(const std::vector<QString>& translateToLanguageChain_par_con);

    std::vector<QString> lastLoadedJSONFilePaths_f() const;
    //void setJSONFilePaths_f(const std::vector<QString>& JSONFilePaths_par_con);

    //translateFromLanguage, translateToLanguageChain and languageLinks not empty
    bool isConfigSet_f() const;

    //returns the all languageFrom-languageTo from the loaded languageLinks
    std::vector<std::pair<QString, QString>> translationFromToPairs_f() const;

    bool addNotFoundKeys_f() const;
    //if true, when translating if a key doesn't exists it's automatically inserted
    //in the first "from language"-"to language" pair/"first language" (language chains will be ignored),
    //the translate function will return a "translation" = prependNotFoundValue_f + key
    //the found arg will return true
    //this setting is to help add hardcoded keys to a language
    void setAddNotFoundKeys_f(const bool addNotFoundKeys_par_con);

    QString prependNotFoundValue_f() const;
    void setPrependNotFoundValue_f(const QString& prependNotFoundValue_par_con);
};


#endif // TRANSLATORJSONQTSO_TRANSLATOR_HPP
