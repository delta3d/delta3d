// guimgr.h: interface for the GUI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUIMGR_H__88197C25_4C00_4251_9F90_21565007F1F7__INCLUDED_)
#define AFX_GUIMGR_H__88197C25_4C00_4251_9F90_21565007F1F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "OpenThreads/thread"
#include "gui_fltk.h"
#include "base.h"
#include "notify.h"
#include "scene.h"


///Creates and manages the UserInterface


/** This class creates and manages a 2D GUI that can interact dynamically with 
  * most dtCore instantiations.
  * Currently, GUI must be instantiated after all other dtCore classes have been
  * created.  The interface will pickup all classes created at that point and
  * render the user interface.
  * NOTE: any instantiated classes that change values during runtime will not
  * reflect their new values on the GUI.
  */
class GUI : public OpenThreads::Thread
{
public:  
   GUI()
   {
      ui = new UserInterface();
      win = ui->make_window();
      Init();
      startThread();
   }
   ~GUI() {}

   void Show( bool show=true)
   {
      if (show) ui->MainWindow->show();
      else ui->MainWindow->hide();
   }

   bool IsShown(void) 
   {
      if (ui->MainWindow->shown()) return true;
      else return false;
   }

private:
   void Init(void)
   {
      ui->TransformGroup->hide();
      ui->ObjectGroup->hide();
      ui->CameraGroup->hide();
      
      for (int i=0; i<dtCore::Base::GetInstanceCount(); i++)
      {
         dtCore::Base *o = dtCore::Base::GetInstance(i);
         ui->InstanceList->add( o->GetName().c_str(), o);
      }
      

      ui->InstanceList->select(1);
      ui->SelectInstance();
   }
   
   virtual void run() 
   { 
      //show the window until it closes, then quit the thread
      ui->MainWindow->show();
      Fl::run(); //blocking call
      cancel(); //stop the thread
   }
private:
   Fl_Window *win;
   UserInterface *ui;
};

//
// Automatic library inclusion macros that use the #pragma/lib feature
//
#undef _AUTOLIBNAME
#if defined(_DEBUG)
   #define _AUTOLIBNAME  "gui_fld.lib"
#else
   #define _AUTOLIBNAME  "gui_fl.lib"
#endif

#ifndef _NOAUTOLIBMSG
   #pragma message( "Will automatically link with " _AUTOLIBNAME )
#endif

#pragma comment(lib, _AUTOLIBNAME)


#endif // !defined(AFX_GUIMGR_H__88197C25_4C00_4251_9F90_21565007F1F7__INCLUDED_)
