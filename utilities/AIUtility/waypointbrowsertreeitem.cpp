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

#include "waypointbrowsertreeitem.h"
#include <dtUtil/stringutils.h>


//////////////////////////////////////////////////
WaypointBrowserTreeItem::WaypointBrowserTreeItem(QTreeWidgetItem* parent, dtAI::WaypointInterface& waypoint)
: QTreeWidgetItem(parent)
, mWaypoint(&waypoint)
{
   setText(0, dtUtil::ToString(waypoint.GetID()).c_str());
   setText(1, dtUtil::ToString(waypoint.GetPosition()).c_str());
}

//////////////////////////////////////////////////
WaypointBrowserTreeItem::WaypointBrowserTreeItem(QTreeWidgetItem* parent, const QString& str)
: QTreeWidgetItem(parent)
, mWaypoint(NULL)
{
   setText(0, str);
}

//////////////////////////////////////////////////
WaypointBrowserTreeItem::WaypointBrowserTreeItem(QTreeWidget* parent, const QString& str)
: QTreeWidgetItem(parent)
, mWaypoint(NULL)
{
   setText(0, str);
}

//////////////////////////////////////////////////
WaypointBrowserTreeItem::~WaypointBrowserTreeItem()
{
}

////////////////////////////////////////////////////////////////////////////////
bool WaypointBrowserTreeItem::operator<(const QTreeWidgetItem& other) const
{
   const WaypointBrowserTreeItem& comparisonItem = static_cast<const WaypointBrowserTreeItem&>(other);
   return (this->GetWaypoint()->GetID() < comparisonItem.GetWaypoint()->GetID());
}

//////////////////////////////////////////////////
dtAI::WaypointInterface* WaypointBrowserTreeItem::GetWaypoint()
{
   return mWaypoint;
}

//////////////////////////////////////////////////
const dtAI::WaypointInterface* WaypointBrowserTreeItem::GetWaypoint() const
{
   return mWaypoint;
}
