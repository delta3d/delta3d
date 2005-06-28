/* 
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2005, BMH Associates, Inc. 
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
* @author Curtiss Murphy
*/

#ifndef __ActorBrowser_h
#define __ActorBrowser_h

#include <QWidget>
#include <osg/ref_ptr>
#include <vector>

#include "dtDAL/tree.h"
#include "dtDAL/actorproxy.h"
#include "dtDAL/actortype.h"
#include "dtEditQt/actortypetreewidget.h"

class QGroupBox;
class QTreeWidget;
class QPushButton;
class QModelIndex;

namespace dtEditQt 
{
    /**
    * @class ActorBrowser
    * @brief This class displays the currently available actors loaded with the map
    */
    class ActorBrowser : public QWidget
    {
        Q_OBJECT

    public:
        /**
        * Constructor
        */
        ActorBrowser(QWidget *parent = 0);

        /**
        * Destructor
        */
        ~ActorBrowser();

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
         * Slot - Handles refreshing the actor types upon user library importation
         */
        void refreshActorTypes();

        /**
         * Slot - handles the about to remove library event.  This is necessary because
         * before the actual library removed event, we need to clear our list or else we'll 
         * be holding onto memory that has since been removed.  It completely clears the tree.
         */
        void clearActorTypesTree();
    
    private:
        // known list of actor types
        std::vector<osg::ref_ptr<dtDAL::ActorType> > actorTypes;

        /**
        * Creates the main User Interface for the Actor Browser
        */
        void setupGUI();

        /**
        * Clear out and rebuild the actor types tree.  This should be called internall when
        * you suspect that the list of Actor Types changed in the DAL.
        */
        void reloadActors();

        /**
        * A convenience method to returns the selected tree widget or NULL. 
        * @return The selected actor tree widget.  NULL if no selection.
        */
        ActorTypeTreeWidget *getSelectedActorTreeWidget();

        QTreeWidget *tree; 
        QPushButton *createActorBtn;

        ActorTypeTreeWidget *rootActorType;

        // this is a tree of actor type names which were expanded.  It is used 
        // when we reload actor types.  We walk the tree and look for 
        // expanded items.  For each one, we add it to this tree.  Then, we walk 
        // back through this tree to reexpand items later
        core::tree<QString> expandedActorTypeNames; 
        bool rootNodeWasExpanded;
        int lastScrollBarLocation;

        /**
         * Looks at the current actor tree and tries to mark which actor types are 
         * currently expanded.  This is then used to re-expand them with restorePreviousExpansion()
         * after the tree is rebuilt.
         */
        void markCurrentExpansion();

        /** 
         * recursive method to support markCurrentExpansion().
         */
        void recurseMarkCurrentExpansion(ActorTypeTreeWidget *parent, 
            core::tree<QString> &currentTree);

        /**
         * Attempts to re-expand previously expanded actor types.  This is a nicity for the user
         * for when they load libraries and such.  
         */
        void restorePreviousExpansion();

        /**
         * Recursive method to support restorePreviousExpansion().
         */
        void recurseRestorePreviousExpansion(ActorTypeTreeWidget *parent, 
            core::tree<QString> &currentTree);
    };
}

#endif
