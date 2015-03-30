// =============================================================================
//
//  utils.cpp
//  ocv
//
//  Created by Guy Bar-Nahum on 3/4/15.
//  Copyright (c) 2015 DarwinLabs. All rights reserved.
//
// =============================================================================

// ==================================================================== includes

#include "utils.hpp"
#include <sys/stat.h>

// =============================================================================
//                                                                     key utils
// =============================================================================

// ................................................................ is_printable

bool is_printable( int key )
{
    return ( key > 32 ) && ( key < 127 );
}

// ...................................................................... to_key

void
to_key( int key, string &s_key )
{
    switch( key ){
        case KEY_ESCAPE       : s_key = "<escape>"; break;
        case KEY_SPACE        : s_key = "<space>" ; break;
       
        default : if ( is_printable( key )){
                    s_key = "'"; s_key += (char)key; s_key += "'";
                  }
                  else{
                    s_key  = "<"; s_key += to_string(key); s_key += ">";
                  }
                  break;
    }
}

// =============================================================================
//                                                                    file utils
// =============================================================================
// Can one be more corny than that?! :).
// How many times one has to write those?

// ................................................................. file_exists

bool file_exists( string path )
{
    struct stat st;
    return ( 0 == stat( path.c_str(), &st) );
}

// ................................................................ file_to_path

bool file_to_path( string &path )
{
    // TODO: expand ~ and .
    string user_home = getenv( "HOME" );
    
    // if relative look for it in few places
    vector<string> base_dir = { "", "./", user_home };
    
    for( size_t ix = 0 ; ix < base_dir.size() ; ix++ ){
        
        string full_path = base_dir[ ix ] + path;
        
        if ( file_exists( full_path ) ){
            path = full_path;
            return true;
        }
    }
    
    return false;
}

// ................................................................ path_to_file
// get base name from long ugly path names, don't ask me why..

char *path_to_file( char* path )
{
    char *file = path;
    char  ch;
    
    // set file as the charachter after the last '/'
    while( ( ch = *path++ ) != '\0' ){
        if ( ch == '/'    ) file = path; // notice: path was ++ already
    }
    
    return file;
}

// =============================================================================
//                                                                   print utils
// =============================================================================

// .................................................................. print_argv

void print_argv( int argc, const char * argv[] )
{
    string cli_str;
    
    // argv[0] is especially ugly.. clean it up
    cli_str = path_to_file( (char *)argv[ 0 ] );
    
    for( int ix = 1 ; ix < argc ; ix++ ){
        
        // has whitespace? put qoutes around it
        char ch;
        bool qoutes = false;
        const char *p = argv[ ix ];
        
        while( ( ch  = *p++ ) != '\0' )
            if ( ch <= ' '  ){ qoutes = true; break; }
        
        // print one arg
                      cli_str += " ";
        if ( qoutes ) cli_str += "\"";
                      cli_str += argv[ ix ];
        if ( qoutes ) cli_str += "\"";
    }
    
    LOG( LEVEL_INFO ) << cli_str;
}

// =============================================================================
//                                                                ocvError utils
// =============================================================================

namespace OcvError
{

static vector<string>  err_codes;
static vector<string>  err_msg;
static err_t           err_last;

// built in errors
err_t OK;
err_t UNKNOWN;
err_t INVALID_ARGS;
err_t FILE_IO;
err_t INCOMPATIBLE;
err_t XML_ERR;
err_t NOT_READY;
err_t OCV_FILE_STORAGE;
err_t OCV_EXCEPTION;
    
// force the following code to be executed before main()!
static bool init_ok = init_err();

// .................................................................... init_err

bool  init_err()
{
    err_codes.clear();
    err_msg.clear();
    
    // setup common errors
    OK               = make_err( "OK" );
    UNKNOWN          = make_err( "Unknown"         );
    INVALID_ARGS     = make_err( "Inavlid args"    );
    FILE_IO          = make_err( "File io"         );
    INCOMPATIBLE     = make_err( "Incompatible"    );
    XML_ERR          = make_err( "XML error"       );
    NOT_READY        = make_err( "Not ready"       );
    OCV_FILE_STORAGE = make_err( "OCV FileStorage" );
    OCV_EXCEPTION    = make_err( "OCV Exception"   );
    
    err_last = OK;
    return true;
}

// .................................................................... make_err
    
err_t make_err( string err_msg, size_t err_code )
{
    size_t code = err_codes.size();
    
    // Can't change item 0
    if ( err_code && ( code > err_code ) ){
        code = err_code ;
        err_codes[ code ] = err_msg;
    }
    else{
        err_codes.push_back( err_msg );
    }
    
   return (err_t)(code);
}

// .................................................................... last_err

err_t last_err(){ return err_last; }

// ..................................................................... set_err

err_t set_err( err_t err, string desc )
{
    size_t err_ix = (size_t)err;
    
    if ( err_ix < err_codes.size() ){
        desc += " (" + err_codes[ err_ix ] + ")";
    }
    else{
        
        desc += " (Unknown Error:";
        desc += err_ix;
        desc += ")";
    }
    
    string *msg = new string( desc );
    err_msg.push_back( *msg );
    
    err_last = err;
    
    return err;
}

// ................................................................... print_err
    
void print_err( bool reset_errs )
{
    for( size_t ix = 0 ; ix < err_msg.size(); ix++ ){
        LOG( LEVEL_ERROR ) <<  err_msg[ ix ] ;
    }
    
    if ( reset_errs ){
        // TODO: I always forget if clear also deletes the pointers..
        // It should right?
        err_msg.clear();
        err_last = OK;
    }
}
    
} // namespace OcvError


// =============================================================================
//                                                  geometry / conversions utils
// =============================================================================

// =============================================================================
//                                                                        camera
// =============================================================================

bool import_camera_mat( string xml, Mat &cam )
{
    bool ok = true;
    FileStorage fs;
    
    try{
        fs.open( xml, FileStorage::READ );
        ok = fs.isOpened();
        if (ok){
            fs["Camera_Matrix"] >> cam;
        }
        else{
            string msg  = "could not open "; msg += xml;
            set_err( FILE_IO, msg );
        }
    }
    catch( Exception e ){
        set_err( FILE_IO, e.what() );
        ok = false;
    }
    
    if ( ok ){
        cout << "import_camera_mat : " << cam << endl;
        // Perform some sanity checks on Camera Matrix
        ok = !cam.empty() && (cam.rows == 3) && (cam.cols == 3);
        if (!ok){
            string msg = "invalid camera matrix in "; msg += xml;
            set_err( INVALID_ARGS, msg );
        }
    }
    
    return ok;
}

// ............................................................... obj_transform
bool gray( const Mat &mat, Mat &gray )
{
    bool ok = true;
    
    switch( mat.channels() ){
            
        case 3 : cvtColor( mat, gray, CV_BGR2GRAY  ); break;
        case 4 : cvtColor( mat, gray, CV_BGRA2GRAY ); break;
        case 1 : gray = mat; break;
        
        default: ok = false; break;
    }
    
    return ok;
}

// ................................................................... poly_area

double poly_area( const vector<Point2f> &poly )
{
    double area = 0;
    for (size_t ix = 0; ix < poly.size(); ix++){
        
        size_t next_ix =  (ix+1)%poly.size();
        
        double dX = poly[ next_ix ].x - poly[ ix ].x;
        double dY = poly[ next_ix ].y + poly[ ix ].y;
        
        area += dX * dY;  // This is the integration step.
    }
    
    area = abs( area / 2 );
    return area;
}

// .................................................................... mat_area

double mat_area( const Mat &m ){ return (double)( m.rows * m.cols ); }

// ............................................................. round_points_2f

void convert_round_points_2f( vector<Point2f> &points_v )
{
    for( size_t ix = 0 ; ix < points_v.size(); ix++ ){
        points_v[ ix ].x = (int)cvRound( points_v[ ix ].x );
        points_v[ ix ].y = (int)cvRound( points_v[ ix ].y );
    }
}

// ....................................................................... prune

// instanciate template
void prune_vector(vector<Point2f> &v, vector<size_t> keep_ix );
void prune_vector(vector<float> &v, vector<size_t> keep_ix );

void prune_vector(vector<uchar> &v, vector<size_t> keep_ix );

// =============================================================================
//                                                                    draw utils
// =============================================================================

void drawArrows(Mat& draw_mat,
                const vector<Point2f>& fromPts,
                const vector<Point2f>& toPts,
                const Scalar&          color,
                const int line_thickness    ,
                const double size           ,
                const double threshold      )
{
    for (size_t ix = 0; ix < fromPts.size(); ix++){
        
        Point p = fromPts[ ix ];
        Point q = toPts  [ ix ];
        
        double angle      = atan2( (double) p.y - q.y, (double) p.x - q.x );
        double hypotenuse = sqrt ( (double)(p.y - q.y)*(p.y - q.y) +
                                   (double)(p.x - q.x)*(p.x - q.x) );
            
        if ( hypotenuse < threshold ) continue;
            
        // Here we lengthen the arrow by a factor of three.
        q.x = (int) (p.x - size * hypotenuse * cos(angle));
        q.y = (int) (p.y - size * hypotenuse * sin(angle));
            
        // Now we draw the main line of the arrow.
        line( draw_mat, p, q, color, line_thickness);
        
        // Now draw the tips of the arrow. I do some scaling so that the
        // tips look proportional to the main line of the arrow.
        
        p.x = (int) (q.x + 5. * cos(angle + CV_PI / 6));
        p.y = (int) (q.y + 5. * sin(angle + CV_PI / 6));
        line( draw_mat, p, q, color, line_thickness );
        
        p.x = (int) (q.x + 5. * cos(angle - CV_PI / 6));
        p.y = (int) (q.y + 5. * sin(angle - CV_PI / 6));
        line( draw_mat, p, q, color, line_thickness );

    }
}

// normalize val from [min,max] into [0,1] range
#define intrpmnmx(val,min,max) (max==min ? 0.0 : ((val)-min)/(max-min))

void drawArrows(Mat& draw_mat,
                const vector<Point2f>& fromPts,
                const vector<Point2f>& toPts  ,
                const vector<uchar>&   use    ,
                const vector<float>&   verror ,
                const Scalar&          color  ,
                const int       line_thickness,
                const double           size   ,
                const double        threshold )
{
    // if we have v_error normalize it wiht min_err / max_err
    double min_err = 0., max_err = 1.;
    bool has_err = verror.size();
    bool has_use = use.size();
    
    Scalar ca = OCV_RED;
    Scalar cb = OCV_BLUE;
    
    if ( has_err ){
        minMaxIdx( verror, &min_err, &max_err, 0, 0, use );
    }
    
    for (size_t ix = 0; ix < fromPts.size(); ix++)
    {
        if ( has_use && !use[ ix ] ) continue;

        double t = ((double)ix) / ((double)fromPts.size());
        double s = 1.0 - t;
        Scalar color = Scalar( t * ca[0] + s * cb[0] ,
                               t * ca[1] + s * cb[1] ,
                               t * ca[2] + s * cb[3] );

        if ( has_err ){
            
            
            // alpha = intrpmnmx( verror[ ix ], min_err, max_err );
            // alpha = 1.0 - alpha;
        
        }

        Point p = fromPts[ ix ];
        Point q = toPts  [ ix ];
        
        double angle      = atan2( (double) p.y - q.y, (double) p.x - q.x );
        double hypotenuse = sqrt ( (double)(p.y - q.y)*(p.y - q.y) +
                                   (double)(p.x - q.x)*(p.x - q.x) );
        
        if (hypotenuse < threshold ) continue;
        
        // Here we lengthen the arrow by a factor of three.
        q.x = (int) (p.x - size * hypotenuse * cos(angle));
        q.y = (int) (p.y - size * hypotenuse * sin(angle));
        
        // Now we draw the main line of the arrow.
        line( draw_mat, p, q, color, line_thickness);
        
        // Now draw the tips of the arrow. I do some scaling so that the
        // tips look proportional to the main line of the arrow.
        
        p.x = (int) (q.x + 5. * cos(angle + CV_PI / 6));
        p.y = (int) (q.y + 5. * sin(angle + CV_PI / 6));
        line( draw_mat, p, q, color, line_thickness );
        
        p.x = (int) (q.x + 5. * cos(angle - CV_PI / 6));
        p.y = (int) (q.y + 5. * sin(angle - CV_PI / 6));
        line( draw_mat, p, q, color, line_thickness );
    }
}
