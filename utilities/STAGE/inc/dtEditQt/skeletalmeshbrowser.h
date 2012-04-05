/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2007-2008, MOVES Institute
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
 * Michael Guerrero
 */

#ifndef DELTA_SKELETAL_MESH_BROWSER
#define DELTA_SKELETAL_MESH_BROWSER

#include <dtEditQt/resourceabstractbrowser.h>
#include <dtAnim/chardrawable.h>

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
}

namespace dtEditQt
{

   class PerspectiveViewport;
   class ViewportContainer;
   class StageCamera;

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
      SkeletalMeshBrowser(dtCore::DataType& type, QWidget* parent = 0);

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
      QGroupBox* listGroup();

      /**
       * This defines the layout for the skeletal mesh preview
       * @return QGroupBox layout widget
       */
      QGroupBox* previewGroup();

      void doubleClickEvent();

      // Viewports
      ViewportContainer*             container;
      PerspectiveViewport*           perspView;
      dtCore::RefPtr<dtCore::Scene>  meshScene;
      dtCore::RefPtr<StageCamera>    camera;
      dtCore::RefPtr<dtAnim::CharDrawable> previewObject;

      // Layout Objects
      QGridLayout* grid;

      // Checkboxes
      QCheckBox* previewChk;

      // Buttons
      QPushButton* previewBtn;

      // Actions
      QAction* setCreateAction;
   };

} // namespace dtEditQt

#endif // DELTA_SKELETAL_MESH_BROWSER
