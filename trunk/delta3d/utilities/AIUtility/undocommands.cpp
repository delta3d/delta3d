#include "undocommands.h"
#include <dtAI/aidebugdrawable.h>

////////////////////////////////////////////////////////////////////////////////
DeleteWaypointCommand::DeleteWaypointCommand(dtAI::WaypointInterface& wp,
                                             dtAI::AIPluginInterface* aiInterface,
                                             QUndoCommand* parent)
: QUndoCommand(parent)
, mWp(&wp)
, mAIInterface(aiInterface)
{
   setText("Delete Waypoint");

   if (mAIInterface)
   {
      mAIInterface->GetAllEdgesFromWaypoint(mWp->GetID(), mConnectedWaypoints);
   }
}

////////////////////////////////////////////////////////////////////////////////
DeleteWaypointCommand::~DeleteWaypointCommand()
{
}

////////////////////////////////////////////////////////////////////////////////
void DeleteWaypointCommand::redo()
{
   //redo this command should re-delete the wp
   if (mAIInterface)
   {
      mAIInterface->RemoveWaypoint(mWp);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DeleteWaypointCommand::undo()
{
   //undo the command should add this wp and it's original connections back
   if (mAIInterface)
   {
      mAIInterface->InsertWaypoint(mWp);

      //reconnect previous connections as well
      dtAI::AIPluginInterface::ConstWaypointArray::iterator itr = mConnectedWaypoints.begin();
      while (itr != mConnectedWaypoints.end())
      {
         mAIInterface->AddEdge(mWp->GetID(), (*itr)->GetID());

         //need to manually update the debug drawing
         if (mAIInterface->GetDebugDrawable())
         {
            mAIInterface->GetDebugDrawable()->AddEdge(mWp, (*itr));
         }

         ++itr;
      }
   }
}
