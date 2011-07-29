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

#ifndef STRING_MANIPULATOR_ACTION_NODE
#define STRING_MANIPULATOR_ACTION_NODE

////////////////////////////////////////////////////////////////////////////////

#include <dtDirector/actionnode.h>
#include <dtDirectorNodes/nodelibraryexport.h>
#include <dtCore/actorproperty.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   class NODE_LIBRARY_EXPORT StringManipulatorAction: public ActionNode
   {
   public:
      enum InputType
      {
         INPUT_INSERT = 0,
         INPUT_APPEND,
         INPUT_REMOVE,
         INPUT_REPLACE,
      };

      /**
       * Constructor.
       */
      StringManipulatorAction();

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
       * Accessors for property values.
       */
      void SetIndex(int value);
      int GetIndex() const;

      void SetA(const std::string& value);
      std::string GetA() const;

      void SetB(const std::string& value);
      std::string GetB() const;

      void SetReplaceWith(const std::string& value);
      std::string GetReplaceWith() const;

      void SetResult(const std::string& value);
      std::string GetResult() const;


   protected:

      /**
       * Destructor.
       */
      ~StringManipulatorAction();

   private:

      int         mIndex;

      std::string mValueA;
      std::string mValueB;
      std::string mReplaceWith;
      std::string mResult;
      
      dtCore::RefPtr<dtCore::ActorProperty> mResultProp;
   };
}

////////////////////////////////////////////////////////////////////////////////

#endif // STRING_MANIPULATOR_ACTION_NODE
