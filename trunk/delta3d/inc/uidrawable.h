#ifndef DELTA_UIDRAWABLE
#define DELTA_UIDRAWABLE

#if !defined(_WIN32) && !defined(WIN32) && !defined(__WIN32__)
#define stricmp strcasecmp
#endif

#include "base.h"
#include "deltadrawable.h"
#include "mouse.h"
#include "keyboard.h"
#include "deltawin.h"
#include <osg/Drawable>
#include "UI/CUI_UI.h"
#include "UI/CUI_ListItem.h"
#include "UI/CUI_ListBox.h"
#include "UI/CUI_ScrollableFrame.h"
#include "cui_openglrenderer.h"
#include <osg/Group>
#include "sg.h"
#include "tinystr.h"
#include "tinyxml.h"
#include <stack>
#include <queue>

namespace dtCore
{

   ///ties the OSG GUI node to a dtCore DeltaDrawable

   /** The UIDrawable class allows you to associate a glGUI User Interface with
     * a dtCore::DeltaDrawable so it can be added to the dtCore::Scene.
     * To use, just create an instance of it, then add in the UI elements using
     * AddFrame(), AddShader(), AddFont(), AddBorder(), etc.
     * Then add the UIDrawable to the Scene for it to be rendered.
     * 
     * Default Shaders will be created and used if the UI elements are added 
     * without a Shader assigned to it.
     *
     * This class will read Keyboard and Mouse events and pass them on to the
     * CUI_UI for processing.
     *
     * @see Scene::AddDrawable()
     */
   class DT_EXPORT UIDrawable : public dtCore::Base,
                                public dtCore::DeltaDrawable,
                                public dtCore::MouseListener,
                                public dtCore::KeyboardListener
   {

      ///private class that ties the GUI rendering to an OSG node

      /** This is a private class that is used by the UIDrawable class.  To use,
        * it needs to have a valid CUI_UI passed on the constructor.
        */
      class osgCUIDrawable : public osg::Drawable
      {
      public:
         osgCUIDrawable(const osgCUIDrawable& drawable,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY){}

         osgCUIDrawable(CUI_UI *ui):
         mUI(ui)
         {
            this->setSupportsDisplayList(false);
            this->setUseDisplayList(false);
         }

         virtual ~osgCUIDrawable() {}


         virtual Object* cloneType() const { return new osgCUIDrawable(mUI); }
         virtual Object* clone(const osg::CopyOp& copyop) const { return new osgCUIDrawable(*this,copyop); }        

         virtual void drawImplementation(osg::State& state) const
         { //tell the UI to update and to render
            if (!mUI) return;
            mUI->UpdateStatus();
            mUI->Render(51);
         }

      private:
         CUI_UI *mUI; ///< osgCUIDrawable's pointer to the CUI_UI
      };

   public:
   
      DECLARE_MANAGEMENT_LAYER(UIDrawable)
        
      ///Default constructor - accepts the width and height of window
      UIDrawable(int width=1, int height=1);
      virtual ~UIDrawable();

      ///Override to receive messages
      virtual void OnMessage(MessageData *data);

      ///Get a pointer to the underlying CUI_UI
      CUI_UI* GetUI(void) {return mUI;}

      ///Load a xml GUI file and create the UI elements
      void LoadGUIFile(std::string filename);

      ///Set the resolution of the DeltaWin this UI is in (pixels)
      void SetWindowResolution(const int w, const int h);

      ///Get a pointer to the underlying CUI_OpenGLRender
      CUI_OpenGLRenderer *GetRenderer(void) const {return mRenderer;}

      ///Create a Shader that uses a solid color
      void CreateShader(std::string name);

      ///Create a Shader that uses a solid color
      void CreateShader(std::string name, sgVec4 color);

      ///Create a Shader that uses a texture
      void CreateShader(std::string name, std::string textureFilename);

      ///Create a Shader that uses a color and a texture
      void CreateShader(std::string name, sgVec4 color, std::string textureFilename);

      ///Get a pointer of an already created Shader
      IUI_Shader *GetShader(std::string name) {return mUI->GetShader(name.c_str());}
         
      ///Create a Border using the supplied coordinates.
      void CreateBorder(std::string name, sgVec2 *coords, const int numCoords);

      ///Get a pointer of an already created Border
      IUI_Border *GetBorder(std::string name) {return mUI->GetBorder(name.c_str()); }

      ///Create a new fixed width font
      void CreateFixedFont(std::string name, std::string shader,
                           int charW=16, int charH=24, int xres=256, int yres=256,
                           int startoffset=0);

      ///Get a pointer of an already created Font
      IUI_Font *GetFont(std::string name) {return mUI->GetFont(name.c_str());}         

      ///Add a Frame to this UI
      void AddFrame(CUI_Frame *frame);

      ///Add a Root Frame to this UI.  There must be at least one RootFrame
      void AddRootFrame( std::string name, CUI_Frame *rootFrame );

      ///Select which Root Frame to make active
      void SetActiveRootFrame(std::string name) {mUI->SetActiveRootFrame(name.c_str()); mActiveRootFrame = name;}

      ///Returns the name of the active root frame
      std::string GetActiveRootFrame() { return mActiveRootFrame; }
      
      ///Supply the calback function for an already created RootFrame
      void SetCallbackFunc(std::string rootFrameName, CUI_UI::callbackfunc func);

      virtual osg::Node* GetOSGNode() {return mNode.get();}

   private:
      virtual void ButtonReleased(dtCore::Mouse* mouse, dtCore::MouseButton button);
      virtual void ButtonPressed(dtCore::Mouse* mouse, dtCore::MouseButton button);
      virtual void MouseMoved(dtCore::Mouse* mouse, float x, float y); 
      virtual void MouseDragged(Mouse* mouse, float x, float y);
      virtual void KeyPressed(Keyboard* keyboard, 
                              Producer::KeyboardKey key,
                              Producer::KeyCharacter character);

      ///used for the xml file parsing
      typedef struct ELEMDATA
      {
         ELEMDATA():elem(NULL),data(NULL){}
         TiXmlElement *elem;
         void *data;
      };

      /// type - STL stack of TiXml Elements
      typedef std::stack<ELEMDATA*> ELEMSTACK;

      //! stack of frame hierarchy
      ELEMSTACK m_elementStack;

      ///used for caching mouse events
      typedef struct MOUSEEVENT
      {
         MOUSEEVENT(float X = 0.f, float Y =0.f, unsigned int S = 0L):x(X),y(Y),s(S){}
         MOUSEEVENT& operator=( const MOUSEEVENT& that ) { x=that.x; y=that.y; s=that.s; return *this; }
         float          x;
         float          y;
         unsigned int   s;
      };

      /// type - STL queue of mouse events
      typedef std::queue<MOUSEEVENT> MOUSEQUEUE;

      //! queues of mouse events (buffered for access protection)
      enum  {  NumMouseQueues = 2L  };
      MOUSEQUEUE     mMouseQueue[NumMouseQueues];
      unsigned int   mCurrentQueue;

      void LoadControlFrame( ELEMDATA *elem, CUI_Frame *frame = NULL );
      void LoadControlDraggableFrame( ELEMDATA *elem );
      void LoadControlDraggableViewFrame( ELEMDATA *elem );
      void LoadControlTextLabel( ELEMDATA *elem, CUI_Widget *frame = NULL );
      void LoadControlButton( ELEMDATA *elem );
      void LoadControlResizeButton( ELEMDATA *elem );
      void LoadControlProgressBar( ELEMDATA *elem, CUI_ProgressBar *frame = NULL );
      void LoadControlSliderBar( ELEMDATA *elem );
      void LoadControlTextBox( ELEMDATA *elem, CUI_TextBox *frame = NULL );
      void LoadControlEditTextBox( ELEMDATA *elem );
      void LoadControlListBox( ELEMDATA *elem, CUI_ListBox *frame = NULL );
      void LoadControlComboBox( ELEMDATA *elem );
      void LoadControlMenu( ELEMDATA *elem );
      void LoadControlRadioGroup( ELEMDATA *elem );
      void LoadControlScrollFrame( ELEMDATA *elem );
      void LoadControlScrollTextBox( ELEMDATA *elem );
      void LoadResourceFBMFont( ELEMDATA *elem );
      void LoadResourceCoordSys( ELEMDATA *elem);
      void LoadResourcePolyBorder(ELEMDATA *elem);
      void LoadResourceShader( ELEMDATA *elem);
      void LoadDataState(ELEMDATA *elem);
      void LoadDataRadioItem(ELEMDATA *elem);
      void LoadDataMenuItem(ELEMDATA *elem);
      void LoadDataListItem(ELEMDATA *elem,CUI_ListItem *item=NULL);
      void LoadDataRect(ELEMDATA *obj);
      void LoadDataPoint2d(ELEMDATA *elem);
      void ParseElement(TiXmlElement *elem);
      void EndElement();

      ///create default shaders, coordinate systems, etc.
      void SetupDefaults();

      osg::ref_ptr<osg::Group> mNode; ///<Contains the node which renders the UI
      CUI_UI *mUI; ///<Pointer to the CUI_UI
      std::string mActiveRootFrame; ///<The name of the active root frame
      unsigned short mButtonState; ///<The current mouse button state
      int mWidth; ///<the width of the DeltaWin
      int mHeight; ///<The height of the DeltaWin
      float mMouseX; ///<The current Mouse X position
      float mMouseY; ///<the current Mouse Y position
      CUI_OpenGLRenderer *mRenderer; ///<The opengl renderer we're using
   };
}


#endif // DELTA_UIDRAWABLE
