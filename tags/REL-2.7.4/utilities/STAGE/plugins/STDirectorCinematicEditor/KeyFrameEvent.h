#ifndef __KEY_FRAME_EVENT_H__
#define __KEY_FRAME_EVENT_H__

////////////////////////////////////////////////////////////////////////////////

#include <BaseEvent.h>
#include <export.h>

#include <dtCore/transform.h>

////////////////////////////////////////////////////////////////////////////////

class DT_DIRECTOR_CINEMATIC_EDITOR_EXPORT KeyFrameEvent : public BaseEvent
{
public:
   KeyFrameEvent(int newTime = 0);
   virtual ~KeyFrameEvent();

   virtual void SetStartTime(int newStartTime) {BaseEvent::SetStartTime(newStartTime); BaseEvent::SetEndTime(newStartTime);}
   virtual void SetEndTime(int newEndTime) {BaseEvent::SetStartTime(newEndTime); BaseEvent::SetEndTime(newEndTime);}
   virtual void SetNewTimes(int newStartTime, int newEndTime) {BaseEvent::SetNewTimes(newStartTime, newStartTime);}

   virtual bool IsPointOnLeftResizer(QPoint position) const;
   virtual bool IsPointOnEvent(QPoint position) const;
   virtual bool IsPointOnRightResizer(QPoint position) const;

   virtual void PaintEvent(QPainter* painter);
};

////////////////////////////////////////////////////////////////////////////////

#endif //__KEY_FRAME_EVENT_H__
