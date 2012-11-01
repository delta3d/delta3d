#ifndef WAYPOINTMOTIONMODEL_h__
#define WAYPOINTMOTIONMODEL_h__

#include <dtCore/objectmotionmodel.h>
#include <dtCore/observerptr.h>
#include <QtCore/QObject>
#include <dtAI/aiplugininterface.h>

namespace dtAI
{
   class WaypointInterface;
}

class QUndoCommand;

class WaypointMotionModel : public QObject, public dtCore::ObjectMotionModel
{
   Q_OBJECT
public:
   WaypointMotionModel(dtCore::View* view);

   virtual void OnTranslate(const osg::Vec3& delta);

   void UpdateWidgetsForSelection();
   void SetAIInterface(dtAI::AIPluginInterface* aiInterface);

public slots:
   void OnWaypointSelectionChanged(std::vector<dtAI::WaypointInterface*>& selectedWaypoints);
   void OnWaypointsMoved();

signals:
   void UndoCommandGenerated(QUndoCommand* command);
   void WaypointTranslationBeginning();

protected:
   virtual ~WaypointMotionModel();

   virtual void OnTranslateBegin();
   virtual void OnTranslateEnd();

private:
   std::vector<dtAI::WaypointInterface*> mCurrentWaypoints;
   dtCore::ObserverPtr<dtAI::AIPluginInterface> mAIInterface;
   osg::Vec3 mStartMoveXYZ; ///<the position of the MotionModel target when the move begins
};
#endif // WAYPOINTMOTIONMODEL_h__
