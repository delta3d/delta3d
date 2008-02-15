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
  
   void AddMesh(const dtAnim::PoseMesh &mesh);   

   PoseMeshItem* GetPoseMeshItemByName(const std::string &name);

protected: 

private: 

   void CreateTest();

   std::vector<PoseMeshItem*> mItemList;
 
   QRectF mCanvasRect;
};

#endif //_POSEMESH_SCENE_H_
