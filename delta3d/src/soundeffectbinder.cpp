#include <cassert>

#include "system.h"
#include "soundeffectbinder.h"



// namespaces
using namespace dtCore;
using namespace std;



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
SoundEffectBinder::SoundEffectBinder( std::string name /*= "SoundEffectBinder"*/ )
:  Base(name)
{
   System*  sys   = System::GetSystem();
   assert( sys );

   AddSender( sys );
}



/**
 * Destructor.
 */
SoundEffectBinder::~SoundEffectBinder()
{
   Shutdown();
}



/**
 * Initialize the SoundEffectBinder.
 *
 * @param fxMgr the effect manager to add
 */
void 
SoundEffectBinder::Initialize( EffectManager* fxMgr /*= NULL*/ )
{
   AddEffectManager( fxMgr );
}



/**
* Shutdown the SoundEffectBinder.
*/
void 
SoundEffectBinder::Shutdown( void )
{
   mQueued.clear();
   mDone.clear();
   mActive.clear();

   for( SFX_MAP::iterator iter(mSfxMap.begin()); iter != mSfxMap.end(); iter++ )
   {
      delete   iter->second;
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
void 
SoundEffectBinder::AddEffectManager( EffectManager* fxMgr )
{
   if( fxMgr == NULL )
      return;

   for( MGR_LST::iterator iter = mFxMgr.begin(); iter != mFxMgr.end(); iter++ )
   {
      if( *iter == fxMgr )
         return;
   }

   fxMgr->AddEffectListener( this );
   mFxMgr.push_back( fxMgr );
}



/**
 * Remove an effect manager from our list.
 *
 * @param fxMgr the effect manager to remove
 */
void 
SoundEffectBinder::RemoveEffectManager( EffectManager* fxMgr )
{
   if( fxMgr == NULL )
      return;

   for( MGR_LST::iterator iter(mFxMgr.begin()); iter != mFxMgr.end(); iter++ )
   {
      if( *iter != fxMgr )
         continue;

      fxMgr->RemoveEffectListener( this );
      mFxMgr.erase( iter );
      break;
   }
}



/**
 * Maps the specified effect type to the given filename.
 *
 * @param fxType the effect type to map
 * @param filename the sound filename corresponding to the effect type
 */
void 
SoundEffectBinder::AddEffectTypeMapping( unsigned int fxType, const char* filename )
{
   if( ( filename == NULL ) || ( std::string(filename) == std::string("") ) )
      return;

   mFileMap[fxType]  = filename;
}



/**
 * Removes the specified effect type from the mapping.
 *
 * @param fxType the effect type to map
 */
void 
SoundEffectBinder::RemoveEffectTypeMapping( unsigned int fxType )
{
   mFileMap.erase( fxType );
}



/**
* Called when a message is sent to this object.
*
* @param data the message received by this object
*/
void 
SoundEffectBinder::OnMessage( MessageData* data )
{
   if( data == NULL )
      return;

   if( data->message == kPreFrame )
   {
      PreFrame( *static_cast<const double*>(data->userData) );
      return;
   }

   if( data->message == kFrame )
   {
      Frame( *static_cast<const double*>(data->userData) );
      return;
   }

   if( data->message == kPostFrame )
   {
      PostFrame( *static_cast<const double*>(data->userData) );
      return;
   }
}



/**
 * Called when an effect is added to the manager.
 *
 * @param fxMgr the effect manager that generated the event
 * @param fx the effect object
 */
void 
SoundEffectBinder::EffectAdded( EffectManager* fxMgr, Effect* fx )
{
   if( ( fxMgr == NULL ) || ( fx == NULL ) )
      return;

   bool  ignore(true);
   for( MGR_LST::iterator iter(mFxMgr.begin()); iter != mFxMgr.end(); iter++ )
   {
      if( *iter != fxMgr )
         continue;

      ignore   = false;
   }

   if( ignore )
      return;

   Detonation* det   = dynamic_cast<Detonation*>(fx);
   if( det != NULL )
   {
      // handle Detonations differently than regular effects
      DetonationAdded( fxMgr, det );
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
void 
SoundEffectBinder::EffectRemoved( EffectManager* fxMgr, Effect* fx )
{
   if( ( fxMgr == NULL ) || ( fx == NULL ) )
      return;

   bool  ignore(true);
   for( MGR_LST::iterator iter(mFxMgr.begin()); iter != mFxMgr.end(); iter++ )
   {
      if( *iter != fxMgr )
         continue;

      ignore   = false;
   }

   if( ignore )
      return;

   Detonation* det   = dynamic_cast<Detonation*>(fx);
   if( det != NULL )
   {
      // handle Detonations differently than regular effects
      DetonationRemoved( fxMgr, det );
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
void 
SoundEffectBinder::DetonationAdded( EffectManager* fxMgr, Detonation* fx )
{
   assert( fxMgr );
   assert( fx );


   // locate filename in the map
   FIL_MAP::iterator iter(mFileMap.find( fx->GetType() ));
   if( iter == mFileMap.end() )
   {
      // sound file is not mapped
      return;
   }


   // create a new sfx object and load the sound file
   SfxObj*  sfx   = new SfxObj( iter->second );
   assert( sfx );

   sfx->LoadFile( iter->second );


   // map the sfx object and queue up for playing
   mSfxMap[fx] = sfx;
   sfx->SetList( &mQueued );
}



/**
 * Called when a detonation is removed from the manager.
 *
 * @param fxMgr the effect manager that generated the event
 * @param fx the detonation object
 */
void 
SoundEffectBinder::DetonationRemoved( EffectManager* fxMgr, Detonation* fx )
{
   assert( fxMgr );
   assert( fx );

   // locate sfx object in the map
   SFX_MAP::iterator iter(mSfxMap.find( fx ));
   if( iter == mSfxMap.end() )
   {
      // sound is not mapped
      return;
   }

   // move the sfx object onto the done list for cleanup
   iter->second->SetList( &mDone );
}



/**
 * Override for preframe
 */
void 
SoundEffectBinder::PreFrame( const double deltaFrameTime )
{
   while( mQueued.size() )
   {
      SfxObj*  sfx   = *(mQueued.begin());
      sfx->SetList( &mActive );
      sfx->Play();
   }
}



/**
 * Override for frame
 */
void 
SoundEffectBinder::Frame( const double deltaFrameTime )
{
   SFX_QUE  temp;

   for( SFX_LST::iterator iter(mActive.begin()); iter != mActive.end(); iter++ )
   {
      SfxObj*  sfx   = *iter;
      if( sfx->IsPlaying() )
         continue;

      temp.push( sfx );
   }

   while( temp.size() )
   {
      SfxObj*  sfx   = temp.front();
      sfx->SetList( &mDone );
      temp.pop();
   }
}



/**
 * Override for postframe
 */
void 
SoundEffectBinder::PostFrame( const double deltaFrameTime )
{
   while( mDone.size() )
   {
      SfxObj*  sfx   = *(mDone.begin());
      sfx->Stop();
      sfx->SetList( NULL );

      for( SFX_MAP::iterator iter(mSfxMap.begin()); iter != mSfxMap.end(); iter++ )
      {
         if( iter->second != sfx )
            continue;

         mSfxMap.erase( iter );
         break;
      }

      delete   sfx;
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
SoundEffectBinder::SfxObj::SfxObj( std::string name /*= "sfxobj"*/ )
:  Sound(name),
   mList(NULL)
{
}



/**
 * Destructor.
 */
SoundEffectBinder::SfxObj::~SfxObj()
{
   Sound::Stop();
}



/**
 * Called when this object gets put on a new sound effects list.
 *
 * @param list the sound effects list this object should put itself on
 */
void
SoundEffectBinder::SfxObj::SetList( SFX_LST* list )
{
   // same list?
   if( mList == list )
   {
      // then bail out
      return;
   }

   // already on a list?
   if( mList != NULL )
   {
      // then remove ourself from it
      for( SFX_LST::iterator iter(mList->begin()); iter != mList->end(); iter++ )
      {
         if( *iter != this )
            continue;

         mList->erase( iter );
         break;
      }

      mList = NULL;
   }

   // not adding to any new list?
   if( list == NULL )
   {
      // then bail out
      return;
   }

   // put ourself on the new list
   mList = list;
   mList->push_back( this );
}
