// orbitmotionmodel.cpp: Implementation of the OrbitMotionModel class.
//
//////////////////////////////////////////////////////////////////////

#include "dtCore/orbitmotionmodel.h"
#include "dtCore/notify.h"

using namespace dtCore;
using namespace std;


IMPLEMENT_MANAGEMENT_LAYER(OrbitMotionModel)


/**
 * Constructor.
 *
 * @param keyboard the keyboard instance, or NULL to
 * avoid creating default input mappings
 * @param mouse the mouse instance, or NULL to avoid
 * creating default input mappings
 */
OrbitMotionModel::OrbitMotionModel(Keyboard* keyboard,
                                   Mouse* mouse)
   : MotionModel("OrbitMotionModel"),
     mAzimuthAxis(NULL),
     mElevationAxis(NULL),
     mDistanceAxis(NULL),
     mLeftRightTranslationAxis(NULL),
     mUpDownTranslationAxis(NULL),
     mAngularRate(90.0f),
     mLinearRate(1.0f),
     mDistance(100.0f)
{
   RegisterInstance(this);
   
   if(keyboard != NULL && mouse != NULL)
   {
      SetDefaultMappings(keyboard, mouse);
   }
}

/**
 * Destructor.
 */
OrbitMotionModel::~OrbitMotionModel()
{
   SetAzimuthAxis(NULL);
   SetElevationAxis(NULL);
   SetDistanceAxis(NULL);
   SetLeftRightTranslationAxis(NULL);
   SetUpDownTranslationAxis(NULL);
   
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
   if(mDefaultInputDevice.get() == NULL)
   {
      mDefaultInputDevice = new LogicalInputDevice;
      
      mDefaultElevationAxis = mDefaultInputDevice->AddAxis(
         "left mouse button up/down",
         mLeftButtonUpDownMapping = new ButtonAxisToAxis(
            mouse->GetButton(LeftButton),
            mouse->GetAxis(1)
         )
      );
      
      mDefaultAzimuthAxis = mDefaultInputDevice->AddAxis(
         "left mouse button left/right",
         mLeftButtonLeftRightMapping = new ButtonAxisToAxis(
            mouse->GetButton(LeftButton),
            mouse->GetAxis(0)
         )
      );
   
      mDefaultDistanceAxis = mDefaultInputDevice->AddAxis(
         "middle mouse button left/right",
         mMiddleButtonUpDownMapping = new ButtonAxisToAxis(
            mouse->GetButton(MiddleButton),
            mouse->GetAxis(1)
         )
      );
   
      mDefaultUpDownTranslationAxis = mDefaultInputDevice->AddAxis(
         "right mouse button up/down",
         mRightButtonUpDownMapping = new ButtonAxisToAxis(
            mouse->GetButton(RightButton),
            mouse->GetAxis(1)
         )
      );
      
      mDefaultLeftRightTranslationAxis = mDefaultInputDevice->AddAxis(
         "right mouse button left/right",
         mRightButtonLeftRightMapping = new ButtonAxisToAxis(
            mouse->GetButton(RightButton),
            mouse->GetAxis(0)
         )
      );
   }
   else
   {
      mLeftButtonUpDownMapping->SetSourceButton(mouse->GetButton(LeftButton));
      mLeftButtonUpDownMapping->SetSourceAxis(mouse->GetAxis(1));
      
      mLeftButtonLeftRightMapping->SetSourceButton(mouse->GetButton(LeftButton));
      mLeftButtonLeftRightMapping->SetSourceAxis(mouse->GetAxis(0));
      
      mRightButtonUpDownMapping->SetSourceButton(mouse->GetButton(RightButton));
      mRightButtonUpDownMapping->SetSourceAxis(mouse->GetAxis(1));
      
      mRightButtonLeftRightMapping->SetSourceButton(mouse->GetButton(RightButton));
      mRightButtonLeftRightMapping->SetSourceAxis(mouse->GetAxis(0));
      
      mMiddleButtonUpDownMapping->SetSourceButton(mouse->GetButton(MiddleButton));
      mMiddleButtonUpDownMapping->SetSourceAxis(mouse->GetAxis(1));
   }
   
   SetAzimuthAxis(mDefaultAzimuthAxis);
      
   SetElevationAxis(mDefaultElevationAxis);
   
   SetDistanceAxis(mDefaultDistanceAxis);
   
   SetLeftRightTranslationAxis(mDefaultLeftRightTranslationAxis);
   
   SetUpDownTranslationAxis(mDefaultUpDownTranslationAxis);
}

/**
 * Sets the axis that affects the azimuth of the orbit.
 *
 * @param azimuthAxis the new azimuth axis
 */
void OrbitMotionModel::SetAzimuthAxis(Axis* azimuthAxis)
{
   if(mAzimuthAxis != NULL)
   {
      mAzimuthAxis->RemoveAxisListener(this);
   }
   
   mAzimuthAxis = azimuthAxis;
   
   if(mAzimuthAxis != NULL)
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
   return mAzimuthAxis;
}

/**
 * Sets the axis that affects the elevation of the orbit.
 *
 * @param elevationAxis the new elevation axis
 */
void OrbitMotionModel::SetElevationAxis(Axis* elevationAxis)
{
   if(mElevationAxis != NULL)
   {
      mElevationAxis->RemoveAxisListener(this);
   }
   
   mElevationAxis = elevationAxis;
   
   if(mElevationAxis != NULL)
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
   return mElevationAxis;
}

/**
 * Sets the axis that affects the distance of the orbit.
 *
 * @param distanceAxis the new distance axis
 */
void OrbitMotionModel::SetDistanceAxis(Axis* distanceAxis)
{
   if(mDistanceAxis != NULL)
   {
      mDistanceAxis->RemoveAxisListener(this);
   }
   
   mDistanceAxis = distanceAxis;
   
   if(mDistanceAxis != NULL)
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
   return mDistanceAxis;
}

/**
 * Sets the axis that affects the left/right translation of the orbit.
 *
 * @param leftRightTranslationAxis the new left/right translation axis
 */
void OrbitMotionModel::SetLeftRightTranslationAxis(Axis* leftRightTranslationAxis)
{
   if(mLeftRightTranslationAxis != NULL)
   {
      mLeftRightTranslationAxis->RemoveAxisListener(this);
   }
   
   mLeftRightTranslationAxis = leftRightTranslationAxis;
   
   if(mLeftRightTranslationAxis != NULL)
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
   return mLeftRightTranslationAxis;
}

/**
 * Sets the axis that affects the up/down translation of the orbit.
 *
 * @param upDownTranslationAxis the new up/down translation axis
 */
void OrbitMotionModel::SetUpDownTranslationAxis(Axis* upDownTranslationAxis)
{
   if(mUpDownTranslationAxis != NULL)
   {
      mUpDownTranslationAxis->RemoveAxisListener(this);
   }
   
   mUpDownTranslationAxis = upDownTranslationAxis;
   
   if(mUpDownTranslationAxis != NULL)
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
   return mUpDownTranslationAxis;
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
void OrbitMotionModel::AxisStateChanged(Axis* axis,
                                        double oldState, 
                                        double newState, 
                                        double delta)
{
   if(GetTarget() != NULL && IsEnabled())
   {
      Transform transform;
      
      GetTarget()->GetTransform(&transform);
      
      sgVec3 xyz, hpr;
      
      transform.Get(xyz, hpr);
      
      if(axis == mAzimuthAxis)
      {
         sgVec3 focus = { 0.0f, mDistance, 0.0f };
         
         sgMat4 mat;
         
         transform.Get(mat);
         
         sgXformPnt3(focus, mat);
         
         hpr[0] -= (float)(delta * mAngularRate);
         
         sgVec3 offset = { 0.0f, -mDistance, 0.0f };
         
         sgMakeCoordMat4(mat, focus, hpr);
         
         sgXformPnt3(xyz, offset, mat);
      }
      else if(axis == mElevationAxis)
      {
         sgVec3 focus = { 0.0f, mDistance, 0.0f };
         
         sgMat4 mat;
         
         transform.Get(mat);
         
         sgXformPnt3(focus, mat);
         
         hpr[1] += (float)(delta * mAngularRate);
         
         if(hpr[1] < -89.9f)
         {
            hpr[1] = -89.9f;
         }
         else if(hpr[1] > 89.9f)
         {
            hpr[1] = 89.9;
         }
         
         sgVec3 offset = { 0.0f, -mDistance, 0.0f };
         
         sgMakeCoordMat4(mat, focus, hpr);
         
         sgXformPnt3(xyz, offset, mat);
      }
      else if(axis == mDistanceAxis)
      {
         float distDelta = -(float)(delta * mDistance * mLinearRate);
         
         if(mDistance + distDelta < 1.0f)
         {
            distDelta = 1.0f - mDistance;
         }
         
         sgVec3 translation = { 0.0f, -distDelta, 0.0f };
         
         sgMat4 mat;
         
         sgMakeRotMat4(mat, hpr);
         
         sgXformVec3(translation, mat);
         
         sgAddVec3(xyz, translation);
         
         mDistance += distDelta;
      }
      else if(axis == mLeftRightTranslationAxis)
      {
         sgVec3 translation =
         {
            -(float)(delta * mDistance * mLinearRate),
            0.0f,
            0.0f
         };
         
         sgMat4 mat;
         
         sgMakeRotMat4(mat, hpr);
         
         sgXformVec3(translation, mat);
         
         sgAddVec3(xyz, translation);
      }
      else if(axis == mUpDownTranslationAxis)
      {
         sgVec3 translation =
         {
            0.0f,
            0.0f,
            -(float)(delta * mDistance * mLinearRate)
         };
         
         sgMat4 mat;
         
         sgMakeRotMat4(mat, hpr);
         
         sgXformVec3(translation, mat);
         
         sgAddVec3(xyz, translation);
      }
      
      transform.Set(xyz, hpr);
      
      GetTarget()->SetTransform(&transform);
   }
}
