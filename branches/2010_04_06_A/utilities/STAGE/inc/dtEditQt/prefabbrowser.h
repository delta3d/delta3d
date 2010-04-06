/* -*-c++-*-
* Delta3D Simulation Training And Game Editor (STAGE)
* STAGE - actorbrowser (.h & .cpp) - Using 'The MIT License'
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
* Jeffrey P. Houde
*/

#ifndef DELTA_PREFAB_BROWSER
#define DELTA_PREFAB_BROWSER

#include <QtGui/QMenu>
#include <QtGui/QShortcut>
#include <QtGui/QWidget>
#include <vector>

#include <dtEditQt/actortypetreewidget.h>
#include <dtActors/prefabactorproxy.h>
#include <dtCore/refptr.h>
#include <dtDAL/actortype.h>
#include <dtDAL/resourcetreenode.h>
#include <dtUtil/tree.h>

class QAction;
class QGroupBox;
class QPushButton;
class QModelIndex;
class QBoxLayout;
class QGridLayout;
class QCheckBox;
class QListWidgetItem;
class QPoint;

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
   
   class ResourceDragListWidget;
   class ResourceListWidgetItem;   

   /**
   * @class PrefabBrowser
   * @brief This class displays the currently available actors loaded with the map
   */
   class PrefabBrowser : public QWidget
   {
      Q_OBJECT

   public:
      /**
      * Constructor
      */
      PrefabBrowser(QWidget* parent = NULL);

      /**
      * Destructor
      */
      ~PrefabBrowser();

      /**
      * Determines whether the create actor button is supposed to be enabled or
      * disabled based on whether an appropriate actor is selected.
      */
      void handleEnableCreateActorBtn();

      public slots:

         /*
         * Slot - handles popup menu event adding a new Prefab category.
         */
         void addCategorySlot();

         /**
         * Slot - handles the event when the create button is pressed
         */
         void createPrefabPressed();

         /**
         * Slot - handles the event when the create instance button is pressed.
         */
         void createPrefabInstancePressed();

		 /**
         * Slot - handles the delete key push event.
         */
         void deleteKeyPushedSlot();

         /**
         * Slot - handles exporting of a new prefab
         */
         void exportNewPrefabSlot();

         /**
         * Slot - Called when the list selection changes                                                                         
         */
         void listSelectionChanged();

         /**
         * Slot - Called when the list selection is accepted (double-clicked)
         */
         void listSelectionDoubleClicked(QListWidgetItem* activatedItem);

         /**
         * Slot - Called when ResourceListWidget is double clicked...
         * May need to be moved to ResourceListWidget class file...
         */
         void rightClickMenu(const QPoint& clickPoint); 

         /**
         * Slot - Handles refreshing the prefabs.
         */
         void refreshPrefabs();

         /**
         * Slot - handles the about to remove library event.  This is necessary because
         * before the actual library removed event, we need to clear our list or else we'll
         * be holding onto memory that has since been removed.  It completely clears the tree.
         */
         void clearPrefabTree();

   private:

      /**
      * Creates the main User Interface for the Actor Browser
      */
      void setupGUI();

      /**
      * A convenience method to returns the selected list widget or NULL.
      * @return The selected list widget.  NULL if no selection.
      */      
      ResourceListWidgetItem* getSelectedPrefabWidget();

      /**
      * This defines the layout for the prefab list.
      * @return QGroupBox layout widget
      */
      QGroupBox* listGroup();

      /**
      * This defines the layout for the prefab action buttons.
      * @return QWidget layout widget.
      */
      QBoxLayout* buttonLayout();

      /**
      * Convenience method that builds a Prefab resource descriptor using
      * the absolute path to the XML file defining the Prefab.
      */
      dtDAL::ResourceDescriptor createResDescriptorFromPath(std::string path);

      // Layout Objects
      QGridLayout* mGrid;

      QIcon                mResourceIcon;
      QPushButton*         mCreatePrefabBtn;
      QPushButton*         mCreateInstanceBtn;
      QPushButton*         mRefreshPrefabBtn;

      //List that browses prefab directories      
      ResourceDragListWidget*         mListWidget;      
      std::string          mCurrentDir;
      std::string          mTopPrefabDir;

      //Popup menu for creating new prefabs and categories
      QMenu                mPopupMenu;
      QAction*             mExportNewPrefabAction;
      
      //Keyboard shortcuts
      QShortcut mDeleteShortcut;

   };

} // namespace dtEditQt

#endif // DELTA_ACTOR_BROWSER
