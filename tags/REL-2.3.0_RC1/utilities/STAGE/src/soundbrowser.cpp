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
#include <prefix/dtstageprefix-src.h>
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

#include <dtDAL/project.h>

#ifdef __APPLE__
#include <OpenAL/alut.h>
#else
#include <AL/alut.h>
#endif

namespace dtEditQt
{
   ///////////////////////////////////////////////////////////////////////////////
   SoundBrowser::SoundBrowser(dtDAL::DataType& type, QWidget* parent)
      : ResourceAbstractBrowser(&type, parent)
   {
      // This sets our resource icon that is visible on leaf nodes
      QIcon resourceIcon;
      resourceIcon.addPixmap(QPixmap(UIResources::ICON_SOUND_RESOURCE.c_str()));
      ResourceAbstractBrowser::resourceIcon = resourceIcon;

      // setup the grid layouts
      grid = new QGridLayout(this);
      grid->addWidget(previewSoundGroup(), 0, 0);
      grid->addWidget(listSoundGroup(), 1, 0);
      grid->addWidget(standardButtons(QString("Resource Tools")), 2, 0, Qt::AlignCenter);

      mSoundBuffers[0] = NULL;
      mSoundSources[0] = NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   SoundBrowser::~SoundBrowser() {}

   ///////////////////////////////////////////////////////////////////////////////
   QGroupBox* SoundBrowser::previewSoundGroup()
   {
      QGroupBox* groupBox = new QGroupBox(tr("Preview"));

      QHBoxLayout* hbox = new QHBoxLayout(groupBox);

      playBtn = new QPushButton(""/*Play*/, groupBox);
      playBtn->setToolTip("Play currently selected sound");
      playBtn->setIcon(QPixmap(UIResources::ICON_SOUND_PLAY.c_str()));
      connect(playBtn, SIGNAL(clicked()), this, SLOT(playSelected()));

      stopBtn = new QPushButton(""/*Stop*/, groupBox);
      stopBtn->setToolTip("Stop currently playing sound");
      stopBtn->setIcon(QPixmap(UIResources::ICON_SOUND_STOP.c_str()));
      connect(stopBtn, SIGNAL(clicked()), this, SLOT(stopSelected()));

      hbox->addStretch(1);
      hbox->addWidget(stopBtn, 0, Qt::AlignTop);
      hbox->addWidget(playBtn, 0, Qt::AlignTop);
      hbox->addStretch(1);

      playBtn->setDisabled(true);
      stopBtn->setDisabled(true);

      return groupBox;
   }

   ///////////////////////////////////////////////////////////////////////////////
   QGroupBox* SoundBrowser::listSoundGroup()
   {
      QGroupBox* group = new QGroupBox(this);

      QGridLayout* grid = new QGridLayout(group);

      grid->addWidget(tree,0,0);

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

         dtDAL::Project& project = dtDAL::Project::GetInstance();

         // Find the currently selected tree item
         dtDAL::ResourceDescriptor resource = EditorData::GetInstance().getCurrentSoundResource();

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
                  file = context + "\\" + file;
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
                  if (mSoundSources[0] != NULL)
                  {
                     alDeleteSources(1, mSoundSources);
                     mSoundSources[0] = NULL;
                  }
                  if (mSoundBuffers[0] != NULL)
                  {
                     alDeleteBuffers(1, mSoundBuffers);
                     mSoundBuffers[0] = NULL;
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
      if (mSoundSources[0] == NULL)
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
      if (obj == tree)
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
               if (selection->isResource())
               {
                  playSound();
               }
               break;
            case Qt::Key_Enter:
               if (selection->isResource())
               {
                  playSound();
               }
               break;
            case Qt::Key_Backspace:
               stopSound();
               break;
            default:
               return tree->eventFilter(obj, e);
            }
         }
         else
         {
            // pass the event on to the parent class
            return tree->eventFilter(obj, e);
         }
      }
      return false;
   }
   ///////////////////////////////////////////////////////////////////////////////
   void SoundBrowser::selectionChanged()
   {
      // This is the abstract base classes original functionality
      ResourceAbstractBrowser::selectionChanged();
      if (selection != NULL)
      {
         if (selection->isResource())
         {
            playBtn->setDisabled(false);
            stopBtn->setDisabled(false);
         }
         else
         {
            playBtn->setDisabled(true);
            stopBtn->setDisabled(true);
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SoundBrowser::doubleClickEvent()
   {
      if (selection->isResource())
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
      playBtn->setDisabled(true);
      stopBtn->setDisabled(true);
   }

} // namespace dtEditQt
