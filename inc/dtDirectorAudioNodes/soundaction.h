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
 * Author: MG
 */

#ifndef SOUND_ACTION_NODE
#define SOUND_ACTION_NODE

////////////////////////////////////////////////////////////////////////////////

#include <dtDirector/latentactionnode.h>
#include <dtDirectorAudioNodes/nodelibraryexport.h>

#include <dtCore/resourcedescriptor.h>

namespace dtAudio
{
   class Sound;
}

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   class AUDIO_NODE_LIBRARY_EXPORT SoundAction: public LatentActionNode
   {
   public:

      enum InputType
      {
         INPUT_PLAY = 0,
         INPUT_STOP,
         INPUT_PAUSE
      };

      /**
       * Constructor.
       */
      SoundAction();

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
      void SetSoundActor(const dtCore::UniqueId& value);
      dtCore::UniqueId GetSoundActor();

      void SetSoundResource(const dtCore::ResourceDescriptor& value);
      dtCore::ResourceDescriptor GetSoundResource();

      void SetGain(float value);
      float GetGain();

      void SetPitch(float value);
      float GetPitch();

      void SetListenerRelative(bool value);
      bool GetListenerRelative();

      void SetLooping(bool value);
      bool GetLooping();

      void SetBlockThread(bool value);
      bool GetBlockThread();

   protected:

      /**
       * Destructor.
       */
      ~SoundAction();

   private:

      dtCore::UniqueId mSoundActor;
      dtCore::ResourceDescriptor mSoundResourceResource;
      float mGain;
      float mPitch;
      bool mListenerRelative;
      bool mLooping;
      dtAudio::Sound* mpSound;
      bool mBlockThread;

      void PlaySoundsOnActors();
      void PlaySoundResource();
      void StopSoundsOnActors();
      void StopSoundResource();
      void PauseSoundsOnActors();
      void PauseSoundResource();

      bool AreAnySoundsOnActorsStillPlaying();
   };
}

////////////////////////////////////////////////////////////////////////////////

#endif // SOUND_ACTION_NODE
