#include <cassert>
#include <string>

#include "dtCore/system.h"
#include "dtCore/uidrawable.h"
#include "dtCore/notify.h"
#include "UI/CUI_Menu.h"
#include "UI/CUI_PolyEdgeBorder.h"
#include "UI/CUI_RadioGroup.h"
#include "UI/CUI_ListItem.h"
#include "UI/CUI_ListBox.h"
#include "UI/CUI_ScrollableFrame.h"
#include "UI/CUI_ComboBox.h"
#include "UI/CUI_ResizeButton.h"
#include "UI/CUI_DraggableViewFrame.h"
#include "UI/CUI_DraggableFrame.h"
#include "UI/CUI_FixedBMFont.h"
#include "UI/CUI_MultipleStateButton.h"

#include <osg/Geode>
#include <osg/Projection>
#include <osg/MatrixTransform>
#include <osgDB/FileUtils>

using namespace dtCore;


IMPLEMENT_MANAGEMENT_LAYER(UIDrawable)


/** The constructor.  Supply the width and height
  * of the parent DeltaWin.  The constructor will create a new CUI_UI and 
  * CUI_OpenGLRenderer, setup some default Shaders, and create the OSG
  * nodes.
  */
UIDrawable::UIDrawable(int width, int height):
mUI(NULL),
mButtonState(0),
mWidth(width),
mHeight(height),
mRenderer(NULL),
mCurrentQueue(0L)
{
   RegisterInstance(this);
   
   dtCore::Mouse::GetInstance(0)->AddMouseListener(this);
   dtCore::DeltaWin::GetInstance(0)->GetKeyboard()->AddKeyboardListener(this);

   AddSender( System::GetSystem() );
   
   mUI = new CUI_UI();

   mRenderer = new CUI_OpenGLRenderer();
   mUI->SetRenderer(mRenderer);

   SetupDefaults();

   mNode = new osg::Group();
   osg::Geode *geod = new osg::Geode();

   osg::StateSet* stateset = geod->getOrCreateStateSet();
   stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);

   stateset->setRenderBinDetails(11,"RenderBin");
   stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
  // stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	geod->setStateSet(stateset);

   geod->addDrawable(new osgCUIDrawable(mUI)); //add our osg node here

   osg::MatrixTransform* modelview_abs = new osg::MatrixTransform;
   modelview_abs->setReferenceFrame(osg::Transform::RELATIVE_TO_ABSOLUTE);
   modelview_abs->setMatrix(osg::Matrix::identity());
   modelview_abs->addChild(geod);

   osg::Projection* projection = new osg::Projection();
   projection->setMatrix(osg::Matrix::ortho2D(0,1,0,1));
   projection->addChild(modelview_abs);

   mNode.get()->addChild(projection);
}

UIDrawable::~UIDrawable(void)
{
   DeregisterInstance(this);
   
   mNode = NULL;
}

void UIDrawable::OnMessage(MessageData *data)
{
   if(!mUI) return;

   assert( data );
   if( data->message == "frame" )
   {
      unsigned int   flush = mCurrentQueue;

      // the following line could cause a crash if an asynchronous mouse
      // event happens at the exact moment this line is being accessed
      mCurrentQueue  =  ( mCurrentQueue + 1L ) % NumMouseQueues;

      while( mMouseQueue[flush].size() )
      {
         MOUSEEVENT& event = mMouseQueue[flush].front();
         mUI->UpdateCursor( event.x, event.y, event.s );
         mMouseQueue[flush].pop();
      }
   }
}

void UIDrawable::ButtonReleased(dtCore::Mouse* mouse, dtCore::MouseButton button)
{      
   if (!mUI) return;

   switch(button) 
   {
   case dtCore::LeftButton:   mButtonState &= ~UI_LBUTTONDOWN;   	break;
   case dtCore::RightButton:  mButtonState &= ~UI_RBUTTONDOWN;   	break;
   case dtCore::MiddleButton: mButtonState &= ~UI_MBUTTONDOWN;   	break;
   default:                mButtonState = mButtonState;        break; 
   }

   mMouseQueue[mCurrentQueue].push( MOUSEEVENT(mMouseX,mMouseY,mButtonState) );
//   mUI->UpdateCursor(mMouseX, mMouseY, mButtonState);
}

void UIDrawable::ButtonPressed(dtCore::Mouse* mouse, dtCore::MouseButton button)
{
   if (!mUI) return;

   switch(button) 
   {
   case dtCore::LeftButton:   mButtonState |= UI_LBUTTONDOWN;   	break;
   case dtCore::RightButton:  mButtonState |= UI_RBUTTONDOWN;   	break;
   case dtCore::MiddleButton: mButtonState |= UI_MBUTTONDOWN;   	break;
   default:                mButtonState = mButtonState;        break;
   }

   mMouseQueue[mCurrentQueue].push( MOUSEEVENT(mMouseX,mMouseY,mButtonState) );
//   mUI->UpdateCursor(mMouseX, mMouseY, mButtonState);
}

void UIDrawable::MouseMoved(dtCore::Mouse* mouse, float x, float y) 
{
   if (!mUI) return;

   //do a little magic to convert Mouse coords to CUI_UI coords
   x = (x+1) * 0.5f;
   y = (y+1) * 0.5f;
   y = (y*-1.f) + 1.f;
   mMouseX = x*mWidth;
   mMouseY = y*mHeight;

   mMouseQueue[mCurrentQueue].push( MOUSEEVENT(mMouseX,mMouseY,mButtonState) );
//   mUI->UpdateCursor(mMouseX, mMouseY, mButtonState);
}

void UIDrawable::MouseDragged(Mouse* mouse, float x, float y)
{
   if (!mUI) return;

   //do a little magic to convert Mouse coords to CUI_UI coords
   x = (x+1) * 0.5f;
   y = (y+1) * 0.5f;
   y = (y*-1.f) + 1.f;
   mMouseX = x*mWidth;
   mMouseY = y*mHeight;

   mMouseQueue[mCurrentQueue].push( MOUSEEVENT(mMouseX,mMouseY,mButtonState) );
//   mUI->UpdateCursor(mMouseX, mMouseY, mButtonState);
}

void UIDrawable::KeyPressed(Keyboard* keyboard, 
                        Producer::KeyboardKey key,
                        Producer::KeyCharacter character)
{
   if (!mUI) return;

   //do a little magic to convert Producer keys to UI keys
   int uikey = (int)character;
   switch(character) 
   {
   case Producer::KeyChar_Right: uikey = UI_KEY_RIGHT;  	break;
   case Producer::KeyChar_Left:  uikey = UI_KEY_LEFT;  	break;
   case Producer::KeyChar_Up:    uikey = UI_KEY_UP;  	break;
   case Producer::KeyChar_Down:  uikey = UI_KEY_DOWN;  	break;
   case Producer::KeyChar_BackSpace:   uikey = '\b';  	break;
   case Producer::KeyChar_Delete:      uikey = 127;  	break;
   case Producer::KeyChar_Shift_L:     return;   break;
   case Producer::KeyChar_Shift_R:     return;   break;
   default: break;
   }
   mUI->ProcessKey(uikey);
}


void UIDrawable::LoadResourceShader( ELEMDATA *elem)
{
   std::string name = elem->elem->Attribute("name");
   sgVec4 color = {-1.f, -1.f, -1.f, -1.f};
   std::string filename;
   CUI_OpenGLShader *shader = NULL;

   bool hasColor = false;
   if (elem->elem->Attribute("r"))
   {
      color[0] = atof(elem->elem->Attribute("r")); 
   }
   if (elem->elem->Attribute("g"))
   {
      color[1] = atof(elem->elem->Attribute("g"));
   }
   if (elem->elem->Attribute("b"))
   {
      color[2] = atof(elem->elem->Attribute("b"));
   }
   if (elem->elem->Attribute("a"))
   {
      color[3] = atof(elem->elem->Attribute("a"));
   }

   if (elem->elem->Attribute("filename"))
   {
      filename = elem->elem->Attribute("filename");
   }

   if (color[0]>=0.f && color[1]>=0.f && color[2]>=0.f && color[3]>=0.f)
   {
      if (filename.empty())
      {
         CreateShader(name, color);  
      }
      else CreateShader(name, color, filename);
   }
   else
   {
      if (filename.empty())
      {
         CreateShader(name);
      }
      else
      {
         CreateShader(name, filename);
      }
   }
}


void UIDrawable::LoadResourcePolyBorder( ELEMDATA *elem)
{
   //<xsd:element name="point2d" minOccurs="3" maxOccurs="unbounded"/>
   //<xsd:attribute name="name" type="xsd:string" use="required"/>
   CUI_PolyEdgeBorder *poly = new CUI_PolyEdgeBorder();

   std::string name = elem->elem->Attribute("name");
   mUI->AddBorder(name.c_str(), poly);
   poly->SetOwnedByUI();

   elem->data = poly;
}


/** Load a predefined GUI from an XML file.  This routine does not use
 *  any search paths so the fully qualified filename must be supplied.
 */
bool UIDrawable::LoadGUIFile( std::string filename )
{
   std::string path = osgDB::findDataFile(filename);
   
   //check if filename exists
   TiXmlDocument doc(path.c_str());

   if(path.empty())
   {
      dtCore::Notify(dtCore::WARN, "Can't load GUI config file '%s'", filename.c_str());
      dtCore::Notify(dtCore::WARN, "Description: '%s'", doc.ErrorDesc());
      return false;
   }

   doc.LoadFile();
   TiXmlNode *root = doc.RootElement();

   TiXmlNode *sibling= root->FirstChildElement();
   while( sibling != NULL)
   {
      ParseElement(sibling->ToElement());
      sibling = sibling->NextSiblingElement();
   }

   return true;
}

void UIDrawable::LoadDataRect(ELEMDATA *obj)
{
   float *rect = new v4_f;

   // set defaults
   rect[0] = 0;
   rect[1] = 0;
   rect[2] = 1;
   rect[3] = 1;

   if (obj->elem->Attribute("xmin"))
   {
      rect[0] = atof(obj->elem->Attribute("xmin"));
   }
   if (obj->elem->Attribute("ymin"))
   {
      rect[1] = atof(obj->elem->Attribute("ymin"));
   }
   if (obj->elem->Attribute("xmax"))
   {
      rect[2] = atof(obj->elem->Attribute("xmax"));
   }
   if (obj->elem->Attribute("ymax"))
   {
      rect[3] = atof(obj->elem->Attribute("ymax"));
   }

   obj->data = rect;
}

void UIDrawable::LoadDataPoint2d( ELEMDATA *elem)
{
   float *point = new v2_f;

   point[0] = 0; point[1] = 0;

   if( elem->elem->Attribute("x"))
   {
      point[0] = atof(elem->elem->Attribute("x"));
   }

   if( elem->elem->Attribute("y") )
   {
      point[1] = atof(elem->elem->Attribute("y"));
   }

   elem->data = point;
}

void UIDrawable::LoadDataState(ELEMDATA *elem)
{
   IUI_Shader **shaders = new IUI_Shader*[4];
   memset( shaders, 0, sizeof(IUI_Shader*)*4 );

   if( elem->elem->Attribute( "shader") )
   {
      shaders[0] = mUI->GetShader(elem->elem->Attribute("shader"));
   }

   if( elem->elem->Attribute( "hovershader") )
   {
      shaders[1] = mUI->GetShader(elem->elem->Attribute( "hovershader"));
   }

   if( elem->elem->Attribute( "disableshader" ) )
   {
      shaders[2] = mUI->GetShader(elem->elem->Attribute( "disableshader" ));
   }

   if( elem->elem->Attribute( "clickshader" ) )
   {
      shaders[3] = mUI->GetShader( elem->elem->Attribute( "clickshader" ));
   }

   elem->data = shaders;
}

void UIDrawable::LoadDataListItem(ELEMDATA *elem, CUI_ListItem *item)
{
   CUI_ListItem *newitem = item;

   if( newitem == NULL )
   {
      newitem = new CUI_ListItem();
   }

   if (elem->elem->Attribute("id"))
   {
      newitem->SetItemID(atoi(elem->elem->Attribute("id")));
   }

   if (elem->elem->Attribute("text"))
   {
      newitem->SetText((char*)elem->elem->Attribute("text"));
   }

   if (elem->elem->Attribute("iconshader"))
   {
      newitem->SetIcon( mUI->GetShader(elem->elem->Attribute("iconshader")));
   }

   if (elem->elem->Attribute("toggle"))
   {
      newitem->SetFlag(UI_ITEM_TOGGLE, true);
   }

   elem->data = newitem;
}

void UIDrawable::LoadDataRadioItem(ELEMDATA *elem)
{
   CUI_RadioItem *item = new CUI_RadioItem();
   LoadDataListItem( elem, item );

   elem->data = item;
}

void UIDrawable::LoadDataMenuItem(ELEMDATA *elem)
{
   CUI_MenuItem *item = new CUI_MenuItem();
   LoadDataListItem( elem, item );

   if( elem->elem->Attribute("submenuid"))
   {
      unsigned long valuel = atol(elem->elem->Attribute("submenuid"));
      item->SetSubMenu( mUI->GetFrame(valuel));
      item->SetFlag( UI_ITEM_SUBMENU, true );
      mUI->GetFrame(valuel)->SetFlag( UI_INACTIVE, true );
   }

   elem->data = item;
}

void UIDrawable::LoadControlScrollFrame( ELEMDATA *elem )
{
   float w=1.0, h=1.0;

   CUI_ScrollableFrame *newframe = new CUI_ScrollableFrame();

   // get values
   // load inherited data
   LoadControlFrame( elem, newframe );

   // add to ui
   mUI->AddFrame( newframe );

   if( elem->elem->Attribute( "viewwidth") )
   {
      w = atof(elem->elem->Attribute( "viewwidth"));
   }

   if( elem->elem->Attribute("viewheight") )
   {
      h = atof(elem->elem->Attribute("viewheight"));
   }

   newframe->SetViewArea( w, h );

   if( elem->elem->Attribute("scrollshader") )
   {
      newframe->SetScrollShader( mUI->GetShader( elem->elem->Attribute("scrollshader") ) );
   }

   if( elem->elem->Attribute("slidershader") )
   {
      newframe->SetSliderShader(  mUI->GetShader( elem->elem->Attribute("slidershader") ) );
   }

   elem->data = newframe;
}

void UIDrawable::LoadControlScrollTextBox( ELEMDATA *elem )
{
   unsigned long w=28, h=26;

   CUI_ScrollableTextBox *newframe = new CUI_ScrollableTextBox();

   // get values
   // load inherited data
   LoadControlFrame( elem, newframe );

   mUI->AddFrame( newframe );

   if( elem->elem->Attribute("columns" ) )
   {
      w = atoi( elem->elem->Attribute("columns"));
   }

   if( elem->elem->Attribute("rows") )
   {
      h = atoi(elem->elem->Attribute("rows"));
   }

   newframe->SetTextBoxSize( w, h );

   if( elem->elem->Attribute("font") )
   {
      newframe->SetFont( mUI->GetFont( elem->elem->Attribute("font") ) );
   }

   if( elem->elem->Attribute("scrollshader" ) )
   {
      newframe->SetScrollShader( mUI->GetShader( elem->elem->Attribute("scrollshader" ) ) );
   }

   if( elem->elem->Attribute("slidershader") )
   {
      newframe->SetSliderShader( mUI->GetShader( elem->elem->Attribute("slidershader") ) );
   }

   if( elem->elem->Attribute( "wordwrap" ) )
   {
      bool val = false;
      if (elem->elem->Attribute("wordwrap") == std::string("true"))
         val = true;
      newframe->SetWordWrap( val );
   }

   elem->data = newframe;

}

void UIDrawable::LoadControlRadioGroup( ELEMDATA *elem )
{
   IUI_Shader *on=NULL, *off=NULL, *onh=NULL, *offh=NULL;

   CUI_RadioGroup *newframe = new CUI_RadioGroup();

   // get values
   // load inherited data
   LoadControlFrame( elem, newframe );

   mUI->AddFrame( newframe );

   if( elem->elem->Attribute( "columns" ) )
   {
      newframe->SetColumns( atoi(elem->elem->Attribute("columns")) );
   }

   if( elem->elem->Attribute( "font" ) )
   {
      newframe->SetFont( mUI->GetFont(elem->elem->Attribute( "font" )) );
   }

   if( elem->elem->Attribute( "onshader" ) )
   {
      on = mUI->GetShader(elem->elem->Attribute( "onshader" ));
   }

   if( elem->elem->Attribute( "offshader" ) )
   {
      off = mUI->GetShader(elem->elem->Attribute( "offshader" ));
   }

   if( elem->elem->Attribute( "onhovershader" ) )
   {
      onh = mUI->GetShader(elem->elem->Attribute( "onhovershader" ));
   }

   if( elem->elem->Attribute( "offhovershader" ) )
   {
      offh = mUI->GetShader(elem->elem->Attribute("offhovershader"));
   }

   newframe->SetButtonShaders( on, off, onh, offh );

   elem->data = newframe;

}


void UIDrawable::LoadControlComboBox( ELEMDATA *elem )
{
   CUI_ComboBox *newframe = new CUI_ComboBox();

   // get values
   // load inherited data
   LoadControlFrame( elem, newframe );

   // add to ui
   mUI->AddFrame( newframe );

   if( elem->elem->Attribute( "itemshader" ) )
   {
      newframe->SetItemShader( mUI->GetShader(elem->elem->Attribute( "itemshader" )) );      
   }

   if( elem->elem->Attribute( "hovershader" ) )
   {
      newframe->SetHoverShader( mUI->GetShader( elem->elem->Attribute( "hovershader" )) );
   }

   if( elem->elem->Attribute( "selectedshader" ) )
   {
      newframe->SetSelectedShader( mUI->GetShader(elem->elem->Attribute( "selectedshader" )) );
   }

   if( elem->elem->Attribute( "dropiconshader" ) )
   {
      newframe->SetDropIconShader( mUI->GetShader(elem->elem->Attribute( "dropiconshader" )) );
   }

   // boolean flags
   if( elem->elem->Attribute( "vertical" ) )
   {
      bool val = false;
      if (elem->elem->Attribute("vertical") == std::string("true"))
         val = true;

      newframe->SetFlag( UI_VERTICAL, val );
   }

   elem->data= newframe;

}

void UIDrawable::LoadControlMenu( ELEMDATA *elem )
{
   CUI_Menu *newframe = new CUI_Menu();

   // get values
   // load inherited data
   LoadControlListBox( elem, newframe );

   if( elem->elem->Attribute( "submenuicon" ) )
   {
      newframe->SetSubMenuIcon( mUI->GetShader(elem->elem->Attribute( "submenuicon" )) );
   }

   elem->data= newframe;

}
void UIDrawable::LoadControlSliderBar( ELEMDATA *elem )
{
   CUI_SliderBar *newframe = new CUI_SliderBar();

   // get values
   // load inherited data
   LoadControlProgressBar( elem, newframe );

   if( elem->elem->Attribute( "sliderrange" ) )
   {
      newframe->SetValueRange( atof(elem->elem->Attribute( "sliderrange")) );
   }

   // boolean flags
   if( elem->elem->Attribute( "vertical" ) )
   {
      bool val = false;
      if (elem->elem->Attribute("vertical") == std::string("true"))
         val = true;

      newframe->SetFlag( UI_VERTICAL, val);
   }

   elem->data= newframe;
}

void UIDrawable::LoadControlEditTextBox( ELEMDATA *elem )
{
   unsigned short cursor = UI_CURSOR_NONE;

   CUI_EditableTextBox *newframe = new CUI_EditableTextBox();

   // get values
   // load inherited data
   LoadControlTextBox( elem, newframe );

   if( elem->elem->Attribute( "cursortype" ) )
   {
      if( stricmp(elem->elem->Attribute( "cursortype" ),"bar") == 0 )
         cursor = UI_RESIZE_MIN;
      else if( stricmp(elem->elem->Attribute( "cursortype" ),"underline") == 0 )
         cursor = UI_CURSOR_UNDERLINE;
   }

   newframe->SetCursorType( cursor );

   // boolean flags
   if( elem->elem->Attribute( "maxrow" ) )
   {
      newframe->SetMaxRows( atoi(elem->elem->Attribute( "maxrow")) );
   }

   elem->data= newframe;

}
void UIDrawable::LoadControlListBox( ELEMDATA *elem, CUI_ListBox *frame  )
{
   IUI_Shader *onshader = NULL, *offshader = NULL;

   CUI_ListBox *newframe = frame;

   if( newframe == NULL )
   {
      newframe = new CUI_ListBox();
   }

   // get values
   // load inherited data
   LoadControlFrame( elem, newframe );

   // add to ui
   mUI->AddFrame( newframe );

   if( elem->elem->Attribute( "itemshader" ) )
   {
      newframe->SetItemShader( mUI->GetShader(elem->elem->Attribute( "itemshader" )) );
   }

   if( elem->elem->Attribute( "font" ) )
   {
      newframe->SetFont( mUI->GetFont( elem->elem->Attribute( "font" )) );
   }

   if( elem->elem->Attribute( "hovershader" ) )
   {
      newframe->SetHoverShader( mUI->GetShader(elem->elem->Attribute( "hovershader" )) );
   }

   if( elem->elem->Attribute( "selectedshader" ) )
   {
      newframe->SetSelectedShader( mUI->GetShader(elem->elem->Attribute( "selectedshader" )) );
   }

   if( elem->elem->Attribute( "itemsize" ) )
   {
      newframe->SetItemSize( atof(elem->elem->Attribute( "itemsize")) );
   }

   // boolean flags
   if( elem->elem->Attribute( "squeeze") )
   {
      bool val = false;
      if (elem->elem->Attribute("squeeze") == std::string("true"))
         val = true;

      newframe->SetFlag( UI_SQUEEZE, val);
   }

   if( elem->elem->Attribute( "vertical" ) )
   {
      bool val = false;
      if (elem->elem->Attribute("vertical") == std::string("true"))
         val = true;

      newframe->SetFlag( UI_VERTICAL, val);
   }

   if( elem->elem->Attribute( "toggleonshader" ) )
   {
      onshader = mUI->GetShader(elem->elem->Attribute( "toggleonshader" ));
   }

   if( elem->elem->Attribute( "toggleoffshader" ) )
   {
      offshader = mUI->GetShader(elem->elem->Attribute( "toggleoffshader" ));
   }

   newframe->SetToggleIcons( onshader, offshader );

   elem->data= newframe;

}

void UIDrawable::LoadControlTextBox( ELEMDATA *elem, CUI_TextBox *frame )
{
   unsigned short col = 30, row = 40;

   CUI_TextBox *newframe = frame;

   if( newframe == NULL )
   {
      newframe = new CUI_TextBox();
   }

   // get values
   // load inherited data
   LoadControlTextLabel( elem, newframe );

   if( elem->elem->Attribute( "columns" ) )
   {
      col = atoi(elem->elem->Attribute( "columns" ));
   }

   if( elem->elem->Attribute( "rows" ) )
   {
      row = atoi(elem->elem->Attribute( "rows"));
   }

   newframe->SetSize( col, row );

   // boolean flags
   if( elem->elem->Attribute( "wordwrap" ) )
   {
      bool val = false;
      if (elem->elem->Attribute("wordwrap") == std::string("true"))
         val = true;
      newframe->SetWordWrap( val);
   }

   elem->data= newframe;

}

void UIDrawable::LoadControlProgressBar( ELEMDATA *elem, CUI_ProgressBar *frame)
{
   float min = 0, max = 100;

   CUI_ProgressBar *newframe = frame;

   if( newframe == NULL )
   {
      newframe = new CUI_ProgressBar();
   }

   // get values
   // load inherited data
   LoadControlTextLabel( elem, newframe );

   if( elem->elem->Attribute( "rangemin" ) )
   {
      min = atof(elem->elem->Attribute( "rangemin" ));
   }

   if( elem->elem->Attribute( "rangemax" ) )
   {
      max = atof(elem->elem->Attribute( "rangemax" ));
   }

   newframe->SetRange( min, max );

   // boolean flags
   if( elem->elem->Attribute( "vertical") )
   {
      bool val = false;
      if (elem->elem->Attribute("vertical") == std::string("true"))
         val = true;
      newframe->SetFlag( UI_VERTICAL, val);
   }

   if( elem->elem->Attribute( "invert") )
   {
      bool val = false;
      if (elem->elem->Attribute("invert") == std::string("true"))
         val = true;
      newframe->SetFlag( UI_INVERT,val);
   }

   if( elem->elem->Attribute( "squeeze") )
   {
      bool val = false;
      if (elem->elem->Attribute("squeeze") == std::string("true"))
         val = true;
      newframe->SetFlag( UI_SQUEEZE, val);
   }

   if( elem->elem->Attribute( "slidershader" ) )
   {
      newframe->SetBarShader( mUI->GetShader(elem->elem->Attribute( "slidershader" )) );
   }

   elem->data= newframe;
}

void UIDrawable::LoadControlResizeButton( ELEMDATA *elem )
{
   float width=0.1,height=0.1;
   unsigned short x = UI_RESIZE_NONE, y = UI_RESIZE_NONE;

   CUI_ResizeButton *newframe = new CUI_ResizeButton();

   // get values
   // load inherited data
   LoadControlTextLabel( elem, newframe );

   if( elem->elem->Attribute( "shader" ) )
   {
      newframe->SetShader( mUI->GetShader(elem->elem->Attribute( "shader" )) );
   }

   if( elem->elem->Attribute( "hovershader" ) )
   {
      newframe->SetHoverShader( mUI->GetShader(elem->elem->Attribute( "hovershader" )) );
   }

   if( elem->elem->Attribute( "disableshader" ) )
   {
      newframe->SetDisableShader( mUI->GetShader(elem->elem->Attribute( "disableshader" ) ) );
   }

   if( elem->elem->Attribute( "clickshader" ) )
   {
      newframe->SetClickShader( mUI->GetShader(elem->elem->Attribute( "clickshader" )) );
   }

   if( elem->elem->Attribute( "xresize" ) )
   {
      if( stricmp(elem->elem->Attribute( "xresize" ),"min") == 0 )
         x = UI_RESIZE_MIN;
      else if( stricmp(elem->elem->Attribute( "xresize" ),"max") == 0 )
         x = UI_RESIZE_MAX;
      else if( stricmp(elem->elem->Attribute( "xresize" ),"both") == 0 )
         x = UI_RESIZE_MIN | UI_RESIZE_MAX;
   }

   if( elem->elem->Attribute( "yresize" ) )
   {
      if( stricmp(elem->elem->Attribute( "yresize" ),"min") == 0 )
         y = UI_RESIZE_MIN;
      else if( stricmp(elem->elem->Attribute( "yresize" ),"max") == 0 )
         y = UI_RESIZE_MAX;
      else if( stricmp(elem->elem->Attribute( "yresize" ),"both") == 0 )
         y = UI_RESIZE_MIN | UI_RESIZE_MAX;
   }

   newframe->SetResizing( x, y );

   if( elem->elem->Attribute( "minimumwidth" ) )
   {
      width = atof(elem->elem->Attribute( "minimumwidth" ));
   }

   if( elem->elem->Attribute( "minimumheight" ) )
   {
      height = atof(elem->elem->Attribute( "minimumheight" ));
   }

   newframe->SetMinimumSize( width, height );

   elem->data= newframe;

}
void UIDrawable::LoadControlButton( ELEMDATA *elem )
{
   CUI_MultipleStateButton *newframe = new CUI_MultipleStateButton();

   // get values
   // load inherited data
   LoadControlTextLabel( elem, newframe );

   if( elem->elem->Attribute( "shader" ) )
   {
      newframe->SetShader( mUI->GetShader(elem->elem->Attribute( "shader" )) );
   }

   if( elem->elem->Attribute( "hovershader" ) )
   {
      newframe->SetHoverShader( mUI->GetShader(elem->elem->Attribute( "hovershader" )) );
   }

   if( elem->elem->Attribute( "disableshader" ) )
   {
      newframe->SetDisableShader( mUI->GetShader(elem->elem->Attribute( "disableshader" )) );
   }

   if( elem->elem->Attribute( "clickshader" ) )
   {
      newframe->SetClickShader( mUI->GetShader(elem->elem->Attribute( "clickshader" )) );
   }

   elem->data= newframe;

}
void UIDrawable::LoadControlTextLabel( ELEMDATA *elem, CUI_Widget *frame)
{
   CUI_Widget *newframe = frame;

   if( newframe == NULL )
   {
      newframe = new CUI_Widget();
   }

   // get values
   // load basic frame data
   LoadControlFrame( elem, newframe );

   if( elem->elem->Attribute( "text" ) )
   {
      newframe->SetText( elem->elem->Attribute( "text" ) );
   }

   if( elem->elem->Attribute( "font" ) )
   {
      newframe->SetFont( mUI->GetFont( elem->elem->Attribute( "font" )) );
   }

   if( elem->elem->Attribute( "textscale" ) )
   {
      newframe->SetTextScale( atof(elem->elem->Attribute( "textscale" )) );
   }

   if( elem->elem->Attribute( "border" ) )
   {
      newframe->SetBorder( mUI->GetBorder(elem->elem->Attribute( "border" )) );
   }

   elem->data= newframe;

}

void UIDrawable::LoadControlDraggableViewFrame( ELEMDATA *elem )
{
   float width = 0.5, height = 0.5;

   CUI_DraggableViewFrame *frame = new CUI_DraggableViewFrame();

   // get values
   // load basic frame data
   LoadControlFrame( elem, frame );

   if( elem->elem->Attribute( "keyscrollamount" ) )
   {
      frame->SetKeyScrollAmount( atof(elem->elem->Attribute( "keyscrollamount" )) );
   }

   if( elem->elem->Attribute( "viewwidth" ) )
   {
      width = atof(elem->elem->Attribute( "viewwidth" ));
   }

   if( elem->elem->Attribute( "viewheight" ) )
   {
      height = atof(elem->elem->Attribute( "viewheight" ));
   }
   frame->SetView( width, height );

   elem->data= frame;

}
void UIDrawable::LoadControlDraggableFrame( ELEMDATA *elem )
{
   CUI_DraggableFrame *frame = new CUI_DraggableFrame();

   // get values
   // load basic frame data
   LoadControlFrame( elem, frame );

   if( elem->elem->Attribute( "cliptoparent" ) )
   {
      frame->SetClipToParent( atoi(elem->elem->Attribute( "cliptoparent" ))==0 ? false : true );
   }

   elem->data= frame;
}

void UIDrawable::LoadControlFrame( ELEMDATA *elem, CUI_Frame *frame )
{
   CUI_Frame *newframe = frame;

   if( newframe == NULL )
   {
      newframe = new CUI_Frame();
   }

   // get values
   if( elem->elem->Attribute( "frameid" ) )
   {
      newframe->SetFrameID( atoi(elem->elem->Attribute( "frameid" )) );
   }

   if( elem->elem->Attribute( "shader" ) )
   {
      newframe->SetShader( mUI->GetShader( elem->elem->Attribute( "shader" )) );
   }

   if( elem->elem->Attribute( "coordsys" ) )
   {
      newframe->SetCoordSys( mUI->GetCoordSys(elem->elem->Attribute( "coordsys" )) );
   }

   if( elem->elem->Attribute( "tooltiptext" ) )
   {
      newframe->SetToolTipText( (char*)elem->elem->Attribute( "tooltiptext" ) );
   }

   if( elem->elem->Attribute( "tooltipframe" ) )
   {
      //		newframe->SetToolTipnewframe( valueul );
   }

   elem->data= newframe;
}

void UIDrawable::LoadResourceFBMFont( ELEMDATA *elem )
{
   int w=16, h=24;

   CUI_FixedBMFont *font = new CUI_FixedBMFont();

   mUI->AddFont( elem->elem->Attribute( "name" ), font );
   font->SetOwnedByUI();

   if( elem->elem->Attribute( "shader" ) )
   {
      font->SetShader( mUI->GetShader(elem->elem->Attribute( "shader" )) );
   }

   if( elem->elem->Attribute( "characterwidth" ) )
   {
      w = atoi(elem->elem->Attribute( "characterwidth" ));
   }

   if( elem->elem->Attribute( "characterheight" ) )
   {
      h = atoi(elem->elem->Attribute( "characterheight" ));
   }

   font->SetCharSize( w, h );

   // default bitmap res
   w = 256;
   h = 256;

   if( elem->elem->Attribute( "bitmapwidth" ) )
   {
      w = atoi(elem->elem->Attribute( "bitmapwidth" ));
   }

   if( elem->elem->Attribute( "bitmapheight" ) )
   {
      h = atoi(elem->elem->Attribute( "bitmapheight" ));
   }

   font->SetBitmapResolution( w, h );

   if( elem->elem->Attribute("charactermapoffset" ) )
   {
      font->SetOffset( atoi(elem->elem->Attribute("charactermapoffset" )) );
   }

   elem->data= font;
}


// Load a coordinate system
void UIDrawable::LoadResourceCoordSys( ELEMDATA *elem)
{
   char *value = NULL;

   CUI_CoordSys *coordsys = new CUI_CoordSys();

   // have to add it no matter what.
   std::string name = elem->elem->Attribute("name");
   mUI->AddCoordSys(name.c_str(), coordsys );
   coordsys->SetOwnedByUI();

   elem->data = coordsys;
}

void UIDrawable::ParseElement(TiXmlElement *elem)
{
   if (!elem) return;
   ELEMDATA *newelement;

   if( elem->Value() == std::string("rect") ||
      elem->Value() == std::string("range") ||
      elem->Value() == std::string("viewrange") )
   {
      newelement = new ELEMDATA;
      newelement->elem = elem;
      LoadDataRect(newelement);
      m_elementStack.push(newelement);
   }
   else if(elem->Value() == std::string("point2d"))
   {
      newelement = new ELEMDATA;
      newelement->elem = elem;
      LoadDataPoint2d( newelement );
      m_elementStack.push(newelement);
   }
   else if( elem->Value() == std::string("state") )
   {
      newelement = new ELEMDATA;
      newelement->elem = elem;
      LoadDataState( newelement );
      m_elementStack.push( newelement );
   }
   else if( elem->Value() == std::string("listitem") )
   {
      newelement = new ELEMDATA;
      newelement->elem = elem;
      LoadDataListItem( newelement );
      m_elementStack.push( newelement );
   }
   else if( elem->Value() == std::string("menuitem") )
   {
      newelement = new ELEMDATA;
      newelement->elem = elem;
      LoadDataMenuItem( newelement );
      m_elementStack.push( newelement );
   }
   // RADIO ITEM
   else if( elem->Value() == std::string("radioitem") )
   {
      newelement = new ELEMDATA;
      newelement->elem = elem;
      LoadDataRadioItem( newelement );
      m_elementStack.push( newelement );
   }
   else if( elem->Value() == std::string("polyedgeborder") )
   {
      newelement = new ELEMDATA;
      newelement->elem = elem;
      LoadResourcePolyBorder( newelement );
      m_elementStack.push( newelement );
   }
   // COORDINATE SYSTEM
   else if( elem->Value() == std::string("coordsys") )
   {
      newelement = new ELEMDATA;
      newelement->elem = elem;
      LoadResourceCoordSys( newelement );
      m_elementStack.push( newelement );
   }
   // FIXED WIDTH BITMAP FONT
   else if( elem->Value() == std::string("fixedwidthbitmapfont") )
   {
      newelement = new ELEMDATA;
      newelement->elem = elem;
      LoadResourceFBMFont( newelement );
      m_elementStack.push( newelement );
   }
   // ROOT FRAME
   else if( elem->Value() == std::string("rootframe") )
   {
      newelement = new ELEMDATA;
      newelement->elem = elem;
      LoadControlFrame( newelement );
      //newelement->GetValue( "name", &(newelement->m_objectName) );
      m_elementStack.push( newelement );	
   }
   // FRAME
   else if( elem->Value() == std::string("frame") )
   {
      newelement = new ELEMDATA;
      newelement->elem = elem;
      LoadControlFrame( newelement );
      m_elementStack.push( newelement );
   }
   // DRAGGABLE FRAME
   else if( elem->Value() == std::string("draggableframe") )
   {
      dtCore::Notify(dtCore::DEBUG_INFO,"Starting draggableframe");
      newelement = new ELEMDATA;
      newelement->elem = elem;
      LoadControlDraggableFrame( newelement );
      m_elementStack.push( newelement );
   }
   // DRAGGABLE VIEW FRAME
   else if( elem->Value() == std::string("draggableviewframe") )
   {
      newelement = new ELEMDATA;
      newelement->elem = elem;
      LoadControlDraggableViewFrame( newelement );
      m_elementStack.push( newelement );
   }
   // TEXT LABEL
   else if( elem->Value() == std::string("textlabel") )
   {
      newelement = new ELEMDATA;
      newelement->elem = elem;
      LoadControlTextLabel( newelement );
      m_elementStack.push( newelement );
   }
   // BUTTON
   else if( elem->Value() == std::string("button") )
   {
      newelement = new ELEMDATA;
      newelement->elem = elem;
      LoadControlButton( newelement );
      m_elementStack.push( newelement );
   }
   // RESIZE BUTTON
   else if( elem->Value() == std::string("resizeparentbutton") )
   {
      newelement = new ELEMDATA;
      newelement->elem = elem;
      LoadControlResizeButton( newelement );
      m_elementStack.push( newelement );
   }
   // PROGRESS BAR
   else if( elem->Value() == std::string("progressbar") )
   {
      newelement = new ELEMDATA;
      newelement->elem = elem;
      LoadControlProgressBar( newelement );
      m_elementStack.push( newelement );
   }
   // SLIDER BAR
   else if( elem->Value() == std::string("sliderbar") )
   {
      newelement = new ELEMDATA;
      newelement->elem = elem;
      LoadControlSliderBar( newelement );
      m_elementStack.push( newelement );
   }
   // TEXT BOX
   else if( elem->Value() == std::string("textbox") )
   {
      newelement = new ELEMDATA;
      newelement->elem = elem;
      LoadControlTextBox( newelement );
      m_elementStack.push( newelement );
   }
   // EDITABLE TEXT BOX
   else if( elem->Value() == std::string("edittextbox") )
   {
      newelement = new ELEMDATA;
      newelement->elem = elem;
      LoadControlEditTextBox( newelement );
      m_elementStack.push( newelement );
   }
   // LIST BOX
   else if( elem->Value() == std::string("listbox") )
   {
      newelement = new ELEMDATA;
      newelement->elem = elem;
      LoadControlListBox( newelement );
      m_elementStack.push( newelement );
   }
   // COMBO BOX
   else if( elem->Value() == std::string("combobox") )
   {
      newelement = new ELEMDATA;
      newelement->elem = elem;
      LoadControlComboBox( newelement );
      m_elementStack.push( newelement );
   }
   // MENU
   else if( elem->Value() == std::string("menu") )
   {
      newelement = new ELEMDATA;
      newelement->elem = elem;
      LoadControlMenu( newelement );
      m_elementStack.push( newelement );
   }
   // RADIO GROUP
   else if( elem->Value() == std::string("radiogroup") )
   {
      newelement = new ELEMDATA;
      newelement->elem = elem;
      LoadControlRadioGroup( newelement );
      m_elementStack.push( newelement );
   }
   // SCROLLABLE FRAME
   else if(elem->Value() == std::string("scrollframe") )
   {
      newelement = new ELEMDATA;
      newelement->elem = elem;
      LoadControlScrollFrame( newelement );
      m_elementStack.push( newelement );
   }
   // SCROLLABLE TEXT BOX
   else if( elem->Value() == std::string("scrolltextbox") )
   {
      newelement = new ELEMDATA;
      newelement->elem = elem;
      LoadControlScrollTextBox( newelement );
      m_elementStack.push( newelement );
   }
   else if ( elem->Value() == std::string("shader") )
   {
      newelement = new ELEMDATA;
      newelement->elem = elem;
      LoadResourceShader( newelement );
      m_elementStack.push( newelement );
   }
   else if ( elem->Value() == std::string("font") )
   {

   }
   else
   {
      dtCore::Notify(dtCore::WARN, "Parse unknown element: '%s'",
         elem->Value());
      ELEMDATA *obj = new ELEMDATA;
      obj->elem = elem;
      obj->data = NULL;
      m_elementStack.push(obj);
   }


   //Parse this element's children recursively
   TiXmlNode *child = 0;
   while( child = elem->IterateChildren( child ) )
   {
      ParseElement(child->ToElement());
   }

   EndElement();
}

void UIDrawable::EndElement()
{
   float *tempf;
   ELEMDATA *parent = NULL;
   IUI_Shader **shaders;

   ELEMDATA *elem = m_elementStack.top();
   m_elementStack.pop();

   if (elem->elem == NULL) return;

   if (!m_elementStack.empty())
   {
      parent = m_elementStack.top();
   }

   if (elem->elem->Value() == std::string("rect"))
   {
      tempf = (float*)elem->data;      

      if ((parent->elem->Value() == std::string("frame"))     ||
         (parent->elem->Value() == std::string("rootframe")) ||
         (parent->elem->Value() == std::string("draggableframe")) ||
         (parent->elem->Value() == std::string("sliderbar")) ||
         (parent->elem->Value() == std::string("resizeparentbutton")) ||
         (parent->elem->Value() == std::string("textlabel")) ||
         (parent->elem->Value() == std::string("edittextbox")) ||
         (parent->elem->Value() == std::string("textbox")) ||
         (parent->elem->Value() == std::string("button")) ||
         (parent->elem->Value() == std::string("listbox")) ||
         (parent->elem->Value() == std::string("combobox")) ||
         (parent->elem->Value() == std::string("menu")) ||
         (parent->elem->Value() == std::string("radiogroup")) ||
         (parent->elem->Value() == std::string("scrollframe")) ||
         (parent->elem->Value() == std::string("scrolltextbox")) ||
         (parent->elem->Value() == std::string("scrolltext")) )
      {
         CUI_Frame *fr = (CUI_Frame*)parent->data;
         fr->Move( tempf[0], tempf[1], tempf[2], tempf[3] );
      }
      else if (parent->elem->Value() == std::string("coordsys"))
      {
         if (elem->elem->Value() == std::string("range"))
         {
            ((CUI_CoordSys*)(parent->data))->SetCoord( tempf[0], tempf[1], tempf[2], tempf[3] );
         }
         else if(elem->elem->Value() == std::string("viewrange"))
         {
            ((CUI_CoordSys*)(parent->data))->SetViewRange( tempf[0], tempf[1], tempf[2], tempf[3] );
         }
      }
   }
   else if (elem->elem->Value() == std::string("point2d"))
   {
      tempf = (float*)elem->data;
      if ((parent->elem->Value() == std::string("polyedgeborder")) ||
         (parent->elem->Value() == std::string("border")))
      {
         ((IUI_Border*)(parent->data))->AddPoint(tempf);
      }
   }
   else if (elem->elem->Value() == std::string("state"))
   {
      shaders = (IUI_Shader**)elem->data;
      if (parent->elem->Value() == std::string("button"))
      {
         ((CUI_MultipleStateButton*)parent->data)->AddState(
            shaders[0], shaders[1], shaders[2], shaders[3] );
      }
   }
   else if (elem->elem->Value() == std::string("listitem"))
   {
      if (parent->elem->Value() == std::string("listbox"))
      {
         ((CUI_ListBox*)(parent->data))->AddItem( ((CUI_ListItem*)(elem->data)) );
      }
      else if (parent->elem->Value() == std::string("combobox"))
      {
         ((CUI_ComboBox*)(parent->data))->AddItem( ((CUI_ListItem*)(elem->data)) );
      }
   }
   else if (elem->elem->Value() == std::string("menuitem"))
   {
      if (parent->elem->Value() == std::string("menu"))
      {
         ((CUI_Menu*)(parent->data))->AddItem( ((CUI_MenuItem*)(elem->data)) );
      }
   }
   else if (elem->elem->Value() == std::string("radioitem"))
   {
      if (parent->elem->Value() == std::string("radiogroup"))
      {
         ((CUI_RadioGroup*)(parent->data))->AddItem( ((CUI_RadioItem*)(elem->data)) );
      }
   }
   else if (elem->elem->Value() == std::string("rootframe"))
   {
      // owned by ui
      ((CUI_Frame*)elem->data)->SetOwnedByUI();
      // add to ui
      std::string name = elem->elem->Attribute("name");
      mUI->AddRootFrame(name.c_str(), (CUI_Frame*)(elem->data) );
      // add to ui
      //mUI->AddFrame( (CUI_Frame*)elem->data );
   }
   else if ((elem->elem->Value() == std::string("button")) ||
      (elem->elem->Value() == std::string("textbox")) ||
      (elem->elem->Value() == std::string("edittextbox")) ||
      (elem->elem->Value() == std::string("textlabel")) ||
      (elem->elem->Value() == std::string("draggableframe")) ||
      (elem->elem->Value() == std::string("resizeparentbutton")) ||
      (elem->elem->Value() == std::string("sliderbar")) ||
      (elem->elem->Value() == std::string("frame") ))
   {
      // add to parent
      ((CUI_Frame*)(parent->data))->AddChild( (CUI_Frame*)elem->data, true );
      // set owned by UI
      ((CUI_Frame*)(elem->data))->SetOwnedByUI();
      // add to ui
      mUI->AddFrame( (CUI_Frame*)elem->data );
   }
   else if (elem->elem->Value() == std::string("listbox"))
   {
      if (parent->elem->Value() == std::string("scrollframe"))
      {
         ((CUI_ScrollableFrame*)(parent->data))->AddChild( ((CUI_ListBox*)(elem->data)) );
      }
      else
      {
         // add to parent
         //  might be a submenu - which has no parent
         if( parent->elem->Value() )
         {
            ((CUI_Frame*)(parent->data))->AddChild( (CUI_Frame*)elem->data, true );
         }
         // set owned by UI
         ((CUI_Frame*)(elem->data))->SetOwnedByUI();
      }
   }
   else if ((elem->elem->Value() == std::string("combobox")) ||
      (elem->elem->Value() == std::string("menu")) ||
      (elem->elem->Value() == std::string("radiogroup")) ||
      (elem->elem->Value() == std::string("scrollframe")) ||
      (elem->elem->Value() == std::string("scrolltextbox")) ||
      (elem->elem->Value() == std::string("scrolltext") ))
   {
      // add to parent
      //  might be a submenu - which has no parent
      if( parent->elem->Value() )
      {
         ((CUI_Frame*)(parent->data))->AddChild( (CUI_Frame*)elem->data, true );
      }
      // set owned by UI
      ((CUI_Frame*)(elem->data))->SetOwnedByUI();
   }
   else
   {
      //dtCore::Notify(dtCore::WARN, "End element unknown type '%s'",
         //elem->elem->Value()); 
   }

}

/** Set the resolution of the DeltaWin this UI is being displayed in.
  * @param w : width in pixels
  * @param h : height in pixels
  */
void UIDrawable::SetWindowResolution(const int w, const int h)
{
   if (!mUI) return;
   mUI->SetResolution(w, h);
}

/** Create a new Shader with the given name and color.
* @param name : the name of this Shader
* @param color : the RGBA color
*/
void UIDrawable::CreateShader(std::string name)
{
   mUI->AddShader( name.c_str(), new CUI_OpenGLShader() );
}

/** Create a new Shader with the given name and color.
 * @param name : the name of this Shader
 * @param color : the RGBA color
 */
void UIDrawable::CreateShader(std::string name, sgVec4 color)
{
   mUI->AddShader( name.c_str(), new CUI_OpenGLShader(color) );
}

/** Create a new Shader with the given name and texture.
  * @param name : the name of this Shader
  * @param textureFilename : The texture file to load.  This will use the search
  *                          paths to find the file.
  * @see: dtCore::SetDataFilePathList
  */
void UIDrawable::CreateShader(std::string name, std::string textureFilename)
{
   mUI->AddShader( name.c_str(), 
      new CUI_OpenGLShader(
      mRenderer->LoadTexture((char*)textureFilename.c_str())) );
}

/** Create a new Shader with the given name, texture, and color.  This Shader
  * will blend the texture with the color.
* @param name : the name of this Shader
* @param color : the RGBA color
* @param textureFilename : The texture file to load.  This will use the search
*                          paths to find the file.
* @see: dtCore::SetDataFilePathList
*/
void UIDrawable::CreateShader(std::string name, sgVec4 color, std::string textureFilename)
{
   mUI->AddShader( name.c_str(),
      new CUI_OpenGLShader(mRenderer->LoadTexture((char*)textureFilename.c_str()),
      color ));
}


/** Create a new Border with the given name, 2D coordinates, and the number
 *  of coordinates.
 * @param name : the name of this Shader
 * @param coords : The Array of 2D coordinates (arranged counterclockwise)
 * @param numCoords : The number of coordinates in the array
 */
void UIDrawable::CreateBorder(std::string name, sgVec2 *coords, const int numCoords)
{
   mUI->AddBorder(name.c_str(), new CUI_PolyEdgeBorder(coords, numCoords));
}

/** Create a bitmapped fixed width font using the supplied parameters.  
  * This Font will
  * calculate the position of each character on the Shader.
  * @param name : The name of this Shader
  * @param shader : The name of the Shader (textured) to use as the font
  * @param charW: The width of the character (pixels)
  * @param charH: The height of the character (pixels)
  * @param xres : The x resolution of the bitmap (pixels)
  * @param yres : The y resolution of the bitmap (pixels)
  * @param startoffset  : Character offset from "!" that the font starts with
  */
void UIDrawable::CreateFixedFont(std::string name, std::string shader,
                              int charW, int charH, int xres, int yres,
                              int startoffset)
{
   IUI_Font *font = new CUI_FixedBMFont( mUI->GetShader(shader.c_str()),
                        charW, charH, xres, yres, startoffset );
   mUI->AddFont(name.c_str(), font);
}

/** Add a Root Frame to this UI.  There must be at least one Root Frame in the
  * UI.
  */
void UIDrawable::AddRootFrame( std::string name, CUI_Frame *rootFrame )
{
   if (!rootFrame->GetShader()) rootFrame->SetShader(GetShader("default"));

   mUI->AddRootFrame(name.c_str(), rootFrame);
}


/** Attach a callback function to this Root Frame.  This function will get
 *  called whenever there is an action on any child elements of this 
 *  root frame.
 * @param rootFrameName : The name of the Root Frame to add the callback to
 * @param func : The static function to use
 */
void UIDrawable::SetCallbackFunc(std::string rootFrameName, CUI_UI::callbackfunc func)
{
   mUI->SetCallbackFunc(rootFrameName.c_str(), func);
}

/** Add a CUI_Frame to this UI.  If the CUI_Frame doesn't already have a 
 *  Shader assigned to it, a default one will be applied.
 */
void UIDrawable::AddFrame( CUI_Frame *frame )
 {
//    if (IS_A(frame,CUI_SliderBar*)) //<<Needs RTTI for this
//    {
//      ((CUI_SliderBar*)(frame))->SetBarShader( GetShader("defaultHi"));
//    }

   if (!frame->GetShader()) frame->SetShader(GetShader("default"));

    mUI->AddFrame(frame);
 }

 void UIDrawable::SetupDefaults(void)
 {
    mUI->AddCoordSys( "default", new CUI_CoordSys() );

    sgVec4 def_col = { 0.75, 0.75, 0.75, 1.0 };
    CreateShader("default", def_col);

    sgVec4 hi_col = { 0.9, 0.9, 0.9, 1.0 };
    CreateShader("defaultHi", hi_col);

    sgVec4 lo_col = { 0.55, 0.55, 0.55, 1.0 };
    CreateShader("defaultLo", lo_col);

    sgVec2 defborder[] = { {0,0}, {1,0}, {1,1}, {0,1} };
    CreateBorder( "default", defborder, 4 );
 }

