#ifndef ___BASE_EVENT_H___
#define ___BASE_EVENT_H___

////////////////////////////////////////////////////////////////////////////////

#include <export.h>

#include <QtGui/QColor>
#include <QtCore/QRect>

class QPainter;

////////////////////////////////////////////////////////////////////////////////

class DT_DIRECTOR_CINEMATIC_EDITOR_EXPORT BaseEvent
{
public:
   static const int RESIZE_END_WIDTH = 5;
   static const int FRAME_WIDTH = 2;

   BaseEvent(int newStartTime = 0, int newEndTime = 1000000);
   virtual ~BaseEvent();

   virtual int GetStartTime() const { return mStartTime; }
   virtual void SetStartTime(int newStartTime);

   virtual int GetEndTime() const { return mEndTime; }
   virtual void SetEndTime(int newEndTime);

   virtual void SetNewTimes(int newStartTime, int newEndTime);

   QColor GetLeftResizerColor() const        { return mLeftResizerColor;     }
   void SetLeftResizerColor(QColor newColor) { mLeftResizerColor = newColor; }

   QColor GetRightResizerColor() const        { return mRightResizerColor;     }
   void SetRightResizerColor(QColor newColor) { mRightResizerColor = newColor; }

   QColor GetColor() const        { return mColor;     }
   void SetColor(QColor newColor) { mColor = newColor; }

   QColor GetFrameColor() const        { return mFrameColor;}
   void SetFrameColor(QColor newColor) { mFrameColor = newColor;}

   QColor GetDefaultColor() {return mDefaultColor;}
   void SetDefaultColor(QColor newColor);

   QRect GetBoundingBox() const { return mBoundingBox; }
   void SetBoundingBox(QRect newBox) {mBoundingBox = newBox; }

   virtual bool IsPointOnLeftResizer(QPoint position) const;
   virtual bool IsPointOnEvent(QPoint position) const;
   virtual bool IsPointOnRightResizer(QPoint position) const;

   virtual void ClearEventHighlights();
   virtual void HighlightEventLeftResizer(bool highlighted);
   virtual void HighlightEvent(bool highlighted);
   virtual void HighlightEventRightResizer(bool highlighted);

   bool IsSelected() const { return mIsSelected; }
   virtual void SelectEvent(bool selected);

   bool IsMovable() const { return mIsMovable; }
   void SetMovable(bool movable) { mIsMovable = movable; }

   virtual bool IsMinTimeLocked() const {return true;}
   virtual bool IsMaxTimeLocked() const {return true;}

   virtual void PaintEvent(QPainter* painter);

   virtual void SaveEvent(std::ofstream* factFile);
   virtual void LoadEvent(std::ifstream* factFile);

private:
   int mStartTime;
   int mEndTime;

   QRect mBoundingBox;

   // Color variables
   QColor mColor;
   QColor mLeftResizerColor;
   QColor mRightResizerColor;
   QColor mFrameColor;
   QColor mDefaultColor;

   bool mIsSelected;
   bool mIsMovable;
};

////////////////////////////////////////////////////////////////////////////////

#endif //___BASE_EVENT_H___
