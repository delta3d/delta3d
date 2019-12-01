/*
 * Delta3D Open Source Game and Simulation Engine
 * Simulation, Training, and Game Editor (STAGE)
 * Copyright (C) 2005, BMH Associates, Inc.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Teague Coonan
 */
#include <prefix/stageprefix.h>
#include <QtCore/QDir>
#include <QtGui/QHeaderView>

#include <QtGui/QHBoxLayout>
#include <QtGui/QGridLayout>
#include <QtCore/QString>

#include <QtGui/QGroupBox>

#include <QtGui/QPushButton>
#include <QtGui/QPixmap>
#include <QtGui/QIcon>

#include <QtGui/QAction>
#include <QtGui/QContextMenuEvent>

#include "dtEditQt/soundbrowser.h"
#include "dtEditQt/resourcetreewidget.h"
#include "dtEditQt/editordata.h"
#include "dtEditQt/editorevents.h"
#include "dtEditQt/uiresources.h"

#include <dtCore/project.h>

#ifdef __APPLE__
#include <OpenAL/alut.h>
#else
#include <AL/alut.h>
#endif

namespace dtEditQt
{
   ///////////////////////////////////////////////////////////////////////////////
   SoundBrowser::SoundBrowser(dtCore::DataType& type, QWidget* parent)
      : ResourceAbstractBrowser(&type, parent)
   {
      // This sets our resource icon that is visible on leaf nodes
      QIcon resourceIcon;
      resourceIcon.addPixmap(QPixmap(UIResources::ICON_SOUND_RESOURCE.c_str()));
      ResourceAbstractBrowser::mResourceIcon = resourceIcon;

      // setup the grid layouts
      mGrid = new QGridLayout(this);
      mGrid->addWidget(previewSoundGroup(), 0, 0);
      mGrid->addWidget(listSoundGroup(), 1, 0);
      mGrid->addWidget(standardButtons(QString("Resource Tools")), 2, 0, Qt::AlignCenter);

      mSoundBuffers[0] = 0U;
      mSoundSources[0] = 0U;
   }

   ///////////////////////////////////////////////////////////////////////////////
   SoundBrowser::~SoundBrowser() {}

   ///////////////////////////////////////////////////////////////////////////////
   QGroupBox* SoundBrowser::previewSoundGroup()
   {
      QGroupBox* groupBox = new QGroupBox(tr("Preview"));

      QHBoxLayout* hbox = new QHBoxLayout(groupBox);

      mPlayBtn = new QPushButton(""/*Play*/, groupBox);
      mPlayBtn->setToolTip("Play currently selected sound");
      mPlayBtn->setIcon(QPixmap(UIResources::ICON_SOUND_PLAY.c_str()));
      connect(mPlayBtn, SIGNAL(clicked()), this, SLOT(playSelected()));

      mStopBtn = new QPushButton(""/*Stop*/, groupBox);
      mStopBtn->setToolTip("Stop currently playing sound");
      mStopBtn->setIcon(QPixmap(UIResources::ICON_SOUND_STOP.c_str()));
      connect(mStopBtn, SIGNAL(clicked()), this, SLOT(stopSelected()));

      hbox->addStretch(1);
      hbox->addWidget(mStopBtn, 0, Qt::AlignTop);
      hbox->addWidget(mPlayBtn, 0, Qt::AlignTop);
      hbox->addStretch(1);

      mPlayBtn->setDisabled(true);
      mStopBtn->setDisabled(true);

      return groupBox;
   }

   ///////////////////////////////////////////////////////////////////////////////
   QGroupBox* SoundBrowser::listSoundGroup()
   {
      QGroupBox* group = new QGroupBox(this);

      QGridLayout* grid = new QGridLayout(group);

      grid->addWidget(mTree,0,0);

      mTree->setResourceName("Sound");

      return group;
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool SoundBrowser::playSound()
   {
      bool returnVal = false;
      bool validFile = false;

      ResourceTreeWidget* selection = currentSelection();

      if (selection != NULL)
      {
         QString file;
         QString context;

         dtCore::Project& project = dtCore::Project::GetInstance();

         // Find the currently selected tree item
         dtCore::ResourceDescriptor resource = EditorData::GetInstance().getCurrentResource(dtCore::DataType::SOUND);

         if (!resource.GetResourceIdentifier().empty())
         {
            context = QString(project.GetContext().c_str());
            if (!context.isEmpty())
            {
               try
               {
                  file = QString(project.GetResourcePath(resource).c_str());
                  validFile = true;
               }
               catch (dtUtil::Exception &)
               {
                  validFile = false;
               }

               if (!file.isEmpty() && validFile == true)
               {
                  // The following is performed to comply with linux and windows file systems
                  file.replace("\\", "/");

                  //Load the sound manually from OpenAL / ALUT (dtAudio doesn't currently provide a
                  // way to do this without firing up a game/application loop).-------------------
                  //
                  //TODO: Have dtAudio provide a way to play a sound from STAGE.
                  ALenum format;
                  ALsizei size;
                  ALfloat freq;
                  ALvoid* soundData = alutLoadMemoryFromFile(file.toAscii(), &format, &size, &freq);
                  if (soundData == NULL)
                  {
                     return false;
                  }

                  //clean up previous source and buffer
                  if (mSoundSources[0] != 0)
                  {
                     alDeleteSources(1, mSoundSources);
                     mSoundSources[0] = 0;
                  }
                  if (mSoundBuffers[0] != 0)
                  {
                     alDeleteBuffers(1, mSoundBuffers);
                     mSoundBuffers[0] = 0;
                  }

                  //create buffer
                  alGenBuffers(1, mSoundBuffers);
                  alBufferData(mSoundBuffers[0], format, soundData, size, ALsizei(freq));

                  //data's memory can be deleted once it's copied to the buffer
                  free(soundData);

                  //create source
                  alGenSources(1, mSoundSources);
                  alSourcei(mSoundSources[0], AL_BUFFER, mSoundBuffers[0]);
                  {
                     alSourcePlay(mSoundSources[0]);
                  }
                  //Done loading and playing the sound via OpenAL / ALUT ------------

                  returnVal = true;
               }
            }
         }
      }
      return returnVal;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SoundBrowser::stopSound()
   {
      if (mSoundSources[0] == 0)
      {
         return;
      }

      alSourceStop(mSoundSources[0]);
   }
   ///////////////////////////////////////////////////////////////////////////////
   // Keyboard Event filter
   ///////////////////////////////////////////////////////////////////////////////
   bool SoundBrowser::eventFilter(QObject* obj, QEvent* e)
   {
      if (obj == mTree)
      {
         //For some reason, KeyPress is getting defined by something...
         //Without this undef, it will not compile under Linux..
         //It would be great if someone could figure out exactly what's
         //going on.
#undef KeyPress
         if (e->type() == QEvent::KeyPress)
         {
            QKeyEvent* keyEvent = (QKeyEvent *)e;
            switch (keyEvent->key())
            {
            case Qt::Key_Return:
               if (mSelection->isResource())
               {
                  playSound();
               }
               break;
            case Qt::Key_Enter:
               if (mSelection->isResource())
               {
                  playSound();
               }
               break;
            case Qt::Key_Backspace:
               stopSound();
               break;
            default:
               return mTree->eventFilter(obj, e);
            }
         }
         else
         {
            // pass the event on to the parent class
            return mTree->eventFilter(obj, e);
         }
      }
      return false;
   }
   ///////////////////////////////////////////////////////////////////////////////
   void SoundBrowser::selectionChanged()
   {
      // This is the abstract base classes original functionality
      ResourceAbstractBrowser::selectionChanged();
      if (mSelection != NULL)
      {
         if (mSelection->isResource())
         {
            mPlayBtn->setDisabled(false);
            mStopBtn->setDisabled(false);
         }
         else
         {
            mPlayBtn->setDisabled(true);
            mStopBtn->setDisabled(true);
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SoundBrowser::doubleClickEvent()
   {
      if (mSelection->isResource())
      {
         playSound();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   // SLOTS:
   ///////////////////////////////////////////////////////////////////////////////
   void SoundBrowser::playSelected()
   {
      playSound();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SoundBrowser::stopSelected()
   {
      stopSound();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SoundBrowser::deleteItemEvent()
   {
      // disable the sound play and stop buttons
      mPlayBtn->setDisabled(true);
      mStopBtn->setDisabled(true);
   }

} // namespace dtEditQt
