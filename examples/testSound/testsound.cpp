#include <cassert>

#include "testsound.h"
#include <dtCore/keyboard.h>
#include <dtCore/transform.h>
#include <iostream>
#include <dtUtil/log.h>
#include <dtUtil/datapathutils.h>

// namespaces
using namespace dtAudio;
using namespace dtABC;
using namespace dtCore;
using namespace dtUtil;



// static member variables
const char* TestSoundApp::kSoundFile1  = "sounds/exp57.wav";
const char* TestSoundApp::kSoundFile2  = "sounds/exp35.wav";



IMPLEMENT_MANAGEMENT_LAYER(TestSoundApp)



/**
 * Default constructor.
 *
 * @param configuration file name
 */
TestSoundApp::TestSoundApp(const std::string& configFilename /*= "config.xml"*/)
   : Application(configFilename)
   , mSound(NULL)
{
   // create and configure the audio manager
   AudioManager::Instantiate();

   // pre-load the sound files
   // (an optional optimization)
   AudioManager::GetInstance().LoadFile(kSoundFile1);
   AudioManager::GetInstance().LoadFile(kSoundFile2);

   // get a single sound from the manager
   // and load the sound file into it
   mSound = AudioManager::GetInstance().NewSound();
   assert(mSound);

   mSound->LoadFile(kSoundFile1);

   // set the callbacks for our sound
   // since we're holding the pointer to this sound
   // these callbacks are optional
   mSound->SetPlayCallback(SoundStartedCB, this);
   mSound->SetStopCallback(SoundStoppedCB, this);

   CreateHelpLabel();
}

/**
 * Destructor.
 */
TestSoundApp::~TestSoundApp()
{
   // release our one sound
   AudioManager::GetInstance().FreeSound(mSound);

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
bool TestSoundApp::KeyPressed(const dtCore::Keyboard* keyboard, int key)
{
   // pass the keyboard event to the overlords
   bool handled = Application::KeyPressed(keyboard, key);
   if (handled)  /// == true
   {
      return handled;
   }

   // do something depending on which key was pressed
   switch (key)
   {
   case osgGA::GUIEventAdapter::KEY_Space:
      {
         // get a new sound from the manager
         Sound* sound = AudioManager::GetInstance().NewSound();
         assert(sound);

         // load the sound file into our sound
         sound->LoadFile(kSoundFile2);

         // set the callbacks for our sound
         sound->SetPlayCallback(SoundStartedCB, this);
         sound->SetStopCallback(SoundStoppedCB, this);

         // play our sound
         sound->Play();

         // note:
         // since we're NOT holding the pointer to the
         // sound created here, the stop callback must be
         // set to free the sounds after playing.
         // this allows the AudioManager to recycle sound objects.
         handled = true;
      } break;

   case osgGA::GUIEventAdapter::KEY_F1:
      {
         mLabel->SetActive(!mLabel->GetActive());
      } break;

   default:
      {
         // play our one sound.
         // if it's currently playing, this
         // call will have no effect.
         mSound->Play();
         handled = true;
      } break;
   }

   return handled;
}



/**
 * Call back to know when a sound has started.
 *
 * @param Sound pointer to the sound that started
 * @param void pointer to user data
 */
void
TestSoundApp::SoundStartedCB(dtAudio::Sound* sound, void* param)
{
   assert(sound);

   // tell user the sound has started
   Log::GetInstance().LogMessage(Log::LOG_ALWAYS, __FUNCTION__,
      " \"%s\" has started", sound->GetFilename());
}

/**
 * Call back to know when a sound has stopped.
 *
 * @param Sound pointer to the sound that stopped
 * @param void pointer to user data
 */
void
TestSoundApp::SoundStoppedCB(dtAudio::Sound* sound, void* param)
{
   assert(sound);
   assert(param);

   // tell user the sound has stopped
   Log::GetInstance().LogMessage(Log::LOG_ALWAYS, __FUNCTION__,
      " \"%s\" has stopped", sound->GetFilename());

   // don't free the one sound the app is holding
   if (sound == static_cast<TestSoundApp*>(param)->mSound)
   {
      return;
   }

   // free all other sounds
   AudioManager::GetInstance().FreeSound(sound);
}

/**
 * Function that creates the label that explains keyboard inputs
 */
void TestSoundApp::CreateHelpLabel()
{
   mLabel = new dtABC::LabelActor();
   osg::Vec2 testSize(24.5f, 3.5f);
   mLabel->SetBackSize(testSize);
   mLabel->SetFontSize(0.8f);
   mLabel->SetTextAlignment(dtABC::LabelActor::AlignmentEnum::LEFT_CENTER);
   mLabel->SetText(CreateHelpLabelText());
   mLabel->SetEnableDepthTesting(false);
   mLabel->SetEnableLighting(false);

   GetCamera()->AddChild(mLabel.get());
   dtCore::Transform labelOffset(-17.0f, 50.0f, 11.25f, 0.0f, 90.0f, 0.0f);
   mLabel->SetTransform(labelOffset, dtCore::Transformable::REL_CS);
   AddDrawable(GetCamera());
}

/**
 * Function that creates the text for the help label
 */
std::string TestSoundApp::CreateHelpLabelText()
{
   std::string testString("");
   testString += "F1: Toggle Help Screen\n";
   testString += "\n";
   testString += "Space: Play explosion sound\n";
   testString += "Any other key: Play gunfire sound\n";

   return testString;
}

/// A simple application that demonstrates the most basic methods
/// for managing sounds.  This application just loads sounds and
/// plays them.  It doesn't demonstrate more advanced functions.
/// See the TestSoundApp class for more information.
int
main(int argc, const char* argv[])
{
   // set the directory to find the sound files & config.xml
   std::string dataPath = dtUtil::GetDeltaDataPathList();
   dtUtil::SetDataFilePathList(dataPath + ";" +
                               dtUtil::GetDeltaRootPath() + "/examples/data" + ";" +
                               dtUtil::GetDeltaRootPath() + "/examples/testSound" + ";");

   RefPtr<TestSoundApp> app = new TestSoundApp("config.xml");

   std::cout << "Push spacebar to hear one sound.  Push any other key to hear the other.\n";

   app->Run();

   return 0;
}

