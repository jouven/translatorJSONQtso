//because windows sucks...

#ifndef TRANSLATORJSONQTSO_CROSSPLATFORMMACROS_HPP
#define TRANSLATORJSONQTSO_CROSSPLATFORMMACROS_HPP

#include <QtCore/QtGlobal>

//remember to define this variable in the .pro file
#if defined(TRANSLATORJSONQTSO_LIB)
#  define EXPIMP_TRANSLATORJSONQTSO Q_DECL_EXPORT
#else
#  define EXPIMP_TRANSLATORJSONQTSO Q_DECL_IMPORT
#endif

#endif // TRANSLATORJSONQTSO_CROSSPLATFORMMACROS_HPP
