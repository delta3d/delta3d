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
 * Author: Eric R. Heine
 */

#ifndef sliderevent_h__
#define sliderevent_h__

#include <dtDirector/eventnode.h>
#include <dtDirectorGUINodes/nodelibraryexport.h>

#include <CEGUI/CEGUIEventArgs.h>
#include <CEGUI/CEGUIEvent.h>

namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   class GUI_NODE_LIBRARY_EXPORT SliderEvent : public EventNode
   {
   public:

      /**
       * Constructs the Node.
       */
      SliderEvent();

      /**
       * Initializes the Node.
       *
       * @param[in]  nodeType  The node type.
       * @param[in]  graph     The graph that owns this node.
       */
      virtual void Init(const NodeType& nodeType, DirectorGraph* graph);

      /**
       * Event handler called after the entire script has been loaded.
       */
      virtual void OnStart();

      /**
       * Event handler called when a script has finished loading.
       */
      virtual void OnFinishedLoading();

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
       * Retrieves whether this Event uses an instigator.
       */
      virtual bool UsesActorFilters();

      /**
       * This event is called by value nodes that are linked via
       * value links when that value has changed.
       *
       * @param[in]  linkName  The name of the value link that is changing.
       */
      virtual void OnLinkValueChanged(const std::string& linkName);

      /**
       * Updates the display name of the node.
       */
      void UpdateName();

      /**
       * Refreshes the callback connections.
       */
      void RefreshConnections();

      /**
       * Accessors for property values.
       */
      void SetLayout(const std::string& value);
      std::string GetLayout();
      std::vector<std::string> GetLayoutList();

      void SetSlider(const std::string& value);
      std::string GetSlider();
      std::vector<std::string> GetSliderList();
      void RecurseSliderList(std::vector<std::string>& sliderList, CEGUI::Window* parent);

      /**
       * Callback event handlers.
       */
      bool OnValueChanged(const CEGUI::EventArgs& e);
      bool OnThumbTrackStarted(const CEGUI::EventArgs& e);
      bool OnThumbTrackEnded(const CEGUI::EventArgs& e);

   protected:

      /**
       *	Protected Destructor.  dtCore::RefPtr will handle its destruction.
       */
      virtual ~SliderEvent();

   private:

      std::string mLayout;
      std::string mSlider;

      CEGUI::Event::Connection mValueChangedConnection;
      CEGUI::Event::Connection mThumbTrackStartedConnection;
      CEGUI::Event::Connection mThumbTrackEndedConnection;

      void DestroyConnection(CEGUI::Event::Connection& connection);
   };
}

#endif // sliderevent_h__
