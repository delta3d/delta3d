/* -*-c++-*-
* Delta3D Simulation Training And Game Editor (STAGE)
* STAGE - actorresultstable (.h & .cpp) - Using 'The MIT License'
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
* Curtiss Murphy
*/

#ifndef DELTA_ACTORRESULTSTABLE
#define DELTA_ACTORRESULTSTABLE

#include <QtGui/QWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>
#include <vector>
#include <dtDAL/actorproxy.h>
#include <dtDAL/actorproperty.h>
#include <dtEditQt/typedefs.h>

class QGroupBox;
class QPushButton;
class QKeyEvent;

namespace dtEditQt 
{
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
         * Find the item in our table and update it's display columns.
         */
        void HandleProxyUpdated(dtCore::RefPtr<dtDAL::ActorProxy> proxy);

       /**
         * Enable or disable the action buttons appropriately based 
         * on selection
         */
        void doEnableButtons();

        /**
         * Manually loops through our table and unselects all items except the one passed in. 
         * Yes, clearSelection() does that, but this method fixes glitches. See the method for details.
         */
        void UnselectAllItemsManually(QTreeWidgetItem* item);

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
         * Traps the table double clicked event - causes a single select by unselecting other items.
         -- can't be used because the itemdoubleclicked signal from Qt isn't found at runtime.
         */
        //void OnItemDoubleClicked(QTreeWidgetItem * item); 

        //void OnDoCurrentItemChanged(QTreeWidgetItem *test1, QTreeWidgetItem *test2);

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
