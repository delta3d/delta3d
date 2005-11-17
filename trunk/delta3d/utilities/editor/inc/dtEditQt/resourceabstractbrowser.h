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

#ifndef _ResourceAbstractBrowser_h
#define _ResourceAbstractBrowser_h

#include <QString>
#include <QWidget>
#include <QMessageBox>

#include <QPushButton>

#include "dtDAL/project.h"
#include "dtEditQt/editordata.h"
#include "dtEditQt/editorevents.h"
#include "dtDAL/datatype.h"
#include "dtEditQt/resourceimportdialog.h"

class QPushButton;
class QGroupBox;
class QAction;
class QMenu;
class QTreeWidgetItem;
class QContextMenuEvent;
class QIcon;

namespace dtDAL
{
    class ResourceDescriptor;
}

namespace dtEditQt
{

    class ResourceTreeWidget;
    class ResourceTree;

    class ResourceAbstractBrowser  : public QWidget
    {
        Q_OBJECT

    public:
        /**
        * Constructor
        */
        ResourceAbstractBrowser(dtDAL::DataType *type,QWidget *parent);

        /**
        * Destructor
        */
        virtual ~ResourceAbstractBrowser(){}

        /**
        * Adds a tree node
        * @brief This adds a new node to the tree
        */
        virtual void addTreeNode(QString &nodeText, dtDAL::ResourceDescriptor descriptor, bool resource = false);

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
        virtual bool eventFilter(QObject *target, QEvent *e);

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
        QString getRootName(){return rootName;}

        /**
        * getsCurrentParent
        * @return QWidget parent
        */
        QWidget *getCurrentParent(){return parent;}

        /**
        * standardButtons
        * @brief This creates the standard tree buttons for the resource browsers
        * @return QGroupBox
        */
        QGroupBox* standardButtons(const QString &groupBoxName);

        /**
        * resetEditorDataDescriptor
        * @brief This resets the resource descriptor assigned to the EditorData class
        */
        void resetEditorDataDescriptor();

        /**
        * setEditorDataDescriptor
        * @brief This sets the resource descriptor in the EditorData class
        */
        void setEditorDataDescriptor(dtDAL::ResourceDescriptor &descriptor);

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
        * @Note The selectionChanged slot sets the selection variable to the currently
        *       selected ResourceTreeWidget.
        */
        virtual void selectionChanged();

        /**
        * This slot registers the double click event
        * @Note the doubleClickEvent slot informs the resource browsers that they
        * should be able to preview a selection
        */
        virtual void doubleClickEvent();

        /**
        * This slot registers a deleted item event
        * @Note This slot is used to inform the resource browsers that a deletion
        * has occurred.
        */
        virtual void deleteItemEvent();

    private:
        /**
        * We don't want this called directly
        * @note This builds the resource tree by assigning a name, the data type,
        * and the parent widget
        * @param dtDAL DataType - a simple 2 string resource path and name
        * @param QIcon resource icon for our leaf nodes
        * @param QWidget the parent of the widget
        */
        virtual void buildResourceTree(dtDAL::DataType &type, QWidget *parent, QIcon *resourceIcon);

    protected:
        /**
        * Overridden to ensure the right-click menu only appears when right clicking
        * on the tree widget.
        * @param Event
        */
        void contextMenuEvent(QContextMenuEvent *e);

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

        QString *currentPath;
        QString *resourceName;
        QString rootName;

        QWidget *parent;
        QString *context;
        QString fileDialogDir;

        dtUtil::tree<dtDAL::ResourceTreeNode> iterTree;
        dtDAL::ResourceDescriptor *selectedDescriptor;
        dtDAL::DataType *resourceType;
        ResourceTree *tree;
        ResourceTreeWidget *root;
        ResourceTreeWidget *selection;

        // Importing
        ResourceImportDialog *importDialog;

        // Button Objects
        QPushButton *categoryBtn;
        QPushButton *importBtn;
        QPushButton *deleteBtn;
        QPushButton *refreshBtn;

        // Context Menu Objects
        QMenu *contextMenu;

        // Action Objects
        QAction *setRenameAction;
        QAction *setRefreshAction;
        QAction *setDeleteAction;
        QAction *setCategoryAction;
        QAction *setImportAction;

        QIcon resourceIcon;
    };
}
#endif
