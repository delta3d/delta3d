/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2005 MOVES Institute
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
 */

#ifndef DELTA_SOUNDEFFECTBINDER
#define DELTA_SOUNDEFFECTBINDER

#include <string>
#include <vector>
#include <map>
#include <queue>

#include <dtCore/base.h>
#include <dtCore/effectmanager.h>
#include <dtAudio/audiomanager.h>
#include <dtAudio/export.h>
#include <dtUtil/functor.h>

// soundeffectbinder.h: Declaration of the EffectManager class.
//
//////////////////////////////////////////////////////////////////////

/******************************************************************************
 * Change Advisory (15 January 2006 submitted by LT Ryan Ernst)
 *
 * Discussion:
 * The Detonation class replaced the DetonationType parameter with a
 * std::stringdue for extensibility.  The soundeffectbinder class utilized
 * DetonationType to associate sound effects to a particular Detonation.
 *
 * Solution:
 * A string value replaces DetonationType.
 *
 * Backward Compatability:
 * DetonationType remains supported but is converted internally to a string
 * representing the numeric value of the DetonationType.
 * For example; SmokeDetonation is converted to the string value "2000".
 *
 * Developer Recommendations:
 * Utilize string functionality to avoid future deprecation.
 *****************************************************************************/


namespace dtAudio
{
   /**
    * A class that binds audible effects to visual effects
    *
    * User must specify which (visual) effect manager will be used,
    * then supply a unique id for an audible effect with the sound-
    * filename.
    */
   class DT_AUDIO_EXPORT SoundEffectBinder : public dtCore::Base
   {
      DECLARE_MANAGEMENT_LAYER(SoundEffectBinder)

   private:
      // forward references
      class SfxObj;

      typedef std::vector<dtCore::EffectManager*>    MGR_LST;
      typedef std::vector<SfxObj*>                   SFX_LST;
      typedef std::queue<SfxObj*>                    SFX_QUE;
      typedef std::map<std::string,std::string>      FIL_MAP;
      typedef std::map<dtCore::Detonation*,SfxObj*>  SFX_MAP;
      typedef std::map<std::string,float>            FLT_MAP;

      static const char* kPreFrame;
      static const char* kFrame;
      static const char* kPostFrame;

      /**
       * A sound effect object adding mapping managment
       * ability to a Delta3D sound object
       */
      class SfxObj : public dtCore::Base
      {
      private:
         typedef std::vector<SfxObj*> SFX_LST;

      public:
         SfxObj(const std::string& name = "sfxobj");
         virtual ~SfxObj();

         void SetList(SFX_LST* list);
         Sound* GetSound() const {  return mSnd; }

      private:
         Sound*   mSnd;
         SFX_LST* mList;
      };

      class SoundEffectListener : public dtCore::EffectListener
      {
      public:
         typedef dtUtil::Functor<void, TYPELIST_2(dtCore::EffectManager*, dtCore::Effect*)> EffectFunctor;
         SoundEffectListener(const EffectFunctor& addEffect,
                             const EffectFunctor& removeEffect);
      protected:
         virtual ~SoundEffectListener();
      public:
         virtual void EffectAdded(dtCore::EffectManager* effectManager, dtCore::Effect* effect);
         virtual void EffectRemoved(dtCore::EffectManager* effectManager, dtCore::Effect* effect);
      private:

         EffectFunctor mAddEffect;
         EffectFunctor mRemoveEffect;
      };

   public:
      SoundEffectBinder(const std::string& name = "soundeffectbinder");

   protected:

      virtual ~SoundEffectBinder();


   private:

      // Disallowed to prevent compile errors on VS2003. It apparently
      // creates this functions even if they are not used, and if
      // this class is forward declared, these implicit functions will
      // cause compiler errors for missing calls to "ref".
      SoundEffectBinder& operator=(const SoundEffectBinder&);
      SoundEffectBinder(const SoundEffectBinder&);

   public:

      /**
       * Initialize the SoundEffectBinder.
       *
       * @param fxMgr the effect manager to add
       */
      virtual void Initialize(dtCore::EffectManager* fxMgr = NULL);

      /**
       * Shutdown the SoundEffectBinder.
       */
      virtual void Shutdown();

      /**
       * Adds an effect manager whos effects we'll monitor.
       *
       * @param fxMgr the effect manager to add
       */
      virtual void AddEffectManager(dtCore::EffectManager* fxMgr);

      /**
       * Remove an effect manager from our list.
       *
       * @param fxMgr the effect manager to remove
       */
      virtual void RemoveEffectManager(dtCore::EffectManager* fxMgr);

      /**
       * Maps the specified effect type to the given filename.
       *
       * @param fxType the effect type to map
       * @param filename the sound filename corresponding to the effect type
       */
      virtual void AddEffectTypeMapping(const std::string& fxType, const std::string& filename);

      /**
       * Removes the specified effect type from the mapping.
       *
       * @param fxType the effect type to remove
       */
      virtual void RemoveEffectTypeMapping(const std::string& fxType);

      /**
       * Maps the specified effect type to and audible range value.
       *
       * @param fxType the effect type to map
       * @param value to map
       * @param minimum range if true, else maximum range
       */
      virtual void AddEffectTypeRange(const std::string& fxType, float value, bool minimum_range = true);


      /**
       * Removes the specified effect type's audible range value.
       *
       * @param fxType the effect type to map
       * @param minimum range if true, else maximum range
       */
      virtual void RemoveEffectTypeRange(const std::string& fxType, bool minimum_range = true);

   private:
      /**
       * Called when a message is sent to this object.
       *
       * @param data the message received by this object
       */
      virtual void OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)
;

      /**
       * Called when an effect is added to the manager.
       *
       * @param fxMgr the effect manager that generated the event
       * @param fx the effect object
       */
      virtual void EffectAdded(dtCore::EffectManager* fxMgr, dtCore::Effect* fx);

      /**
       * Called when an effect is removed from the manager.
       *
       * @param fxMgr the effect manager that generated the event
       * @param fx the effect object
       */
      virtual void EffectRemoved(dtCore::EffectManager* fxMgr, dtCore::Effect* fx);

      /**
       * Called when a detonation is added to the manager.
       *
       * @param fxMgr the effect manager that generated the event
       * @param fx the Detonation object
       */
      inline void DetonationAdded(dtCore::EffectManager* fxMgr, dtCore::Detonation* fx);

      /**
       * Called when a detonation is removed from the manager.
       *
       * @param fxMgr the effect manager that generated the event
       * @param fx the Detonation object
       */
      inline void DetonationRemoved(dtCore::EffectManager* fxMgr, dtCore::Detonation* fx);

      /**
       * Override for preframe
       */
      inline void PreFrame(const double deltaFrameTime);

      /**
       * Override for frame
       */
      inline void Frame(const double deltaFrameTime);

      /**
       * Override for postframe
       */
      inline void PostFrame(const double deltaFrameTime);

      /**
       * Callback for when sound gets played.
       *
       * @param Sound pointer to the sound object
       * @param void pointer to the containing SfxObj
       */
      static void PlayCB(Sound* sound, void* param);

      /**
       * Callback for when sound get stopped.
       *
       * @param Sound pointer to the sound object
       * @param void pointer to the containing SfxObj
       */
      static void StopCB(Sound* sound, void* param);

   private:
      dtCore::RefPtr<SoundEffectListener> mSoundEffectListener;

      SFX_LST mQueued;
      SFX_LST mActive;
      SFX_LST mDone;
      FIL_MAP mFileMap;
      SFX_MAP mSfxMap;
      MGR_LST mFxMgr;
      FLT_MAP mMinDist;
      FLT_MAP mMaxDist;
   };
} // namespace dtAudio

#endif // DELTA_SOUNDEFFECTBINDER
