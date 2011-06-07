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
#include <dtDirectorNodes/keypressevent.h>

#include <dtDAL/actorproxy.h>
#include <dtDAL/bitmaskactorproperty.h>
#include <dtDAL/stringselectoractorproperty.h>
#include <dtDAL/booleanactorproperty.h>

#include <dtABC/application.h>
#include <dtCore/keyboard.h>
#include <dtCore/button.h>

#include <osgGA/GUIEventAdapter>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   MyKeyboardListener::MyKeyboardListener(KeyPressEvent* event)
      : KeyboardListener()
      , mEvent(event)
      , mIsActive(false)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool MyKeyboardListener::HandleKeyPressed(const dtCore::Keyboard* keyboard, int keyCode)
   {
      if (!mIsActive && Check(keyboard, keyCode))
      {
         mIsActive = true;
         mEvent->Trigger("Pressed", 0, true, false);
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool MyKeyboardListener::HandleKeyReleased(const dtCore::Keyboard* keyboard, int keyCode)
   {
      if (mIsActive && Check(keyboard, keyCode, false))
      {
         mIsActive = false;
         mEvent->Trigger("Released", 0, true, false);
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool MyKeyboardListener::HandleKeyTyped(const dtCore::Keyboard* keyboard, int keyCode)
   {
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool MyKeyboardListener::Check(const dtCore::Keyboard* keyboard, int keyCode, bool checkMods)
   {
      std::string checkKeyName = mEvent->GetString("Key");

      const dtCore::Button* button = keyboard->GetButton(keyCode);
      if (button)
      {
         // Check if our active key matches our desired event key.
         if (dtUtil::StrCompare(button->GetDescription(), checkKeyName, false) == 0)
         {
            // Check modifiers.
            if (checkMods)
            {
               unsigned int mods = mEvent->GetInt("Modifiers");

               bool controlL  = keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Control_L)->GetState();
               bool controlR  = keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Control_R)->GetState();
               bool altL      = keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Alt_L)->GetState();
               bool altR      = keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Alt_R)->GetState();
               bool shiftL    = keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Shift_L)->GetState();
               bool shiftR    = keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Shift_R)->GetState();

               if ((mods & MOD_CONTROL) == MOD_CONTROL &&
                  (!controlL && !controlR))
               {
                  return false;
               }
               else if ((mods & MOD_LEFT_CONTROL && !controlL) ||
                  (mods & MOD_RIGHT_CONTROL && !controlR))
               {
                  return false;
               }

               if ((mods & MOD_ALT) == MOD_ALT &&
                  (!altL && !altR))
               {
                  return false;
               }
               else if ((mods & MOD_LEFT_ALT && !altL) ||
                  (mods & MOD_RIGHT_ALT && !altR))
               {
                  return false;
               }

               if ((mods & MOD_SHIFT) == MOD_SHIFT &&
                  (!shiftL && !shiftR))
               {
                  return false;
               }
               else if ((mods & MOD_LEFT_SHIFT && !shiftL) ||
                  (mods & MOD_RIGHT_SHIFT && !shiftR))
               {
                  return false;
               }
            }

            return true;
         }
      }

      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////////////
   KeyPressEvent::KeyPressEvent()
       : EventNode()
       , mModifierKeys(0x00)
       , mMainKey("Space")
   {
      AddAuthor("Jeff P. Houde");
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   KeyPressEvent::~KeyPressEvent()
   {
      if (mListener.valid())
      {
         dtABC::Application* app = dtABC::Application::GetInstance(0);
         if (!app)
         {
            return;
         }

         dtCore::Keyboard* keyboard = app->GetKeyboard();
         if (!keyboard)
         {
            return;
         }

         keyboard->RemoveKeyboardListener(mListener);
         mListener = NULL;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void KeyPressEvent::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      EventNode::Init(nodeType, graph);

      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "Pressed"));
      mOutputs.push_back(OutputLink(this, "Released"));

      UpdateName();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void KeyPressEvent::BuildPropertyMap()
   {
      EventNode::BuildPropertyMap();

      dtDAL::BitMaskActorProperty* modsProp = new dtDAL::BitMaskActorProperty(
         "Modifiers", "Modifiers",
         dtDAL::BitMaskActorProperty::SetFuncType(this, &KeyPressEvent::SetModifiers),
         dtDAL::BitMaskActorProperty::GetFuncType(this, &KeyPressEvent::GetModifiers),
         dtDAL::BitMaskActorProperty::GetMaskListFuncType(this, &KeyPressEvent::GetModifierMasks),
         "Modifier keys to listen for along with the main key.");
      AddProperty(modsProp);

      dtDAL::StringSelectorActorProperty* keyProp = new dtDAL::StringSelectorActorProperty(
         "Key", "Key",
         dtDAL::StringSelectorActorProperty::SetFuncType(this, &KeyPressEvent::SetKey),
         dtDAL::StringSelectorActorProperty::GetFuncType(this, &KeyPressEvent::GetKey),
         dtDAL::StringSelectorActorProperty::GetListFuncType(this, &KeyPressEvent::GetKeyList),
         "Key to listen for.", "");
      AddProperty(keyProp);

      mValues.push_back(ValueLink(this, modsProp, false, false, true, false));
      mValues.push_back(ValueLink(this, keyProp, false, false, true, false));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void KeyPressEvent::OnStart()
   {
      dtABC::Application* app = dtABC::Application::GetInstance(0);
      if (!app)
      {
         return;
      }

      dtCore::Keyboard* keyboard = app->GetKeyboard();
      if (!keyboard)
      {
         return;
      }

      mListener = new MyKeyboardListener(this);
      keyboard->InsertKeyboardListener(0, mListener);
   }

   //////////////////////////////////////////////////////////////////////////
   bool KeyPressEvent::UsesInstigator()
   {
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void KeyPressEvent::UpdateName()
   {
      mName.clear();
      
      unsigned int mods = GetInt("Modifiers");
      if ((mods & MOD_CONTROL) == MOD_CONTROL)
      {
         mName += "Ctrl + ";
      }
      else if (mods & MOD_LEFT_CONTROL)
      {
         mName += "L_Ctrl + ";
      }
      else if (mods & MOD_RIGHT_CONTROL)
      {
         mName += "R_Ctrl + ";
      }

      if ((mods & MOD_ALT) == MOD_ALT)
      {
         mName += "Alt + ";
      }
      else if (mods & MOD_LEFT_ALT)
      {
         mName += "L_Alt + ";
      }
      else if (mods & MOD_RIGHT_ALT)
      {
         mName += "R_Alt + ";
      }

      if ((mods & MOD_SHIFT) == MOD_SHIFT)
      {
         mName += "Shift + ";
      }
      else if (mods & MOD_LEFT_SHIFT)
      {
         mName += "L_Shift + ";
      }
      else if (mods & MOD_RIGHT_SHIFT)
      {
         mName += "R_Shift + ";
      }

      mName += GetString("Key");
   }

   ////////////////////////////////////////////////////////////////////////////////
   void KeyPressEvent::SetModifiers(const unsigned int& value)
   {
      mModifierKeys = value;
      UpdateName();
   }

   ////////////////////////////////////////////////////////////////////////////////
   unsigned int KeyPressEvent::GetModifiers() const
   {
      return mModifierKeys;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void KeyPressEvent::GetModifierMasks(std::vector<std::string>& names, std::vector<unsigned int>& values)
   {
      names.push_back("Left Ctrl");
      names.push_back("Right Ctrl");
      names.push_back("Ctrl");

      names.push_back("Left Alt");
      names.push_back("Right Alt");
      names.push_back("Alt");

      names.push_back("Left Shift");
      names.push_back("Right Shift");
      names.push_back("Shift");

      values.push_back(MOD_LEFT_CONTROL);
      values.push_back(MOD_RIGHT_CONTROL);
      values.push_back(MOD_CONTROL);

      values.push_back(MOD_LEFT_ALT);
      values.push_back(MOD_RIGHT_ALT);
      values.push_back(MOD_ALT);

      values.push_back(MOD_LEFT_SHIFT);
      values.push_back(MOD_RIGHT_SHIFT);
      values.push_back(MOD_SHIFT);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void KeyPressEvent::SetKey(const std::string& value)
   {
      mMainKey = value;
      UpdateName();
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string KeyPressEvent::GetKey() const
   {
      return mMainKey;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<std::string> KeyPressEvent::GetKeyList() const
   {
      std::vector<std::string> keyList;

      dtABC::Application* app = dtABC::Application::GetInstance(0);
      if (!app)
      {
         return keyList;
      }

      dtCore::Keyboard* keyboard = app->GetKeyboard();
      if (!keyboard)
      {
         return keyList;
      }

      int count = keyboard->GetButtonCount();
      for (int index = 0; index < count; ++index)
      {
         dtCore::Button* button = keyboard->GetButton(index);
         if (button)
         {
            keyList.push_back(button->GetDescription());
         }
      }

      return keyList;
   }
}
