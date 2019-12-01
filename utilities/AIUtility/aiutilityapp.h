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

#ifndef AIUTILITYAPP_H_
#define AIUTILITYAPP_H_

#include <QtCore/QObject>
#include <dtABC/application.h>
#include <dtCore/motionmodel.h>
#include <dtCore/transform.h>
#include <dtQt/deltastepper.h>

#include <dtGame/gamemanager.h>

class AIUtilityInputComponent;
class WaypointMotionModel;

class QUndoCommand;

namespace dtAI
{
   class AIPluginInterface;
   class WaypointInterface;
}

class AIUtilityApp: public QObject, public dtABC::Application
{
   Q_OBJECT
public:
   typedef dtABC::Application BaseClass;

   AIUtilityApp();
   virtual ~AIUtilityApp();
   virtual void Config();
   void SetAIPluginInterface(dtAI::AIPluginInterface* interface, bool selectionBasedRenderingHint);
signals:
   void AIPluginInterfaceChanged(dtAI::AIPluginInterface* interface, bool selectionBasedRenderingHint);
   void CameraTransformChanged(const dtCore::Transform& xform);
   void Error(const std::string& message);
   void UndoCommandGenerated(QUndoCommand* command);

public slots:
   void DoQuit();
   void SetProjectContext(const std::string& path);
   void ChangeMap(const std::string& map);
   void CloseMap();
   void TransformCamera(const dtCore::Transform&);
   void AddAIInterfaceToMap(const std::string& map);
   void OnSelectWaypontBrushMode(bool enable);
   void OnWaypointBrushSizeChanged(double value);
   void OnRenderOnSelectChanged(bool enabled);
   void OnRenderBackfacesChanged(bool enabled);
   void OnUndoCommandCreated(QUndoCommand* undoCommand);
   void OnGroundClampSelectedWaypoints();
   void OnWaypointPropertyBaseChanged();
   void OnWaypointSelectionChanged(std::vector<dtAI::WaypointInterface*>& selectedWaypoints);

protected:
   ///override for preframe
   virtual void PreFrame(const double deltaSimTime);
private:
   QUndoCommand* GroundClampWaypoints(std::vector<dtAI::WaypointInterface*> &selected);

   dtQt::DeltaStepper mStepper;
   dtCore::RefPtr<dtGame::GameManager> mGM;
   dtCore::RefPtr<dtCore::MotionModel> mMotionModel;
   dtCore::RefPtr<WaypointMotionModel> mWaypointMotionModel;
   dtCore::Transform mLastCameraTransform;
   dtCore::RefPtr<AIUtilityInputComponent> mInputComponent;
   dtCore::RefPtr<dtAI::AIPluginInterface> mAIInterface;
   bool mSelectionBasedRendering; ///Do we only render expensive graphics for selected waypoints
};

#endif /* AIUTILITYAPP_H_ */
