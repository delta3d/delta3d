/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
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
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * Teague Coonan
 */

#ifndef DELTA_SOUND_BROWSER
#define DELTA_SOUND_BROWSER

#include <dtEditQt/resourceabstractbrowser.h>
#include <dtUtil/mswinmacros.h>

#if defined(DELTA_WIN32)
#   include <alc.h>
#   include <al.h>
#elif defined(__APPLE__)
#   include <OpenAL/alc.h>
#   include <OpenAL/al.h>
#else
#   include <AL/al.h>
#   include <AL/alc.h>
#endif

class QAction;
class QGroupBox;
class QGridLayout;
class QPushButton;
class QKeyEvent;
class QContextMenuEvent;

namespace dtEditQt
{

   /**
    * @class SoundBrowser
    * @brief Lists sounds that can be selected and played
    */
   class SoundBrowser : public ResourceAbstractBrowser
   {
      Q_OBJECT

   public:
      /**
       * Constructor
       */
      SoundBrowser(dtCore::DataType& type, QWidget* parent = 0);

      /**
       * Destructor
       */
      virtual ~SoundBrowser();

      /**
       * Derived from our abstract base class. When a tree selection changes
       * this method will be called. This will handle our context sensitive buttons
       * so we know when to play a sound.
       */
      void selectionChanged();

   private slots:
      /**
       * Slot - handles the event when the play button is pressed
       */
      void playSelected();

      /**
       * Slot - handles the event when the stop button is pressed
       */
      void stopSelected();

   protected:
      /**
       * Added an event filter to capture keyboard events sent to the tree widget
       * so we can trap the enter key and play sounds. All other events are
       * passed on to the parent. This has been overridden from the base abstract
       * class to provide the sound browser specific functionality.
       * @param Event
       * @return bool if the event was not captured for the appropriate widget
       */
      bool eventFilter(QObject* target, QEvent* e);

   private:
      void deleteItemEvent();
      void doubleClickEvent();

      /**
       * Loads and plays a selected sound file
       * @param fname - String identifier for the name of our resource
       * @return bool true on success
       */
      bool playSound();

      /**
       * Stops playing a sound file loaded in the audiomanager
       * @brief This stops the currently playing sound file
       */
      void stopSound();

      /**
       * This defines the layout for the buttons
       * @return QGroupBox layout widget
       */
      QGroupBox* previewSoundGroup();

      /**
       * This defines the layout for the sound list
       * @return QGroupBox layout widget
       */
      QGroupBox* listSoundGroup();

      // Button Objects
      QPushButton* mPlayBtn;
      QPushButton* mStopBtn;

      ALuint mSoundBuffers[1];
      ALuint mSoundSources[1];

      // Layout Objects
      QGridLayout* mGrid;
   };

} // namespace dtEditQt

#endif // DELTA_SOUND_BROWSER
