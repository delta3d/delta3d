// widgetbindings.cpp: Widget binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtABC/widget.h>

using namespace boost::python;
using namespace dtABC;
using namespace dtCore;

class WidgetWrap : public Widget
{
   public:

      WidgetWrap(PyObject* self, std::string name = "Widget")
         : Widget(name),
           mSelf(self)
      {}

      virtual void Config(const WinData* data)
      {
         call_method<void>(mSelf, "Config", data);
      }
      
      void DefaultConfig(const WinData* data)
      {
         Widget::Config(data);
      }
      
      virtual void Quit()
      {
         call_method<void>(mSelf, "Quit");
      }
      
      void DefaultQuit()
      {
         Widget::Quit();
      }
      
   protected:
      
      virtual void PreFrame(const double deltaFrameTime)
      {
         if(PyObject_HasAttrString(mSelf, "PreFrame"))
         {
            call_method<void>(mSelf, "PreFrame", deltaFrameTime);
         }
         else
         {
            Widget::PreFrame(deltaFrameTime);
         }
      }
      
      virtual void Frame(const double deltaFrameTime)
      {
         if(PyObject_HasAttrString(mSelf, "Frame"))
         {
            call_method<void>(mSelf, "Frame", deltaFrameTime);
         }
         else
         {
            Widget::Frame(deltaFrameTime);
         }
      }
      
      virtual void PostFrame(const double deltaFrameTime)
      {
         if(PyObject_HasAttrString(mSelf, "PostFrame"))
         {
            call_method<void>(mSelf, "PostFrame", deltaFrameTime);
         }
         else
         {
            Widget::PostFrame(deltaFrameTime);
         }
      }
      
   private:
      
      PyObject* mSelf;
};

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(C_overloads, Config, 0, 1)

#if defined(__APPLE__)
BOOST_PYTHON_OPAQUE_SPECIALIZED_TYPE_ID(OpaqueWindowPtr)
#endif

void SendEmptyMessage(Base* base, std::string message)
{
   base->SendMessage(message);
}

void SendMouseMessage(Base* base, std::string message, MouseEvent* userData)
{
   base->SendMessage(message, userData);
}

void SendKeyboardMessage(Base* base, std::string message, KeyboardEvent* userData)
{
   base->SendMessage(message, userData);
}

void SendResizeMessage(Base* base, std::string message, WinRect* userData)
{
   base->SendMessage(message, userData);
}

void SendWindowDataMessage(Base* base, std::string message, WinData* userData)
{
   base->SendMessage(message, userData);
}

// Windows needs the long to HWND* conversion done ahead of time
#ifdef WIN32
void SetWinDataHWND(WinData* winData, long hwnd)
{
   winData->hwnd = HWND(hwnd);
}

long GetWinDataHWND(WinData* winData)
{
   return long(winData->hwnd);
}
#endif

void initWidgetBindings()
{
   Widget* (*WidgetGI1)(int) = &Widget::GetInstance;
   Widget* (*WidgetGI2)(std::string) = &Widget::GetInstance;

   class_<Widget, bases<BaseABC>, dtCore::RefPtr<WidgetWrap>, boost::noncopyable>("Widget", init<optional<std::string> >())
      .def("GetInstanceCount", &Widget::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", WidgetGI1, return_internal_reference<>())
      .def("GetInstance", WidgetGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def_readonly("msgRedraw", &Widget::msgRedraw)
      .def_readonly("msgStopped", &Widget::msgStopped)
      .def_readonly("msgStep", &Widget::msgStep)
      .def_readonly("msgMouseEvent", &Widget::msgMouseEvent)
      .def_readonly("msgKeyboardEvent", &Widget::msgKeyboardEvent)
      .def_readonly("msgResize", &Widget::msgResize)
      .def_readonly("msgAddDrawable", &Widget::msgAddDrawable)
      .def_readonly("msgSetPath", &Widget::msgSetPath)
      .def_readonly("msgWindowData", &Widget::msgWindowData)
      .def_readonly("msgQuit", &Widget::msgQuit)
      .def("Config", &Widget::Config, &WidgetWrap::DefaultConfig)
      .def("Quit", &Widget::Quit, &WidgetWrap::DefaultQuit)
      .def("SetPath", &Widget::SetPath)
      .def("SendMessage", SendEmptyMessage)
      .def("SendMessage", SendMouseMessage)
      .def("SendMessage", SendKeyboardMessage)
      .def("SendMessage", SendResizeMessage)
      .def("SendMessage", SendWindowDataMessage);
 
   class_<WinRect>("WinRect", init<optional<int, int, int, int> >())
      .def_readwrite("pos_x", &WinRect::pos_x)
      .def_readwrite("pos_y", &WinRect::pos_y)
      .def_readwrite("width", &WinRect::width)
      .def_readwrite("height", &WinRect::height);

   class_<WinData, bases<WinRect> >("WinData", init<optional<WindowHandle, int, int, int, int> >())
      #ifdef WIN32
      // Windows needs the long to HWND* conversion done ahead of time
      .add_property("hwnd", &GetWinDataHWND, &SetWinDataHWND) 
      #else
      .def_readwrite("hwnd", &WinData::hwnd)
      #endif
      ;
   
   {   
      scope MouseEvent_scope = class_<MouseEvent>("MouseEvent", init<optional<int, float, float, int> >())
         .def_readwrite("event", &MouseEvent::event)
         .def_readwrite("pos_x", &MouseEvent::pos_x)
         .def_readwrite("pos_y", &MouseEvent::pos_y)
         .def_readwrite("button", &MouseEvent::button);
         
      enum_<MouseEvent::Type>("Type")
         .value("NO_EVENT", MouseEvent::NO_EVENT)
         .value("PUSH", MouseEvent::PUSH)
         .value("DOUBLE", MouseEvent::DOUBLE)
         .value("RELEASE", MouseEvent::RELEASE)
         .value("ENTER", MouseEvent::ENTER)
         .value("LEAVE", MouseEvent::LEAVE)
         .value("DRAG", MouseEvent::DRAG)
         .value("MOVE", MouseEvent::MOVE)
         .value("WHEEL_UP", MouseEvent::WHEEL_UP)
         .value("WHEEL_DN", MouseEvent::WHEEL_DN)
         .value("DND_ENTER", MouseEvent::DND_ENTER)
         .value("DND_DRAG", MouseEvent::DND_DRAG)
         .value("DND_LEAVE", MouseEvent::DND_LEAVE)
         .value("DND_RELEASE", MouseEvent::DND_RELEASE)
         .export_values();
   }
   
   {   
      scope KeyboardEvent_scope = class_<KeyboardEvent>("KeyboardEvent", init<optional<int, int, int, char> >())
         .def_readwrite("event", &KeyboardEvent::event)
         .def_readwrite("key", &KeyboardEvent::key)
         .def_readwrite("mod", &KeyboardEvent::mod)
         .def_readwrite("chr", &KeyboardEvent::chr)
         .def_readonly("KEY_BackSpace", &KeyboardEvent::KEY_BackSpace)
         .def_readonly("KEY_Tab", &KeyboardEvent::KEY_Tab)
         .def_readonly("KEY_Enter", &KeyboardEvent::KEY_Enter)
         .def_readonly("KEY_Pause", &KeyboardEvent::KEY_Pause)
         .def_readonly("KEY_Scroll_Lock", &KeyboardEvent::KEY_Scroll_Lock)
         .def_readonly("KEY_Sys_Req", &KeyboardEvent::KEY_Sys_Req)
         .def_readonly("KEY_Escape", &KeyboardEvent::KEY_Escape)
         .def_readonly("KEY_Home", &KeyboardEvent::KEY_Home)
         .def_readonly("KEY_Left", &KeyboardEvent::KEY_Left)
         .def_readonly("KEY_Up", &KeyboardEvent::KEY_Up)
         .def_readonly("KEY_Right", &KeyboardEvent::KEY_Right)
         .def_readonly("KEY_Down", &KeyboardEvent::KEY_Down)
         .def_readonly("KEY_Page_Up", &KeyboardEvent::KEY_Page_Up)
         .def_readonly("KEY_Page_Down", &KeyboardEvent::KEY_Page_Down)
         .def_readonly("KEY_End", &KeyboardEvent::KEY_End)
         .def_readonly("KEY_Print", &KeyboardEvent::KEY_Print)
         .def_readonly("KEY_Insert", &KeyboardEvent::KEY_Insert)
         .def_readonly("KEY_Menu", &KeyboardEvent::KEY_Menu)
         .def_readonly("KEY_Help", &KeyboardEvent::KEY_Help)
         .def_readonly("KEY_Num_Lock", &KeyboardEvent::KEY_Num_Lock)
         .def_readonly("KEY_KP_Enter", &KeyboardEvent::KEY_KP_Enter)
         .def_readonly("KEY_Shift_L", &KeyboardEvent::KEY_Shift_L)
         .def_readonly("KEY_Shift_R", &KeyboardEvent::KEY_Shift_R)
         .def_readonly("KEY_Control_L", &KeyboardEvent::KEY_Control_L)
         .def_readonly("KEY_Control_R", &KeyboardEvent::KEY_Control_R)
         .def_readonly("KEY_Caps_Lock", &KeyboardEvent::KEY_Caps_Lock)
         .def_readonly("KEY_Meta_L", &KeyboardEvent::KEY_Meta_L)
         .def_readonly("KEY_Meta_R", &KeyboardEvent::KEY_Meta_R)
         .def_readonly("KEY_Alt_L", &KeyboardEvent::KEY_Alt_L)
         .def_readonly("KEY_Alt_R", &KeyboardEvent::KEY_Alt_R)
         .def_readonly("KEY_Delete", &KeyboardEvent::KEY_Delete)
         .def_readonly("KEY_Space", &KeyboardEvent::KEY_Space)
         .def_readonly("KEY_Exclam", &KeyboardEvent::KEY_Exclam)
         .def_readonly("KEY_DblQuote", &KeyboardEvent::KEY_DblQuote)
         .def_readonly("KEY_NumSign", &KeyboardEvent::KEY_NumSign)
         .def_readonly("KEY_Dollar", &KeyboardEvent::KEY_Dollar)
         .def_readonly("KEY_Percent", &KeyboardEvent::KEY_Percent)
         .def_readonly("KEY_Ampersand", &KeyboardEvent::KEY_Ampersand)
         .def_readonly("KEY_Apostrophe", &KeyboardEvent::KEY_Apostrophe)
         .def_readonly("KEY_Paren_L", &KeyboardEvent::KEY_Paren_L)
         .def_readonly("KEY_Paren_R", &KeyboardEvent::KEY_Paren_R)
         .def_readonly("KEY_Asterisk", &KeyboardEvent::KEY_Asterisk)
         .def_readonly("KEY_Plus", &KeyboardEvent::KEY_Plus)
         .def_readonly("KEY_Comma", &KeyboardEvent::KEY_Comma)
         .def_readonly("KEY_Minus", &KeyboardEvent::KEY_Minus)
         .def_readonly("KEY_Period", &KeyboardEvent::KEY_Period)
         .def_readonly("KEY_Slash", &KeyboardEvent::KEY_Slash)
         .def_readonly("KEY_0", &KeyboardEvent::KEY_0)
         .def_readonly("KEY_1", &KeyboardEvent::KEY_1)
         .def_readonly("KEY_2", &KeyboardEvent::KEY_2)
         .def_readonly("KEY_3", &KeyboardEvent::KEY_3)
         .def_readonly("KEY_4", &KeyboardEvent::KEY_4)
         .def_readonly("KEY_5", &KeyboardEvent::KEY_5)
         .def_readonly("KEY_6", &KeyboardEvent::KEY_6)
         .def_readonly("KEY_7", &KeyboardEvent::KEY_7)
         .def_readonly("KEY_8", &KeyboardEvent::KEY_8)
         .def_readonly("KEY_9", &KeyboardEvent::KEY_9)
         .def_readonly("KEY_Colon", &KeyboardEvent::KEY_Colon)
         .def_readonly("KEY_Semicolon", &KeyboardEvent::KEY_Semicolon)
         .def_readonly("KEY_Less", &KeyboardEvent::KEY_Less)
         .def_readonly("KEY_Equal", &KeyboardEvent::KEY_Equal)
         .def_readonly("KEY_Greater", &KeyboardEvent::KEY_Greater)
         .def_readonly("KEY_Question", &KeyboardEvent::KEY_Question)
         .def_readonly("KEY_At", &KeyboardEvent::KEY_At)
         .def_readonly("KEY_A", &KeyboardEvent::KEY_A)
         .def_readonly("KEY_B", &KeyboardEvent::KEY_B)
         .def_readonly("KEY_C", &KeyboardEvent::KEY_C)
         .def_readonly("KEY_D", &KeyboardEvent::KEY_D)
         .def_readonly("KEY_E", &KeyboardEvent::KEY_E)
         .def_readonly("KEY_F", &KeyboardEvent::KEY_F)
         .def_readonly("KEY_G", &KeyboardEvent::KEY_G)
         .def_readonly("KEY_H", &KeyboardEvent::KEY_H)
         .def_readonly("KEY_I", &KeyboardEvent::KEY_I)
         .def_readonly("KEY_J", &KeyboardEvent::KEY_J)
         .def_readonly("KEY_K", &KeyboardEvent::KEY_K)
         .def_readonly("KEY_L", &KeyboardEvent::KEY_L)
         .def_readonly("KEY_M", &KeyboardEvent::KEY_M)
         .def_readonly("KEY_N", &KeyboardEvent::KEY_N)
         .def_readonly("KEY_O", &KeyboardEvent::KEY_O)
         .def_readonly("KEY_P", &KeyboardEvent::KEY_P)
         .def_readonly("KEY_Q", &KeyboardEvent::KEY_Q)
         .def_readonly("KEY_R", &KeyboardEvent::KEY_R)
         .def_readonly("KEY_S", &KeyboardEvent::KEY_S)
         .def_readonly("KEY_T", &KeyboardEvent::KEY_T)
         .def_readonly("KEY_U", &KeyboardEvent::KEY_U)
         .def_readonly("KEY_V", &KeyboardEvent::KEY_V)
         .def_readonly("KEY_W", &KeyboardEvent::KEY_W)
         .def_readonly("KEY_X", &KeyboardEvent::KEY_X)
         .def_readonly("KEY_Y", &KeyboardEvent::KEY_Y)
         .def_readonly("KEY_Z", &KeyboardEvent::KEY_Z)
         .def_readonly("KEY_Bracket_L", &KeyboardEvent::KEY_Bracket_L)
         .def_readonly("KEY_Backslash", &KeyboardEvent::KEY_Backslash)
         .def_readonly("KEY_Bracket_R", &KeyboardEvent::KEY_Bracket_R)
         .def_readonly("KEY_Caret", &KeyboardEvent::KEY_Caret)
         .def_readonly("KEY_UnderScore", &KeyboardEvent::KEY_UnderScore)
         .def_readonly("KEY_Quote_L", &KeyboardEvent::KEY_Quote_L)
         .def_readonly("KEY_a", &KeyboardEvent::KEY_a)
         .def_readonly("KEY_b", &KeyboardEvent::KEY_b)
         .def_readonly("KEY_c", &KeyboardEvent::KEY_c)
         .def_readonly("KEY_d", &KeyboardEvent::KEY_d)
         .def_readonly("KEY_e", &KeyboardEvent::KEY_e)
         .def_readonly("KEY_f", &KeyboardEvent::KEY_f)
         .def_readonly("KEY_g", &KeyboardEvent::KEY_g)
         .def_readonly("KEY_h", &KeyboardEvent::KEY_h)
         .def_readonly("KEY_i", &KeyboardEvent::KEY_i)
         .def_readonly("KEY_j", &KeyboardEvent::KEY_j)
         .def_readonly("KEY_k", &KeyboardEvent::KEY_k)
         .def_readonly("KEY_l", &KeyboardEvent::KEY_l)
         .def_readonly("KEY_m", &KeyboardEvent::KEY_m)
         .def_readonly("KEY_n", &KeyboardEvent::KEY_n)
         .def_readonly("KEY_o", &KeyboardEvent::KEY_o)
         .def_readonly("KEY_p", &KeyboardEvent::KEY_p)
         .def_readonly("KEY_q", &KeyboardEvent::KEY_w)
         .def_readonly("KEY_r", &KeyboardEvent::KEY_r)
         .def_readonly("KEY_s", &KeyboardEvent::KEY_s)
         .def_readonly("KEY_t", &KeyboardEvent::KEY_t)
         .def_readonly("KEY_u", &KeyboardEvent::KEY_u)
         .def_readonly("KEY_v", &KeyboardEvent::KEY_v)
         .def_readonly("KEY_w", &KeyboardEvent::KEY_w)
         .def_readonly("KEY_x", &KeyboardEvent::KEY_x)
         .def_readonly("KEY_y", &KeyboardEvent::KEY_y)
         .def_readonly("KEY_z", &KeyboardEvent::KEY_z)
         .def_readonly("KEY_Brace_L", &KeyboardEvent::KEY_Brace_L)
         .def_readonly("KEY_Bar", &KeyboardEvent::KEY_Bar)
         .def_readonly("KEY_Brace_R", &KeyboardEvent::KEY_Brace_R)
         .def_readonly("KEY_Tilde", &KeyboardEvent::KEY_Tilde)
         .def_readonly("KEY_KP_Home", &KeyboardEvent::KEY_KP_Home)
         .def_readonly("KEY_KP_Left", &KeyboardEvent::KEY_KP_Left)
         .def_readonly("KEY_KP_Up", &KeyboardEvent::KEY_KP_Up)
         .def_readonly("KEY_KP_Right", &KeyboardEvent::KEY_KP_Right)
         .def_readonly("KEY_KP_Down", &KeyboardEvent::KEY_KP_Down)
         .def_readonly("KEY_KP_PageUp", &KeyboardEvent::KEY_KP_PageUp)
         .def_readonly("KEY_KP_PageDown", &KeyboardEvent::KEY_KP_PageDown)
         .def_readonly("KEY_KP_End", &KeyboardEvent::KEY_KP_End)
         .def_readonly("KEY_KP_Insert", &KeyboardEvent::KEY_KP_Insert)
         .def_readonly("KEY_KP_Delete", &KeyboardEvent::KEY_KP_Delete)
         .def_readonly("KEY_KP_Mul", &KeyboardEvent::KEY_KP_Mul)
         .def_readonly("KEY_KP_Add", &KeyboardEvent::KEY_KP_Add)
         .def_readonly("KEY_KP_Sub", &KeyboardEvent::KEY_KP_Sub)
         .def_readonly("KEY_KP_Dec", &KeyboardEvent::KEY_KP_Dec)
         .def_readonly("KEY_KP_Div", &KeyboardEvent::KEY_KP_Div)
         .def_readonly("KEY_KP_0", &KeyboardEvent::KEY_KP_0)
         .def_readonly("KEY_KP_1", &KeyboardEvent::KEY_KP_1)
         .def_readonly("KEY_KP_2", &KeyboardEvent::KEY_KP_2)
         .def_readonly("KEY_KP_3", &KeyboardEvent::KEY_KP_3)
         .def_readonly("KEY_KP_4", &KeyboardEvent::KEY_KP_4)
         .def_readonly("KEY_KP_5", &KeyboardEvent::KEY_KP_5)
         .def_readonly("KEY_KP_6", &KeyboardEvent::KEY_KP_6)
         .def_readonly("KEY_KP_7", &KeyboardEvent::KEY_KP_7)
         .def_readonly("KEY_KP_8", &KeyboardEvent::KEY_KP_8)
         .def_readonly("KEY_KP_9", &KeyboardEvent::KEY_KP_9)
         .def_readonly("KEY_KP_Equal", &KeyboardEvent::KEY_KP_Equal)
         .def_readonly("KEY_F1", &KeyboardEvent::KEY_F1)
         .def_readonly("KEY_F2", &KeyboardEvent::KEY_F2)
         .def_readonly("KEY_F3", &KeyboardEvent::KEY_F3)
         .def_readonly("KEY_F4", &KeyboardEvent::KEY_F4)
         .def_readonly("KEY_F5", &KeyboardEvent::KEY_F5)
         .def_readonly("KEY_F6", &KeyboardEvent::KEY_F6)
         .def_readonly("KEY_F7", &KeyboardEvent::KEY_F7)
         .def_readonly("KEY_F8", &KeyboardEvent::KEY_F8)
         .def_readonly("KEY_F9", &KeyboardEvent::KEY_F9)
         .def_readonly("KEY_F10", &KeyboardEvent::KEY_F10)
         .def_readonly("KEY_F11", &KeyboardEvent::KEY_F11)
         .def_readonly("KEY_F12", &KeyboardEvent::KEY_F12);
         
      enum_<KeyboardEvent::Type>("Type")
         .value("NO_EVENT", KeyboardEvent::NO_EVENT)
         .value("FOCUS", KeyboardEvent::FOCUS)
         .value("UNFOCUS", KeyboardEvent::UNFOCUS)
         .value("KEYDOWN", KeyboardEvent::KEYDOWN)
         .value("KEYUP", KeyboardEvent::KEYUP)
         .export_values();
         
      enum_<KeyboardEvent::Modifier>("Modifier")
         .value("SHIFT", KeyboardEvent::SHIFT)
         .value("CAPLOCK", KeyboardEvent::CAPLOCK)
         .value("CONTROL", KeyboardEvent::CONTROL)
         .value("ALT", KeyboardEvent::ALT)
         .value("NUMLOCK", KeyboardEvent::NUMLOCK)
         .value("META", KeyboardEvent::META)
         .value("SCROLLLOCK", KeyboardEvent::SCROLLLOCK)
         .export_values();
   }
}
