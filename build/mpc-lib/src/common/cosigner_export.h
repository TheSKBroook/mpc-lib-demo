
#ifndef COSIGNER_EXPORT_H
#define COSIGNER_EXPORT_H

#ifdef COSIGNER_STATIC_DEFINE
#  define COSIGNER_EXPORT
#  define COSIGNER_NO_EXPORT
#else
#  ifndef COSIGNER_EXPORT
#    ifdef cosigner_EXPORTS
        /* We are building this library */
#      define COSIGNER_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define COSIGNER_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef COSIGNER_NO_EXPORT
#    define COSIGNER_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef COSIGNER_DEPRECATED
#  define COSIGNER_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef COSIGNER_DEPRECATED_EXPORT
#  define COSIGNER_DEPRECATED_EXPORT COSIGNER_EXPORT COSIGNER_DEPRECATED
#endif

#ifndef COSIGNER_DEPRECATED_NO_EXPORT
#  define COSIGNER_DEPRECATED_NO_EXPORT COSIGNER_NO_EXPORT COSIGNER_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef COSIGNER_NO_DEPRECATED
#    define COSIGNER_NO_DEPRECATED
#  endif
#endif

#endif /* COSIGNER_EXPORT_H */
