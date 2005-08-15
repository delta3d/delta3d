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

#ifndef __ParticleBrowser_h
#define __ParticleBrowser_h

#include "dtEditQt/resourceabstractbrowser.h"
#include <osg/ref_ptr>

class QGroupBox;
class QGridLayout;
class QPushButton;
class QAction;
class QMenu;
class QContextMenuEvent;
class QIcon;

namespace dtCore 
{
    class Scene;
    class Object;
}

namespace dtEditQt 
{

    class PerspectiveViewport;
    class ViewportContainer;
    class Camera;

    /**
    * @class ParticleBrowser
    * @brief Lists and previews available particle effects
    */  
    class ParticleBrowser : public ResourceAbstractBrowser
    {
        Q_OBJECT
    public:
        /**
        * Constructor
        */
        ParticleBrowser(dtDAL::DataType &type,QWidget *parent=0);
        /**
        * Destructor
        */
        virtual ~ParticleBrowser();

        /**
        * Derived from our abstract base class. When a tree selection changes 
        * this method will be called. This will handle our context sensitive buttons
        * so we know when to enable/disable menu items
        */
        void selectionChanged();

    public slots:

        /**
        * Slot - Display currently selected mesh
        */
        void displaySelection();

        /**
        * Slot - Handles the event when the user creates a selected actor
        */
        void createActor();

    private:

        /**
        * Overridden function to create our own context menu items for this browser
        */
        void createContextMenu();

        /**
        * Overridden function to create our own actions for this browser
        */
        void createActions();

        QGroupBox *listGroup();

        /**
        * This defines the layout for the static mesh preview
        * @return QGroupBox layout widget
        */
        QGroupBox *previewGroup();

        QGroupBox *buttonGroup();

        // Button Objects
        QPushButton *previewBtn;

        // Viewports
        ViewportContainer *container;
        PerspectiveViewport *perspView;
        osg::ref_ptr<dtCore::Scene> particleScene;
        osg::ref_ptr<Camera> camera;
        osg::ref_ptr<dtCore::Object> previewObject;
        
        // Layout Objects
        QGridLayout *grid;

        // Actions
        QAction *setCreateAction;
        
        QIcon *resourceIcon;
    };

}

#endif
