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


class WaypointMotionModel : public QObject, public dtCore::ObjectMotionModel
{
   Q_OBJECT
public:
   WaypointMotionModel(dtCore::View* view);

   virtual void OnTranslate(const osg::Vec3& delta);
   void SetAIInterface(dtAI::AIPluginInterface* aiInterface);

public slots:
   void OnWaypointSelectionChanged(std::vector<dtAI::WaypointInterface*>& selectedWaypoints);

protected:
   virtual ~WaypointMotionModel();
   
private:
   std::vector<dtAI::WaypointInterface*> mCurrentWaypoints;
   dtCore::ObserverPtr<dtAI::AIPluginInterface> mAIInterface;
};
#endif // WAYPOINTMOTIONMODEL_h__
