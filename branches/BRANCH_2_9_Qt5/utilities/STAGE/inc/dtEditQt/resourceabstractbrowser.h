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

#ifndef DELTA_RESOURCE_ABSTRACT_BROWSER
#define DELTA_RESOURCE_ABSTRACT_BROWSER

#include <QtCore/QString>
#include <QtGui/QWidget>

#include <QtGui/QPushButton>

#include <dtCore/datatype.h>
#include <dtEditQt/resourceimportdialog.h>

class QPushButton;
class QGroupBox;
class QAction;
class QMenu;
class QTreeWidgetItem;
class QContextMenuEvent;
class QIcon;

namespace dtCore
{
   class Transformable;
}

namespace dtCore
{
   class ResourceDescriptor;
}

namespace dtEditQt
{
   class StageCamera;
   class ResourceTreeWidget;
   class ResourceDragTree;

   class ResourceAbstractBrowser: public QWidget
   {
      Q_OBJECT

   public:
      /**
       * Constructor
       */
      ResourceAbstractBrowser(dtCore::DataType* type, QWidget* parent);

      /**
       * Destructor
       */
      virtual ~ResourceAbstractBrowser(){}

      dtCore::DataType& GetResourceType() const { return *mResourceType; }

      /**
       * Adds a tree node
       * @brief This adds a new node to the tree
       */
      virtual void addTreeNode(QString& nodeText, dtCore::ResourceDescriptor descriptor, bool resource = false);

      /**
       * Removes a tree node
       * @brief This will remove the currently selected tree node and then
       *        expand the parent node to circumvent Qt's default behaviour.
       */
      virtual void removeTreeNode();

      /**
       * Added an event filter to capture events sent to the tree widget
       * All other events are passed to the parent. This will handle mouse clicks and
       * key presses. We can overide this if browser specific functionality is required.
       * @param Event
       * @return bool if the event was not captured for the appropriate widget
       */
      virtual bool eventFilter(QObject* target, QEvent* e);

      /**
       * This returns the currently selected node
       * @brief This returns the currently selected tree item
       */
      virtual ResourceTreeWidget* currentSelection();

      /**
       * Refreshes the resource tree
       * @brief Refreshes the resources by grabbing an instance to the project and then
       *        rebuilding the tree.
       */
      void refreshResourceTree();

      /**
       * expandTreeNode
       * @brief expands the currently selected tree node based on the currently selected
       *        item.
       */
      void expandTreeNode();

      /**
       * collapseTreeNode
       * @brief collapses the currently selected tree node based on the currently selected
       *        item.
       */
      void collapseTreeNode();

      /**
       * getSelectedNode
       * @brief This returns the selectedNode
       */
      void getSelectedNode();

      /**
       * getRootName
       * @brief This returns the name of the root that was set by the constructor
       */
      QString getRootName() { return mRootName; }

      /**
       * getsCurrentParent
       * @return QWidget parent
       */
      QWidget* getCurrentParent() { return mParent; }

      /**
       * standardButtons
       * @brief This creates the standard tree buttons for the resource browsers
       * @return QGroupBox
       */
      QGroupBox* standardButtons(const QString& groupBoxName);

      /**
       * resetEditorDataDescriptor
       * @brief This resets the resource descriptor assigned to the EditorData class
       */
      void resetEditorDataDescriptor();

      /**
       * setEditorDataDescriptor
       * @brief This sets the resource descriptor in the EditorData class
       */
      void setEditorDataDescriptor(dtCore::ResourceDescriptor& descriptor);

   private slots:
      void onProjectChanged();
      void deleteSelected();
      void createCategory();
      void refreshSelected();
      void importSelected();

   public slots:
      /**
       * This slot registers the selection variable whenever an item from the tree
       * has been selected.
       * @note The selectionChanged slot sets the selection variable to the currently
       *       selected ResourceTreeWidget.
       */
      virtual void selectionChanged();

      /**
       * This slot registers the double click event
       * @note the doubleClickEvent slot informs the resource browsers that they
       * should be able to preview a selection
       */
      virtual void doubleClickEvent();

      /**
       * This slot registers a deleted item event
       * @note This slot is used to inform the resource browsers that a deletion
       * has occurred.
       */
      virtual void deleteItemEvent();

   private:
      /**
       * We don't want this called directly
       * @note This builds the resource tree by assigning a name, the data type,
       * and the parent widget
       * @param dtCore DataType - a simple 2 string resource path and name
       * @param QIcon resource icon for our leaf nodes
       * @param QWidget the parent of the widget
       */
       virtual void buildResourceTree(dtCore::DataType& type, QWidget* parent, const QIcon& resourceIcon);

   protected:
      /**
       * Sets the camera to look at the given transformable based on the bounding sphere.
       * @param camera The camera to position.
       * @param transformableToView the transformable actor to look at.
       */
      static void SetCameraLookAt(dtEditQt::StageCamera& camera, dtCore::Transformable& transformableToView);

      /**
       * Overridden to ensure the right-click menu only appears when right clicking
       * on the tree widget.
       * @param Event
       */
      void contextMenuEvent(QContextMenuEvent* e);

      /**
       * Creates a right-click context menu for the tree widget
       * @brief This will create a default context menu. It has been declared
       * virtual in case the sub class requires additional functionality to be
       * added.
       */
      virtual void createContextMenu();

      /**
       * Add the actions
       * @brief This creates the actions associated with the context menu
       */
      virtual void createActions();

      QString* mCurrentPath;
      QString* mResourceName;
      QString  mRootName;

      QWidget* mParent;
      QString* mContext;
      QString  mFileDialogDir;

      dtUtil::tree<dtCore::ResourceTreeNode> mIterTree;
      dtCore::ResourceDescriptor*            mSelectedDescriptor;
      dtCore::DataType*                      mResourceType;
      ResourceDragTree*                     mTree;
      ResourceTreeWidget*                   mRoot;
      ResourceTreeWidget*                   mSelection;

      // Importing
      ResourceImportDialog* mImportDialog;

      // Button Objects
      QPushButton* mCategoryBtn;
      QPushButton* mImportBtn;
      QPushButton* mDeleteBtn;
      QPushButton* mRefreshBtn;

      // Context Menu Objects
      QMenu* mContextMenu;

      // Action Objects
      QAction* mSetRenameAction;
      QAction* mSetRefreshAction;
      QAction* mSetDeleteAction;
      QAction* mSetCategoryAction;
      QAction* mSetImportAction;

      QIcon    mResourceIcon;
   };

} // namespace dtEditQt

#endif // DELTA_RESOURCE_ABSTRACT_BROWSER
