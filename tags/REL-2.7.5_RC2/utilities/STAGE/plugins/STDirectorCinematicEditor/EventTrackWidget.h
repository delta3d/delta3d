#ifndef ___EVENT_TRACK_WIDGET_H___
#define ___EVENT_TRACK_WIDGET_H___

////////////////////////////////////////////////////////////////////////////////

#include <QtGui/QWidget>

#include <BaseEvent.h>
#include <export.h>

////////////////////////////////////////////////////////////////////////////////

class DT_DIRECTOR_CINEMATIC_EDITOR_EXPORT EventTrackWidget : public QWidget
{
   Q_OBJECT

public:
   EventTrackWidget(QWidget* parent = NULL);
   virtual ~EventTrackWidget(void);

   void SetMinimum(int value, bool resizeEvents = true);
   void SetMaximum(int value, bool resizeEvents = true);
   void SetSelectionTimes(int start, int end);
   BaseEvent* SetCurrent(int value);

   void AddEvent(BaseEvent* event);
   void RemoveEvent(BaseEvent* event);
   void ClearEvents();
   int GetEventCount() const { return mEvents.size(); }
   int GetEventIndex(BaseEvent* event) const;
   BaseEvent* GetEventAtIndex(int index) const { return mEvents[index]; }
   BaseEvent* GetEventAtTime(int time);

   BaseEvent* GetSelectedEvent() const { return mSelectedEvent; }
   void SelectEvent(BaseEvent* event);
   void DeselectEvents();

   BaseEvent* GetPreviousEvent() const;
   BaseEvent* GetNextEvent() const;

   void UpdateEventStart(int newTime);
   void UpdateEventEnd(int newTime);

   void MoveEventToNewStartTime(int newStartTime);
   void MoveEventToNewEndTime(int newEndTime);

   void ComputeEventBoundingBox(BaseEvent* event);

   void SaveTrackEvents(std::ofstream* factFile);
   void LoadTrackEvents(std::ifstream* factFile);

signals:
   void EventSelected(BaseEvent* event);
   void EventDeleted(BaseEvent* event);
   void EventTimesChanged(int startTime, int endTime);
   void EditingFinished();

protected:
   void keyPressEvent(QKeyEvent* event);
   void leaveEvent(QEvent* event);
   void mouseMoveEvent(QMouseEvent* event);
   void mousePressEvent(QMouseEvent* event);
   void mouseReleaseEvent(QMouseEvent* event);
   void paintEvent(QPaintEvent* event);
   void resizeEvent(QResizeEvent* event);

private:
   void ResizeEventBoundingBoxes();
   int  CalculateTimeFromMouseX(int mouseX);

   int mMinimum;
   int mMaximum;
   int mSelectionStart;
   int mSelectionEnd;
   int mCurrent;
   std::vector<BaseEvent*> mEvents;

   BaseEvent* mSelectedEvent;
   BaseEvent* mLeftResizeEvent;
   BaseEvent* mMovingEvent;
   BaseEvent* mRightResizeEvent;
   int mMouseOffset;
};

////////////////////////////////////////////////////////////////////////////////

#endif //___EVENT_TRACK_WIDGET_H___
