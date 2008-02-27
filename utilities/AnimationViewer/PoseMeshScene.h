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
   enum eMODE
   {
      MODE_INVALID = -1,
      MODE_BLEND_PICK = 0,
      MODE_GRAB,

      MODE_TOTAL
   };

   PoseMeshScene(QObject *parent = 0);
   PoseMeshScene(const QRectF &sceneRect, QObject *parent = 0);
   ~PoseMeshScene();   

  
   eMODE GetMode();
  
   void AddMesh(const dtAnim::PoseMesh &poseMesh, dtAnim::Cal3DModelWrapper *model);   

   PoseMeshItem* GetPoseMeshItemByName(const std::string &name);

   virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);


signals:

   void ViewPoseMesh(const std::string &meshName);
   
public slots:
   void SetMode(eMODE newMode);

private slots:
   void OnPoseMeshStatusChanged(const std::string &meshName, bool isEnabled);

private: 

   void CreateTest();

   std::vector<PoseMeshItem*> mItemList;
 
   QRectF mCanvasRect;
   eMODE  mMode;
};

#endif //_POSEMESH_SCENE_H_
