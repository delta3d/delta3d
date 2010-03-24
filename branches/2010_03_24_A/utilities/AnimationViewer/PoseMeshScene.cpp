#include "PoseMeshScene.h"
#include "PoseMeshItem.h"
#include <QtCore/QRectF>
#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsSceneMouseEvent>

#include <dtAnim/chardrawable.h>

#include <assert.h>

////////////////////////////////////////////////////////////////////////////////
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

   mVerticalMeshOffset = mCanvasRect.y() + 64;

   QBrush canvasBrush(QColor(192, 192, 255));

   QGraphicsRectItem *canvasItem = addRect(mCanvasRect, QPen(), canvasBrush);  
   canvasItem->setZValue(-100.0f);
   canvasItem->setEnabled(false);

   QBrush backgroundBrush(QPixmap(":/images/checker.png"));  
   setBackgroundBrush(backgroundBrush);

   CreateTest();
}

////////////////////////////////////////////////////////////////////////////////
PoseMeshScene::~PoseMeshScene()
{

}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshScene::AddMesh(const dtAnim::PoseMesh &poseMesh, dtAnim::CharDrawable *model)
{   
   PoseMeshItem *newItem = new PoseMeshItem(poseMesh, model);
   mItemList.push_back(newItem);
  
   float itemHeight = newItem->boundingRect().height();
   float canvasWidth = mCanvasRect.width();

   float middleX = mCanvasRect.x() + canvasWidth * 0.5f;
   float middleY = mVerticalMeshOffset + itemHeight * 0.5f;

   // Increment the new starting height
   mVerticalMeshOffset += itemHeight + 64.0f;

   newItem->setPos(middleX, middleY);  

   addItem(newItem);

   emit PoseMeshItemAdded(newItem);
}

////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////
void PoseMeshScene::OnBlendUpdate()
{
   // Perform necessary updates in response to a new blend
   for (size_t itemIndex = 0; itemIndex < mItemList.size(); ++itemIndex)
   {
      mItemList[itemIndex]->OnBlendUpdate();
   }
}

////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////
void PoseMeshScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
   QGraphicsScene::mouseReleaseEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{   
   // temp disable
   return;

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

////////////////////////////////////////////////////////////////////////////////
void PoseMeshScene::OnPoseMeshStatusChanged(const std::string &meshName, bool isEnabled)
{
   PoseMeshItem *meshItem = GetPoseMeshItemByName(meshName);
   assert(meshItem);

   meshItem->SetEnabled(isEnabled);
}

