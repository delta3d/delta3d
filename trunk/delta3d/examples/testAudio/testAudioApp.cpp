#include <assert.h>
#include <stack>

#include <globals.h>
#include <system.h>
#include <notify.h>
#include <scene.h>
#include <object.h>

#include "testaudioapp.h"



// name spaces
using namespace   dtABC;
using namespace   dtAudio;
using namespace   std;



// static member variables
//const char*    testAudioApp::kDataPath = "C:/projects/delta3d/examples/testAudio";
const char*    testAudioApp::kDataPath = "C:/projects/delta3d/data";
unsigned int   testAudioApp::kNumSoundFiles(4L);
const char*    testAudioApp::kSoundFile[] =
               {
                  "exp35.wav",
                  "exp57.wav",
                  "WIND.wav",
                  "clunk.wav"
               };
unsigned int   testAudioApp::kNumGfxFiles(4L);
const char*    testAudioApp::kGfxFile[] =
               {
                  "box.flt",
                  "ground.flt",
                  "explosion.osg",
                  "smoke.osg"
               };



testAudioApp::testAudioApp( string configFilename /*= ""*/ )
:  Application(configFilename),
   mSndGain(1.0f),
   mSndPitch(1.0f),
   mLooping(false)
{
   dtCore::SetDataFilePathList( kDataPath );

   AddSender( dtCore::System::GetSystem() );

   AudioManager::Instantiate();

   AudioConfigData   data(32L, true);
   AudioManager::GetManager()->Config( data );

   for( unsigned int ii(0L); ii < kNumSoundFiles; ii++ )
   {
      AudioManager::GetManager()->LoadWaveFile( kSoundFile[ii] );
   }

   for( unsigned int ii(0L); ii < kNumGfxFiles; ii++ )
   {
      LoadGfxFile( kGfxFile[ii] );
   }

   dtCore::Notify( dtCore::ALWAYS, " " );
   dtCore::Notify( dtCore::ALWAYS, "   [ASDF]    plays a sound" );
   dtCore::Notify( dtCore::ALWAYS, "   [0-9]     sets gain" );
   dtCore::Notify( dtCore::ALWAYS, "   [+|-]     rase|lower pitch" );
   dtCore::Notify( dtCore::ALWAYS, "   [L]       set|unset all sounds looping" );
   dtCore::Notify( dtCore::ALWAYS, "   [space]   stops all sounds" );
   dtCore::Notify( dtCore::ALWAYS, "   [enter]   rewinds all sounds" );
   dtCore::Notify( dtCore::ALWAYS, "   [pause]   pause|unpause all sounds" );
   dtCore::Notify( dtCore::ALWAYS, " " );
}



testAudioApp::~testAudioApp()
{
   StopAllSounds();
   FreeAllStoppedSounds( true );

   for( unsigned int ii(0L); ii < kNumSoundFiles; ii++ )
   {
      AudioManager::GetManager()->UnloadWaveFile( kSoundFile[ii] );
   }

   AudioManager::Destroy();

   RemoveSender( dtCore::System::GetSystem() );
}



void
testAudioApp::PreFrame( const double deltaFrameTime )
{
   Application::PreFrame( deltaFrameTime );
   FlushQueuedSounds();
}



void
testAudioApp::Frame( const double deltaFrameTime )
{
   Application::Frame( deltaFrameTime );
}



void
testAudioApp::PostFrame( const double deltaFrameTime )
{
   Application::PostFrame( deltaFrameTime );
   FreeAllStoppedSounds();
}



void
testAudioApp::KeyPressed(  dtCore::Keyboard*       keyboard,
                           Producer::KeyboardKey   key,
                           Producer::KeyCharacter  character   )
{
   dtABC::Application::KeyPressed( keyboard, key, character );

   switch( key )
   {
      case  Producer::Key_A:
         LoadPlaySound( kSoundFile[0L] );
         break;

      case  Producer::Key_S:
         LoadPlaySound( kSoundFile[1L] );
         break;

      case  Producer::Key_D:
         LoadPlaySound( kSoundFile[2L] );
         break;

      case  Producer::Key_F:
         LoadPlaySound( kSoundFile[3L] );
         break;

      case  Producer::Key_0:
      case  Producer::Key_KP_Insert:
         ChangeSoundGain( 0.0f / 9.0f );
         break;

      case  Producer::Key_1:
      case  Producer::Key_KP_End:
         ChangeSoundGain( 1.0f / 9.0f );
         break;

      case  Producer::Key_2:
      case  Producer::Key_KP_Down:
         ChangeSoundGain( 2.0f / 9.0f );
         break;

      case  Producer::Key_3:
      case  Producer::Key_KP_Page_Down:
         ChangeSoundGain( 3.0f / 9.0f );
         break;

      case  Producer::Key_4:
      case  Producer::Key_KP_Left:
         ChangeSoundGain( 4.0f / 9.0f );
         break;

      case  Producer::Key_5:
      case  Producer::Key_KP_Begin:
         ChangeSoundGain( 5.0f / 9.0f );
         break;

      case  Producer::Key_6:
      case  Producer::Key_KP_Right:
         ChangeSoundGain( 6.0f / 9.0f );
         break;

      case  Producer::Key_7:
      case  Producer::Key_KP_Home:
         ChangeSoundGain( 7.0f / 9.0f );
         break;

      case  Producer::Key_8:
      case  Producer::Key_KP_Up:
         ChangeSoundGain( 8.0f / 9.0f );
         break;

      case  Producer::Key_9:
      case  Producer::Key_KP_Page_Up:
         ChangeSoundGain( 9.0f / 9.0f );
         break;

      case  Producer::Key_minus:
      case  Producer::Key_KP_Subtract:
         ChangeSoundPitch( 0.9f );
         break;

      case  Producer::Key_equal:
      case  Producer::Key_KP_Add:
         ChangeSoundPitch( 1.1f );
         break;

      case  Producer::Key_L:
         ToggleSoundLooping();
         break;

      case  Producer::Key_Pause:
         PauseAllSounds();
         break;

      case  Producer::Key_Return:
      case  Producer::Key_KP_Enter:
         RewindAllSounds();
         break;

      case  Producer::Key_space:
         StopAllSounds();
         break;

      default:
         break;
   }
}



void
testAudioApp::LoadPlaySound( const char* fname )
{
   assert( fname );

   dtCore::Notify( dtCore::ALWAYS, " LoadPlaySound( %s )", fname );

   Sound*   snd = AudioManager::GetManager()->NewSound();
   assert( snd );

   snd->LoadFile( fname );
   snd->SetGain( mSndGain );
   snd->SetPitch( mSndPitch );
   snd->SetLooping( mLooping );
   snd->Play();
   mQueued.push( snd );
}



void
testAudioApp::StopAllSounds( void )
{
   Sound*   snd(NULL);
   SND_ITR  iter(NULL);
   for( iter = mActive.begin(); iter != mActive.end(); iter++ )
   {
      snd   = *iter;
      if( snd == NULL )
         continue;

      if( ! snd->IsStopped() )
      {
         snd->Stop();

         dtCore::Notify( dtCore::ALWAYS, " StopAllSounds( %s )", snd->GetFilename() );
      }
   }
}



void
testAudioApp::FreeAllStoppedSounds( bool forced /*= false*/ )
{
   SND_ITR              iter(NULL);
   std::stack<SND_ITR>  stk;
   Sound*               snd(NULL);

   for( iter = mActive.begin(); iter != mActive.end(); iter++ )
   {
      snd   = *iter;
      if( snd == NULL )
         continue;

      if( snd->IsStopped() || forced )
      {
         stk.push( iter );
      }
   }

   while( stk.size() )
   {
      iter  = stk.top();
      stk.pop();

      snd   = *iter;
      if( snd == NULL )
         continue;

      dtCore::Notify( dtCore::ALWAYS, " FreeAllStoppedSounds( %s )", snd->GetFilename() );

      AudioManager::GetManager()->FreeSound( snd );
      mActive.erase( iter );
   }
}



void
testAudioApp::FlushQueuedSounds( void )
{
   Sound*   snd(NULL);

   while( mQueued.size() )
   {
      snd   = mQueued.front();
      if( snd == NULL )
      {
         mQueued.pop();
         continue;
      }

      if( ! snd->IsStopped() )
      {
         mQueued.pop();
         mActive.push_back( snd );
         continue;
      }

      break;
   }
}



void
testAudioApp::ChangeSoundGain( float gain )
{
   mSndGain = gain;

   dtCore::Notify( dtCore::ALWAYS, " ChangeSoundGain( %1.1f )", mSndGain );

   Sound*   snd(NULL);
   SND_ITR  iter(NULL);

   for( iter = mActive.begin(); iter != mActive.end(); iter++ )
   {
      snd   = *iter;
      assert( snd );

      snd->SetGain( mSndGain );
   }
}



void
testAudioApp::ChangeSoundPitch( float pitch )
{
   mSndPitch   *= pitch;

   dtCore::Notify( dtCore::ALWAYS, " ChangeSoundPitch( %1.4f )", mSndPitch );

   Sound*   snd(NULL);
   SND_ITR  iter(NULL);

   for( iter = mActive.begin(); iter != mActive.end(); iter++ )
   {
      snd   = *iter;
      assert( snd );

      snd->SetPitch( mSndPitch );
   }
}



void
testAudioApp::ToggleSoundLooping( void )
{
   mLooping =  !mLooping;

   dtCore::Notify( dtCore::ALWAYS, " ToggleSoundLooping( %s )", (mLooping)? "true": "false" );

   Sound*   snd(NULL);
   SND_ITR  iter(NULL);

   for( iter = mActive.begin(); iter != mActive.end(); iter++ )
   {
      snd   = *iter;
      assert( snd );

      snd->SetLooping( mLooping );
   }
}



void
testAudioApp::PauseAllSounds( void )
{
   Sound*   snd(NULL);
   SND_ITR  iter(NULL);
   for( iter = mActive.begin(); iter != mActive.end(); iter++ )
   {
      snd   = *iter;
      if( snd == NULL )
         continue;

      snd->Pause();

      if( snd->IsPlaying() )
      {
         dtCore::Notify( dtCore::ALWAYS, " PauseAllSounds( %s ) paused", snd->GetFilename() );
      }
      else  if( snd->IsPaused() )
      {
         dtCore::Notify( dtCore::ALWAYS, " PauseAllSounds( %s ) un-paused", snd->GetFilename() );
      }
   }
}



void
testAudioApp::RewindAllSounds( void )
{
   Sound*   snd(NULL);
   SND_ITR  iter(NULL);
   for( iter = mActive.begin(); iter != mActive.end(); iter++ )
   {
      snd   = *iter;
      if( snd == NULL )
         continue;

      snd->Rewind();

      dtCore::Notify( dtCore::ALWAYS, " RewindAllSounds( %s )", snd->GetFilename() );
   }
}



dtCore::Object*
testAudioApp::LoadGfxFile( const char* fname )
{
   if( fname == NULL )
      // no file name, bail...
      return   NULL;

   std::string filename = osgDB::findDataFile( fname );
   if( filename == "" )
   {
      // still no file name, bail...
      dtCore::Notify( dtCore::WARN, "AudioManager: can't load file %s", fname );
      return   NULL;
   }


   dtCore::Object*  fileobj  = new dtCore::Object;
   assert( fileobj );


   // load the graphics file from disk
   bool fileLoaded = false;
   fileLoaded = fileobj->LoadFile( filename );

   if( ! fileLoaded )
   {
      dtCore::Notify( dtCore::WARN, "can't load gfx file '%s'", filename );
      delete   fileobj;
      return   NULL;
   }

   // add the object to the scene
   dtCore::Scene* scene = GetScene();
   assert( scene );

   AddDrawable( fileobj );

   osg::Node*  filenode = fileobj->GetOSGNode();
   assert( filenode );

   filenode->setNodeMask( 0L );
   return   fileobj;
}
