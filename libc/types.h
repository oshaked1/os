typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long ulonglong;

typedef uchar uint8;
typedef signed char int8;
typedef ushort uint16;
typedef signed short int16;
typedef uint uint32;
typedef signed int int32;
typedef ulonglong uint64;
typedef signed long long int64;

typedef uchar byte;
typedef uint8 bool;

#ifdef __i386__
typedef uint32 size_t;
#endif
#ifdef __x86_64__
typedef uint64 size_t;
#endif

#define TRUE  1
#define FALSE 0
#define NULL  0