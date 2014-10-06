/*
     PLIB - A Suite of Portable Game Libraries
     Copyright (C) 1998,2002  Steve Baker

     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Library General Public
     License as published by the Free Software Foundation; either
     version 2 of the License, or (at your option) any later version.

     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Library General Public License for more details.

     You should have received a copy of the GNU Library General Public
     License along with this library; if not, write to the Free Software
     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

     For further information visit http://plib.sourceforge.net

     $Id: ul.h 2117 2007-09-13 23:21:09Z fayjf $
*/

//
//  UL - utility library
//
//  Contains:
//  - necessary system includes
//  - basic types
//  - error message routines
//  - high performance clocks
//  - ulList
//  - ulLinkedList
//  - more to come (endian support, version ID)
//

#ifndef _INCLUDED_UL_H_
#define _INCLUDED_UL_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

/**********************\
*                      *
*  Determine OS type   *
*                      *
\**********************/

#if defined(__CYGWIN__)

#define UL_WIN32     1
#define UL_CYGWIN    1    /* Windoze AND Cygwin. */

#elif defined(_WIN32) || defined(__WIN32__) || defined(_MSC_VER)

#define UL_WIN32     1
#define UL_MSVC      1    /* Windoze AND MSVC. */

#elif defined(__BEOS__)

#define UL_BEOS      1

#elif defined( macintosh )

#define UL_MACINTOSH 1

#elif defined(__APPLE__)

#define UL_MAC_OSX   1 

#elif defined(__linux__)

#define UL_LINUX     1

#elif defined(__sgi)

#define UL_IRIX      1

#elif defined(_AIX)

#define UL_AIX       1

#elif defined(SOLARIS) || defined(sun)

#define UL_SOLARIS   1

#elif defined(hpux)

#define UL_HPUX      1

#elif (defined(__unix__) || defined(unix)) && !defined(USG)

#define UL_BSD       1

#endif


/*
  Add specialised includes/defines...
*/

#ifdef UL_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <regstr.h>
#define  UL_WGL     1
#endif

#ifdef UL_CYGWIN
#include <unistd.h>
#define  UL_WGL     1
#endif

#ifdef UL_BEOS
#include <be/kernel/image.h>
#define  UL_GLX     1
#endif

#ifdef UL_MACINTOSH
#include <CodeFragments.h>
#include <unistd.h>
#define  UL_AGL     1
#endif

#if defined(UL_MAC_OSX) || defined(UL_LINUX) || defined(UL_BSD) || defined(UL_IRIX) || defined(UL_SOLARIS) || defined(UL_AIX)
#include <unistd.h>
#include <dlfcn.h>
#include <fcntl.h>
#define  UL_GLX     1
#endif

#if defined(UL_BSD)
#include <sys/param.h>
#define  UL_GLX     1
#endif

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <float.h>
#include <errno.h>

/* PLIB version macros */

#define PLIB_MAJOR_VERSION 1
#define PLIB_MINOR_VERSION 8
#define PLIB_TINY_VERSION  5

#define PLIB_VERSION (PLIB_MAJOR_VERSION*100 \
                     +PLIB_MINOR_VERSION*10 \
                     +PLIB_TINY_VERSION)

/* SGI machines seem to suffer from a lack of FLT_EPSILON so... */

#ifndef FLT_EPSILON
#define FLT_EPSILON 1.19209290e-07f        
#endif

#ifndef DBL_EPSILON
#define DBL_EPSILON 1.19209290e-07f
#endif

#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif

/* SUNWspro 4.2 and earlier need bool to be defined */

#if defined(__SUNPRO_CC) && __SUNPRO_CC < 0x500
typedef int bool ;
const   int true  = 1 ;
const   int false = 0 ;
#endif

/* Let's define our own "min" and "max" so that different operating systems
 * don't complain
 */
#define ulMax(a,b)	((a)>(b)?(a):(b))
#define ulMin(a,b)	((a)<(b)?(a):(b))


/*
  Basic Types
*/


/*
  High precision clocks.
*/

class ulClock
{
  double start ;
  double now   ;
  double delta ;
  double last_time ;
  double max_delta ;
  
#ifdef UL_WIN32
  static double res ;
  static int perf_timer ;
  void initPerformanceTimer () ;
#endif

  double getRawTime () const ;

public:

  ulClock () { reset () ; }

  void reset ()
  {
#ifdef UL_WIN32
    initPerformanceTimer () ;
#endif
    start     = getRawTime () ;
    now       = 0.0 ;
    max_delta = 0.2 ; 
    delta     = 0.0000001 ;  /* Faked so stoopid programs won't div0 */
    last_time = 0.0 ;
  }

  void   setMaxDelta  ( double maxDelta ) { max_delta = maxDelta ; }
  double getMaxDelta  () const { return max_delta ; }
  void   update       () ;
  double getAbsTime   () const { return now   ; }
  double getDeltaTime () const { return delta ; }
  double getFrameRate () const { return 1.0 / delta ; }
} ;


inline void ulSleep ( int seconds )
{
  if ( seconds >= 0 )
  {
#ifdef UL_WIN32
    Sleep ( 1000 * seconds ) ;
#else
    sleep ( seconds ) ;
#endif
  }
}


inline void ulMilliSecondSleep ( int milliseconds )
{
  if ( milliseconds >= 0 )
  {
#ifdef UL_WIN32
    Sleep ( milliseconds ) ;
#else
    usleep ( milliseconds * 1000 ) ;
#endif
  }
}


/*
  This is extern C to enable 'configure.in' to
  find it with a C-coded probe.
*/

extern "C" void ulInit () ;

/*
  Error handler.
*/

enum ulSeverity
{
  UL_DEBUG,    // Messages that can safely be ignored.
  UL_WARNING,  // Messages that are important.
  UL_FATAL,    // Errors that we cannot recover from.
  UL_MAX_SEVERITY
} ;


typedef void (*ulErrorCallback) ( enum ulSeverity severity, char* msg ) ;

void            ulSetError         ( enum ulSeverity severity, const char *fmt, ... ) ;
char*           ulGetError         ( void ) ;
void            ulClearError       ( void ) ;
ulErrorCallback ulGetErrorCallback ( void ) ;
void            ulSetErrorCallback ( ulErrorCallback cb ) ;

/*
  Directory Reading
*/

#define UL_NAME_MAX 256
typedef struct _ulDir ulDir ;
struct ulDirEnt
{
  char d_name [ UL_NAME_MAX+1 ];
  bool d_isdir ;
} ;

int ulIsAbsolutePathName ( const char *pathname ) ;
char *ulGetCWD ( char *result, int maxlength ) ;

ulDir* ulOpenDir ( const char* dirname ) ;
ulDirEnt* ulReadDir ( ulDir* dir ) ;
void ulCloseDir ( ulDir* dir ) ;

// file handling

char* ulMakePath( char* path, const char* dir, const char* fname );

bool ulFileExists ( const char *fileName ) ;

void ulFindFile( char *filenameOutput, const char *path, 
											  const char * tfnameInput, const char *sAPOM ) ;


/*
  Endian handling
*/

static const int _ulEndianTest = 1;
#define ulIsLittleEndian (*((char *) &_ulEndianTest ) != 0)
#define ulIsBigEndian    (*((char *) &_ulEndianTest ) == 0)

inline void ulEndianSwap(unsigned int *x)
{
  *x = (( *x >> 24 ) & 0x000000FF ) | 
       (( *x >>  8 ) & 0x0000FF00 ) | 
       (( *x <<  8 ) & 0x00FF0000 ) | 
       (( *x << 24 ) & 0xFF000000 ) ;
}

  
inline void ulEndianSwap(unsigned short *x)
{
  *x = (( *x >>  8 ) & 0x00FF ) | 
       (( *x <<  8 ) & 0xFF00 ) ;
}
  

inline void ulEndianSwap(float *x) { ulEndianSwap((unsigned int   *)x); }
inline void ulEndianSwap(int   *x) { ulEndianSwap((unsigned int   *)x); }
inline void ulEndianSwap(short *x) { ulEndianSwap((unsigned short *)x); }


inline unsigned short ulEndianLittle16(unsigned short x) {
  if (ulIsLittleEndian) {
    return x;
  } else {
    ulEndianSwap(&x);
    return x;
  }
}

inline unsigned int ulEndianLittle32(unsigned int x) {
  if (ulIsLittleEndian) {
    return x;
  } else {
    ulEndianSwap(&x);
    return x;
  }
}

inline float ulEndianLittleFloat(float x) {
  if (ulIsLittleEndian) {
    return x;
  } else {
    ulEndianSwap(&x);
    return x;
  }
}

inline void ulEndianLittleArray16(unsigned short *x, int length) {
  if (ulIsLittleEndian) {
    return;
  } else {
    for (int i = 0; i < length; i++) {
      ulEndianSwap(x++);
    }
  }
}

inline void ulEndianLittleArray32(unsigned int *x, int length) {
  if (ulIsLittleEndian) {
    return;
  } else {
    for (int i = 0; i < length; i++) {
      ulEndianSwap(x++);
    }
  }
}

inline void ulEndianLittleArrayFloat(float *x, int length) {
  if (ulIsLittleEndian) {
    return;
  } else {
    for (int i = 0; i < length; i++) {
      ulEndianSwap(x++);
    }
  }
}

inline void ulEndianBigArray16(unsigned short *x, int length) {
  if (ulIsBigEndian) {
    return;
  } else {
    for (int i = 0; i < length; i++) {
      ulEndianSwap(x++);
    }
  }
}

inline void ulEndianBigArray32(unsigned int *x, int length) {
  if (ulIsBigEndian) {
    return;
  } else {
    for (int i = 0; i < length; i++) {
      ulEndianSwap(x++);
    }
  }
}

inline void ulEndianBigArrayFloat(float *x, int length) {
  if (ulIsBigEndian) {
    return;
  } else {
    for (int i = 0; i < length; i++) {
      ulEndianSwap(x++);
    }
  }
}

inline unsigned short ulEndianBig16(unsigned short x) {
  if (ulIsBigEndian) {
    return x;
  } else {
    ulEndianSwap(&x);
    return x;
  }
}

inline unsigned int ulEndianBig32(unsigned int x) {
  if (ulIsBigEndian) {
    return x;
  } else {
    ulEndianSwap(&x);
    return x;
  }
}

inline float ulEndianBigFloat(float x) {
  if (ulIsBigEndian) {
    return x;
  } else {
    ulEndianSwap(&x);
    return x;
  }
}

inline unsigned short ulEndianReadLittle16(FILE *f) {
  unsigned short x;
  fread(&x, 2, 1, f);
  return ulEndianLittle16(x);
}

inline unsigned int ulEndianReadLittle32(FILE *f) {
  unsigned int x;
  fread(&x, 4, 1, f);
  return ulEndianLittle32(x);
}

inline float ulEndianReadLittleFloat(FILE *f) {
  float x;
  fread(&x, 4, 1, f);
  return ulEndianLittleFloat(x);
}

inline unsigned short ulEndianReadBig16(FILE *f) {
  unsigned short x;
  fread(&x, 2, 1, f);
  return ulEndianBig16(x);
}

inline unsigned int ulEndianReadBig32(FILE *f) {
  unsigned int x;
  fread(&x, 4, 1, f);
  return ulEndianBig32(x);
}

inline float ulEndianReadBigFloat(FILE *f) {
  float x;
  fread(&x, 4, 1, f);
  return ulEndianBigFloat(x);
}

inline size_t ulEndianWriteLittle16(FILE *f, unsigned short x) {
  x = ulEndianLittle16(x);
  return fwrite( &x, 2, 1, f );
}

inline size_t ulEndianWriteLittle32(FILE *f, unsigned int x) {
  x = ulEndianLittle32(x);
  return fwrite( &x, 4, 1, f );
}

inline size_t ulEndianWriteLittleFloat(FILE *f, float x) {
  x = ulEndianLittleFloat(x);
  return fwrite( &x, 4, 1, f );
}

inline size_t ulEndianWriteBig16(FILE *f, unsigned short x) {
  x = ulEndianBig16(x);
  return fwrite( &x, 2, 1, f );
}

inline size_t ulEndianWriteBig32(FILE *f, unsigned int x) {
  x = ulEndianBig32(x);
  return fwrite( &x, 4, 1, f );
}

inline size_t ulEndianWriteBigFloat(FILE *f, float x) {
  x = ulEndianBigFloat(x);
  return fwrite( &x, 4, 1, f );
}


/*
  Windoze/BEOS code based on contribution from Sean L. Palmer 
*/


#ifdef UL_WIN32

class ulDynamicLibrary
{
  HMODULE handle ;

public:

  ulDynamicLibrary ( const char *libname )
  {
    char dllname[1024];
    strcpy ( dllname, libname ) ;
    strcat ( dllname, ".dll"  ) ;
    handle = (HMODULE) LoadLibrary ( dllname ) ;
  }

  void *getFuncAddress ( const char *funcname )
  {
    return (void *) GetProcAddress ( handle, funcname ) ; //lint !e611
  }

  ~ulDynamicLibrary ()
  {
    if ( handle != NULL )
      FreeLibrary ( handle ) ;
  }
} ;

#elif defined (UL_MACINTOSH)

class ulDynamicLibrary
{
    CFragConnectionID connection;
    OSStatus          error;

public:

    ulDynamicLibrary ( const char *libname )
    {
        Str63    pstr;
        int        sz;

        sz = strlen (libname);

        if (sz < 64) {

            pstr[0] = sz;
            memcpy (pstr+1, libname, sz);

            error = GetSharedLibrary (pstr, kPowerPCCFragArch, kReferenceCFrag,
                                      &connection, NULL, NULL);
        }
        else
            error = 1;
    }

    ~ulDynamicLibrary ()
    {
        if ( ! error )
            CloseConnection (&connection);
    }

    void* getFuncAddress ( const char *funcname )
    {
        if ( ! error ) {

            char*  addr;
            Str255 sym;
            int    sz;

            sz = strlen (funcname);
            if (sz < 256) {

                sym[0] = sz;
                memcpy (sym+1, funcname, sz);

                error = FindSymbol (connection, sym, &addr, 0);
                if ( ! error )
                    return addr;
            }
        }

        return NULL;
    }
};

/*
OSX works like "else" now.
#elif defined (UL_MAC_OSX)


class ulDynamicLibrary
{

 public:

    ulDynamicLibrary ( const char *libname )
    {
    }

    ~ulDynamicLibrary ()
    {
    }

    void* getFuncAddress ( const char *funcname )
    {
      ulSetError ( UL_WARNING, "ulDynamicLibrary unsuppored on Mac OS X" );
      return NULL;
    }
};
*/
#elif defined (__BEOS__)

class ulDynamicLibrary
{
  image_id *handle ;

public:

  ulDynamicLibrary ( const char *libname )
  {
    char addonname[1024] ;
    strcpy ( addonname, libname ) ;
    strcat ( addonname, ".so" ) ;
    handle = new image_id ;

    *handle = load_add_on ( addonname ) ;

    if ( *handle == B_ERROR )
    {
      delete handle ;
      handle = NULL ;
    }
  }

  void *getFuncAddress ( const char *funcname )
  {
    void *sym = NULL ;

    if ( handle &&
         get_image_symbol ( handle, "funcname",
                            B_SYMBOL_TYPE_TEXT, &sym ) == B_NO_ERROR )
      return sym ;

    return NULL ;
  }

  ~ulDynamicLibrary ()
  {
    if ( handle != NULL )
      unload_add_on ( handle ) ;

    delete handle ;
  }
} ;

#  else

/*
  Linux/UNIX
*/

class ulDynamicLibrary
{
  void *handle ;

public:

  ulDynamicLibrary ( const char *libname )
  {
    char dsoname [ 1024 ] ;
    strcpy ( dsoname, libname ) ;
    strcat ( dsoname, ".so"  ) ;
    handle = (void *) dlopen ( dsoname, RTLD_NOW | RTLD_GLOBAL ) ;

    if ( handle == NULL )
      ulSetError ( UL_WARNING, "ulDynamicLibrary: %s\n", dlerror() ) ;
  }

  void *getFuncAddress ( const char *funcname )
  {
    return (handle==NULL) ? NULL : dlsym ( handle, funcname ) ;
  }

  ~ulDynamicLibrary ()
  {
    if ( handle != NULL )
      dlclose ( handle ) ;
  }
} ;

#endif


class ulList
{
protected:
  unsigned int total ;  /* The total number of entities in the list */
  unsigned int limit ;  /* The current limit on number of entities  */
  unsigned int next  ;  /* The next entity when we are doing getNext ops */
 
  void **entity_list ;  /* The list. */
 
  void sizeChk (void) ;
 
public:
 
  ulList ( int init_max = 1 ) ;
  virtual ~ulList (void) ;
 
  void *getEntity ( unsigned int n )
  {
    next = n + 1 ;
    return ( n >= total ) ? (void *) NULL : entity_list [ n ] ;
  }
 
  virtual void addEntity ( void *entity ) ;
  virtual void addEntityBefore ( int n, void *entity ) ;
  virtual void removeEntity ( unsigned int n ) ;
 
  void removeAllEntities () ;
 
  void removeEntity ( void *entity )
  {
    removeEntity ( searchForEntity ( entity ) ) ;
  }
 
  virtual void replaceEntity ( unsigned int n, void *new_entity ) ;
 
  void replaceEntity ( void *old_entity, void *new_entity )
  {
    replaceEntity ( searchForEntity ( old_entity ), new_entity ) ;
  }
 
  void *getNextEntity   (void) { return getEntity ( next ) ; }

  int   getNumEntities  (void) const { return total ; }
  int   searchForEntity ( void *entity ) const ;
} ;


typedef bool (*ulIterateFunc)( const void *data, void *user_data ) ;
typedef int  (*ulCompareFunc)( const void *data1, const void *data2 ) ;

/*
  Linked list.
*/

class ulListNode ;

class ulLinkedList
{
protected:

  ulListNode *head ;
  ulListNode *tail ;

  int nnodes ;
  bool sorted ;

  void unlinkNode ( ulListNode *prev, ulListNode *node ) ;

  bool isValidPosition ( int pos ) const
  {
    if ( ( pos < 0 ) || ( pos >= nnodes ) )
    {
      ulSetError ( UL_WARNING, "ulLinkedList: Invalid 'pos' %u", pos ) ;
      return false ;
    }
    return true ;
  }

public:

  ulLinkedList ()
  {
    head = tail = NULL ;
    nnodes = 0 ;
    sorted = true ;
  }

  ~ulLinkedList () { empty () ; }

  int  getNumNodes ( void ) const { return nnodes ; }
  bool isSorted    ( void ) const { return sorted ; }

  int  getNodePosition ( void *data ) const ;

  void insertNode ( void *data, int pos ) ;
  void prependNode ( void *data ) { insertNode ( data, 0 ) ; }
  void appendNode ( void *data ) ;

  int  insertSorted ( void *data, ulCompareFunc comparefn ) ;

  void removeNode ( void *data ) ;
  void * removeNode ( int pos ) ;

  void * getNodeData ( int pos ) const ;

  void * forEach ( ulIterateFunc fn, void *user_data = NULL ) const ;

  void empty ( ulIterateFunc destroyfn = NULL, void *user_data = NULL ) ;
} ;


extern char *ulStrDup ( const char *s ) ;
extern int ulStrNEqual ( const char *s1, const char *s2, int len );
extern int ulStrEqual ( const char *s1, const char *s2 );

//lint -restore

#endif

