/* 
* Delta3D Open Source Game and Simulation Engine Level Editor 
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
* @author Teague Coonan
*/

#ifndef __SoundBrowser_h
#define __SoundBrowser_h

#include <QSound>
#include "dtEditQt/resourceabstractbrowser.h"
#include <osg/ref_ptr>

class QAction;
class QGroupBox;
class QGridLayout;
class QPushButton;
class QKeyEvent;
class QContextMenuEvent;
class QIcon;

namespace dtEditQt {

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
        SoundBrowser(dtDAL::DataType &type,QWidget *parent=0);

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
        bool eventFilter(QObject *target, QEvent *e);

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
        QGroupBox *previewSoundGroup();

        /**
        * This defines the layout for the sound list
        * @return QGroupBox layout widget
        */
        QGroupBox *listSoundGroup();

        // Button Objects
        QPushButton *playBtn;
        QPushButton *stopBtn;
        
        QSound *sound;

        // Layout Objects
        QGridLayout *grid;

        // Resource Icon - this is our leaf node icon
        QIcon *resourceIcon;
    };
}

#endif

