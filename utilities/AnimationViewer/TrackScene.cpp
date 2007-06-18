#include "TrackScene.h"
#include "TrackItem.h"
#include <QRectF>
#include <QGraphicsRectItem>


TrackScene::TrackScene(QObject *parent)
: QGraphicsScene(parent)
{    
   double graphicsWidth  = 1280;
   double graphicsHeight = 768;

   // This rectangle will be a visual representation 
   // of the bounds that the map can be dragged in
   QRectF graphicSceneRect;
   graphicSceneRect.setLeft(0);
   graphicSceneRect.setTop(0);
   graphicSceneRect.setWidth(graphicsWidth);
   graphicSceneRect.setHeight(graphicsHeight);   

   addRect(graphicSceneRect); 

   // Draw our outlines with custom pens
   QPen animRectPen(Qt::blue);
   animRectPen.setStyle(Qt::DashDotDotLine);
   
   QPointF graphicsCenter = graphicSceneRect.center();
  
   // This rectangle will be a visual representation 
   // of the bounds that the map can be dragged in
   mTrackRect.setLeft(graphicsCenter.x() - graphicsWidth / 10);
   mTrackRect.setTop(graphicsCenter.y() - graphicsHeight / 10);
   mTrackRect.setWidth(graphicsWidth / 5);
   mTrackRect.setHeight(graphicsHeight / 5);  

   double animWidth = mTrackRect.width() / 2;

   mAnimRect.setLeft(mTrackRect.left() - 1.2 * animWidth);
   mAnimRect.setTop(mTrackRect.top());
   mAnimRect.setWidth(animWidth);
   mAnimRect.setHeight(mTrackRect.height());

   addRect(mAnimRect)->setPen(animRectPen);
  
   // Add our rectangle objects to the scene,
   //mBorderRect = addRect(mTrackRect); 
   //mBorderRect->setPen(outerRectPen);    

   CreateTestTracks();
}


TrackScene::~TrackScene()
{

}


void TrackScene::AddNewTrack()
{
   
}

void TrackScene::AddAnimation(const std::string &name)
{

}  


void TrackScene::CreateTestTracks()
{
   double height = mTrackRect.height() / 16;

   // Draw our outlines with custom pens
   QPen trackPen(Qt::gray);
   trackPen.setStyle(Qt::DashLine);   

   AnimElement testElement;
   QRectF test(mAnimRect.x() + 10, mAnimRect.y() + 10, 20, 10);

   testElement.mGraphicsItem = new TrackItem;
   testElement.mGraphicsItem->setRect(test);

   addItem(testElement.mGraphicsItem);
   mAnimElements.push_back(testElement);

   for (int index = 0; index < 16; ++index)
   {
      double moveAmount = height * index;

      QRectF test(mTrackRect);
      test.setHeight(height);
      test.adjust(0, moveAmount, 0, moveAmount);

      addRect(test)->setPen(trackPen); 
   }
}

