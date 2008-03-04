#ifndef _POSEMESH_SCENE_H_
#define _POSEMESH_SCENE_H_

#include <QtGui/QGraphicsScene>
#include <QtCore/QTimer>
#include <QtCore/QRectF>

#include <vector>
#include <string>

namespace dtAnim
{
   class PoseMesh;
   class Cal3DModelWrapper;
}

class QGraphicsItem;
class QGraphicsRectItem;
class PoseMeshItem;

class PoseMeshScene: public QGraphicsScene
{
   Q_OBJECT
public:   

   PoseMeshScene(QObject *parent = 0);
   PoseMeshScene(const QRectF &sceneRect, QObject *parent = 0);
   ~PoseMeshScene();   
  
   void AddMesh(const dtAnim::PoseMesh &poseMesh, dtAnim::Cal3DModelWrapper *model);   

   PoseMeshItem* GetPoseMeshItemByName(const std::string &name);

   virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
   virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);


signals:

   void ViewPoseMesh(const std::string &meshName);

private slots:
   void OnPoseMeshStatusChanged(const std::string &meshName, bool isEnabled);

private: 

   void CreateTest();

   std::vector<PoseMeshItem*> mItemList;
 
   QRectF mCanvasRect;
   float mVerticalMeshOffset;
   
};

#endif //_POSEMESH_SCENE_H_
