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

#ifndef DIRECTOR_MOUSE_PRESS_EVENT_NODE
#define DIRECTOR_MOUSE_PRESS_EVENT_NODE

#include <dtDirector/eventnode.h>
#include <dtDirectorNodes/nodelibraryexport.h>
#include <dtCore/mouse.h>

namespace dtDirector
{
   class MousePressEvent;

   class MyMouseListener: public dtCore::MouseListener
   {
   public:
      MyMouseListener(MousePressEvent* event);

      virtual bool HandleButtonPressed(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button);
      virtual bool HandleButtonReleased(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button);
      virtual bool HandleButtonClicked(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button, int clickCount);
      virtual bool HandleMouseMoved(const dtCore::Mouse* mouse, float x, float y);
      virtual bool HandleMouseDragged(const dtCore::Mouse* mouse, float x, float y);
      virtual bool HandleMouseScrolled(const dtCore::Mouse* mouse, int delta);

   private:
      
      bool Check(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button);

      MousePressEvent* mEvent;
   };

   class NODE_LIBRARY_EXPORT MousePressEvent : public EventNode
   {
   public:

      /**
       * Constructs the Node.
       */
      MousePressEvent();

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
      void SetButton(const std::string& value);
      std::string GetButton() const;
      std::vector<std::string> GetButtonList() const;

   protected:

      /**
       *	Protected Destructor.  dtCore::RefPtr will handle its destruction.
       */
      virtual ~MousePressEvent();

   private:

      std::string  mButton;

      dtCore::RefPtr<MyMouseListener> mListener;
   };
}

#endif
