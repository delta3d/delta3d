#include <string>

#include <notify.h>
#include <widget.h>

#include "viewstate.h"
#include "userinterface.h"
#include "viewwindow.h"



/// namespaces
using namespace   dtABC;


///static variables
static   ViewState   bogus( "bogus" );
         const char* ViewWindow::DEF_PATH = "C:/";



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

   SendMessage( msgGetState, mCurFile );
   mContainer->UpdateSettings( mCurFile );
}



void
ViewWindow::OnMessage( dtCore::Base::MessageData* data )
{
   assert( data );

   MyParent::OnMessage( data );

   if( data->message == msgKeyboardEvent )
   {
      assert( data->userData );
      KeyboardEventHandler( *(reinterpret_cast<KeyboardEvent*>(data->userData)) );
   }
   else  if (data->message == "fileNotLoaded")
   {
      //the file we're trying to load failed - don't create GUI stuff for it.
      mFileLoaded = false;
      fl_alert("Error: Can't load file-\n%s", (char*)data->userData );

   }
   else  if (data->message == "fileLoaded")
   {
      //the file we're trying to load was loaded
      mFileLoaded = true;
   }
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
   SendMessage( msgSetPath, &mPath );
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
   SendMessage( msgLoadFile, reinterpret_cast<void*>(vs) );

   //if the file loaded, add it to our list
   if (mFileLoaded)   mFileList.push_back( vs );

   //return the file loading status
   return mFileLoaded;
}

bool
ViewWindow::SaveFileAs(const char *filename)
{
   assert(filename!=NULL);

   SendMessage(msgSaveFileAs, (void*)filename );

   return true;
}

void
ViewWindow::SelectFile( unsigned int indx )
{
   if( indx >= mFileList.size() )
      return;

   // save the old file state
   SendMessage( msgGetState, mCurFile );

   // switch to a new file
   mCurFile = mFileList.at( indx );

   // set the new file state
   SendMessage( msgSetState, mCurFile );
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
         SendMessage( msgResetCam );
         return;
         break;

      default:
         return;
         break;

   }

   SendMessage( msgSetState, reinterpret_cast<void*>(mCurFile) );
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

   SendMessage( msgSetState, reinterpret_cast<void*>(mCurFile) );
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

   SendMessage( msgSetState, reinterpret_cast<void*>(mCurFile) );
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
               dtCore::Notify(dtCore::NOTICE) << "Usage \"Viewer [/p path [file file...]] [/p path [file file...]]...\"" << std::endl << std::endl;
               usage   = false;
            }
            dtCore::Notify(dtCore::NOTICE) << "\t\"" << argv[ii] << "\" was not loaded." << std::endl;
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
      case  ev.KEYDOWN:
         break;

      case  ev.KEYUP:
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
         }
         break;

      default:
         break;
   }
}
