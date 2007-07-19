#ifndef _TRACKSCENE_H_
#define _TRACKSCENE_H_

#include <QtGui/QGraphicsScene>
#include <QtCore/QTimer>
#include <QtCore/QRectF>

#include <vector>
#include <string>

class QGraphicsItem;
class QGraphicsRectItem;

struct AnimElement
{
public:     
   QGraphicsRectItem *mGraphicsItem;

};

class TrackScene: public QGraphicsScene
{
   Q_OBJECT
public:
   TrackScene(QObject *parent = 0);
   TrackScene(const QRectF &sceneRect, QObject *parent = 0);
   ~TrackScene();   
  
   void AddAnimation(const std::string &name);
   void AddNewTrack();

   void GetAnimElements(std::vector<AnimElement> *& elements){ elements = &mAnimElements; }

protected: 

private: 

   void CreateTestTracks();
 
   QRectF mTrackRect;
   QRectF mAnimRect;

   std::vector<AnimElement> mAnimElements;



};

#endif
