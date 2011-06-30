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

#ifndef SCHEDULER_ACTION_NODE
#define SCHEDULER_ACTION_NODE

////////////////////////////////////////////////////////////////////////////////

#include <dtDirector/latentactionnode.h>
#include <dtDirectorNodes/nodelibraryexport.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   class NODE_LIBRARY_EXPORT SchedulerAction: public LatentActionNode
   {
   public:

      enum InputType
      {
         INPUT_PLAY = 0,
         INPUT_REVERSE,
         INPUT_STOP,
         INPUT_PAUSE,
      };

      struct OutputEventData
      {
         std::string name;
         float       time;
         bool        triggerNormal;
         bool        triggerReverse;
      };

      /**
       * Constructor.
       */
      SchedulerAction();

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
       * Determines whether a value link on this node can connect
       * to a given value.
       *
       * @param[in]  link   The link.
       * @param[in]  value  The value to connect to.
       *
       * @return     True if a connection can be made.
       */
      virtual bool CanConnectValue(ValueLink* link, ValueNode* value);

      /**
       * Accessors for property values.
       */
      void SetTime(float value);
      float GetTime();

      void SetTotalTime(float value);
      float GetTotalTime();

      void SetUseSimTime(bool value);
      bool GetUseSimTime();

      void SetEventName(const std::string& value);
      std::string GetEventName();

      void SetEventTime(float value);
      float GetEventTime();

      void SetEventNormal(bool value);
      bool GetEventNormal();

      void SetEventReverse(bool value);
      bool GetEventReverse();

      void SetEventIndex(int index);
      OutputEventData GetDefaultEvent(void);
      std::vector<OutputEventData> GetEventList(void);
      void SetEventList(const std::vector<OutputEventData>& value);


   protected:

      /**
       * Destructor.
       */
      ~SchedulerAction();

   private:

      /**
      * Updates the list of outputs based on the event list.
      */
      void UpdateOutputs();

      /**
      * Triggers events based on a given time range.
      *
      * @param[in]  start       The starting time.
      * @param[in]  end         The ending time.
      * @param[in]  normalPlay  Playing normally, or in reverse.
      */
      void TestEvents(float start, float end, bool normalPlay);

      float mElapsedTime;
      float mTotalTime;
      bool  mUseSimTime;

      bool  mIsActive;
      int   mPlayDirection;

      std::vector<OutputEventData> mEventList;
      int                          mEventIndex;
   };
}

////////////////////////////////////////////////////////////////////////////////

#endif // SCHEDULER_ACTION_NODE
