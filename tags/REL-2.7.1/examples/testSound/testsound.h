/* -*-c++-*-
* testSound - testsound (.h & .cpp) - Using 'The MIT License'
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
#ifndef DELTA_TEST_SOUND_APP
#define DELTA_TEST_SOUND_APP

#include "dtAudio/dtaudio.h"
#include <dtABC/application.h>
#include <dtABC/labelactor.h>

/// TestSoundApp class, the main application class

/** The Application class for this test.  It handles the basic requirments
* for a dtABC::Application.
* 
* In the constructor, this class will create and configure the
* dtAudio::AudioManager, pre-load some sound files (optional), and
* get a single sound object for use through out the durration of this app.
*
* The destructor, frees the single sound object and shuts down the
* dtAudio::AudioManager.
* 
* The KeyPress function demonstrates two ways to play sounds.  Any key other
* than the space-bar will play the one sound that this app is holding on to.
* If that one sound is currently playing, extra calls to it's play function
* will have no effect.  Pressing the space bar will create as many transient
* sound objects as needed and play them.  This app intentionally does not
* save pointers to those transient sound objects.
*
* The SoundStartedCB is just a window dressing callback to alert the user
* that a sound started playing.
*
* The SoundStoppedCB also alerts the user that a sound stopped, but is more
* important, in this case, because it also frees all the transient sounds
* that were not saved by this app when they were created.
*
* Both methods of sound managment are appropriate depending on the user's
* needs.  If a small number of sounds are needed, holding pointers to them
* for the durration of the application may be the better choice.  However,
* if large numbers of sound must be played, using the transient sound
* method is more efficient, since the dtAudio::AudioManager will recycle
* sound objects after they have been freed.
*/
class TestSoundApp   :  public   dtABC::Application
{
   DECLARE_MANAGEMENT_LAYER( TestSoundApp )

private:
   static   const char* kSoundFile1;
   static   const char* kSoundFile2;

public:

   /**
   * Default constructor.
   *
   * @param configuration file name
   */
   TestSoundApp( const std::string& configFilename = "config.xml" );
protected:
   /**
   * Destructor.
   */
   virtual                    ~TestSoundApp();
public:
   /**
   * KeyboardListener override
   * Called when a key is pressed.
   *
   * @param keyboard the source of the event
   * @param key the key pressed
   * @param character the corresponding character
   */
   virtual  bool              KeyPressed(const dtCore::Keyboard* keyboard, int key);

private:
   /**
   * Call back to know when a sound has started.
   *
   * @param Sound pointer to the sound that started
   * @param void pointer to user data
   */
   static   void              SoundStartedCB( dtAudio::Sound* sound, void* param );

   /**
   * Call back to know when a sound has stopped.
   *
   * @param Sound pointer to the sound that stopped
   * @param void pointer to user data
   */
   static   void              SoundStoppedCB( dtAudio::Sound* sound, void* param );

private:
   /**
    * Function that creates the label that explains keyboard inputs
    */
   void CreateHelpLabel();
   /**
    * Function that creates the text for the help label
    */
   std::string CreateHelpLabelText();

   /**
   * A single sound this app holds onto for the duration.
   */
   dtAudio::Sound*   mSound;

   /**
    * The LabelActor that displays the keyboard functionality for this test
    */
   dtCore::RefPtr<dtABC::LabelActor> mLabel;
};

#endif // DELTA_TEST_SOUND_APP
