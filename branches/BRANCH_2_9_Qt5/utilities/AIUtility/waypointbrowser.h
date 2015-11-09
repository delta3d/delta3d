/* -*-c++-*-
 * Delta3D
 * Copyright 2007-2008, Alion Science and Technology
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
 * david
 */

#ifndef WAYPOINTBROWSER_H_
#define WAYPOINTBROWSER_H_

#include <QtGui/QDockWidget>
#include <dtCore/transform.h>
#include <dtCore/objecttype.h>
#include <dtUtil/utiltree.h>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace dtAI
{
   class AIPluginInterface;
   class WaypointInterface;
}

namespace Ui
{
   class WaypointBrowser;
}

class QTreeWidget;
class QTreeWidgetItem;
class QUndoCommand;

/// @endcond

class WaypointBrowser: public QDockWidget
{
   Q_OBJECT
public:
   WaypointBrowser(QWidget* parent=NULL);
   virtual ~WaypointBrowser();

   void SetPluginInterface(dtAI::AIPluginInterface* plugin);

   void GetCameraTransform(dtCore::Transform& xform);
   const dtCore::ObjectType* GetSelectedWaypointType();

signals:
   void RequestCameraTransformChange(const dtCore::Transform& xform);
   void UndoCommandGenerated(QUndoCommand* command);
   void WaypointTypeSelected(const dtCore::ObjectType* type);

protected:
   /**
    * Looks at the current actor tree and tries to mark which actor types are
    * currently expanded.  This is then used to re-expand them with RestorePreviousExpansion()
    * after the tree is rebuilt.
    */
   void MarkCurrentExpansion(QTreeWidget& tree, dtUtil::tree<QString>& storageTree, int& scrollBarLocation);

   /**
    * recursive method to support MarkCurrentExpansion().
    */
   void RecurseMarkCurrentExpansion(QTreeWidget& tree, QTreeWidgetItem* parent,
            dtUtil::tree<QString>& storageTree);

   /**
    * Attempts to re-expand previously expanded actor types.  This is a nicity for the user
    * for when they load libraries and such.
    */
   void RestorePreviousExpansion(QTreeWidget& tree, const dtUtil::tree<QString>& storageTree, int scrollBarLocation);

   /**
    * Recursive method to support restorePreviousExpansion().
    */
   void RecurseRestorePreviousExpansion(QTreeWidget& tree, QTreeWidgetItem* parent,
      const dtUtil::tree<QString>& currentTree);

public slots:
   void OnCreate();
   void OnDelete();
   void OnGoto();
   void SetCameraTransform(const dtCore::Transform& xform);
   void ResetWaypointResult();
   void OnWaypointSelectionChanged(std::vector<dtAI::WaypointInterface*>& selectedWaypoints);
protected slots:
   void EnableDisable();
   void WaypointsSelectedFromBrowser();
   void WaypointTypeSelectionChanged();

private:

   float GetCreateAndGotoDistance() const;
   void ResetTypesTree();

   Ui::WaypointBrowser* mUi;
   dtAI::AIPluginInterface* mAIPluginInterface;
   dtCore::Transform mCameraTransform;
};

#endif /* WAYPOINTBROWSER_H_ */
