/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2005, BMH Associates, Inc.
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* @author Teague Coonan
*/
#include <QDir>
#include <QHeaderView>

#include <QHBoxLayout>
#include <QGridLayout>
#include <QString>

#include <QGroupBox>

#include <QPushButton>
#include <QPixmap>

#include <QAction>
#include <QContextMenuEvent>

#include "dtEditQt/soundbrowser.h"
#include "dtEditQt/resourcetreewidget.h"
#include "dtDAL/project.h"
#include "dtEditQt/editordata.h"
#include "dtEditQt/editorevents.h"
#include "dtEditQt/uiresources.h"

namespace dtEditQt 
{
    ///////////////////////////////////////////////////////////////////////////////
    SoundBrowser::SoundBrowser(dtDAL::DataType &type,QWidget *parent)
        : ResourceAbstractBrowser(&type,parent)
    {
        sound = new QSound("");
        // setup the grid layouts
        grid = new QGridLayout(this);
        grid->addWidget(previewSoundGroup(), 0, 0);
        grid->addWidget(listSoundGroup(), 1, 0);
        grid->addWidget(standardButtons(QString("Resource Tools")),2,0,Qt::AlignCenter);
    }
    ///////////////////////////////////////////////////////////////////////////////
    SoundBrowser::~SoundBrowser(){}
    ///////////////////////////////////////////////////////////////////////////////
    QGroupBox *SoundBrowser::previewSoundGroup()
    {
        QGroupBox *groupBox = new QGroupBox(tr("Preview"));

        QHBoxLayout *hbox = new QHBoxLayout(groupBox);

        playBtn = new QPushButton(""/*Play*/,groupBox);
        playBtn->setToolTip("Play currently selected sound");
        playBtn->setIcon(QPixmap(UIResources::ICON_SOUND_PLAY.c_str()));
        connect(playBtn, SIGNAL(clicked()), this, SLOT(playSelected()));

        stopBtn = new QPushButton(""/*Stop*/,groupBox);
        stopBtn->setToolTip("Stop currently playing sound");
        stopBtn->setIcon(QPixmap(UIResources::ICON_SOUND_STOP.c_str()));
        connect(stopBtn, SIGNAL(clicked()), this, SLOT(stopSelected()));

        hbox->addStretch(1);
        hbox->addWidget(stopBtn,0,Qt::AlignTop);
        hbox->addWidget(playBtn,0,Qt::AlignTop);
        hbox->addStretch(1);

        playBtn->setDisabled(true);
        stopBtn->setDisabled(true);

        return groupBox;
    }
    ///////////////////////////////////////////////////////////////////////////////
    QGroupBox *SoundBrowser::listSoundGroup()
    {
        QGroupBox *group = new QGroupBox(this);

        QGridLayout *grid = new QGridLayout(group);

        grid->addWidget(tree,0,0);

        return group;
    }
    ///////////////////////////////////////////////////////////////////////////////
    bool SoundBrowser::playSound()
    {
        bool returnVal = false;
        bool validFile = false;

        ResourceTreeWidget *selection = currentSelection();

        if(selection != NULL)
        {
            QString file;
            QString context;

            dtDAL::Project &project = dtDAL::Project::GetInstance();

            // Find the currently selected tree item
            dtDAL::ResourceDescriptor resource = EditorData::getInstance().getCurrentSoundResource();
            
            if(!resource.GetResourceIdentifier().empty())
            {
                context = QString(project.GetContext().c_str());
                if(!context.isEmpty())
                {
                    try 
                    {
                        file = QString(project.GetResourcePath(resource).c_str());
                        validFile = true;
                    } 
                    catch (dtDAL::Exception &e) 
                    {
                        validFile = false;
                    }

                    if(!file.isEmpty() && validFile == true)
                    {
                        file = context+"\\"+file;
                        // The following is performed to comply with linux and windows file systems
                        file.replace("\\","/");

                        sound = new QSound(file);
                        sound->play();

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
        sound->stop();
    }
    ///////////////////////////////////////////////////////////////////////////////
    // Keyboard Event filter
    ///////////////////////////////////////////////////////////////////////////////
    bool SoundBrowser::eventFilter(QObject *obj, QEvent *e)
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
                QKeyEvent *keyEvent = (QKeyEvent *)e;
                switch(keyEvent->key())
                {
                case Qt::Key_Return :
                    if(selection->isResource())
                    {
                        playSound();
                    }
                    break;
                case Qt::Key_Enter:
                    if(selection->isResource())
                    {
                        playSound();
                    }
                    break;
                case Qt::Key_Backspace:
                    stopSound();
                    break;
                default:
                    return tree->eventFilter(obj,e);
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
        if(selection != NULL)
        {
            if(selection->isResource())
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
        if(selection->isResource())
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
}
