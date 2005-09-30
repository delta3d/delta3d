/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004-2005 MOVES Institute 
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

#ifndef DELTA_INSPECTOR
#define DELTA_INSPECTOR

// inspector.h: interface for the GUI class.
//
//////////////////////////////////////////////////////////////////////

#include <OpenThreads/Thread>
#include <dtInspector/gui_fltk.h>
#include <dtCore/base.h>
#include <dtCore/scene.h>

/** The dtInspector namespace contains functionality to inspect Delta3D classes
  * at runtime.
  */
namespace dtInspector
{
   ///Creates and manages the UserInterface

   /** This class creates and manages a 2D GUI that can interact dynamically with 
   * most dtCore instantiations.
   * Currently, Inspector must be instantiated after all other dtCore classes have been
   * created.  The interface will pickup all classes created at that point and
   * render the user interface.
   * NOTE: any instantiated classes that change values during runtime will not
   * reflect their new values on the Inspector.
   */
   class DT_EXPORT Inspector : public OpenThreads::Thread
   {
   public:  
      Inspector()
      {
         ui = new UserInterface();
         win = ui->make_window();
         Init();
         startThread();
      }
      ~Inspector() {}

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

      //not implemented by design
      Inspector( const Inspector& rhs ); 
      Inspector& operator= ( const Inspector& rhs ); 

      void Init(void)
      {
         ui->TransformGroup->hide();
         ui->DrawableGroup->hide();
         ui->LoadableGroup->hide();
         ui->CameraGroup->hide();
         ui->ParticleGroup->hide();

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

      Fl_Window *win;
      UserInterface *ui;
   };
}//namespace dtInspector

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

//
// Automatic library inclusion macros that use the #pragma/lib feature
//

#ifndef DT_LIBRARY

   #undef _AUTOLIBNAME
   #if defined(_DEBUG)
      #define _AUTOLIBNAME  "dtInspectord.lib"
   #else
      #define _AUTOLIBNAME  "dtInspector.lib"
   #endif

   #ifndef _NOAUTOLIBMSG
      #pragma message( "Will automatically link with " _AUTOLIBNAME )
   #endif

   #pragma comment(lib, _AUTOLIBNAME)
   
#endif

#endif  // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#endif // DELTA_INSPECTOR
