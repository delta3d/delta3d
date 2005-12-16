/**
 * Included files.
 */
#include "viewstate.h"



/**
 * Name spaces.
 */
using namespace   dtCore;



/**
 * Static member variables.
 */
const int   ViewState::kDisplayMask(COMPASS|XY_PLANE|YZ_PLANE|ZX_PLANE|SCENEPOLY|SCENEWIRE|SCENETXT|SCENELIGHT);
const int   ViewState::kModelMask(~kDisplayMask&~RESET&~DISPLAY);
      int   ViewState::mDisplayFlags(0L);



/**
 * Default Constructor member function
 */
ViewState::ViewState( std::string filename /*= ""*/, unsigned int indx /*= (unsigned int)(-1)*/ )
:  mFilename(filename),
   mFileIndex(indx),
   mCamPosition(0.f,0.f,0.f,0.f,0.f,0.f),
   mOrbitDist(0L),
   mCamPositionDef(0.f,0.f,0.f,0.f,0.f,0.f),
   mOrbitDistDef(0L),
   mModelFlags(0L),
   mMotionFlags(0L),
   mJoystickFlags(0)
{
}



/**
 * Copy Constructor member function
 */
ViewState::ViewState( const ViewState& that )
:  mFilename(that.mFilename),
   mFileIndex(that.mFileIndex),
   mCamPosition(that.mCamPosition),
   mOrbitDist(that.mOrbitDist),
   mCamPositionDef(that.mCamPositionDef),
   mOrbitDistDef(that.mOrbitDistDef),
   mModelFlags(that.mModelFlags),
   mMotionFlags(that.mMotionFlags),
   mJoystickFlags(that.mJoystickFlags)
{
}



/**
 * Destructor member function
 */
ViewState::~ViewState()
{
}



/**
 * Copy operator member function
 */
ViewState&
ViewState::operator=( const ViewState& that )
{
   if( this == &that )
      return   *this;

   mFilename         = that.mFilename;
   mFileIndex        = that.mFileIndex;
   mCamPosition      = that.mCamPosition;
   mOrbitDist        = that.mOrbitDist;
   mCamPositionDef   = that.mCamPositionDef;
   mOrbitDistDef     = that.mOrbitDistDef;
   mModelFlags       = that.mModelFlags;
   mMotionFlags      = that.mMotionFlags;
   mJoystickFlags    = that.mJoystickFlags;

   return   *this;
}



/**
 * Equality operator member function
 */
bool
ViewState::operator==( const ViewState& that )
{
   if( this == &that )
      return   true;

   return   (
               ( mFilename       == that.mFilename       )  &&
               ( mFileIndex      == that.mFileIndex      )  &&
               ( mCamPosition    == that.mCamPosition    )  &&
               ( mOrbitDist      == that.mOrbitDist      )  &&
               ( mCamPositionDef == that.mCamPositionDef )  &&
               ( mOrbitDistDef   == that.mOrbitDistDef   )  &&
               ( mModelFlags     == that.mModelFlags     )  &&
               ( mMotionFlags    == that.mMotionFlags    )  &&
               ( mJoystickFlags  == that.mJoystickFlags  )
            );
}



/**
 * Return the current camera position and orientation
 * and clears the position update flag.
 *
 * @param   camPos reference which will receive current position data
 * @param   def flag gets the default position instead if true
 */
void
ViewState::GetCamPosition( Transform& camPos, bool def /*= false*/ ) const
{
   if( def )
   {
      camPos   = mCamPositionDef;
   }
   else
   {
      camPos   = mCamPosition;
   }
}



/**
 * Set the current camera position and orientation
 * and sets the position update flag.
 *
 * @param   camPos reference to transform which will supply position data
 * @param   def flag sets the default position instead if true
 */
void
ViewState::SetCamPosition( const Transform& camPos, bool def /*= false*/ )
{
   if( def )
   {
      mCamPositionDef   = camPos;
   }
   else
   {
      mCamPosition      = camPos;
   }
}



/**
 * Get the current distance for the orbit motion model.
 *
 * @param   def flag gets the default distance instead if true
 *
 * @return  current distance for the orbit motion model
 */
float
ViewState::GetCamOrbitDist( bool def /*= false*/ ) const
{
   if( def )
   {
      return   mOrbitDistDef;
   }

   return   mOrbitDist;
}



/**
 * Set the current distance for the orbit motion model.
 *
 * @param   orbitDist is the new distance for the orbit motion model
 * @param   def flag sets the default distance instead if true
*/
void
ViewState::SetCamOrbitDist( float orbitDist, bool def /*= false*/ )
{
   if( orbitDist <= 0.f )
      return;

   if( def )
   {
      mOrbitDistDef  = orbitDist;
   }
   else
   {
      mOrbitDist     = orbitDist;
   }
}



/**
 * Get one of the display flags
 *
 * @param  flag to get
 *
 * @return  non-zero if flag is set, else zero
 */
int
ViewState::GetDisplayFlag( DISPLAY_FLAG flag )  const
{
   unsigned int   disp(DISPLAY & mDisplayFlags);

   if( flag & DISPLAY )
   {
      // clear the display-changed flag once user looks at it
      mDisplayFlags  &= ~DISPLAY;
   }

   return   ( disp | mDisplayFlags | mModelFlags ) & flag;
}



/**
 * Set one of the display flags
 *
 * @param  flag to set
 * @param  on if setting, !on if resetting
 */
void
ViewState::SetDisplayFlag( DISPLAY_FLAG flag, bool on /*= true*/ )
{
   if( flag & RESET )
   {
      Transform   xform;
      GetCamPosition( xform, true );
      SetCamPosition( xform );
      SetCamOrbitDist( GetCamOrbitDist( true ) );
   }

   if( flag & kDisplayMask )
   {
      mDisplayFlags  |= DISPLAY;
   }

   if( on )
   {
      mDisplayFlags  |= ( flag & kDisplayMask );
      mModelFlags    |= ( flag & kModelMask );
   }
   else
   {
      mDisplayFlags  &= ~( flag & kDisplayMask );
      mModelFlags    &= ~( flag & kModelMask );
   }
}



/**
 * Get one of the motion flags
 *
 * @param  flag to get
 *
 * @return  non-zero if flag is set, else zero
 */
int
ViewState::GetMotionFlag( MOTION_FLAG flag ) const
{
   return   mMotionFlags & flag;
}



/**
 * Set one of the motion flags
 *
 * @param  flag to set
 */
void
ViewState::SetMotionFlag( MOTION_FLAG flag )
{
   // motion flags are radio flags, so copy it, don't 'or' it
   mMotionFlags   = flag;
}



/**
 * Get one of the joystick flags
 *
 * @param  flag to get
 *
 * @return  non-zero if flag is set, else zero
 */
int
ViewState::GetJoystickFlag( JOYSTICK_FLAG flag )   const
{
   return   mJoystickFlags & flag;
}



/**
 * Set one of the joystick flags
 *
 * @param  flag to set
 * @param  on if setting, !on if resetting
 */
void
ViewState::SetJoystickFlag( JOYSTICK_FLAG flag, bool on /*= true*/ )
{
   if( on )
   {
      mJoystickFlags |= flag;
   }
   else
   {
      mJoystickFlags &= ~flag;
   }
}
