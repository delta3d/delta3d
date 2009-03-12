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

#ifndef KEYBOARDMOUSEHANDLER
#define KEYBOARDMOUSEHANDLER

#include <osgGA/GUIEventHandler>

#include <dtCore/observerptr.h>
#include <dtCore/refptr.h>
#include <dtCore/view.h>
#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>

namespace dtCore
{

   ///Routes the OSG keyboard/mouse input events to the internal
   ///Mouse and Keyboard instances.
   class KeyboardMouseHandler : public osgGA::GUIEventHandler
   {
      public:
   
         KeyboardMouseHandler(dtCore::View* view);
         KeyboardMouseHandler(Keyboard* keyboard, Mouse* mouse);
   
         virtual bool handle(const osgGA::GUIEventAdapter& ea, 
                             osgGA::GUIActionAdapter& aa, osg::Object*,
                             osg::NodeVisitor *);
   
         void SetKeyboard(Keyboard* kb);
         Keyboard* GetKeyboard();
         const Keyboard* GetKeyboard() const;
   
         void SetMouse(Mouse* m);
         
         Mouse* GetMouse();
         const Mouse* GetMouse() const;
            
      protected:
         virtual ~KeyboardMouseHandler();
   
         friend class View;
   
         /// define the mView
         void SetView(dtCore::View* view);
           
      private:
   
         ObserverPtr<View> mView; // owner view of this instance
   
         KeyboardMouseHandler(); ///< not implemented by design
         KeyboardMouseHandler& operator=(const KeyboardMouseHandler&);
         KeyboardMouseHandler(const KeyboardMouseHandler&); ///< not implemented by design
   
         RefPtr<Keyboard> mKeyboard;
         RefPtr<Mouse> mMouse;
   };

}

#endif /*KEYBOARDMOUSEHANDLER_H_*/
