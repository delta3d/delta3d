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
 * @author Curtiss Murphy
*/

#ifndef DELTA_ACTORRESULTSTABLE
#define DELTA_ACTORRESULTSTABLE

#include <QtGui/QWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>
#include <vector>
#include <dtDAL/actorproxy.h>
#include "dtEditQt/typedefs.h"

class QGroupBox;
class QPushButton;
class QKeyEvent;
//class QTableWidget;

namespace dtEditQt {
    class ActorResultsTable;

    /**
     * @class ActorResultsTreeItem
     * @brief This is a utility table that shows a list of actor Proxies and allows some 
     * basic behavior on them such as select and goto.  
     */
    class ActorResultsTreeItem : public QTreeWidgetItem
    {
    public:
        /**
         * Constructor - showActions lets you indicate whether you should show the 
         * buttons for various behaviors such as select and goto.
         */
        ActorResultsTreeItem(QTreeWidget *parent, dtCore::RefPtr<dtDAL::ActorProxy> proxy);

        /**
         * Destructor
         */
        virtual ~ActorResultsTreeItem();

        /**
         * Gets the actor proxy for this tree item
         */
        dtCore::RefPtr<dtDAL::ActorProxy> getProxy()  {  return myProxy;  }

    private: 
        dtCore::RefPtr<dtDAL::ActorProxy> myProxy;
    };


    /**
     * @class ActorResultsSubTree
     * @brief Overrides the keyPressEvent from QTreeWidget 
     */
    class ActorResultsSubTree : public QTreeWidget
    {
    public: 
        /**
         * Constructor
         */
        ActorResultsSubTree(QWidget *parent, ActorResultsTable *newOwner) 
            : QTreeWidget(parent), owner(newOwner)     {   }

        /**
         * Destructor
         */
        virtual ~ActorResultsSubTree()   {   }

    protected: 
        virtual void keyPressEvent(QKeyEvent *e)   { QTreeWidget::keyPressEvent(e);   }
    private:
        ActorResultsTable *owner;
    };



    /**
    * @class ActorResultsTable 
    * @brief This is a utility table that shows a list of actor Proxies and allows some 
    * basic behavior on them such as select and goto.  
    */
    class ActorResultsTable : public QWidget
    {
        Q_OBJECT
    public:
        /**
         * Constructor - showActions lets you indicate whether you should show the 
         * buttons for various behaviors such as select and goto.
         */
        ActorResultsTable(bool showActions, bool showGoto, QWidget *parent = 0);

        /**
         * Destructor
         */
        virtual ~ActorResultsTable();

        /**
         * Add a vector of ActorProxies to the table. 
         */
        void addProxies(std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > foundProxies);

        /** 
         * Add just a single ActorProxy to the table. Puts it at the end
         */
        void addProxy(dtCore::RefPtr<dtDAL::ActorProxy> myProxy, bool updateCount = true);

        /**
         * Enable or disable the action buttons appropriately based 
         * on selection
         */
        void doEnableButtons();

    public slots:
        /**
         * Clear out the table.  Removes all entries.  
         */
        void clearAll();

        /**
         * User pressed the "Select" Button 
         */
        void sendSelection();

        /**
         * User pressed the goto button
         */
        void gotoPressed();

        /**
         * User pressed the duplicate button
         */
        void duplicatePressed();

        /**
         * User pressed the delete button
         */
        void deletePressed();

        /** 
         * Traps the table change event
         */
        //void currentItemChanged(QTreeWidgetItem *current, QTableWidgetItem *previous ) ;

        /**
         * Traps the table double clicked event
         */
        //void itemDoubleClicked(QTreeWidgetItem * item, int column); 

        /**
         * Remove an item from our search results if it's about to be destroyed
         */
        void actorProxyAboutToBeDestroyed(ActorProxyRefPtr proxy);

        /**
         * Handle the selection changed event in the tree table.  Should enable/disable buttons
         * Note - This event happens from our table and is different from selectedActors
         */
        void onSelectionChanged();

        /**
         * Change our selection if objects get selected somewhere else
         * Note - This slot occurs from EditorEvents.  And is separate from our internal method.
         */
        void selectedActors(ActorProxyRefPtrVector &actors);

    private:
        /** 
         * Simple utility to get the currently selected item.  Only one.
         */
        ActorResultsTreeItem *getSelectedResultTreeWidget();

        /**
         * Update our results count in the parent group box, if one is present
         */
        void updateResultsCount();

        // indicates whether to show the action buttons (select, goto, ...)
        bool showActions;
        // do we show the goto?  This is only relevant if showActions is true
        bool showGoto;

        // protect us from recursively emitting selection in the event it comes from us.
        bool recurseProtectSendingSelection;
        // protect us from recursive issues - someone else selects, we clear our list
        // and then we emit a new blank selection.  Bad.
        bool recurseProtectEmitSelectionChanged;

        //QGroupBox *searchGroup();

        ActorResultsSubTree *table;
        QPushButton *gotoBtn;
        //QPushButton *selectBtn;
        QPushButton *dupBtn;
        QPushButton *deleteBtn;
        // if our parent is a group box, then we can change the title.
        QGroupBox *parentBox;
        QString parentBaseTitle;
    };

}

#endif
