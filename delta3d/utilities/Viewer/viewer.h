#ifndef DELTA_VIEWER
#define DELTA_VIEWER

#include <dtABC/widget.h>
#include <dtCore/logicalinputdevice.h>
#include <dtCore/motionmodel.h>



// forward references
class ViewState;



class Viewer :  public   dtABC::Widget
{
   DECLARE_MANAGEMENT_LAYER(Viewer)

               enum     ITEMCOLOR
                        {
                           BLACK    = 0L,
                           WHITE,
                           RED,
                           YELLOW,
                           GREEN,
                           CYAN,
                           BLUE,
                           MAGENTA,

                           NUMITEMCOLORS
                        };

               enum     DISPLAYITEM
                        {
                           XY_PLANE = 0L,
                           YZ_PLANE,
                           ZX_PLANE,
                           COMPASS,
                           FILEOBJS,

                           NUMDISPLAYITEMS
                        };

               enum     GRIDS
                        {
                           XY_GRID = 0L,  // match XY_PLANE
                           YZ_GRID,       // match YZ_PLANE
                           ZX_GRID,       // match ZX_PLANE

                           NUMGRIDS
                        };

               enum     MOTIONMODEL
                        {
                           WALK  = 0L,
                           FLY,
                           UFO,
                           ORBIT,

                           NUMMOTIONMODELS
                        };

               enum     JOYSTICKID
                        {
                           JOY_1 = 0L,
                           JOY_2,

                           NUMJOYSTICKIDS
                        };

               enum     POLYGONMODE
                        {
                           FILL  = 0L,
                           WIRE,

                           NUMPOLYGONMODES
                        };

      static   const float                DEF_X;
      static   const float                DEF_Y;
      static   const float                DEF_Z;
      static   const float                DEF_H;
      static   const float                DEF_P;
      static   const float                DEF_R;
      static   const float                MUL_Y;
      static   const float                MUL_Z;
      static   const osg::Node::NodeMask  NODEMASK_ON;
      static   const osg::Node::NodeMask  NODEMASK_OFF;
      static   const int                  GRID_LINE_COUNT;
      static   const float                GRID_LINE_SPACING;
      static   const unsigned int         SCENE_INDX;

   public:
      ///Message strings (in comming)
      static   const char*    msgLoadFile;
      static   const char*    msgSaveFileAs;
      static   const char*    msgGetState;
      static   const char*    msgSetState;
      static   const char*    msgResetCam;


   public:
                              Viewer( std::string name = "Viewer" );
      virtual                 ~Viewer();

   protected:
      ///Configure the internal components.
      virtual  void           Config( const dtABC::WinData* data = NULL );

      ///Override to receive messages.
      virtual  void           OnMessage( dtCore::Base::MessageData* data );
      virtual  void           FileLoaded( bool loaded, const char* filename ) = 0L;
      inline   osg::Group*    GetFileObj( unsigned int indx );
      inline   osg::Group*    GetDisplayObj( unsigned int indx );
               void           LoadFile( ViewState* vs );
               void           GetState( ViewState* vs );
               void           SaveFileAs( char *filename );
               void           SetState( ViewState* vs );
               void           ResetCam( void );

   private:
      inline   void           GetDefaultState( ViewState* vs );
      inline   void           EnableFile( bool on, unsigned int indx );
      inline   void           EnableDisplay( bool on, DISPLAYITEM di );
      inline   void           EnablePolygonMode( POLYGONMODE mode, bool on, unsigned int indx = SCENE_INDX );
      inline   void           EnableTexture( bool on, unsigned int indx = SCENE_INDX );
      inline   void           EnableLighting( bool on, unsigned int indx = SCENE_INDX );
      inline   void           EnableMotionModel( MOTIONMODEL mm );
      inline   void           EnableJoystick( bool on, JOYSTICKID jy );
      inline   void           InitInputDevices( void );
      inline   void           InitObjects( void );
      inline   void           InitCompass( void );
      inline   void           InitGridPlanes( void );

   private:
               dtCore::LogicalInputDevice*   mInputDevice;
               dtCore::MotionModel*          mMotionModel[NUMMOTIONMODELS];
               osg::MatrixTransform*      mDispXform[NUMGRIDS];
               ViewState&                 mCurState;
               osg::ref_ptr<osg::Group>   mViewerNode;
};


#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
/* You may turn off this include message by defining _NOAUTOLIB */
#undef _AUTOLIBNAME

#if defined(_DEBUG)
   #define _AUTOLIBNAME "osgFX.lib"
#else 
   #define _AUTOLIBNAME "osgFX.lib"
#endif

#if   ! defined(_NOAUTOLIBMSG)
   #pragma message( "Will automatically link with " _AUTOLIBNAME )
#endif

#pragma comment (lib, _AUTOLIBNAME)
#endif  // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#endif // DELTA_VIEWER
