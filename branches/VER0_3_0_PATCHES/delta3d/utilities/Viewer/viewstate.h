#ifndef DELTA_VIEWSTATE
#define DELTA_VIEWSTATE

/**
 * Included files.
 */
#include <string>

#include <transform.h>



/**
 * Typedefs and definitions.
 */
#if   !  defined(BIT)
#define  BIT(a)   (long(1L<<long(a)))
#endif



/**
 * UI data info and view states.
 */
class ViewState
{
   public:
               enum                 DISPLAY_FLAG
                                    {
                                       DISPLAY     = BIT(0),
                                       COMPASS     = BIT(1),
                                       XY_PLANE    = BIT(2),
                                       YZ_PLANE    = BIT(3),
                                       ZX_PLANE    = BIT(4),
                                       SCENEPOLY   = BIT(5),
                                       SCENEWIRE   = BIT(6),
                                       SCENETXT    = BIT(7),
                                       SCENELIGHT  = BIT(8),
                                       POLYGON     = BIT(9),
                                       WIREFRAME   = BIT(10),
                                       TEXTURE     = BIT(11),
                                       LIGHTING    = BIT(12),
                                       RESET       = BIT(13),
                                    };

               enum                 MOTION_FLAG
                                    {
                                       WALK        = BIT(0),
                                       FLY         = BIT(1),
                                       UFO         = BIT(2),
                                       ORBIT       = BIT(3),
                                    };

               enum                 JOYSTICK_FLAG
                                    {
                                       JOY_1       = BIT(0),
                                       JOY_2       = BIT(1),
                                    };

   public:
                                    ViewState(  std::string filename = "",
                                                unsigned int indx = (unsigned int)(-1L) );
                                    ViewState( const ViewState& that );
                                    ~ViewState();

               ViewState&           operator=( const ViewState& that );
               bool                 operator==( const ViewState& that );

               const std::string&   GetFilename( void )        const    {  return   mFilename;     }
               unsigned int         GetIndex( void )           const    {  return   mFileIndex;    }

               void                 GetCamPosition( dtCore::Transform& camPos, bool def = false )  const;
               void                 SetCamPosition( const dtCore::Transform& camPos, bool def = false );

               float                GetCamOrbitDist( bool def = false  )   const;
               void                 SetCamOrbitDist( float orbitDist, bool def = false  );

               int                  GetDisplayFlag( DISPLAY_FLAG flag )    const;
               void                 SetDisplayFlag( DISPLAY_FLAG flag, bool on = true );

               int                  GetMotionFlag( MOTION_FLAG flag )      const;
               void                 SetMotionFlag( MOTION_FLAG flag );

               int                  GetJoystickFlag( JOYSTICK_FLAG flag )  const;
               void                 SetJoystickFlag( JOYSTICK_FLAG flag, bool on = true );

   private:
      static   const int            kDisplayMask;
      static   const int            kModelMask;
               std::string          mFilename;
               unsigned int         mFileIndex;
               dtCore::Transform       mCamPosition;
               float                mOrbitDist;
               dtCore::Transform       mCamPositionDef;
               float                mOrbitDistDef;
      static   int                  mDisplayFlags;
               int                  mModelFlags;
               int                  mMotionFlags;
               int                  mJoystickFlags;
};
#endif // DELTA_VIEWSTATE
