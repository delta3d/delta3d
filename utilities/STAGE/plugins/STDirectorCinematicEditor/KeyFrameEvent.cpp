#include <KeyFrameEvent.h>

#include <QtGui/QPainter>

///////////////////////////////////////////////////////////////////////////////
KeyFrameEvent::KeyFrameEvent(int newTime /*= 0*/)
: BaseEvent(newTime, newTime)
{
}

///////////////////////////////////////////////////////////////////////////////
KeyFrameEvent::~KeyFrameEvent()
{
}

////////////////////////////////////////////////////////////////////////////////
bool KeyFrameEvent::IsPointOnLeftResizer(QPoint position) const
{
   return false;
}

////////////////////////////////////////////////////////////////////////////////
bool KeyFrameEvent::IsPointOnEvent(QPoint position) const
{
   QRect leftResizer = GetBoundingBox();
   leftResizer.setLeft(leftResizer.left() - 2);
   leftResizer.setRight(leftResizer.right() + 5);

   return leftResizer.contains(position);
}

////////////////////////////////////////////////////////////////////////////////
bool KeyFrameEvent::IsPointOnRightResizer(QPoint position) const
{
   return false;
}

///////////////////////////////////////////////////////////////////////////////
void KeyFrameEvent::PaintEvent(QPainter* painter)
{
   QRect boundingBox = GetBoundingBox();

   // Draw our single key frame wedge.
   painter->setBrush(GetColor());
   painter->setPen(GetColor());

   QPolygon poly;
   poly.push_back(QPoint(boundingBox.left() - 2, boundingBox.top()));
   poly.push_back(QPoint(boundingBox.left() + 2, boundingBox.bottom()));
   poly.push_back(QPoint(boundingBox.right() + 5, boundingBox.top()));
   painter->drawPolygon(poly);
}

///////////////////////////////////////////////////////////////////////////////

