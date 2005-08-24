#include <string>

#include <dtCore/globals.h>
#include <dtABC/widget.h>

#include "viewstate.h"
#include "UserInterface.h"
#include "viewwindow.h"



/// namespaces
using namespace   dtABC;


///static variables
static   ViewState   bogus( "bogus" );

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
         const char* ViewWindow::DEF_PATH = "C:/";
#else
         const char* ViewWindow::DEF_PATH = "/";
#endif // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)



ViewWindow::ViewWindow()
:  MyParent(),
   mPath(DEF_PATH),
   mCurFile(NULL),
   mContainer(NULL),
   mFileLoaded(true)
{
   ctor();
}



ViewWindow::ViewWindow( int w, int h, const char* l /*= 0L*/ )
:  MyParent(w, h, l),
   mPath(DEF_PATH),
   mCurFile(NULL),
   mContainer(NULL),
   mFileLoaded(true)
{
   ctor();
}



ViewWindow::ViewWindow( int x, int y, int w, int h, const char* l /*= 0L*/ )
:  MyParent(x, y, w, h, l),
   mPath(DEF_PATH),
   mCurFile(NULL),
   mContainer(NULL),
   mFileLoaded(true)
{
   ctor();
}



ViewWindow::~ViewWindow()
{
}



void
ViewWindow::show( void )
{
   MyParent::show();

   Viewer::GetState(mCurFile);
   mContainer->UpdateSettings( mCurFile );
}



void
ViewWindow::OnMessage( dtCore::Base::MessageData* data )
{
   assert( data );


   if( data->message == msgKeyboardEvent )
   {
      assert( data->userData );
      KeyboardEventHandler( *(reinterpret_cast<KeyboardEvent*>(data->userData)) );
      return; //
   }

   MyParent::OnMessage( data );
}

void
ViewWindow::FileLoaded( bool loaded, const char* filename )
{
   if( !loaded )
      fl_alert("Error: Can't load file-\n%s", filename );

   mFileLoaded = loaded;
}

const char*
ViewWindow::GetPath( void )
{
   return   mPath.c_str();
}



void
ViewWindow::SetPath( const char* path )
{
   mPath = path;
   dtCore::SetDataFilePathList(mPath);
}



void
ViewWindow::Container( UserInterface* container )
{
   assert( container );
   mContainer  = container;
}



bool
ViewWindow::LoadFile( const char* file )
{
   std::string filename = file;
   assert( filename != "" );

   // first test if filename already exists
   FILE_LIST::iterator it;
   for( it = mFileList.begin(); it < mFileList.end(); it++ )
   {
      ViewState*  vs(*it);
      if( vs->GetFilename() == filename )
      {
         // file exists, just select it instead
         SelectFile( vs->GetIndex() );
         return true;
      }
   }

   // create a default state for this file
   ViewState*  vs = new ViewState( filename, mFileList.size() );
   assert( vs );

   // load this file into the scene
   Viewer::LoadFile( vs );

   //if the file loaded, add it to our list
   if (mFileLoaded)   mFileList.push_back( vs );

   //return the file loading status
   return mFileLoaded;
}

bool
ViewWindow::SaveFileAs(const char *filename)
{
   assert(filename!=NULL);

   Viewer::SaveFileAs((char*)filename);

   return true;
}

void
ViewWindow::SelectFile( unsigned int indx )
{
   if( indx >= mFileList.size() )
      return;

   // save the old file state
   Viewer::GetState(mCurFile);

   // switch to a new file
   mCurFile = mFileList.at( indx );

   // set the new file state
   Viewer::SetState(mCurFile);

   mContainer->UpdateSettings( mCurFile );
}



void
ViewWindow::SetDisplay( unsigned int state, bool value /*= true*/ )
{
   switch( state )
   {
      case  ViewState::COMPASS:
      case  ViewState::XY_PLANE:
      case  ViewState::YZ_PLANE:
      case  ViewState::ZX_PLANE:
      case  ViewState::SCENETXT:
      case  ViewState::SCENELIGHT:
      case  ViewState::SCENEPOLY | ViewState::SCENEWIRE:       // scribe = polygon + wireframe
      case  ViewState::TEXTURE:
      case  ViewState::LIGHTING:
      case  ViewState::POLYGON | ViewState::WIREFRAME:         // scribe = polygon + wireframe
         mCurFile->SetDisplayFlag( ViewState::DISPLAY_FLAG(state), value );
         break;

      case  ViewState::SCENEPOLY:                              // polygon = not wireframe
         mCurFile->SetDisplayFlag( ViewState::SCENEPOLY, value );
         mCurFile->SetDisplayFlag( ViewState::SCENEWIRE, !value );
         break;

      case  ViewState::SCENEWIRE:                              // wireframe = not polygon
         mCurFile->SetDisplayFlag( ViewState::SCENEWIRE, value );
         mCurFile->SetDisplayFlag( ViewState::SCENEPOLY, !value );
         break;

      case  ViewState::POLYGON:                                // polygon = not wireframe
         mCurFile->SetDisplayFlag( ViewState::POLYGON, value );
         mCurFile->SetDisplayFlag( ViewState::WIREFRAME, !value );
         break;

      case  ViewState::WIREFRAME:                              // wireframe = not polygon
         mCurFile->SetDisplayFlag( ViewState::WIREFRAME, value );
         mCurFile->SetDisplayFlag( ViewState::POLYGON, !value );
         break;

      case  ViewState::RESET:
         Viewer::ResetCam();
         return;
         break;

      default:
         return;
         break;

   }

   Viewer::SetState(mCurFile);
}



void
ViewWindow::SetMotion( unsigned int state )
{
   switch( state )
   {
      case  ViewState::FLY:
         mCurFile->SetMotionFlag( ViewState::FLY );
         break;

      case  ViewState::ORBIT:
         mCurFile->SetMotionFlag( ViewState::ORBIT );
         break;

      case  ViewState::UFO:
         mCurFile->SetMotionFlag( ViewState::UFO );
         break;

      case  ViewState::WALK:
         mCurFile->SetMotionFlag( ViewState::WALK );
         break;

      default:
         return;
         break;

   }

   Viewer::SetState(mCurFile);
}



void
ViewWindow::SetJoystick( unsigned int state, bool value /*= true*/ )
{
   switch( state )
   {
      case  ViewState::JOY_1:
         mCurFile->SetJoystickFlag( ViewState::JOY_1, value );
         break;

      case  ViewState::JOY_2:
         mCurFile->SetJoystickFlag( ViewState::JOY_2, value );
         break;

      default:
         return;
         break;

   }

   Viewer::SetState(mCurFile);
}


/** Expects one of two possible formats:
 *  viewer.exe <fully_qualified_filename> ...    
 *  viewer.exe -p <search_path> <filename> ...   
 */
void
ViewWindow::CommandLine( int argc, char** argv )
{
   assert( mContainer );

   if( argc <= 1 )
      return;

   std::string path("");
   bool        flag(false);
   bool        usage(true);
   bool        load(false);
   int         len(0L);

   for( int ii(1); ii < argc; ii++ )
   {
      len   = strlen( argv[ii] );
      for( int jj(0); jj < len; jj++ )
      {
         // make sure all slashes are consistent
         if( argv[ii][jj] == '\\' )
         {
            argv[ii][jj]   = '/';
         }
      }

      // check if arg is a path flag
      if(   ( strcmp( argv[ii], "/p" ) == 0 )   ||
         ( strcmp( argv[ii], "/P" ) == 0 )   ||
         ( strcmp( argv[ii], "-p" ) == 0 )   ||
         ( strcmp( argv[ii], "-P" ) == 0 )   )
      {
         flag  = true;
      }
   }

   if (!flag)
   {
      //set our default path to the current working directory
      char path[512];
      ulGetCWD(path, strlen(path) );
      SetPath( path ); 

      for( int ii(1); ii < argc; ii++ )
      {
         // load full path and file name
         mContainer->LoadFile( std::string(argv[ii]) );
        load = true;
      }
   }
   else
   {
      for( int ii(1); ii < argc; ii++ )
      {
         len   = strlen( argv[ii] );

         // check if arg is a path flag
         if(   ( strcmp( argv[ii], "/p" ) == 0 )   ||
            ( strcmp( argv[ii], "/P" ) == 0 )   ||
            ( strcmp( argv[ii], "-p" ) == 0 )   ||
            ( strcmp( argv[ii], "-P" ) == 0 )   )
         {
            flag  = true;
            continue;
         }


         if( flag )
         {
            // last arg was path flag, this arg should be a path
            if( argv[ii][len-1] == '/' )
               path  = std::string(argv[ii]);
            else
               path  = std::string(argv[ii]) + std::string("/");

            SetPath( path.c_str() );
            flag  = false;
            continue;
         }

         if( path == "" )
         {
            // don't load files until a path has been set
            if( usage )
            {
               dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__,
                  "Usage \"Viewer [/p path [file file...]] [/p path [file file...]]...\"" );
               usage   = false;
            }
            dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__,
                "\t\"%s\" was not loaded.", argv[ii] );
            continue;
         }

         // load full path and file name
         mContainer->LoadFile( std::string(path) + std::string(argv[ii]) );
         load = true;
      }
   }

   if( load )
   {
      mContainer->SelectFile( 0 );
   }
}



/// private member functions
void
ViewWindow::ctor( void )
{      
   SetEvent( FL_PUSH );
   SetEvent( FL_RELEASE );
   SetEvent( FL_ENTER );
   SetEvent( FL_LEAVE );
   SetEvent( FL_DRAG );
   SetEvent( FL_MOVE );
   SetEvent( FL_MOUSEWHEEL );
   SetEvent( FL_FOCUS );
   SetEvent( FL_UNFOCUS );
   SetEvent( FL_KEYDOWN );
   SetEvent( FL_KEYUP );

   mCurFile = &bogus;
}



void
ViewWindow::KeyboardEventHandler( const KeyboardEvent& ev )
{
   assert( mContainer );

   switch( ev.event )
   {
   case  KeyboardEvent::KEYDOWN:
         break;

      case  KeyboardEvent::KEYUP:
         {
            if( ev.key == ev.KEY_w )
               mContainer->ToggleWireframeScene();
            else  if( ev.key == ev.KEY_W )
               mContainer->ToggleWireframeModel();
            else  if( ev.key == ev.KEY_l )
               mContainer->ToggleLightingScene();
            else  if( ev.key == ev.KEY_L )
               mContainer->ToggleLightingModel();
            else  if( ev.key == ev.KEY_t )
               mContainer->ToggleTextureScene();
            else  if( ev.key == ev.KEY_T )
               mContainer->ToggleTextureModel();
            else  if( ( ev.key == ev.KEY_c ) || ( ev.key == ev.KEY_C ) )
               mContainer->ToggleCompass();
            else  if( ( ev.key == ev.KEY_x ) || ( ev.key == ev.KEY_X ) )
               mContainer->ToggleXYPlane();
            else  if( ( ev.key == ev.KEY_y ) || ( ev.key == ev.KEY_Y ) )
               mContainer->ToggleYZPlane();
            else  if( ( ev.key == ev.KEY_z ) || ( ev.key == ev.KEY_Z ) )
               mContainer->ToggleZXPlane();
            else  if( ( ev.key == ev.KEY_r ) || ( ev.key == ev.KEY_R ) || ( ev.key == ev.KEY_Space ) )
               mContainer->ResetCam();
            else  if( ev.key == ev.KEY_s )
               GetCamera()->SetNextStatisticsType();
         }
         break;

      default:
         break;
   }
}
