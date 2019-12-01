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
   setText("Delete Waypoint ID#" + QString::number(mWp->GetID()));
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
      mAIInterface->GetAllEdgesFromWaypoint(mWp->GetID(), mConnectedWaypoints);

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
         mAIInterface->GetDebugDrawable()->AddEdge(mWp, (*itr));

         ++itr;
      }
   }
}



////////////////////////////////////////////////////////////////////////////////
AddWaypointCommand::AddWaypointCommand(dtAI::WaypointInterface& wp, dtAI::AIPluginInterface* aiInterface, QUndoCommand* parent/*=NULL*/)
: QUndoCommand(parent)
, mWp(&wp)
, mAIInterface(aiInterface)
{
   setText("Add Waypoint ID#" + QString::number(mWp->GetID()));
}

////////////////////////////////////////////////////////////////////////////////
AddWaypointCommand::~AddWaypointCommand()
{
}

////////////////////////////////////////////////////////////////////////////////
void AddWaypointCommand::redo()
{
   if (mAIInterface)
   {
      mAIInterface->InsertWaypoint(mWp);
   }
}

////////////////////////////////////////////////////////////////////////////////
void AddWaypointCommand::undo()
{
   if (mAIInterface)
   {
      mAIInterface->RemoveWaypoint(mWp);
   }
}

////////////////////////////////////////////////////////////////////////////////
AddEdgeCommand::AddEdgeCommand(dtAI::WaypointInterface& fromWp,
                               dtAI::WaypointInterface& toWp,
                               dtAI::AIPluginInterface* aiInterface,
                               QUndoCommand* parent)
: QUndoCommand(parent)
, mFromWp(&fromWp)
, mToWp(&toWp)
, mAIInterface(aiInterface)
{
   setText("Add Edge #" + QString::number(mFromWp->GetID()) + "->" + QString::number(mToWp->GetID()));
}

////////////////////////////////////////////////////////////////////////////////
AddEdgeCommand::~AddEdgeCommand()
{
}

////////////////////////////////////////////////////////////////////////////////
void AddEdgeCommand::redo()
{
   if (mAIInterface)
   {
      mAIInterface->AddEdge(mFromWp->GetID(), mToWp->GetID());
      mAIInterface->GetDebugDrawable()->AddEdge(mFromWp, mToWp);
   }
}

////////////////////////////////////////////////////////////////////////////////
void AddEdgeCommand::undo()
{
   if (mAIInterface)
   {
      if (mAIInterface->RemoveEdge(mFromWp->GetID(), mToWp->GetID()))
      {
         mAIInterface->GetDebugDrawable()->RemoveEdge(mFromWp, mToWp);
      }
      else
      {
         LOG_ERROR("Could not remove edge" );

      }
   }
}



////////////////////////////////////////////////////////////////////////////////
RemoveEdgeCommand::RemoveEdgeCommand(dtAI::WaypointInterface& fromWp,
                                     dtAI::WaypointInterface& toWp,
                                     dtAI::AIPluginInterface* aiInterface,
                                     QUndoCommand* parent)
: QUndoCommand(parent)
, mFromWp(&fromWp)
, mToWp(&toWp)
, mAIInterface(aiInterface)
{
   setText("Remove Edge #" + QString::number(mFromWp->GetID()) + "->" + QString::number(mToWp->GetID()));
}

////////////////////////////////////////////////////////////////////////////////
RemoveEdgeCommand::~RemoveEdgeCommand()
{
}

////////////////////////////////////////////////////////////////////////////////
void RemoveEdgeCommand::redo()
{
   if (mAIInterface)
   {
      mAIInterface->RemoveEdge(mFromWp->GetID(), mToWp->GetID());
      mAIInterface->GetDebugDrawable()->RemoveEdge(mFromWp, mToWp);
   }
}

////////////////////////////////////////////////////////////////////////////////
void RemoveEdgeCommand::undo()
{
   if (mAIInterface)
   {
      mAIInterface->AddEdge(mFromWp->GetID(), mToWp->GetID());
      mAIInterface->GetDebugDrawable()->AddEdge(mFromWp, mToWp);
   }
}

////////////////////////////////////////////////////////////////////////////////
MoveWaypointCommand::MoveWaypointCommand(const osg::Vec3& oldXYZ, const osg::Vec3& newXYZ,
                                         dtAI::WaypointInterface& waypoint,
                                         dtAI::AIPluginInterface* aiInterface,
                                         QUndoCommand* parent)
: QUndoCommand(parent)
, mOldXYZ(oldXYZ)
, mNewXYZ(newXYZ)
, mWp(&waypoint)
, mAIInterface(aiInterface)
{
   setText("Move Waypoint ID#" + QString::number(mWp->GetID()));
}

////////////////////////////////////////////////////////////////////////////////
MoveWaypointCommand::~MoveWaypointCommand()
{

}

////////////////////////////////////////////////////////////////////////////////
void MoveWaypointCommand::redo()
{
   mAIInterface->MoveWaypoint(mWp, mNewXYZ);
   mAIInterface->GetDebugDrawable()->InsertWaypoint(*mWp);

   emit WaypointsMoved();
}

////////////////////////////////////////////////////////////////////////////////
void MoveWaypointCommand::undo()
{
   mAIInterface->MoveWaypoint(mWp, mOldXYZ);
   mAIInterface->GetDebugDrawable()->InsertWaypoint(*mWp);

   emit WaypointsMoved();
}
