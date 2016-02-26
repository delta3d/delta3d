#include <dtAudio/soundeffectbinder.h>

#include <cassert>

#include <dtCore/system.h>
#include <dtCore/scene.h>
#include <dtCore/transform.h>

#include <osg/Vec3>

#include <dtUtil/stringutils.h>

// namespaces
using namespace dtAudio;
/// @cond DOXYGEN_SHOULD_SKIP_THIS
using namespace std;
/// @endcond


// static member variables
const char* SoundEffectBinder::kPreFrame  = "preframe";
const char* SoundEffectBinder::kFrame     = "frame";
const char* SoundEffectBinder::kPostFrame = "postframe";




IMPLEMENT_MANAGEMENT_LAYER(SoundEffectBinder)



/**
 * Constructor.
 *
 * @param name the instance name
 */
SoundEffectBinder::SoundEffectBinder(const std::string& name /*= "SoundEffectBinder"*/)
   : dtCore::Base(name)
   , mSoundEffectListener(0)
{
   dtCore::System* sys = &dtCore::System::GetInstance();
   assert(sys);

   dtCore::System::GetInstance().TickSignal.connect_slot(this, &SoundEffectBinder::OnSystem);

   SoundEffectListener::EffectFunctor addEffect(this, &SoundEffectBinder::EffectAdded);
   SoundEffectListener::EffectFunctor removeEffect(this, &SoundEffectBinder::EffectRemoved);
   mSoundEffectListener = new SoundEffectListener(addEffect, removeEffect);
}



/**
 * Destructor.
 */
SoundEffectBinder::~SoundEffectBinder()
{
   Shutdown();
   dtCore::System::GetInstance().TickSignal.disconnect(this);

}



/**
 * Initialize the SoundEffectBinder.
 *
 * @param fxMgr the effect manager to add
 */
void SoundEffectBinder::Initialize(dtCore::EffectManager* fxMgr /*= NULL*/)
{
   AddEffectManager(fxMgr);
}



/**
 * Shutdown the SoundEffectBinder.
 */
void SoundEffectBinder::Shutdown()
{
   mQueued.clear();
   mDone.clear();
   mActive.clear();

   for (SFX_MAP::iterator iter(mSfxMap.begin()); iter != mSfxMap.end(); ++iter)
   {
      delete iter->second;
   }
   mSfxMap.clear();

   mFileMap.clear();
   mFxMgr.clear();
}



/**
 * Adds an effect manager whos effects we'll monitor.
 *
 * @param fxMgr the effect manager to add
 */
void SoundEffectBinder::AddEffectManager(dtCore::EffectManager* fxMgr)
{
   if (fxMgr == NULL)
   {
      return;
   }

   for (MGR_LST::iterator iter = mFxMgr.begin(); iter != mFxMgr.end(); ++iter)
   {
      if (*iter == fxMgr)
      {
         return;
      }
   }

   fxMgr->AddEffectListener(mSoundEffectListener.get());
   mFxMgr.push_back(fxMgr);
}



/**
 * Remove an effect manager from our list.
 *
 * @param fxMgr the effect manager to remove
 */
void SoundEffectBinder::RemoveEffectManager(dtCore::EffectManager* fxMgr)
{
   if (fxMgr == NULL)
   {
      return;
   }

   for (MGR_LST::iterator iter(mFxMgr.begin()); iter != mFxMgr.end(); ++iter)
   {
      if (*iter != fxMgr)
      {
         continue;
      }

      fxMgr->RemoveEffectListener(mSoundEffectListener.get());
      mFxMgr.erase(iter);
      break;
   }
}



/**
 * Maps the specified effect type to the given filename.
 *
 * @param fxType the effect type to map
 * @param filename the sound filename corresponding to the effect type
 */
void SoundEffectBinder::AddEffectTypeMapping(const std::string& fxType, const std::string& filename)
{
   mFileMap[fxType] = filename;
}


/**
 * Removes the specified effect type from the mapping.
 *
 * @param fxType the effect type to map
 */
void SoundEffectBinder::RemoveEffectTypeMapping(const std::string& fxType)
{
   mFileMap.erase(fxType);
   RemoveEffectTypeRange(fxType, true);
   RemoveEffectTypeRange(fxType, false);
}

/**
 * Maps the specified effect type to and audible range value.
 *
 * @param fxType the effect type to map
 * @param value to map
 * @param minimum range if true, else maximum range
 */
void SoundEffectBinder::AddEffectTypeRange(const std::string& fxType, float value, bool minimum_range /*= true*/)
{
   if (minimum_range)
   {
      mMinDist[fxType] = value;
   }
   else
   {
      mMaxDist[fxType] = value;
   }
}


/**
 * Removes the specified effect type's audible range value.
 *
 * @param fxType the effect type to map
 * @param minimum range if true, else maximum range
 */
void SoundEffectBinder::RemoveEffectTypeRange(const std::string& fxType, bool minimum_range /*= true*/)
{
   if (minimum_range)
   {
      mMinDist.erase(fxType);
   }
   else
   {
      mMaxDist.erase(fxType);
   }
}


/**
 * Called when a message is sent to this object.
 *
 * @param data the message received by this object
 */
void SoundEffectBinder::OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)

{
   if (str == kPreFrame)
   {
      PreFrame(deltaSim);
   }
   else if (str == kFrame)
   {
      Frame(deltaSim);
   }
   else if (str == kPostFrame)
   {
      PostFrame(deltaSim);
   }
}



/**
 * Called when an effect is added to the manager.
 *
 * @param fxMgr the effect manager that generated the event
 * @param fx the effect object
 */
void SoundEffectBinder::EffectAdded(dtCore::EffectManager* fxMgr, dtCore::Effect* fx)
{
   if ((fxMgr == NULL) || (fx == NULL))
   {
      return;
   }

   bool ignore(true);
   for (MGR_LST::iterator iter(mFxMgr.begin()); iter != mFxMgr.end(); ++iter)
   {
      if (*iter != fxMgr)
      {
         continue;
      }

      ignore = false;
   }

   if (ignore)
   {
      return;
   }

   dtCore::Detonation* det = dynamic_cast<dtCore::Detonation*>(fx);
   if (det != NULL)
   {
      // handle Detonations differently than regular effects
      DetonationAdded(fxMgr, det);
      return;
   }

   // TBD: what do we do with non-detonation effects?
}



/**
 * Called when an effect is removed from the manager.
 *
 * @param fxMgr the effect manager that generated the event
 * @param fx the effect object
 */
void SoundEffectBinder::EffectRemoved(dtCore::EffectManager* fxMgr, dtCore::Effect* fx)
{
   if ((fxMgr == NULL) || (fx == NULL))
   {
      return;
   }

   bool ignore(true);
   for (MGR_LST::iterator iter(mFxMgr.begin()); iter != mFxMgr.end(); ++iter)
   {
      if (*iter != fxMgr)
      {
         continue;
      }

      ignore = false;
   }

   if (ignore)
   {
      return;
   }

   dtCore::Detonation* det = dynamic_cast<dtCore::Detonation*>(fx);
   if (det != NULL)
   {
      // handle Detonations differently than regular effects
      DetonationRemoved(fxMgr, det);
      return;
   }

   // TBD: what do we do with non-detonation effects?
}



/**
 * Called when a detonation is added to the manager.
 *
 * @param fxMgr the effect manager that generated the event
 * @param fx the detonation object
 */
void SoundEffectBinder::DetonationAdded(dtCore::EffectManager* fxMgr, dtCore::Detonation* fx)
{
   assert(fxMgr);
   assert(fx);


   // locate filename in the map
   FIL_MAP::iterator iter = mFileMap.find(fx->GetType());
   if (iter == mFileMap.end())
   {
      // sound file is not mapped
      return;
   }


   // create a new sfx object and load the sound file
   SfxObj* sfx = new SfxObj(iter->second);
   assert(sfx);

   Sound* snd = sfx->GetSound();
   assert(snd);

   snd->LoadFile(iter->second.c_str());
   snd->SetPlayCallback(PlayCB, this);
   snd->SetStopCallback(StopCB, this);

   FLT_MAP::iterator minmax = mMaxDist.find(fx->GetType());
   if (minmax != mMaxDist.end())
   {
      snd->SetMaxDistance(minmax->second);
   }

   dtCore::Transformable* parent = fx->GetParent();
   if (parent)
   {
      //bool success = parent->AddChild(snd);
      parent->AddChild(snd);

      dtCore::Transform transform(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
      snd->SetTransform(transform, dtCore::Transformable::REL_CS);
   }
   else
   {
      osg::Vec3 pos (0.0f, 0.0f, 0.0f);
      fx->GetPosition(pos);

      snd->SetPosition(pos);
   }

   // map the sfx object and queue up for playing
   mSfxMap[fx] = sfx;
   sfx->SetList(&mQueued);
}



/**
 * Called when a detonation is removed from the manager.
 *
 * @param fxMgr the effect manager that generated the event
 * @param fx the detonation object
 */
void SoundEffectBinder::DetonationRemoved(dtCore::EffectManager* fxMgr, dtCore::Detonation* fx)
{
   assert(fxMgr);
   assert(fx);

   // locate sfx object in the map
   SFX_MAP::iterator iter(mSfxMap.find(fx));
   if (iter == mSfxMap.end())
   {
      // sound is not mapped
      return;
   }

   // move the sfx object onto the done list for cleanup
   iter->second->SetList(&mDone);
}



/**
 * Override for preframe
 */
void SoundEffectBinder::PreFrame(const double deltaFrameTime)
{
   while (mQueued.size())
   {
      SfxObj* sfx = *(mQueued.begin());
      sfx->SetList(NULL);      // play callback will set sfx on the active list
      sfx->GetSound()->Play();
   }
}



/**
 * Override for frame
 */
void SoundEffectBinder::Frame(const double deltaFrameTime)
{
   SFX_QUE temp;

   for (SFX_LST::iterator iter(mActive.begin()); iter != mActive.end(); ++iter)
   {
      SfxObj* sfx = *iter;
      if (sfx->GetSound()->IsPlaying())
      {
         continue;
      }

      temp.push(sfx);
   }

   while (temp.size())
   {
      SfxObj* sfx = temp.front();
      sfx->SetList(&mDone);
      temp.pop();
   }
}



/**
 * Override for postframe
 */
void SoundEffectBinder::PostFrame(const double deltaFrameTime)
{
   while (mDone.size())
   {
      SfxObj* sfx = *(mDone.begin());
      sfx->GetSound()->Stop();
      sfx->SetList(NULL);

      for (SFX_MAP::iterator iter(mSfxMap.begin()); iter != mSfxMap.end(); ++iter)
      {
         if (iter->second != sfx)
         {
            continue;
         }

         mSfxMap.erase(iter);
         break;
      }

      delete sfx;
   }
}



/**
 * Callback for when sound gets played.
 *
 * @param Sound pointer to the sound object
 * @param void pointer to the containing SfxObj
 */
void SoundEffectBinder::PlayCB(Sound* sound, void* param)
{
   assert(sound);
   assert(param);

   SoundEffectBinder* binder = static_cast<SoundEffectBinder*>(param);
   SFX_MAP::iterator iter;

   for (iter = binder->mSfxMap.begin(); iter != binder->mSfxMap.end(); ++iter)
   {
      SfxObj* sfx(iter->second);
      assert(sfx);

      Sound* snd(sfx->GetSound());
      assert(snd);

      if (snd != sound)
      {
         continue;
      }

      sfx->SetList(&binder->mActive);
      break;
   }
}



/**
 * Callback for when sound get stopped.
 *
 * @param Sound pointer to the sound object
 * @param void pointer to the containing SfxObj
 */
void SoundEffectBinder::StopCB(Sound* sound, void* param)
{
   assert(sound);
   assert(param);

   SoundEffectBinder* binder = static_cast<SoundEffectBinder*>(param);
   SFX_MAP::iterator iter;

   for (iter = binder->mSfxMap.begin(); iter != binder->mSfxMap.end(); ++iter)
   {
      SfxObj* sfx(iter->second);
      assert(sfx);

      Sound* snd(sfx->GetSound());
      assert(snd);

      if (snd != sound)
      {
         continue;
      }

      sfx->SetList(&binder->mDone);
      break;
   }
}



//////////////////////////////////////////
// SfxObj helper class member functions //
//////////////////////////////////////////
/**
 * Constructor.
 *
 * @param name the instance name
 */
SoundEffectBinder::SfxObj::SfxObj(const std::string& name /*= "sfxobj"*/)
   : Base(name)
   , mSnd(NULL)
   , mList(NULL)
{
   mSnd = AudioManager::GetInstance().NewSound();
   assert(mSnd);

   mSnd->SetName(name);
}



/**
 * Destructor.
 */
SoundEffectBinder::SfxObj::~SfxObj()
{
   AudioManager::GetInstance().FreeSound(mSnd);
}



/**
 * Called when this object gets put on a new sound effects list.
 *
 * @param list the sound effects list this object should put itself on
 */
void SoundEffectBinder::SfxObj::SetList(SFX_LST* list)
{
   // same list?
   if (mList == list)
   {
      // then bail out
      return;
   }

   // already on a list?
   if (mList != NULL)
   {
      // then remove ourself from it
      for (SFX_LST::iterator iter(mList->begin()); iter != mList->end(); ++iter)
      {
         if (*iter != this)
         {
            continue;
         }

         mList->erase(iter);
         break;
      }

      mList = NULL;
   }

   // not adding to any new list?
   if (list == NULL)
   {
      // then bail out
      return;
   }

   // put ourself on the new list
   mList = list;
   mList->push_back(this);
}

SoundEffectBinder::SoundEffectListener::SoundEffectListener(const EffectFunctor& addEffect,
                                                            const EffectFunctor& removeEffect)
   : dtCore::EffectListener()
   , mAddEffect(addEffect)
   , mRemoveEffect(removeEffect)
{
}

SoundEffectBinder::SoundEffectListener::~SoundEffectListener()
{
}

void SoundEffectBinder::SoundEffectListener::EffectAdded(dtCore::EffectManager* effectManager, dtCore::Effect* effect)
{
   mAddEffect(effectManager, effect);
}

void SoundEffectBinder::SoundEffectListener::EffectRemoved(dtCore::EffectManager* effectManager, dtCore::Effect* effect)
{
   mRemoveEffect(effectManager, effect);
}
