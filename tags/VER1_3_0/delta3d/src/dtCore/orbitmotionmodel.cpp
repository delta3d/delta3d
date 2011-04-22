// orbitmotionmodel.cpp: Implementation of the OrbitMotionModel class.
//
//////////////////////////////////////////////////////////////////////

#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <dtCore/logicalinputdevice.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/scene.h>
#include <dtCore/motionmodel.h>
#include <dtCore/transformable.h>
#include <dtUtil/matrixutil.h>

#include <osg/Vec3>
#include <osg/Matrix>

namespace dtCore
{

IMPLEMENT_MANAGEMENT_LAYER(OrbitMotionModel)

/**
 * Constructor.
 *
 * @param keyboard the keyboard instance, or 0 to
 * avoid creating default input mappings
 * @param mouse the mouse instance, or 0 to avoid
 * creating default input mappings
 */
OrbitMotionModel::OrbitMotionModel(Keyboard* keyboard,
                                   Mouse* mouse)
   : MotionModel("OrbitMotionModel"),
     mAzimuthAxis(0),
     mElevationAxis(0),
     mDistanceAxis(0),
     mLeftRightTranslationAxis(0),
     mUpDownTranslationAxis(0),
     mAngularRate(90.0f),
     mLinearRate(1.0f),
     mDistance(100.0f)
{
   RegisterInstance(this);
   
   if(keyboard != 0 && mouse != 0)
   {
      SetDefaultMappings(keyboard, mouse);
   }
}

/**
 * Destructor.
 */
OrbitMotionModel::~OrbitMotionModel()
{
   SetAzimuthAxis(0);
   SetElevationAxis(0);
   SetDistanceAxis(0);
   SetLeftRightTranslationAxis(0);
   SetUpDownTranslationAxis(0);
   
   DeregisterInstance(this);
}

/**
 * Sets the input axes to a set of default mappings for mouse
 * and keyboard.
 *
 * @param keyboard the keyboard instance
 * @param mouse the mouse instance
 */
void OrbitMotionModel::SetDefaultMappings(Keyboard* keyboard, Mouse* mouse)
{
   if(!mDefaultInputDevice.valid())
   {
      mDefaultInputDevice = new LogicalInputDevice;
      
      mLeftButtonUpDownMapping = new ButtonAxisToAxis(   mouse->GetButton(Mouse::LeftButton),
                                                         mouse->GetAxis(1) );
      mDefaultElevationAxis = mDefaultInputDevice->AddAxis( "left mouse button up/down",
                                                            mLeftButtonUpDownMapping.get() );
    
      mLeftButtonLeftRightMapping = new ButtonAxisToAxis(   mouse->GetButton(Mouse::LeftButton),
                                                            mouse->GetAxis(0) );
      mDefaultAzimuthAxis = mDefaultInputDevice->AddAxis(   "left mouse button left/right",
                                                            mLeftButtonLeftRightMapping.get() );
   
      mMiddleButtonUpDownMapping = new ButtonAxisToAxis( mouse->GetButton(Mouse::MiddleButton),
                                                         mouse->GetAxis(1) );
      mDefaultDistanceAxis = mDefaultInputDevice->AddAxis(  "middle mouse button left/right",
                                                            mMiddleButtonUpDownMapping.get() );

      mRightButtonUpDownMapping = new ButtonAxisToAxis(  mouse->GetButton(Mouse::RightButton),
                                                         mouse->GetAxis(1) );
      mDefaultUpDownTranslationAxis = mDefaultInputDevice->AddAxis(  "right mouse button up/down",
                                                                     mRightButtonUpDownMapping.get() );
      
      mRightButtonLeftRightMapping = new ButtonAxisToAxis(  mouse->GetButton(Mouse::RightButton),
                                                            mouse->GetAxis(0) );
      mDefaultLeftRightTranslationAxis = mDefaultInputDevice->AddAxis(  "right mouse button left/right",
                                                                        mRightButtonLeftRightMapping.get() );
   }
   else
   {
      mLeftButtonUpDownMapping->SetSourceButton(mouse->GetButton(Mouse::LeftButton));
      mLeftButtonUpDownMapping->SetSourceAxis(mouse->GetAxis(1));
      
      mLeftButtonLeftRightMapping->SetSourceButton(mouse->GetButton(Mouse::LeftButton));
      mLeftButtonLeftRightMapping->SetSourceAxis(mouse->GetAxis(0));
      
      mRightButtonUpDownMapping->SetSourceButton(mouse->GetButton(Mouse::RightButton));
      mRightButtonUpDownMapping->SetSourceAxis(mouse->GetAxis(1));
      
      mRightButtonLeftRightMapping->SetSourceButton(mouse->GetButton(Mouse::RightButton));
      mRightButtonLeftRightMapping->SetSourceAxis(mouse->GetAxis(0));
      
      mMiddleButtonUpDownMapping->SetSourceButton(mouse->GetButton(Mouse::MiddleButton));
      mMiddleButtonUpDownMapping->SetSourceAxis(mouse->GetAxis(1));
   }
   
   SetAzimuthAxis(mDefaultAzimuthAxis.get());
      
   SetElevationAxis(mDefaultElevationAxis.get());
   
   SetDistanceAxis(mDefaultDistanceAxis.get());
   
   SetLeftRightTranslationAxis(mDefaultLeftRightTranslationAxis.get());
   
   SetUpDownTranslationAxis(mDefaultUpDownTranslationAxis.get());
}

/**
 * Sets the axis that affects the azimuth of the orbit.
 *
 * @param azimuthAxis the new azimuth axis
 */
void OrbitMotionModel::SetAzimuthAxis(Axis* azimuthAxis)
{
   if(mAzimuthAxis != 0)
   {
      mAzimuthAxis->RemoveAxisListener(this);
   }
   
   mAzimuthAxis = azimuthAxis;
   
   if(mAzimuthAxis != 0)
   {
      mAzimuthAxis->AddAxisListener(this);
   }
}

/**
 * Returns the axis that affects the azimuth of the orbit.
 *
 * @return the current azimuth axis
 */
Axis* OrbitMotionModel::GetAzimuthAxis()
{
   return mAzimuthAxis.get();
}

/**
 * Sets the axis that affects the elevation of the orbit.
 *
 * @param elevationAxis the new elevation axis
 */
void OrbitMotionModel::SetElevationAxis(Axis* elevationAxis)
{
   if(mElevationAxis != 0)
   {
      mElevationAxis->RemoveAxisListener(this);
   }
   
   mElevationAxis = elevationAxis;
   
   if(mElevationAxis != 0)
   {
      mElevationAxis->AddAxisListener(this);
   }
}

/**
 * Returns the axis that affects the elevation of the orbit.
 *
 * @return the current elevation axis
 */
Axis* OrbitMotionModel::GetElevationAxis()
{
   return mElevationAxis.get();
}

/**
 * Sets the axis that affects the distance of the orbit.
 *
 * @param distanceAxis the new distance axis
 */
void OrbitMotionModel::SetDistanceAxis(Axis* distanceAxis)
{
   if(mDistanceAxis != 0)
   {
      mDistanceAxis->RemoveAxisListener(this);
   }
   
   mDistanceAxis = distanceAxis;
   
   if(mDistanceAxis != 0)
   {
      mDistanceAxis->AddAxisListener(this);
   }
}

/**
 * Returns the axis that affects the distance of the orbit.
 *
 * @return the current distance axis
 */
Axis* OrbitMotionModel::GetDistanceAxis()
{
   return mDistanceAxis.get();
}

/**
 * Sets the axis that affects the left/right translation of the orbit.
 *
 * @param leftRightTranslationAxis the new left/right translation axis
 */
void OrbitMotionModel::SetLeftRightTranslationAxis(Axis* leftRightTranslationAxis)
{
   if(mLeftRightTranslationAxis != 0)
   {
      mLeftRightTranslationAxis->RemoveAxisListener(this);
   }
   
   mLeftRightTranslationAxis = leftRightTranslationAxis;
   
   if(mLeftRightTranslationAxis != 0)
   {
      mLeftRightTranslationAxis->AddAxisListener(this);
   }
}

/**
 * Returns the axis that affects the left/right translation of the orbit.
 *
 * @return the current left/right translation axis
 */
Axis* OrbitMotionModel::GetLeftRightTranslationAxis()
{
   return mLeftRightTranslationAxis.get();
}

/**
 * Sets the axis that affects the up/down translation of the orbit.
 *
 * @param upDownTranslationAxis the new up/down translation axis
 */
void OrbitMotionModel::SetUpDownTranslationAxis(Axis* upDownTranslationAxis)
{
   if(mUpDownTranslationAxis != 0)
   {
      mUpDownTranslationAxis->RemoveAxisListener(this);
   }
   
   mUpDownTranslationAxis = upDownTranslationAxis;
   
   if(mUpDownTranslationAxis != 0)
   {
      mUpDownTranslationAxis->AddAxisListener(this);
   }
}

/**
 * Returns the axis that affects the up/down translation of the orbit.
 *
 * @return the current up/down translation axis
 */
Axis* OrbitMotionModel::GetUpDownTranslationAxis()
{
   return mUpDownTranslationAxis.get();
}

/**
 * Sets the angular rate (the ratio between axis units and angular
 * movement in degrees).
 *
 * @param angularRate the new angular rate
 */
void OrbitMotionModel::SetAngularRate(float angularRate)
{
   mAngularRate = angularRate;
}

/**
 * Returns the angular rate.
 *
 * @return the current angular rate
 */
float OrbitMotionModel::GetAngularRate()
{
   return mAngularRate;
}

/**
 * Sets the linear rate (the ratio between axis units and linear
 * movement in meters).
 *
 * @param linearRate the new linear rate
 */
void OrbitMotionModel::SetLinearRate(float linearRate)
{
   mLinearRate = linearRate;
}

/**
 * Returns the linear rate.
 *
 * @return the current linear rate
 */
float OrbitMotionModel::GetLinearRate()
{
   return mLinearRate;
}

/**
 * Sets the distance from the focal point.
 *
 * @param distance the new distance
 */
void OrbitMotionModel::SetDistance(float distance)
{
   mDistance = distance;
}

/**
 * Returns the distance from the focal point.
 *
 * @return the current distance
 */
float OrbitMotionModel::GetDistance()
{
   return mDistance;
}
         
/**
 * Called when an axis' state has changed.
 *
 * @param axis the changed axis
 * @param oldState the old state of the axis
 * @param newState the new state of the axis
 * @param delta a delta value indicating stateless motion
 */
bool OrbitMotionModel::AxisStateChanged(const Axis* axis,
                                        double oldState, 
                                        double newState, 
                                        double delta)
{
   if(GetTarget() != 0 && IsEnabled())
   {
      Transform transform;
      
      GetTarget()->GetTransform(&transform);
      
      osg::Vec3 xyz, hpr, scale;
      
      transform.Get(xyz, hpr, scale);
      
      if(axis == mAzimuthAxis.get())
      {
         osg::Vec3 focus( 0.0f, mDistance, 0.0f );
         
         osg::Matrix mat;
         
         transform.Get(mat);
         
         dtUtil::MatrixUtil::TransformVec3(focus, mat);
         
         hpr[0] -= float(delta * mAngularRate);
         
         osg::Vec3 offset( 0.0f, -mDistance, 0.0f );
         
         dtUtil::MatrixUtil::PositionAndHprToMatrix(mat, focus, hpr);

         dtUtil::MatrixUtil::TransformVec3(xyz, offset, mat);
      }
      else if(axis == mElevationAxis.get())
      {
         osg::Vec3 focus( 0.0f, mDistance, 0.0f );
         
         osg::Matrix mat;
         
         transform.Get(mat);
         
         dtUtil::MatrixUtil::TransformVec3(focus, mat);
         
         hpr[1] += float(delta * mAngularRate);
         
         if(hpr[1] < -89.9f)
         {
            hpr[1] = -89.9f;
         }
         else if(hpr[1] > 89.9f)
         {
            hpr[1] = 89.9;
         }
         
         osg::Vec3 offset ( 0.0f, -mDistance, 0.0f );
         
         dtUtil::MatrixUtil::PositionAndHprToMatrix(mat, focus, hpr);
         
         dtUtil::MatrixUtil::TransformVec3(xyz, offset, mat);
      }
      else if(axis == mDistanceAxis.get())
      {
         float distDelta = -float(delta * mDistance * mLinearRate);
         
         if(mDistance + distDelta < 1.0f)
         {
            distDelta = 1.0f - mDistance;
         }
         
         osg::Vec3 translation ( 0.0f, -distDelta, 0.0f );
         
         osg::Matrix mat;
         
         dtUtil::MatrixUtil::HprToMatrix(mat, hpr);
         
         translation = osg::Matrix::transform3x3(translation, mat);

         xyz += translation;
         
         mDistance += distDelta;
      }
      else if(axis == mLeftRightTranslationAxis.get())
      {
         osg::Vec3 translation
         (
            -float(delta * mDistance * mLinearRate),
            0.0f,
            0.0f
         );
         
         osg::Matrix mat;
         
         dtUtil::MatrixUtil::HprToMatrix(mat, hpr);
         
         translation = osg::Matrix::transform3x3(translation, mat);
         
         xyz += translation;
      }
      else if(axis == mUpDownTranslationAxis.get())
      {
         osg::Vec3 translation 
         (
            0.0f,
            0.0f,
            -float(delta * mDistance * mLinearRate)
         );
         
         osg::Matrix mat;
         
         dtUtil::MatrixUtil::HprToMatrix(mat, hpr);
         
         translation = osg::Matrix::transform3x3(translation, mat);
         
         xyz += translation;
      }
      
      transform.Set(xyz, hpr, scale);
      
      GetTarget()->SetTransform(&transform);

      return true;
   }

   return false;
}

}