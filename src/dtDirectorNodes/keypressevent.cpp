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

#include <dtCore/actorproxy.h>
#include <dtCore/bitmaskactorproperty.h>
#include <dtCore/stringselectoractorproperty.h>
#include <dtCore/booleanactorproperty.h>

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
         mEvent->Trigger("Pressed");
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool MyKeyboardListener::HandleKeyReleased(const dtCore::Keyboard* keyboard, int keyCode)
   {
      if (mIsActive && Check(keyboard, keyCode, false))
      {
         mIsActive = false;
         mEvent->Trigger("Released");
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

               // If we don't want any modifiers, then fail on any modifiers used
               if (mods == 0 && (controlL || controlR || altL || altR || shiftL || shiftR))
               {
                  return false;
               }

               if ((mods & MOD_CONTROL) == MOD_CONTROL)
               {
                  if (!controlL && !controlR)
                  {
                     return false;
                  }
               }
               else if ((mods & MOD_LEFT_CONTROL && !controlL) ||
                  (mods & MOD_RIGHT_CONTROL && !controlR))
               {
                  return false;
               }

               if ((mods & MOD_ALT) == MOD_ALT)
               {
                  if (!altL && !altR)
                  {
                     return false;
                  }
               }
               else if ((mods & MOD_LEFT_ALT && !altL) ||
                  (mods & MOD_RIGHT_ALT && !altR))
               {
                  return false;
               }

               if ((mods & MOD_SHIFT) == MOD_SHIFT)
               {
                  if (!shiftL && !shiftR)
                  {
                     return false;
                  }
               }
               else if ((mods & MOD_LEFT_SHIFT && !shiftL) ||
                  (mods & MOD_RIGHT_SHIFT && !shiftR))
               {
                  return false;
               }

               // If we want any modifier, then at least one modifier has to be true
               if (mods & MOD_ANY && (!controlL && !controlR && !altL && !altR && !shiftL && !shiftR))
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
      mOutputs.push_back(OutputLink(this, "Pressed", "Activated when the designated key is pushed down."));
      mOutputs.push_back(OutputLink(this, "Released", "Activated when the designated key has been released."));

      UpdateName();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void KeyPressEvent::BuildPropertyMap()
   {
      EventNode::BuildPropertyMap();

      dtCore::BitMaskActorProperty* modsProp = new dtCore::BitMaskActorProperty(
         "Modifiers", "Modifiers",
         dtCore::BitMaskActorProperty::SetFuncType(this, &KeyPressEvent::SetModifiers),
         dtCore::BitMaskActorProperty::GetFuncType(this, &KeyPressEvent::GetModifiers),
         dtCore::BitMaskActorProperty::GetMaskListFuncType(this, &KeyPressEvent::GetModifierMasks),
         "Modifier keys to listen for along with the main key.");
      AddProperty(modsProp);

      dtCore::StringSelectorActorProperty* keyProp = new dtCore::StringSelectorActorProperty(
         "Key", "Key",
         dtCore::StringSelectorActorProperty::SetFuncType(this, &KeyPressEvent::SetKey),
         dtCore::StringSelectorActorProperty::GetFuncType(this, &KeyPressEvent::GetKey),
         dtCore::StringSelectorActorProperty::GetListFuncType(this, &KeyPressEvent::GetKeyList),
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
   bool KeyPressEvent::UsesActorFilters()
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

      keyList.push_back("Space");
      keyList.push_back("Enter");
      keyList.push_back("Tab");
      keyList.push_back("Backspace");
      keyList.push_back("Esc");

      keyList.push_back("A");
      keyList.push_back("B");
      keyList.push_back("C");
      keyList.push_back("D");
      keyList.push_back("E");
      keyList.push_back("F");
      keyList.push_back("G");
      keyList.push_back("H");
      keyList.push_back("I");
      keyList.push_back("J");
      keyList.push_back("K");
      keyList.push_back("L");
      keyList.push_back("M");
      keyList.push_back("N");
      keyList.push_back("O");
      keyList.push_back("P");
      keyList.push_back("Q");
      keyList.push_back("R");
      keyList.push_back("S");
      keyList.push_back("T");
      keyList.push_back("U");
      keyList.push_back("V");
      keyList.push_back("W");
      keyList.push_back("X");
      keyList.push_back("Y");
      keyList.push_back("Z");

      keyList.push_back("F1");
      keyList.push_back("F2");
      keyList.push_back("F3");
      keyList.push_back("F4");
      keyList.push_back("F5");
      keyList.push_back("F6");
      keyList.push_back("F7");
      keyList.push_back("F8");
      keyList.push_back("F9");
      keyList.push_back("F10");
      keyList.push_back("F12");

      keyList.push_back("`");
      keyList.push_back("1");
      keyList.push_back("2");
      keyList.push_back("3");
      keyList.push_back("4");
      keyList.push_back("5");
      keyList.push_back("6");
      keyList.push_back("7");
      keyList.push_back("8");
      keyList.push_back("9");
      keyList.push_back("0");
      keyList.push_back("-");
      keyList.push_back("=");

      keyList.push_back("[");
      keyList.push_back("]");
      keyList.push_back("\\");
      keyList.push_back(";");
      keyList.push_back("\'");
      keyList.push_back(",");
      keyList.push_back(".");
      keyList.push_back("/");

      keyList.push_back("Left Shift");
      keyList.push_back("Right Shift");
      keyList.push_back("Left Ctrl");
      keyList.push_back("Right Ctrl");
      keyList.push_back("Left Alt");
      keyList.push_back("Right Alt");
      keyList.push_back("Left Super");
      keyList.push_back("Right Super");

      keyList.push_back("Pause");
      keyList.push_back("Home");
      keyList.push_back("End");
      keyList.push_back("Delete");
      keyList.push_back("Insert");
      keyList.push_back("Page Up");
      keyList.push_back("Page Down");

      keyList.push_back("Left");
      keyList.push_back("Up");
      keyList.push_back("Right");
      keyList.push_back("Down");

      keyList.push_back("Keypad 1");
      keyList.push_back("Keypad 2");
      keyList.push_back("Keypad 3");
      keyList.push_back("Keypad 4");
      keyList.push_back("Keypad 5");
      keyList.push_back("Keypad 6");
      keyList.push_back("Keypad 7");
      keyList.push_back("Keypad 8");
      keyList.push_back("Keypad 9");
      keyList.push_back("Keypad 0");
      keyList.push_back("Keypad +");
      keyList.push_back("Keypad -");
      keyList.push_back("Keypad *");
      keyList.push_back("Keypad /");
      keyList.push_back("Keypad .");

      keyList.push_back("Num Lock");
      keyList.push_back("Caps Lock");
      keyList.push_back("Print Screen");
      keyList.push_back("Scroll Lock");
      keyList.push_back("Menu");

      return keyList;
   }
}
