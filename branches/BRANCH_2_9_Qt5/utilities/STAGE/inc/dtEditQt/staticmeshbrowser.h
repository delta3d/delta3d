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

#ifndef DELTA_STATIC_MESH_BROWSER
#define DELTA_STATIC_MESH_BROWSER

#include <dtEditQt/resourceabstractbrowser.h>

class QAction;
class QGroupBox;
class QGridLayout;
class QPushButton;
class QKeyEvent;
class QContextMenuEvent;
class QCheckBox;

namespace dtCore
{
   class Scene;
   class Object;
}

namespace dtEditQt
{

   class PerspectiveViewport;
   class ViewportContainer;
   class StageCamera;

   /**
    * @class StaticMeshBrowser
    * @brief Lists and previews static meshes available to a project
    */
   class StaticMeshBrowser : public ResourceAbstractBrowser
   {
      Q_OBJECT

   public:
      /**
       * Constructor
       */
      StaticMeshBrowser(dtCore::DataType& type, QWidget* parent = 0, bool allowPreview = true, bool allowActorCreate = true);

      /**
       * Destructor
       */
      virtual ~StaticMeshBrowser();

      /**
       * Derived from our abstract base class. When a tree selection changes
       * this method will be called. This will handle our context sensitive buttons
       * so we know when to preview a static mesh
       */
      void selectionChanged();

   public slots:
      /**
       * Slot - Display currently selected mesh
       */
      void displaySelection();

      /**
       * Slot - Handle the event when the checkbox is selected, preview selected item.
       */
      void checkBoxSelected();

      /**
       * Slot - Handles the event when the user creates a selected actor
       */
      void createActor();

      /**
       * Slot - Handles the event when the user views an osg or ive scene graph
       */
      void viewSceneGraph();

      /**
       * Slot - Handles event to preview osg contents
       */
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
      bool eventFilter(QObject* target, QEvent* e);

      dtCore::RefPtr<dtCore::BaseActorObject> CreateActor(const dtCore::ResourceDescriptor& rd);

   private:
      /**
       * Overridden function to create our own context menu items for this browser
       */
      void createContextMenu();

      /**
       * Overridden function to create our own actions for this browser
       */
      void createActions();

      /**
       * Determines whether a static mesh should be loaded
       * based on whether an appropriate actor is selected.
       */
      void loadMesh();

      /**
       * When the base class registeres a delete has occured the browser
       * will clear and refresh any preview windows that currently exist
       */
      void deleteItemEvent();

      /**
       * This defines the layout for the static mesh list
       * @return QGroupBox layout widget
       */
      QGroupBox* listGroup(bool allowPreview);

      /**
       * This defines the layout for the static mesh preview
       * @return QGroupBox layout widget
       */
      QGroupBox* previewGroup();

      void doubleClickEvent();

      // Viewports
      ViewportContainer*             container;
      PerspectiveViewport*           perspView;
      dtCore::RefPtr<dtCore::Scene>  meshScene;
      dtCore::RefPtr<StageCamera>    camera;
      dtCore::RefPtr<dtCore::BaseActorObject> previewObject;

      // Layout Objects
      QGridLayout* grid;

      // Checkboxes
      QCheckBox* previewChk;

      // Buttons
      QPushButton* previewBtn;

      // Actions
      QAction* setCreateAction;
      QAction* setSGPreviewAction;
      QAction* setOSGDump;
      bool mAllowPreview;
      bool mAllowCreateActor;
   };

} // namespace dtEditQt

#endif // DELTA_STATIC_MESH_BROWSER
