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

#ifndef LERP_ACTOR_TRANSLATION_ACTION_NODE
#define LERP_ACTOR_TRANSLATION_ACTION_NODE

////////////////////////////////////////////////////////////////////////////////

#include <dtDirector/latentactionnode.h>
#include <dtDirectorNodes/nodelibraryexport.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   class NODE_LIBRARY_EXPORT LerpActorTranslationAction: public LatentActionNode
   {
   public:

      enum InputType
      {
         INPUT_START = 0,
         INPUT_STOP,
      };

      /**
       * Constructor.
       */
      LerpActorTranslationAction();

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
      void SetLerpActor(const dtCore::UniqueId& value);
      dtCore::UniqueId GetLerpActor();

      void SetStartTime(float value);
      float GetStartTime();

      void SetEndTime(float value);
      float GetEndTime();

      void SetTime(float value);
      float GetTime();

      void SetStartPos(const osg::Vec3& value);
      osg::Vec3 GetStartPos();

      void SetEndPos(const osg::Vec3& value);
      osg::Vec3 GetEndPos();

   protected:

      /**
       * Destructor.
       */
      ~LerpActorTranslationAction();

   private:
      bool IsTimeInternal();

      dtCore::UniqueId mLerpActor;
      float            mStartTime;
      float            mEndTime;
      float            mTime;
      osg::Vec3        mStartPos;
      osg::Vec3        mEndPos;

      bool             mWaitingForStart;

      bool mIsActive;
   };
}

////////////////////////////////////////////////////////////////////////////////

#endif // LERP_ACTOR_TRANSLATION_ACTION_NODE
