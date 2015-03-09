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

inline TimePoint now_time()
{
    return HighResClock::now();
}

// Primitive duration, there has to be a better way..
inline HighResClock::duration duration_time()
{
    static TimePoint base_time;
    static bool is_first = true;
    
    if ( is_first ){
        base_time = now_time();
        is_first = false;
    }
    
    return now_time() - base_time ;
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

template <typename T>
ostringstream& Log<T>::get( TLogLevel level )
{
    os << "- " << duration_time();
    
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
    static TLogLevel reporting_level = LEVEL_DEV;
    return reporting_level;
}

template <typename T>
string Log<T>::to_string( TLogLevel level )
{
    static const char* const buffer[] = { "ERROR"   ,
                                          "WARNING" ,
                                          "INFO"    ,
                                          "DEBUG"   ,
                                          "DEV"     ,
                                        };
    
    if ( level > LEVEL_MAX ) level = LEVEL_MAX;
    return buffer[ level ];
}

template <typename T>
TLogLevel Log<T>::from_string(const string& level)
{
    if (level == "DEV"      ) return LEVEL_DEV;
    if (level == "DEBUG"    ) return LEVEL_DEBUG;
    if (level == "INFO"     ) return LEVEL_INFO;
    if (level == "WARNING"  ) return LEVEL_WARNING;
    if (level == "ERROR"    ) return LEVEL_ERROR;
    
    Log<T>().get(LEVEL_WARNING) << "Unknown logging level '"
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

#endif
