#ifndef UNDOCOMMANDS_h__
#define UNDOCOMMANDS_h__

#include <QtGui/QUndoCommand>
#include <dtAI/aiplugininterface.h>

namespace dtAI
{
   class WaypointInterface;
}

/** Delete a waypoint command.  Undo-ing will re-add it (and it's former connections),
  * while redo-ing will re-delete it.
  */
class DeleteWaypointCommand : public QUndoCommand
{
public:
   DeleteWaypointCommand(dtAI::WaypointInterface& wp,
                         dtAI::AIPluginInterface* aiInterface,
                         QUndoCommand* parent=NULL);
   
   virtual ~DeleteWaypointCommand();
    
   virtual void redo();
   virtual void undo();

private:
   dtAI::WaypointInterface *mWp;
   dtAI::AIPluginInterface::ConstWaypointArray mConnectedWaypoints;
   dtAI::AIPluginInterface* mAIInterface;
};

/** Add a waypoint command.  Undo-ing will remove it while redo-ing will re-add it.
  */
class AddWaypointCommand : public QUndoCommand
{
public:
   AddWaypointCommand(dtAI::WaypointInterface& wp,
                      dtAI::AIPluginInterface* aiInterface,
                      QUndoCommand* parent=NULL);

   virtual ~AddWaypointCommand();

   virtual void redo();
   virtual void undo();
   
private:
   dtAI::WaypointInterface *mWp;
   dtAI::AIPluginInterface* mAIInterface;
};

#endif // UNDOCOMMANDS_h__
