#ifndef __ANIMATION_EVENT_H__
#define __ANIMATION_EVENT_H__

////////////////////////////////////////////////////////////////////////////////

#include <export.h>

#include <BaseEvent.h>

////////////////////////////////////////////////////////////////////////////////

class DT_DIRECTOR_CINEMATIC_EDITOR_EXPORT AnimationEvent : public BaseEvent
{
public:
   AnimationEvent(int newStartTime = 0, int newEndTime = 1000000);
   virtual ~AnimationEvent();

   virtual bool IsPointOnLeftResizer(QPoint position) const;
   virtual bool IsPointOnEvent(QPoint position) const;
   virtual bool IsPointOnRightResizer(QPoint position) const;

   virtual void HighlightEvent(bool highlighted);
   virtual void SelectEvent(bool selected);

   virtual void PaintEvent(QPainter* painter);

   virtual bool IsMinTimeLocked() const {return false;}
   virtual bool IsMaxTimeLocked() const {return false;}

   /**
    * Accessors
    */
   void SetBlendIn(int blendIn) {mBlendIn = blendIn;}
   int GetBlendIn() const {return mBlendIn;}

   void SetBlendOut(int blendOut) {mBlendOut = blendOut;}
   int GetBlendOut() const {return mBlendOut;}

   void SetWeight(float weight) {mWeight = weight;}
   float GetWeight() const {return mWeight;}

private:

   int   mBlendIn;
   int   mBlendOut;
   float mWeight;
};

////////////////////////////////////////////////////////////////////////////////

#endif //__ANIMATION_EVENT_H__
