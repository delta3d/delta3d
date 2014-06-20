#ifndef __POSEMESH_SCENE_H__
#define __POSEMESH_SCENE_H__

#include <QtGui/QGraphicsScene>
#include <QtCore/QTimer>
#include <QtCore/QRectF>

#include <vector>
#include <string>

namespace dtAnim
{
   class PoseMesh;
   class CharDrawable;
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
  
   void AddMesh(const dtAnim::PoseMesh &poseMesh, dtAnim::CharDrawable *model);   

   PoseMeshItem* GetPoseMeshItemByName(const std::string &name);
   const std::vector<PoseMeshItem*>& GetPoseMeshItemList() { return mItemList; }

   void OnBlendUpdate();

   virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
   virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);

signals:

   void ViewPoseMesh(const std::string &meshName);
   void PoseMeshItemAdded(const PoseMeshItem *newItem);

private slots:
   void OnPoseMeshStatusChanged(const std::string &meshName, bool isEnabled);

private: 

   void CreateTest();

   std::vector<PoseMeshItem*> mItemList;
 
   QRectF mCanvasRect;
   float mVerticalMeshOffset;
   
};

#endif //_POSEMESH_SCENE_H_
