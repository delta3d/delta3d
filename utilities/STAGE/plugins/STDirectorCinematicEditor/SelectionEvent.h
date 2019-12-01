#ifndef ___SELECTION_EVENT_H___
#define ___SELECTION_EVENT_H___

////////////////////////////////////////////////////////////////////////////////

#include <BaseEvent.h>
#include <export.h>

////////////////////////////////////////////////////////////////////////////////

class DT_DIRECTOR_CINEMATIC_EDITOR_EXPORT SelectionEvent : public BaseEvent
{
public:
   SelectionEvent(int newStartTime = 0, int newEndTime = 1000000);
   virtual ~SelectionEvent();

   virtual void SetNewTimes(int newStartTime, int newEndTime) {}

   virtual void PaintEvent(QPainter* painter);
};

////////////////////////////////////////////////////////////////////////////////

#endif //___SELECTION_EVENT_H___
