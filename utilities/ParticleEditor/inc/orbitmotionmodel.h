#ifndef __ORBITMOTIONMODEL_H__
#define __ORBITMOTIONMODEL_H__

#include <osg/MatrixTransform>

#include <dtCore/mouse.h>
#include <dtCore/transformable.h>

class OrbitMotionModel : public dtCore::MouseListener
{
public:
   OrbitMotionModel(dtCore::Mouse* mouse, dtCore::Transformable* target);

   virtual bool HandleButtonPressed(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button);
   virtual bool HandleMouseDragged(const dtCore::Mouse* mouse, float x, float y);
   virtual bool HandleMouseScrolled(const dtCore::Mouse* mouse, int delta);

   bool HandleMouseMoved(const dtCore::Mouse* mouse, float x, float y) { return false; }
   bool HandleButtonClicked(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button, int clickCount) { return false; }
   bool HandleButtonReleased(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button) { return false; }

   void SetCompassTransform(osg::MatrixTransform* compassTransform) { mpCompassTransform = compassTransform; }
private:
   ~OrbitMotionModel();

   void UpdateTargetTransform();

   dtCore::Mouse* mMouse;
   dtCore::Transformable* mTarget;
   float mAzimuth, mElevation, mDistance;
   osg::Vec3 mCenter;
   float mLastX, mLastY;
   osg::MatrixTransform* mpCompassTransform;
};

#endif //__ORBITMOTIONMODEL_H__

