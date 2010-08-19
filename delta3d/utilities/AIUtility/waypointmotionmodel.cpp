#include "waypointmotionmodel.h"
#include <dtAI/waypointinterface.h>
#include <dtAI/aidebugdrawable.h>
#include <dtCore/transform.h>

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
      //(*itr)->SetPosition((*itr)->GetPosition() + delta);
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

      dtCore::Transform xform;
      xform.SetTranslation(selectedWaypoints[0]->GetPosition()); //TODO use the median of the selected waypoints
      GetTarget()->SetTransform(xform);

      UpdateWidgets(); //to move the widgets to the new location
   }
}

////////////////////////////////////////////////////////////////////////////////
void WaypointMotionModel::SetAIInterface(dtAI::AIPluginInterface* aiInterface)
{
   mAIInterface = aiInterface;
}
