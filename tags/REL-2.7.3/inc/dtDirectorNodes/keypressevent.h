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

#ifndef DIRECTOR_KEY_PRESS_EVENT_NODE
#define DIRECTOR_KEY_PRESS_EVENT_NODE

#include <dtDirector/eventnode.h>
#include <dtDirectorNodes/nodelibraryexport.h>
#include <dtCore/keyboard.h>

namespace dtDirector
{
   enum KeyMods
   {
      MOD_LEFT_CONTROL = 0x01,
      MOD_RIGHT_CONTROL = 0x02,
      MOD_CONTROL = MOD_LEFT_CONTROL | MOD_RIGHT_CONTROL,

      MOD_LEFT_ALT = 0x04,
      MOD_RIGHT_ALT = 0x08,
      MOD_ALT = MOD_LEFT_ALT | MOD_RIGHT_ALT,

      MOD_LEFT_SHIFT = 0x10,
      MOD_RIGHT_SHIFT = 0x20,
      MOD_SHIFT = MOD_LEFT_SHIFT | MOD_RIGHT_SHIFT,

      MOD_ANY = MOD_CONTROL | MOD_ALT | MOD_SHIFT
   };

   class KeyPressEvent;

   class MyKeyboardListener: public dtCore::KeyboardListener
   {
   public:
      MyKeyboardListener(KeyPressEvent* event);

      virtual bool HandleKeyPressed(const dtCore::Keyboard* keyboard, int keyCode);
      virtual bool HandleKeyReleased(const dtCore::Keyboard* keyboard, int keyCode);
      virtual bool HandleKeyTyped(const dtCore::Keyboard* keyboard, int keyCode);

   private:

      bool Check(const dtCore::Keyboard* keyboard, int keyCode, bool checkMods = true);

      KeyPressEvent* mEvent;
      bool           mIsActive;
   };

   class NODE_LIBRARY_EXPORT KeyPressEvent : public EventNode
   {
   public:

      /**
       * Constructs the Node.
       */
      KeyPressEvent();

      /**
       * Initializes the Node.
       *
       * @param[in]  nodeType  The node type.
       * @param[in]  graph     The graph that owns this node.
       */
      virtual void Init(const NodeType& nodeType, DirectorGraph* graph);

      /**
       * This method is called in init, which instructs the node
       * to create its properties.  Methods implementing this should
       * be sure to call their parent class's buildPropertyMap method to
       * ensure all properties in the proxy inheritance hierarchy are
       * correctly added to the property map.
       *
       * @see GetDeprecatedProperty to handle old properties that need
       *       to be removed.
       */
      virtual void BuildPropertyMap();

      /**
       * Event handler called after the entire script has been loaded.
       */
      virtual void OnStart();

      /**
       * Retrieves whether this Event uses an instigator.
       */
      virtual bool UsesActorFilters();

      /**
       * Updates the display name.
       */
      void UpdateName();

      /**
       * Accessors
       */
      void SetModifiers(const unsigned int& value);
      unsigned int GetModifiers() const;
      void GetModifierMasks(std::vector<std::string>& names, std::vector<unsigned int>& values);

      void SetKey(const std::string& value);
      std::string GetKey() const;
      std::vector<std::string> GetKeyList() const;

   protected:

      /**
       *	Protected Destructor.  dtCore::RefPtr will handle its destruction.
       */
      virtual ~KeyPressEvent();

   private:

      unsigned int mModifierKeys;
      std::string  mMainKey;

      dtCore::RefPtr<MyKeyboardListener> mListener;
   };
}

#endif
