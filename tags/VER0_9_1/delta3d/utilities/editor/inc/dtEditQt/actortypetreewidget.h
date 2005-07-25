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
#ifndef __ActorTypeTreeWidget__h
#define __ActorTypeTreeWidget__h

#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <osg/ref_ptr>
#include "dtDAL/actortype.h"

namespace dtEditQt {


    /**
    * This class is the tree widget that is used to draw the actor type.  The internal 
    * nodes represent categories.  The leaf nodes represent actor types.  They should
    * be drawn differently and only leaf nodes can be created or selected (in theory).
    */
    class ActorTypeTreeWidget : public QTreeWidgetItem {
    public:
        static std::string CATEGORY_SEPARATOR;

        /**
        * Constructor - Use this for leaf nodes.  These should be actor type and can 
        * be created, selected, etc...
        *
        * @param parent The parent of this tree widget 
        * @param actorType The Actor type of this tree entry. 
        */
        ActorTypeTreeWidget(ActorTypeTreeWidget* parent, osg::ref_ptr<dtDAL::ActorType> actorType);

        /**
        * Constructor - Use this for internal, NON leaf nodes.  These are considered to be 
        * category placeholders and should not be creatable.
        *
        * @param parent The parent of this tree widget 
        * @param str The display string for this category (ie, 'dtCore', 'Vehicles', 'Trucks')
        *   You should only pass in the single segement, 'vehicles' not 'stuff.vehicles'
        */
        ActorTypeTreeWidget(ActorTypeTreeWidget* parent, const QString &str);

        /**
        * Constructor - The root node needs a separate constructor
        *
        * @param parent The parent of this tree widget 
        * @param str The display string for this category (ie, 'dtCore', 'Vehicles', 'Trucks')
        *   You should only pass in the single segement, 'vehicles' not 'stuff.vehicles'
        */
        ActorTypeTreeWidget(QTreeWidget* parent, const QString &str);

        /**
        * Gets the actor type for this node.  If it's an internal node, this should be null.
        * A leaf node should always have an actor type.
        *
        * @return The actor type for this leaf node
        */
        osg::ref_ptr<dtDAL::ActorType> getActorType();

        /**
        * Returns true if this is a leaf node. You can check this by checking the actor type for NULL.
        *
        * @return True if this is a leaf node and therefore a valid actor type
        */
        bool isLeafNode();

        /**
        * Gets the category segment.  The category segment is used to draw internal 
        * nodes.  For leaf nodes, this is null.
        *
        * @return The category segment. NULL if this is a leaf node, otherwise, the string segment 
        *   that represents just the internal section - ie, if this was the node at 
        *   'stuff.vehicles.cars', then this method would only return 'cars'.
        */
        const QString &getCategorySegment();

        /**
         * Simple utility method to get the category segment if it's a category, or to get the
         * actor type name if it's a leaf node.
         */
        QString getCategoryOrName();

        /**
        * This recursive function attempts to build up the appropriate parents and 
        * child nodes to make an actor type and it's categories fit into the tree.
        * It takes the categories as a QStringList mutable iterator. It pops the top off
        * and tries to find it.  If it does, then it adds the rest of hte iterators to that 
        * item, recursively.  If it does not find a match, then it creates a new internal node
        * and adds the actor to that node recursively.  If the iterator list is empty, then 
        * it assumes it is at the right level and just adds it to this node.
        * 
        * This returns true if it was successfully added.  False means some error occured.
        * 
        * @param listIterator The iterator that we're working with.  This WILL be modified
        * @param actorType The actor type we're trying to insert
        * @return True if we succcessfully added it, false if some error or already exists.
        */
        bool recursivelyAddCategoryAndActorTypeAsChildren(
            QMutableStringListIterator *listIterator, osg::ref_ptr<dtDAL::ActorType> actorType);

    private:

        // the actor type for this tree element.  Note that only leaf nodes have actors
        // In fact, if the actor type is null, it's assumed to be an internal node
        osg::ref_ptr<dtDAL::ActorType> myActorType;

        // the portion of the category segment that this tree widget represents.
        // This is only valid if it's an internal node (ie, myActorType == null).
        QString categorySegment;

    };
}

#endif
