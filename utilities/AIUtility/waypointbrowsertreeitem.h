/* -*-c++-*-
 * Delta3D
 * Copyright 2009, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * David Guthrie
 */

#ifndef WAYPOINTBROWSERTREEITEM_H_
#define WAYPOINTBROWSERTREEITEM_H_

#include <QtGui/QTreeWidgetItem>
#include <dtAI/waypointinterface.h>

class WaypointBrowserTreeItem: public QTreeWidgetItem
{
public:

   /**
    * Constructor - Use this for leaf nodes.  These should be actor type and can
    * be created, selected, etc...
    *
    * @param parent The parent of this tree widget
    * @param actorType The
    */
   WaypointBrowserTreeItem(QTreeWidgetItem* parent, dtAI::WaypointInterface& waypoint);

   /**
    * Constructor - Use this for internal, NON leaf nodes.  These are considered to be
    * category placeholders and should not be creatable.
    *
    * @param parent The parent of this tree widget
    * @param str The display string for this category
    */
   WaypointBrowserTreeItem(QTreeWidgetItem* parent, const QString& str);

   /**
    * Constructor - The root node needs a separate constructor
    *
    * @param parent The parent of this tree widget
    * @param str The display string for this category
    */
   WaypointBrowserTreeItem(QTreeWidget* parent, const QString& str);

   virtual ~WaypointBrowserTreeItem();

   virtual bool operator<(const QTreeWidgetItem& other) const;

   dtAI::WaypointInterface* GetWaypoint();
   const dtAI::WaypointInterface* GetWaypoint() const;
private:
   dtAI::WaypointInterface* mWaypoint;
};

#endif /* WAYPOINTBROWSERTREEITEM_H_ */
