// =============================================================================
//
//  utils.hpp
//  ocv
//
//  Created by Guy Bar-Nahum on 3/8/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

// ==================================================================== includes

#ifndef ocv_utils_hpp
#define ocv_utils_hpp

#include "ocvstd.hpp"

// ======================================================================== keys

bool is_printable( int key );
void to_key( int key, string &s_key );
inline bool is_odd_num( int num ){ return (( num & 1 ) == 1 ); }
    
// ================================================================== file utils
bool  file_exists ( string  path );
bool  file_to_path( string &path );
char *path_to_file( char*   path );

// ================================================================= print utils

void print_argv( int argc, const char * argv[] );

// ======================================================== high resultion timer
// TODO: FIXME: Do stuff in ms not nano sec: what is stupid about this, is
// that it gives nano second values but takes miliseconds to call the routine
// usage : auto t = HighResClock::now();

#include <chrono>
#include "chrono_io"

typedef chrono::high_resolution_clock       HighResClock;
typedef chrono::time_point<HighResClock>    TimePoint;

inline long long now_ms()
{
    const long long ms = clock();
    return ms;
}

inline TimePoint now_nano()
{
    return HighResClock::now();
}

// Primitive duration, there has to be a better way..
inline HighResClock::duration duration_nano()
{
    static TimePoint base_time;
    static bool is_first = true;
    
    if ( is_first ){
        base_time = now_nano();
        is_first = false;
    }
    
    return now_nano() - base_time ;
}

inline long long duration_ms()
{
    static long long base_ms;
    static bool is_first = true;
    
    if ( is_first ){
        base_ms = now_ms();
        is_first = false;
    }
    
    return now_ms() - base_ms ;
}

// =================================================================== log utils
//
// Taken from : http://www.drdobbs.com/cpp/logging-in-c/201804215
//
// Usage: LOG(LOG_WARNING)  << "Ops, variable x should be "
//                          << expectedX << "; is " << realX;
//
// LOG::reporting_level() = LOG_DEBUG;
// output_to_file::stream() = fopen( "mylogfile.txt", "w" );;
//

#include <sstream>
#include <string>
#include <stdio.h>

inline TimePoint now_time();

enum TLogLevel {
    
    LEVEL_ERROR    ,
    LEVEL_WARNING  ,
    LEVEL_INFO     ,
    LEVEL_DEBUG    ,
    LEVEL_DEV      ,
    
    LEVEL_MAX      = LEVEL_DEV,
};

template <typename T>
class Log
{
public:
    Log();
    virtual ~Log();
    
    ostringstream& get( TLogLevel level = LEVEL_INFO );
    
public:
    static TLogLevel&  reporting_level();
    static string      to_string  ( TLogLevel     level);
    static TLogLevel   from_string( const string& level);
    
protected:
    ostringstream os;
    
private:
    Log( const Log& );
    Log& operator =( const Log& );
};

template <typename T>
Log<T>::Log(){}

#include <iomanip> // for setfill / setw below

template <typename T>
ostringstream& Log<T>::get( TLogLevel level )
{
    os << setfill('0');
    os << setw(8) << duration_ms();
    os << " "  << to_string( level ) << ": ";
    os << string( (level > LEVEL_DEBUG)? (level - LEVEL_DEBUG) : 0, '\t' );
    return os;
}

template <typename T>
Log<T>::~Log()
{
    os << endl;
    T::output(os.str());
}

template <typename T>
TLogLevel& Log<T>::reporting_level()
{
    static TLogLevel reporting_level = LEVEL_INFO;
    return reporting_level;
}

template <typename T>
string Log<T>::to_string( TLogLevel level )
{
    static const char* const buffer[] = {
        "ERROR" , "WARN", "INFO", "DEBUG","DEV",
    };
    
    if ( level > LEVEL_MAX ) level = LEVEL_MAX;
    return buffer[ level ];
}

template <typename T>
TLogLevel Log<T>::from_string(const string& level)
{
    if (level == "DEV"   ) return LEVEL_DEV;
    if (level == "DEBUG" ) return LEVEL_DEBUG;
    if (level == "INFO"  ) return LEVEL_INFO;
    if (level == "WARN"  ) return LEVEL_WARNING;
    if (level == "ERROR" ) return LEVEL_ERROR;
    
    Log<T>().get( LEVEL_WARNING ) << "Unknown logging level '"
                                  << level
                                  << "'. Using INFO level as default.";
    return LEVEL_INFO;
}

class output_to_file
{
public:
    static      FILE*& stream();
    static void output(const string& msg);
};

inline FILE*& output_to_file::stream()
{
    static FILE* st = stderr;
    return st;
}

inline void output_to_file::output(const string& msg)
{
    FILE* st = stream();
    if ( st ){
        fprintf( st , "%s", msg.c_str());
        fflush ( st );
    }
}

class LOG : public Log<output_to_file> {};

#ifndef LOG_MAX_LEVEL
#define LOG_MAX_LEVEL LEVEL_MAX
#endif

#define LOG( level ) \
if (level > LOG_MAX_LEVEL) ;\
else if ( level > LOG::reporting_level() || !output_to_file::stream() ) ; \
else LOG().get( level )

#define FAIL  LOG( LEVEL_ERROR )

// ==================================================================== OcvError

namespace OcvError{

typedef size_t err_t;
    
// built in errs
extern err_t OK;
extern err_t UNKNOWN;
extern err_t INVALID_ARGS;
extern err_t FILE_IO;
extern err_t INCOMPATIBLE;
extern err_t XML_ERR;
extern err_t NOT_READY;
extern err_t OCV_FILE_STORAGE;
extern err_t OCV_EXCEPTION;

bool  init_err();
err_t make_err( string err_msg, size_t err_code = 0 );
err_t last_err();
err_t set_err( err_t err, string desc = "" );
void  print_err( bool reset_errs = true );
}

using namespace OcvError;

// ==================================================================== OcvColor

namespace OcvColor{

#define OCV_BLUE    Scalar(255,0,0)
#define OCV_GREEN   Scalar(0,255,0)
#define OCV_RED     Scalar(0,0,255)
#define OCV_WHITE   Scalar(255,255,255)
    
}

using namespace OcvColor;

#endif
