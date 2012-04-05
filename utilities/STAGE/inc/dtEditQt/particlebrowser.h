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

#ifndef DELTA_PARTICLE_BROWSER
#define DELTA_PARTICLE_BROWSER

#include <dtEditQt/resourceabstractbrowser.h>

class QGroupBox;
class QGridLayout;
class QPushButton;
class QCheckBox;
class QAction;
class QMenu;
class QContextMenuEvent;

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
      ParticleBrowser(dtCore::DataType& type, QWidget* parent=0);

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
       * Slot - Handle the event when the checkbox is selected, preview selected item.
       */
      void checkBoxSelected();

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

      QGroupBox* listGroup();

      /**
       * This defines the layout for the static mesh preview
       * @return QGroupBox layout widget
       */
      QGroupBox* previewGroup();

      QGroupBox* buttonGroup();

      // Checkboxes
      QCheckBox* previewChk;

      // Button Objects
      QPushButton* previewBtn;

      // Viewports
      ViewportContainer*   container;
      PerspectiveViewport* perspView;
      dtCore::RefPtr<dtCore::Scene> particleScene;
      dtCore::RefPtr<StageCamera> camera;
      dtCore::RefPtr<dtCore::Object> previewObject;

      // Layout Objects
      QGridLayout* grid;

      // Actions
      QAction* setCreateAction;
   };

} // namespace dtEditQt

#endif // DELTA_PARTICLE_BROWSER
