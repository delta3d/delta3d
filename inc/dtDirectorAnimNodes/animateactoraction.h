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

#ifndef ANIMATE_ACTOR_ACTION_NODE
#define ANIMATE_ACTOR_ACTION_NODE

////////////////////////////////////////////////////////////////////////////////

#include <dtDirector/latentactionnode.h>
#include <dtDirectorAnimNodes/nodelibraryexport.h>

#include <cal3d/global.h>


#if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
   #define MANUAL_ANIMATIONS
#endif

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   class ANIM_NODE_LIBRARY_EXPORT AnimateActorAction: public LatentActionNode
   {
   public:

      enum InputType
      {
         INPUT_START = 0,
         INPUT_STOP,
      };

      struct AnimData
      {
         std::string mName;
         float       mTime;
         float       mDuration;
         float       mBlendInTime;
         float       mBlendOutTime;
         float       mWeight;
         float       mStartOffset;
         float       mSpeed;

#ifdef MANUAL_ANIMATIONS
         int         mAnimation;
#endif
      };

      /**
       * Constructor.
       */
      AnimateActorAction();

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
      void SetAnimActor(const dtCore::UniqueId& value);
      dtCore::UniqueId GetAnimActor();

      void SetAnimName(const std::string& value);
      std::string GetAnimName();

      void SetAnimTime(float value);
      float GetAnimTime();

      void SetAnimDuration(float value);
      float GetAnimDuration();

      void SetBlendInTime(float value);
      float GetBlendInTime();

      void SetBlendOutTime(float value);
      float GetBlendOutTime();

      void SetAnimWeight(float value);
      float GetAnimWeight();

      void SetAnimOffset(float value);
      float GetAnimOffset();

      void SetAnimSpeed(float value);
      float GetAnimSpeed();

      void SetAnimIndex(int index);
      AnimData GetDefaultAnim(void);
      std::vector<AnimData> GetAnimArray(void);
      void SetAnimArray(const std::vector<AnimData>& value);

      void SetTime(float value);
      float GetTime();

   protected:

      /**
       * Destructor.
       */
      ~AnimateActorAction();

   private:

      dtCore::UniqueId  mAnimActor;

      float             mTime;
      bool              mIsActive;

      std::vector<AnimData>   mAnimList;
      int                     mAnimIndex;
   };
}

////////////////////////////////////////////////////////////////////////////////

#endif // ANIMATE_ACTOR_ACTION_NODE
