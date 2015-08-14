
#ifndef OTADS_PRIVATE_HXX
#define OTADS_PRIVATE_HXX

/* From http://gcc.gnu.org/wiki/Visibility */
/* Generic helper definitions for shared library support */
#if defined _WIN32 || defined __CYGWIN__
#define OTADS_HELPER_DLL_IMPORT __declspec(dllimport)
#define OTADS_HELPER_DLL_EXPORT __declspec(dllexport)
#define OTADS_HELPER_DLL_LOCAL
#else
#if __GNUC__ >= 4
#define OTADS_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
#define OTADS_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
#define OTADS_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define OTADS_HELPER_DLL_IMPORT
#define OTADS_HELPER_DLL_EXPORT
#define OTADS_HELPER_DLL_LOCAL
#endif
#endif

/* Now we use the generic helper definitions above to define OTADS_API and OTADS_LOCAL.
 * OTADS_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
 * OTADS_LOCAL is used for non-api symbols. */

#ifndef OTADS_STATIC /* defined if OT is compiled as a DLL */
#ifdef OTADS_DLL_EXPORTS /* defined if we are building the OT DLL (instead of using it) */
#define OTADS_API OTADS_HELPER_DLL_EXPORT
#else
#define OTADS_API OTADS_HELPER_DLL_IMPORT
#endif /* OTADS_DLL_EXPORTS */
#define OTADS_LOCAL OTADS_HELPER_DLL_LOCAL
#else /* OTADS_STATIC is defined: this means OT is a static lib. */
#define OTADS_API
#define OTADS_LOCAL
#endif /* !OTADS_STATIC */


#endif // OTADS_PRIVATE_HXX

