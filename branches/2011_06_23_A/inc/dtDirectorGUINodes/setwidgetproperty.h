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

#ifndef setwidgetproperty_h__
#define setwidgetproperty_h__

////////////////////////////////////////////////////////////////////////////////

#include <dtDirector/actionnode.h>
#include <dtDirectorGUINodes/nodelibraryexport.h>

#include <CEGUI/CEGUIEventArgs.h>
#include <CEGUI/CEGUIEvent.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   class GUI_NODE_LIBRARY_EXPORT SetWidgetProperty : public ActionNode
   {
   public:

      /**
       * Constructor.
       */
      SetWidgetProperty();

      /**
       * Initializes the Node.
       *
       * @param[in]  nodeType  The node type.
       * @param[in]  graph     The graph that owns this node.
       */
      virtual void Init(const NodeType& nodeType, DirectorGraph* graph);

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
       * Updates the node.
       * @note  Parent implementation will auto activate any trigger
       *        with the "Out" label by default.
       *
       * @param[in]  simDelta     The simulation time step.
       * @param[in]  delta        The real time step.
       * @param[in]  input        The index to the input that is active.
       * @param[in]  firstUpdate  True if this input was just activated,
       *
       * @return     True if the current node should remain active.
       */
      virtual bool Update(float simDelta, float delta, int input, bool firstUpdate);

      /**
       * Updates the display name of the node.
       */
      void UpdateName();

      /**
       * This event is called by value nodes that are linked via
       * value links when that value has changed.
       *
       * @param[in]  linkName  The name of the value link that is changing.
       */
      virtual void OnLinkValueChanged(const std::string& linkName);

      /**
       * Accessors for property values.
       */
      void SetLayout(const std::string& value);
      std::string GetLayout() const;
      std::vector<std::string> GetLayoutList();

      void SetWidget(const std::string& value);
      std::string GetWidget() const;
      std::vector<std::string> GetWidgetList();
      void RecurseWidgetList(std::vector<std::string>& widgetList, CEGUI::Window* parent);

      void SetProperty(const std::string& value);
      std::string GetProperty() const;

      void SetValue(const std::string& value);
      std::string GetValue() const;

   protected:

      /**
       * Destructor.
       */
      ~SetWidgetProperty();

   private:

      std::string mLayout;
      std::string mWidget;
      std::string mProperty;
      std::string mValue;
   };
}

////////////////////////////////////////////////////////////////////////////////

#endif // setwidgetproperty_h__
