#include "waypointmotionmodel.h"
#include "undocommands.h"
#include <dtAI/waypointinterface.h>
#include <dtAI/aidebugdrawable.h>
#include <dtCore/system.h>
#include <dtCore/transform.h>
#include <QtGui/QUndoCommand>

////////////////////////////////////////////////////////////////////////////////
WaypointMotionModel::WaypointMotionModel(dtCore::View* view)
:dtCore::ObjectMotionModel(view)
{
   this->SetScale(GetScale() * 0.35f);

   //set a dummy Transformable to fill the requirement.  Not realy used in this case.
   this->SetTarget(new dtCore::Transformable("DummyWaypointTransformable"));
}

////////////////////////////////////////////////////////////////////////////////
WaypointMotionModel::~WaypointMotionModel()
{

}

////////////////////////////////////////////////////////////////////////////////
void WaypointMotionModel::OnTranslate(const osg::Vec3& delta)
{
   dtCore::ObjectMotionModel::OnTranslate(delta);
   if (!mAIInterface.valid())
   {
      return;
   }

   std::vector<dtAI::WaypointInterface*>::iterator itr = mCurrentWaypoints.begin();

   while (itr != mCurrentWaypoints.end())
   {
      mAIInterface->MoveWaypoint((*itr), (*itr)->GetPosition() + delta);

      //and update the graphics as well
      mAIInterface->GetDebugDrawable()->InsertWaypoint(**itr);

      ++itr;
   }
}

////////////////////////////////////////////////////////////////////////////////
void WaypointMotionModel::OnWaypointSelectionChanged(std::vector<dtAI::WaypointInterface*>& selectedWaypoints)
{
   mCurrentWaypoints = selectedWaypoints;

   if (selectedWaypoints.empty())
   {
      SetEnabled(false);//hide the ObjectMotionModel
   }
   else
   {
      SetEnabled(true);//show the ObjectMotionModel
      UpdateWidgetsForSelection();
   }
}

////////////////////////////////////////////////////////////////////////////////
void WaypointMotionModel::SetAIInterface(dtAI::AIPluginInterface* aiInterface)
{
   mAIInterface = aiInterface;
}

////////////////////////////////////////////////////////////////////////////////
void WaypointMotionModel::OnTranslateBegin()
{
   // Alert the app so that it can clone and reselect if needed
   emit WaypointTranslationBeginning();

   dtCore::Transform xform;
   GetTarget()->GetTransform(xform);
   mStartMoveXYZ = xform.GetTranslation();
}

////////////////////////////////////////////////////////////////////////////////
void WaypointMotionModel::OnTranslateEnd()
{
   dtCore::Transform xform;
   GetTarget()->GetTransform(xform);
   const osg::Vec3 deltaXYZ = xform.GetTranslation() - mStartMoveXYZ;

   //if there's more than one to move, batch the commands under one parent undo command
   QUndoCommand* parentUndo(mCurrentWaypoints.size() == 1 ? NULL : new QUndoCommand("Move Waypoints"));

   std::vector<dtAI::WaypointInterface*>::iterator itr = mCurrentWaypoints.begin();

   while (itr != mCurrentWaypoints.end())
   {
      const osg::Vec3 oldPos = (*itr)->GetPosition() - deltaXYZ;
      MoveWaypointCommand* undoMove = new MoveWaypointCommand(oldPos,
                                                              (*itr)->GetPosition(),
                                                              **itr,
                                                              mAIInterface.get(), parentUndo);

      connect(undoMove, SIGNAL(WaypointsMoved()), this, SLOT(OnWaypointsMoved()));

      if (parentUndo == NULL)
      {
         emit UndoCommandGenerated(undoMove);
      }

      ++itr;
   }

   if (parentUndo != NULL)
   {
      emit UndoCommandGenerated(parentUndo);
   }

   mStartMoveXYZ = xform.GetTranslation();
}

////////////////////////////////////////////////////////////////////////////////
void WaypointMotionModel::OnWaypointsMoved()
{
   UpdateWidgetsForSelection();
}

////////////////////////////////////////////////////////////////////////////////
void WaypointMotionModel::UpdateWidgetsForSelection()
{
   dtCore::Transform xform;
   xform.SetTranslation(mCurrentWaypoints[0]->GetPosition()); //TODO use the median of the selected waypoints
   GetTarget()->SetTransform(xform);

   UpdateWidgets(); //to move the widgets to the new location
}
