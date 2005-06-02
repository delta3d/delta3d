#include <cassert>

#include "dtCore/dt.h"
#include "testsound.h"

// namespaces
using namespace   dtAudio;
using namespace   dtABC;
using namespace   dtCore;
using namespace   Producer;



// static member variables
const char* TestSoundApp::kSoundFile1  = "sounds/exp57.wav";
const char* TestSoundApp::kSoundFile2  = "sounds/exp35.wav";



IMPLEMENT_MANAGEMENT_LAYER( TestSoundApp )



/**
* Default constructor.
*
* @param configuration file name
*/
TestSoundApp::TestSoundApp( std::string configFilename /*= "config.xml"*/ )
:  Application(configFilename),
mSound(NULL)
{
   // create and configure the audio manager
   AudioManager::Instantiate();
   AudioManager::GetManager()->Config();

   // pre-load the sound files
   // (an optional optimization)
   AudioManager::GetManager()->LoadWaveFile( kSoundFile1 );
   AudioManager::GetManager()->LoadWaveFile( kSoundFile2 );

   // get a single sound from the manager
   // and load the sound file into it
   mSound   = AudioManager::GetManager()->NewSound();
   assert( mSound );

   mSound->LoadFile( kSoundFile1 );

   // set the callbacks for our sound
   // since we're holding the pointer to this sound
   // these callbacks are optional
   mSound->SetPlayCallback( SoundStartedCB, this );
   mSound->SetStopCallback( SoundStoppedCB, this );

}



/**
* Destructor.
*/
TestSoundApp::~TestSoundApp()
{
   // release our one sound
   AudioManager::GetManager()->FreeSound( mSound );

   // destroy the audio manager
   AudioManager::Destroy();
}



/**
* KeyboardListener override
* Called when a key is pressed.
*
* @param keyboard the source of the event
* @param key the key pressed
* @param character the corresponding character
*/
void
TestSoundApp::KeyPressed(  dtCore::Keyboard*       keyboard,
                         Producer::KeyboardKey   key,
                         Producer::KeyCharacter  character )
{
   // pass the keyboard event to the overlords
   Application::KeyPressed( keyboard, key, character );

   // do something depending on which key was pressed
   switch( key )
   {
   case  Producer::Key_Escape:
      // app should be shutting down now
      break;

   case  Producer::Key_space:
      {
         // get a new sound from the manager
         Sound*   sound = AudioManager::GetManager()->NewSound();
         assert( sound );

         // load the sound file into our sound
         sound->LoadFile( kSoundFile2 );

         // set the callbacks for our sound
         sound->SetPlayCallback( SoundStartedCB, this );
         sound->SetStopCallback( SoundStoppedCB, this );

         // play our sound
         sound->Play();

         // note:
         // since we're NOT holding the pointer to the
         // sound created here, the stop callback must be
         // set to free the sounds after playing.
         // this allows the AudioManager to recycle sound objects.
      }
      break;

   default:
      // play our one sound.
      // if it's currently playing, this
      // call will have no effect.
      mSound->Play();
      break;
   }
}



/**
* Call back to know when a sound has started.
*
* @param Sound pointer to the sound that started
* @param void pointer to user data
*/
void
TestSoundApp::SoundStartedCB( dtAudio::Sound* sound, void* param )
{
   assert( sound );

   // tell user the sound has started
   Notify( ALWAYS, " \"%s\" has started", sound->GetFilename() );
}



/**
* Call back to know when a sound has stopped.
*
* @param Sound pointer to the sound that stopped
* @param void pointer to user data
*/
void
TestSoundApp::SoundStoppedCB( dtAudio::Sound* sound, void* param )
{
   assert( sound );
   assert( param );

   // tell user the sound has stopped
   Notify( ALWAYS, " \"%s\" has stopped", sound->GetFilename() );

   // don't free the one sound the app is holding
   if( sound == static_cast<TestSoundApp*>(param)->mSound )
      return;

   // free all other sounds
   AudioManager::GetManager()->FreeSound( sound );
}

/// A simple application that demonstrates the most basic methods
/// for managing sounds.  This application just loads sounds and
/// plays them.  It doesn't demonstrate more advanced functions.
/// See the TestSoundApp class for more information.
int
main( int argc, const char* argv[] )
{
   // set the directory to find the sound files & config.xml
   SetDataFilePathList( GetDeltaRootPath() + "/examples/testSound/;" +
                        GetDeltaDataPathList()  );

   RefPtr<TestSoundApp> app = new TestSoundApp("config.xml");
   app->Run();

   return   0;
}
