/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
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
 * Author: Jeff P. Houde
 */
#include <prefix/dtdirectornodesprefix.h>
#include <dtDirectorNodes/mousepressevent.h>

#include <dtCore/actorproxy.h>
#include <dtCore/stringselectoractorproperty.h>

#include <dtABC/application.h>
#include <dtCore/mouse.h>
#include <dtCore/button.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   MyMouseListener::MyMouseListener(MousePressEvent* event)
      : MouseListener()
      , mEvent(event)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool MyMouseListener::HandleButtonPressed(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button)
   {
      if (Check(mouse, button))
      {
         mEvent->Trigger("Pressed");
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool MyMouseListener::HandleButtonReleased(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button)
   {
      if (Check(mouse, button))
      {
         mEvent->Trigger("Released");
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool MyMouseListener::HandleButtonClicked(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button, int clickCount)
   {
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool MyMouseListener::HandleMouseMoved(const dtCore::Mouse* mouse, float x, float y)
   {
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool MyMouseListener::HandleMouseDragged(const dtCore::Mouse* mouse, float x, float y)
   {
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool MyMouseListener::HandleMouseScrolled(const dtCore::Mouse* mouse, int delta)
   {
      std::string checkButton = mEvent->GetString("Button");
      if ((delta > 0 && checkButton == "Wheel Up") ||
         (delta < 0 && checkButton == "Wheel Down"))
      {
         mEvent->Trigger("Pressed");
         mEvent->Trigger("Released");
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool MyMouseListener::Check(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button)
   {
      std::string checkButton = mEvent->GetString("Button");

      if (button == dtCore::Mouse::LeftButton && checkButton == "Left Button")
      {
         return true;
      }

      if (button == dtCore::Mouse::RightButton && checkButton == "Right Button")
      {
         return true;
      }

      if (button == dtCore::Mouse::MiddleButton && checkButton == "Middle Button")
      {
         return true;
      }

      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////////////
   MousePressEvent::MousePressEvent()
       : EventNode()
       , mButton("Left Button")
   {
      AddAuthor("Jeff P. Houde");
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   MousePressEvent::~MousePressEvent()
   {
      if (mListener.valid())
      {
         dtABC::Application* app = dtABC::Application::GetInstance(0);
         if (!app)
         {
            return;
         }

         dtCore::Mouse* mouse = app->GetMouse();
         if (!mouse)
         {
            return;
         }

         mouse->RemoveMouseListener(mListener);
         mListener = NULL;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void MousePressEvent::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      EventNode::Init(nodeType, graph);

      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "Pressed", "Activates when the mouse button has been pressed down."));
      mOutputs.push_back(OutputLink(this, "Released", "Activates when the mouse button has been released."));

      UpdateName();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MousePressEvent::BuildPropertyMap()
   {
      EventNode::BuildPropertyMap();

      dtCore::StringSelectorActorProperty* buttonProp = new dtCore::StringSelectorActorProperty(
         "Button", "Button",
         dtCore::StringSelectorActorProperty::SetFuncType(this, &MousePressEvent::SetButton),
         dtCore::StringSelectorActorProperty::GetFuncType(this, &MousePressEvent::GetButton),
         dtCore::StringSelectorActorProperty::GetListFuncType(this, &MousePressEvent::GetButtonList),
         "Mouse button to listen for.", "");
      AddProperty(buttonProp);

      mValues.push_back(ValueLink(this, buttonProp, false, false, true, false));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MousePressEvent::OnStart()
   {
      dtABC::Application* app = dtABC::Application::GetInstance(0);
      if (!app)
      {
         return;
      }

      dtCore::Mouse* mouse = app->GetMouse();
      if (!mouse)
      {
         return;
      }

      mListener = new MyMouseListener(this);
      mouse->InsertMouseListener(0, mListener);
   }

   //////////////////////////////////////////////////////////////////////////
   bool MousePressEvent::UsesActorFilters()
   {
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MousePressEvent::UpdateName()
   {
      mName = GetString("Button");
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MousePressEvent::SetButton(const std::string& value)
   {
      mButton = value;
      UpdateName();
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string MousePressEvent::GetButton() const
   {
      return mButton;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<std::string> MousePressEvent::GetButtonList() const
   {
      std::vector<std::string> buttonList;

      buttonList.push_back("Left Button");
      buttonList.push_back("Right Button");
      buttonList.push_back("Middle Button");
      buttonList.push_back("Wheel Up");
      buttonList.push_back("Wheel Down");

      return buttonList;
   }
}
