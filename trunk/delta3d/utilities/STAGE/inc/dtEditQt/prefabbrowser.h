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

#include <QtGui/QWidget>
#include <vector>

#include <dtCore/refptr.h>
#include <dtUtil/tree.h>
#include <dtDAL/actortype.h>
#include <dtDAL/resourcetreenode.h>
#include <dtEditQt/actortypetreewidget.h>

class QGroupBox;
class QTreeWidget;
class QPushButton;
class QModelIndex;

namespace dtEditQt
{
   class ResourceTree;
   class ResourceTreeWidget;

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
      void handleEnableCreateActor();

      public slots:
         /**
         * Slot - handles the event when the create button is pressed
         */
         void createActorPressed();

         /**
         * Slot - Called when the tree selection changes
         */
         void treeSelectionChanged();

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
      // Prefab resources.
      dtUtil::tree<dtDAL::ResourceTreeNode> mPrefabList;

      /**
      * Creates the main User Interface for the Actor Browser
      */
      void setupGUI();

      /**
      * A convenience method to returns the selected tree widget or NULL.
      * @return The selected actor tree widget.  NULL if no selection.
      */
      ResourceTreeWidget* getSelectedPrefabWidget();


      QIcon          mResourceIcon;
      ResourceTree*  mTree;
      QPushButton*   mCreatePrefabBtn;

      ResourceTreeWidget*  mRootPrefabTree;

      // this is a tree of actor type names which were expanded.  It is used
      // when we reload actor types.  We walk the tree and look for
      // expanded items.  For each one, we add it to this tree.  Then, we walk
      // back through this tree to reexpand items later
      dtUtil::tree<QString> mExpandedActorTypeNames;
      bool                  mRootNodeWasExpanded;
      int                   mLastScrollBarLocation;

      /**
      * Looks at the current actor tree and tries to mark which actor types are
      * currently expanded.  This is then used to re-expand them with restorePreviousExpansion()
      * after the tree is rebuilt.
      */
      void markCurrentExpansion();

      /**
      * recursive method to support markCurrentExpansion().
      */
      void recurseMarkCurrentExpansion(ResourceTreeWidget* parent,
         dtUtil::tree<QString>& currentTree);

      /**
      * Attempts to re-expand previously expanded actor types.  This is a nicity for the user
      * for when they load libraries and such.
      */
      void restorePreviousExpansion();

      /**
      * Recursive method to support restorePreviousExpansion().
      */
      void recurseRestorePreviousExpansion(ResourceTreeWidget* parent,
         dtUtil::tree<QString>& currentTree);
   };

} // namespace dtEditQt

#endif // DELTA_ACTOR_BROWSER
