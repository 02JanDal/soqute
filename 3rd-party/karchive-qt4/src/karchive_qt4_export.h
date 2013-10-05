
#ifndef KARCHIVE_EXPORT_H
#define KARCHIVE_EXPORT_H

#ifdef KARCHIVE_STATIC_DEFINE
#  define KARCHIVE_EXPORT
#  define KARCHIVE_NO_EXPORT
#else
#  ifndef KARCHIVE_EXPORT
#    ifdef KARCHIVE_QT4_MAKEDLL
        /* We are building this library */
#      define KARCHIVE_EXPORT 
#    else
        /* We are using this library */
#      define KARCHIVE_EXPORT 
#    endif
#  endif

#  ifndef KARCHIVE_NO_EXPORT
#    define KARCHIVE_NO_EXPORT 
#  endif
#endif

#ifndef KARCHIVE_DEPRECATED
#  define KARCHIVE_DEPRECATED __attribute__ ((__deprecated__))
#  define KARCHIVE_DEPRECATED_EXPORT KARCHIVE_EXPORT __attribute__ ((__deprecated__))
#  define KARCHIVE_DEPRECATED_NO_EXPORT KARCHIVE_NO_EXPORT __attribute__ ((__deprecated__))
#endif

#define DEFINE_NO_DEPRECATED 0
#if DEFINE_NO_DEPRECATED
# define KARCHIVE_NO_DEPRECATED
#endif

#endif
