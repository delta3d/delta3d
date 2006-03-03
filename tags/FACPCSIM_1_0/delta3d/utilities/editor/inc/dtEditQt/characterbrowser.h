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
* @author Teague Coonan
*/

#ifndef __CharacterBrowser_h
#define __CharacterBrowser_h

#include "dtEditQt/resourceabstractbrowser.h"
#include <osg/ref_ptr>
#include <dtChar/character.h>

class QAction;
class QGroupBox;
class QGridLayout;
class QPushButton;
class QKeyEvent;
class QContextMenuEvent;
class QCheckBox;
class QIcon;

namespace dtCore {
    class Scene;
    class Object;
}

namespace dtEditQt {

    class PerspectiveViewport;
    class ViewportContainer;
    class Camera;

    class CharacterBrowser : public ResourceAbstractBrowser
    {
        Q_OBJECT
    public:
        CharacterBrowser(dtDAL::DataType &type,QWidget *parent=0);
        virtual ~CharacterBrowser();
            /**
        * Derived from our abstract base class. When a tree selection changes 
        * this method will be called. This will handle our context sensitive buttons
        * so we know when to preview a character
        */
        void selectionChanged();

    protected:
        /**
        * Added an event filter to capture keyboard events sent to the tree widget 
        * so we can trap the enter key and play sounds. All other events are
        * passed on to the parent. This has been overridden from the base abstract
        * class to provide browser specific functionality.
        * @param Event
        * @return bool if the event was not captured for the appropriate widget
        */
        bool eventFilter(QObject *target, QEvent *e);

    public slots:

        /**
        * Slot - Display currently selected mesh
        */
        void displaySelection();

        /**
        * Slot - Handle the event when the checkbox is selected, preview selected item.
        */
        void checkBoxSelected();

    private:

        /**
        * This defines the layout for the static mesh list
        * @return QGroupBox layout widget
        */
        QGroupBox *listGroup();
        
        /**
        * This defines the layout for the static mesh preview
        * @return QGroupBox layout widget
        */
        QGroupBox *previewGroup();
        
        void doubleClickEvent();

        // Viewports
        ViewportContainer *container;
        PerspectiveViewport *perspView;
        osg::ref_ptr<dtCore::Scene> characterScene;
        osg::ref_ptr<Camera> camera;
        osg::ref_ptr<dtChar::Character> previewObject;

        // Layout Objects
        QGridLayout *grid;

        // Checkboxes
        QCheckBox *previewChk;

        // Buttons
        QPushButton *previewBtn;

        QIcon *resourceIcon;
    };
}

#endif
