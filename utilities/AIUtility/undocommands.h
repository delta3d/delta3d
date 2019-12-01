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


/** Add an edge between two waypoints command.  Undo-ing will remove it while redo-ing will re-add it.
  */
class AddEdgeCommand : public QUndoCommand
{
public:
   AddEdgeCommand(dtAI::WaypointInterface& fromWp,
                  dtAI::WaypointInterface& toWp,
                  dtAI::AIPluginInterface* aiInterface,
                  QUndoCommand* parent=NULL);

   virtual ~AddEdgeCommand();

   virtual void redo();
   virtual void undo();
   
private:
   dtAI::WaypointInterface *mFromWp;
   dtAI::WaypointInterface *mToWp;
   dtAI::AIPluginInterface* mAIInterface;
};

/** Remove an edge between two waypoints command.  
  * Undo-ing will re-add it while redo-ing will remove it.
  */
class RemoveEdgeCommand : public QUndoCommand
{
public:
   RemoveEdgeCommand(dtAI::WaypointInterface& fromWp,
                     dtAI::WaypointInterface& toWp,
                     dtAI::AIPluginInterface* aiInterface,
                     QUndoCommand* parent=NULL);

   virtual ~RemoveEdgeCommand();

   virtual void redo();
   virtual void undo();
   
private:
   dtAI::WaypointInterface *mFromWp;
   dtAI::WaypointInterface *mToWp;
   dtAI::AIPluginInterface* mAIInterface;
};


/** Move a waypoint command.  Undo-ing will revert back to the it's original
  * position.  Redo-ing will move it to the new position.
  */
class MoveWaypointCommand : public QObject, public QUndoCommand
{
   Q_OBJECT
public:
   MoveWaypointCommand(const osg::Vec3& oldXYZ, const osg::Vec3& newXYZ, 
                       dtAI::WaypointInterface& waypoint,
                       dtAI::AIPluginInterface* aiInterface,
                       QUndoCommand* parent = NULL);

   virtual ~MoveWaypointCommand();

   virtual void redo();
   virtual void undo();

signals:
   void WaypointsMoved();

protected:
   
private:
   const osg::Vec3 mOldXYZ;
   const osg::Vec3 mNewXYZ;
   dtAI::WaypointInterface *mWp;
   dtAI::AIPluginInterface* mAIInterface;
};
#endif // UNDOCOMMANDS_h__
