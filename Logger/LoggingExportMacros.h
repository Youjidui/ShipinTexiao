#ifdef LOGGING_LIBRARY_EXPORT
#define LOGGING_CLASS_EXPORT __declspec(dllexport)
#else
#define LOGGING_CLASS_EXPORT __declspec(dllimport)
#endif
