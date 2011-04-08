/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004 MOVES Institute 
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free 
 * Software Foundation; either version 2.1 of the License, or (at your option) 
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more 
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
 *
*/

#ifndef DELTA_GUIMGR
#define DELTA_GUIMGR


// guimgr.h: interface for the GUI class.
//
//////////////////////////////////////////////////////////////////////


#include "OpenThreads/Thread"
#include "gui_fl/gui_fltk.h"
#include "dtCore/base.h"
#include "dtCore/notify.h"
#include "dtCore/scene.h"


///Creates and manages the UserInterface


/** This class creates and manages a 2D GUI that can interact dynamically with 
  * most dtCore instantiations.
  * Currently, GUI must be instantiated after all other dtCore classes have been
  * created.  The interface will pickup all classes created at that point and
  * render the user interface.
  * NOTE: any instantiated classes that change values during runtime will not
  * reflect their new values on the GUI.
  */
class DT_EXPORT GUI : public OpenThreads::Thread
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

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

//
// Automatic library inclusion macros that use the #pragma/lib feature
//

#ifndef DT_LIBRARY

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
   
#endif

#endif  // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#endif // DELTA_GUIMGR