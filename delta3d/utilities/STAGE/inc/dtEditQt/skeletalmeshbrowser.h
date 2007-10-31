/* 
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2007 MOVES Institute 
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
* Michael Guerrero
*/

#ifndef DELTA_SKELETAL_MESH_BROWSER
#define DELTA_SKELETAL_MESH_BROWSER

#include <dtEditQt/resourceabstractbrowser.h>

class QAction;
class QGroupBox;
class QGridLayout;
class QPushButton;
class QKeyEvent;
class QContextMenuEvent;
class QCheckBox;
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
   * @class SkeletalMeshBrowser
   * @brief Lists and previews skeletal meshes available to a project 
   */  
   class SkeletalMeshBrowser : public ResourceAbstractBrowser
   {
      Q_OBJECT
   public:
      /**
      * Constructor
      */
      SkeletalMeshBrowser(dtDAL::DataType &type,QWidget *parent=0);

      /**
      * Destructor
      */
      virtual ~SkeletalMeshBrowser();

      /**
      * Derived from our abstract base class. When a tree selection changes 
      * this method will be called. This will handle our context sensitive buttons
      * so we know when to preview a skeletal mesh
      */
      void selectionChanged();

   public slots:

      /// Display currently selected mesh      
      void displaySelection();

      /// Handle the event when the checkbox is selected, preview selected item.   
      void checkBoxSelected();

      /// Handles the event when the user creates a selected actor
      void createActor();

      /// Handles the event when the user views an osg or ive scene graph
      void viewSceneGraph();

      /// Handles event to preview osg contents
      void viewOSGContents();

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

   private:
      /// Overridden function to create our own context menu items for this browser
      void createContextMenu();

      /// Overridden function to create our own actions for this browser
      void createActions();

      /**
      * Determines whether a skeletal mesh should be loaded 
      * based on whether an appropriate actor is selected.  
      */
      void loadMesh();

      /**
      * When the base class registeres a delete has occured the browser
      * will clear and refresh any preview windows that currently exist
      */
      void deleteItemEvent();

      /**
      * This defines the layout for the skeletal mesh list
      * @return QGroupBox layout widget
      */
      QGroupBox *listGroup();

      /**
      * This defines the layout for the skeletal mesh preview
      * @return QGroupBox layout widget
      */
      QGroupBox *previewGroup();

      void doubleClickEvent();

      // Viewports
      ViewportContainer *container;
      PerspectiveViewport *perspView;
      dtCore::RefPtr<dtCore::Scene> meshScene;
      dtCore::RefPtr<Camera> camera;
      dtCore::RefPtr<dtCore::Object> previewObject;

      // Layout Objects
      QGridLayout *grid;

      // Checkboxes
      QCheckBox *previewChk;

      // Buttons
      QPushButton *previewBtn;

      // Actions
      QAction *setCreateAction;
      QAction *setSGPreviewAction;
      QAction *setOSGDump;

      // Resource Icon - this is our leaf node icon
      QIcon *resourceIcon;
   };
}

#endif /*DELTA_SKELETAL_MESH_BROWSER*/
