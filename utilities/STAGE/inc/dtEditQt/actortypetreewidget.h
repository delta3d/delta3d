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
 */
#ifndef DELTA_ACTORTYPETREEWIDGET
#define DELTA_ACTORTYPETREEWIDGET

#include <QtGui/QTreeWidgetItem>
#include <QtGui/QTreeWidget>
#include <dtCore/actortype.h>

namespace dtEditQt
{

   /**
    * This class is the tree widget that is used to draw the actor type.  The internal
    * nodes represent categories.  The leaf nodes represent actor types.  They should
    * be drawn differently and only leaf nodes can be created or selected (in theory).
    */
   class ActorTypeTreeWidget : public QTreeWidgetItem
   {
   public:
      static const std::string CATEGORY_SEPARATOR;

      /**
       * Constructor - Use this for leaf nodes.  These should be actor type and can
       * be created, selected, etc...
       *
       * @param parent The parent of this tree widget
       * @param actorType The Actor type of this tree entry.
       */
      ActorTypeTreeWidget(ActorTypeTreeWidget* parent, dtCore::RefPtr<const dtCore::ActorType> actorType);

      /**
       * Constructor - Use this for internal, NON leaf nodes.  These are considered to be
       * category placeholders and should not be creatable.
       *
       * @param parent The parent of this tree widget
       * @param str The display string for this category (ie, 'dtCore', 'Vehicles', 'Trucks')
       *   You should only pass in the single segement, 'vehicles' not 'stuff.vehicles'
       */
      ActorTypeTreeWidget(ActorTypeTreeWidget* parent, const QString& str);

      /**
       * Constructor - The root node needs a separate constructor
       *
       * @param parent The parent of this tree widget
       * @param str The display string for this category (ie, 'dtCore', 'Vehicles', 'Trucks')
       *   You should only pass in the single segement, 'vehicles' not 'stuff.vehicles'
       */
      ActorTypeTreeWidget(QTreeWidget* parent, const QString& str);

      /**
       * Gets the actor type for this node.  If it's an internal node, this should be null.
       * A leaf node should always have an actor type.
       *
       * @return The actor type for this leaf node
       */
      dtCore::RefPtr<const dtCore::ActorType> getActorType();

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
       *  that represents just the internal section - ie, if this was the node at
       *  'stuff.vehicles.cars', then this method would only return 'cars'.
       */
      const QString& getCategorySegment();

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
         QMutableStringListIterator* listIterator, dtCore::RefPtr<const dtCore::ActorType> actorType);

   private:
      // the actor type for this tree element.  Note that only leaf nodes have actors
      // In fact, if the actor type is null, it's assumed to be an internal node
      dtCore::RefPtr<const dtCore::ActorType> mActorType;

      // the portion of the category segment that this tree widget represents.
      // This is only valid if it's an internal node (ie, myActorType == null).
      QString mCategorySegment;
   };

} // namespace dtEditQt

#endif // DELTA_ACTORTYPETREEWIDGET
