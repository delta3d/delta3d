/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - actortypetreewidget (.h & .cpp) - Using 'The MIT License'
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
 * David Guthrie
 */
#ifndef DELTA_OBJECTTYPETREEWIDGET
#define DELTA_OBJECTTYPETREEWIDGET

#include <QtGui/QTreeWidgetItem>
#include <QtGui/QTreeWidget>
#include <dtCore/objecttype.h>

/**
 * This class is the tree widget that is used to draw the object type.  The internal
 * nodes represent categories.  The leaf nodes represent object types.  They should
 * be drawn differently and only leaf nodes can be created or selected (in theory).
 */
class ObjectTypeTreeWidget : public QTreeWidgetItem
{
public:
   static const std::string CATEGORY_SEPARATOR;

   /**
    * Constructor - Use this for leaf nodes.  These should be object type and can
    * be created, selected, etc...
    *
    * @param parent The parent of this tree widget
    * @param actorType The Actor type of this tree entry.
    */
   ObjectTypeTreeWidget(ObjectTypeTreeWidget* parent, dtCore::RefPtr<const dtCore::ObjectType> actorType);

   /**
    * Constructor - Use this for internal, NON leaf nodes.  These are considered to be
    * category placeholders and should not be creatable.
    *
    * @param parent The parent of this tree widget
    * @param str The display string for this category (ie, 'dtCore', 'Vehicles', 'Trucks')
    *   You should only pass in the single segement, 'vehicles' not 'stuff.vehicles'
    */
   ObjectTypeTreeWidget(ObjectTypeTreeWidget* parent, const QString& str);

   /**
    * Constructor - The root node needs a separate constructor
    *
    * @param parent The parent of this tree widget
    * @param str The display string for this category (ie, 'dtCore', 'Vehicles', 'Trucks')
    *   You should only pass in the single segement, 'vehicles' not 'stuff.vehicles'
    */
   ObjectTypeTreeWidget(QTreeWidget* parent, const QString& str);

   /**
    * Gets the object type for this node.  If it's an internal node, this should be null.
    * A leaf node should always have an object type.
    *
    * @return The object type for this leaf node
    */
   dtCore::RefPtr<const dtCore::ObjectType> GetObjectType();

   /**
    * Returns true if this is a leaf node. You can check this by checking the object type for NULL.
    *
    * @return True if this is a leaf node and therefore a valid object type
    */
   bool IsLeafNode();

   /**
    * Gets the category segment.  The category segment is used to draw internal
    * nodes.  For leaf nodes, this is null.
    *
    * @return The category segment. NULL if this is a leaf node, otherwise, the string segment
    *  that represents just the internal section - ie, if this was the node at
    *  'stuff.vehicles.cars', then this method would only return 'cars'.
    */
   const QString& GetCategorySegment();

   /**
    * Simple utility method to get the category segment if it's a category, or to get the
    * object type name if it's a leaf node.
    */
   QString GetCategoryOrName();

   /**
    * This recursive function attempts to build up the appropriate parents and
    * child nodes to make an object type and it's categories fit into the tree.
    * It takes the categories as a QStringList mutable iterator. It pops the top off
    * and tries to find it.  If it does, then it adds the rest of hte iterators to that
    * item, recursively.  If it does not find a match, then it creates a new internal node
    * and adds the actor to that node recursively.  If the iterator list is empty, then
    * it assumes it is at the right level and just adds it to this node.
    *
    * This returns true if it was successfully added.  False means some error occured.
    *
    * @param listIterator The iterator that we're working with.  This WILL be modified
    * @param actorType The object type we're trying to insert
    * @return True if we succcessfully added it, false if some error or already exists.
    */
   bool RecursivelyAddCategoryAndObjectTypeAsChildren(
      QMutableStringListIterator* listIterator, dtCore::RefPtr<const dtCore::ObjectType> actorType);

private:
   // the object type for this tree element.  Note that only leaf nodes have actors
   // In fact, if the object type is null, it's assumed to be an internal node
   dtCore::RefPtr<const dtCore::ObjectType> mObjectType;

   // the portion of the category segment that this tree widget represents.
   // This is only valid if it's an internal node (ie, myObjectType == null).
   QString mCategorySegment;
};

#endif // DELTA_OBJECTTYPETREEWIDGET
