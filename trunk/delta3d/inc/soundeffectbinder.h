#pragma once
#include <string>
#include <vector>
#include <map>
#include <queue>

#include "sg.h"

#include "base.h"
#include "effectmanager.h"
#include "sound.h"



// definitions
#if   !  defined(BIT)
#define  BIT(a)   ((unsigned int)(1L<<(unsigned int)(a)))
#endif



namespace dtCore
{
   /**
    * A class that binds audible effects to visual effects
    * 
    * User must specify which (visual) effect manager will be used,
    * then supply a unique id for an audible effect with the sound-
    * filename.
    */
   class SoundEffectBinder :  public   EffectListener, public   Base
   {
      private:
         DECLARE_MANAGEMENT_LAYER(SoundEffectBinder)

         // forward references
         class SfxObj;

         typedef  std::vector<EffectManager*>         MGR_LST;
         typedef  std::vector<SfxObj*>                SFX_LST;
         typedef  std::queue<SfxObj*>                 SFX_QUE;
         typedef  std::map<unsigned int,std::string>  FIL_MAP;
         typedef  std::map<Detonation*,SfxObj*>       SFX_MAP;

         static   const char* kPreFrame;
         static   const char* kFrame;
         static   const char* kPostFrame;

         /**
          * A sound effect object adding mapping managment
          * ability to a Delta3D sound object
          */
         class SfxObj   :  public   Sound
         {
            private:
               typedef  std::vector<SfxObj*> SFX_LST;

            public:
                                 SfxObj( std::string name = "sfxobj" );
               virtual           ~SfxObj();

                        void     SetList( SFX_LST* list );

            private:
                        SFX_LST* mList;
         };

      public:
                              SoundEffectBinder( std::string name = "soundeffectbinder" );
         virtual              ~SoundEffectBinder();

         /**
          * Initialize the SoundEffectBinder.
          *
          * @param fxMgr the effect manager to add
          */
         virtual  void        Initialize( EffectManager* fxMgr = NULL );

         /**
          * Shutdown the SoundEffectBinder.
          */
         virtual  void        Shutdown( void );

         /**
          * Adds an effect manager whos effects we'll monitor.
          *
          * @param fxMgr the effect manager to add
          */
         virtual  void        AddEffectManager( EffectManager* fxMgr );

         /**
          * Remove an effect manager from our list.
          *
          * @param fxMgr the effect manager to remove
          */
         virtual  void        RemoveEffectManager( EffectManager* fxMgr );

         /**
          * Maps the specified effect type to the given filename.
          *
          * @param fxType the effect type to map
          * @param filename the sound filename corresponding to the effect type
          */
         virtual  void        AddEffectTypeMapping( unsigned int fxType, const char* filename );

         /**
          * Removes the specified effect type from the mapping.
          *
          * @param fxType the effect type to remove
          */
         virtual  void        RemoveEffectTypeMapping( unsigned int fxType );

      private:
         /**
          * Called when a message is sent to this object.
          *
          * @param data the message received by this object
          */
         virtual  void        OnMessage( MessageData* data );

         /**
          * Called when an effect is added to the manager.
          *
          * @param fxMgr the effect manager that generated the event
          * @param fx the effect object
          */
         virtual  void        EffectAdded( EffectManager* fxMgr, Effect* fx );

         /**
          * Called when an effect is removed from the manager.
          *
          * @param fxMgr the effect manager that generated the event
          * @param fx the effect object
          */
         virtual  void        EffectRemoved( EffectManager* fxMgr, Effect* fx );

         /**
          * Called when a detonation is added to the manager.
          *
          * @param fxMgr the effect manager that generated the event
          * @param fx the Detonation object
          */
         inline   void        DetonationAdded( EffectManager* fxMgr, Detonation* fx );

         /**
          * Called when a detonation is removed from the manager.
          *
          * @param fxMgr the effect manager that generated the event
          * @param fx the Detonation object
          */
         inline   void        DetonationRemoved( EffectManager* fxMgr, Detonation* fx );

         /**
          * Override for preframe
          */
         inline   void        PreFrame( const double deltaFrameTime );

         /**
          * Override for frame
          */
         inline   void        Frame( const double deltaFrameTime );

         /**
          * Override for postframe
          */
         inline   void        PostFrame( const double deltaFrameTime );

      private:
                  SFX_LST     mQueued;
                  SFX_LST     mActive;
                  SFX_LST     mDone;
                  FIL_MAP     mFileMap;
                  SFX_MAP     mSfxMap;
                  MGR_LST     mFxMgr;
   };
};
