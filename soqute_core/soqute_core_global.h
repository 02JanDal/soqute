#pragma once

#include <QtGlobal>

#if defined(SOQUTE_CORE_LIBRARY)
#define SOQUTE_CORESHARED_EXPORT Q_DECL_EXPORT
#else
#define SOQUTE_CORESHARED_EXPORT Q_DECL_IMPORT
#endif
