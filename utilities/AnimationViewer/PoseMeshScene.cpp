#include "PoseMeshScene.h"
#include "PoseMeshItem.h"
#include <QtCore/QRectF>
#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsSceneMouseEvent>


PoseMeshScene::PoseMeshScene(QObject *parent)
: QGraphicsScene(parent)
{    
   double graphicsWidth  = 512;
   double graphicsHeight = 1024;

   // This rectangle will be a visual representation 
   // of the bounds that the map can be dragged in  
   mCanvasRect.setLeft(0);
   mCanvasRect.setTop(0);
   mCanvasRect.setWidth(graphicsWidth);
   mCanvasRect.setHeight(graphicsHeight);   

   QGraphicsRectItem *canvasItem = addRect(mCanvasRect, QPen(), QBrush(QColor(192, 192, 255)));  
   canvasItem->setZValue(-100.0f);
   canvasItem->setEnabled(false);

   CreateTest();
}


PoseMeshScene::~PoseMeshScene()
{

}

void PoseMeshScene::AddMesh(const dtAnim::PoseMesh &mesh)
{      
   static float adjustedHeight = mCanvasRect.y() + 64;

   PoseMeshItem *newItem = new PoseMeshItem(mesh);
   mItemList.push_back(newItem);

   //float itemWidth = newItem->boundingRect().width();
   float itemHeight = newItem->boundingRect().height();

   float canvasWidth = mCanvasRect.width();

   float middleX = mCanvasRect.x() + canvasWidth * 0.5f;
   float middleY = adjustedHeight + itemHeight * 0.5f;

   adjustedHeight += itemHeight + 64.0f;

   newItem->setPos(middleX, middleY);

   //QRectF test(mAnimRect.x() + 10, mAnimRect.y() + 10, 20, 10);   
   //newItem->setRect(test);

   addItem(newItem);
}

PoseMeshItem* PoseMeshScene::GetPoseMeshItemByName(const std::string &name)
{
   PoseMeshItem *foundItem = NULL;

   for (size_t itemIndex = 0; itemIndex < mItemList.size(); ++itemIndex)
   {
      if (mItemList[itemIndex]->GetPoseMeshName() == name)
      {
         foundItem = mItemList[itemIndex];
         break;
      }
   }

   return foundItem;
}

void PoseMeshScene::CreateTest()
{
   double height = mCanvasRect.height() / 16;

   // Draw our outlines with custom pens
   QPen trackPen(Qt::gray);
   trackPen.setStyle(Qt::DashLine);  

   return;

   for (int index = 0; index < 16; ++index)
   {
      double moveAmount = height * index;

      QRectF test(mCanvasRect);
      test.setHeight(height);
      test.adjust(0, moveAmount, 0, moveAmount);

      addRect(test)->setPen(trackPen); 
   }
}

void PoseMeshScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{   
   QList<QGraphicsItem*> clickedItemList = items(mouseEvent->scenePos());   

   for (int itemIndex = 0; itemIndex < clickedItemList.size(); ++itemIndex)
   {
      PoseMeshItem *poseItem = dynamic_cast<PoseMeshItem*>(clickedItemList[itemIndex]);

      if (poseItem)
      {
         emit ViewPoseMesh(poseItem->GetPoseMeshName());         
         break;
      }
   }   
}
