#include <assert.h>
#include <stack>

#include "testAudioApp.h"



// name spaces
using namespace   dtABC;
using namespace   dtAudio;
using namespace   std;



// static member variables
const char*    testAudioApp::kDataPath = "../../data";
unsigned int   testAudioApp::kNumSoundFiles(4L);
const char*    testAudioApp::kSoundFile[] =
               {
                  "pow.wav",
                  "bang.wav",
                  "tone_C_264_sawtooth.wav",
                  "tone_A_440_sawtooth.wav"
               };
const char*    testAudioApp::kGfxFile[kNumGfx] =
               {
                  "ground.flt",
                  "box.flt",
                  "box.flt",
               };
const char*    testAudioApp::kFxFile[kNumFx] =
               {
                  "explosion.osg",
                  "smoke.osg"
               };



testAudioApp::testAudioApp( string configFilename /*= ""*/ )
:  Application(configFilename),
   mSndGain(1.0f),
   mSndPitch(1.0f),
   mLooping(false),
   mMic(NULL),
   mInputDevice(NULL),
   mMotionModel(NULL),
   mSmokeCountA(0L),
   mSmokeCountC(0L)
{
   dtCore::SetDataFilePathList( kDataPath );

   AddSender( dtCore::System::GetSystem() );

   AudioManager::Instantiate();

   AudioConfigData   data;//(32L, true);
   AudioManager::GetManager()->Config( data );

   for( unsigned int ii(0L); ii < kNumSoundFiles; ii++ )
   {
      AudioManager::GetManager()->LoadWaveFile( kSoundFile[ii] );
   }

   mMic  = AudioManager::GetListener();
   assert( mMic );

   SetUpVisuals();

   dtCore::Camera*   cam   = GetCamera();
   assert( cam );

   cam->AddChild( mMic );

   dtCore::Transform transform( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
   mMic->SetTransform( &transform, dtCore::Transformable::REL_CS );

   mSFXBinder  = new dtAudio::SoundEffectBinder;
   assert( mSFXBinder.get() );

   if( mFXMgr.get() )
   {
      mSFXBinder->Initialize( mFXMgr.get() );
      mSFXBinder->AddEffectTypeMapping( dtCore::HighExplosiveDetonation, kSoundFile[1L] );
      mSFXBinder->AddEffectTypeRange( dtCore::HighExplosiveDetonation, 35.0f );
   }

   dtCore::Notify( dtCore::ALWAYS, " " );
   dtCore::Notify( dtCore::ALWAYS, "   [A]       plays a sound" );
   dtCore::Notify( dtCore::ALWAYS, "   [S]       plays a sound bound to an effect" );
   dtCore::Notify( dtCore::ALWAYS, "   [D]       plays a sound with panning" );
   dtCore::Notify( dtCore::ALWAYS, "   [F]       plays a sound with panning and dopler" );
   dtCore::Notify( dtCore::ALWAYS, "   [0-9]     sets gain" );
   dtCore::Notify( dtCore::ALWAYS, "   [+|-]     rase|lower pitch" );
   dtCore::Notify( dtCore::ALWAYS, "   [L]       set|unset all sounds looping" );
   dtCore::Notify( dtCore::ALWAYS, "   [space]   stops all sounds" );
   dtCore::Notify( dtCore::ALWAYS, "   [enter]   rewinds all sounds" );
   dtCore::Notify( dtCore::ALWAYS, "   [pause]   pause|unpause all sounds" );
   dtCore::Notify( dtCore::ALWAYS, " " );
}



testAudioApp::~testAudioApp()
{
   if( mSFXBinder.get() )
   {
      mSFXBinder->RemoveEffectTypeRange( dtCore::HighExplosiveDetonation );
      mSFXBinder->RemoveEffectTypeMapping( dtCore::HighExplosiveDetonation );
      mSFXBinder->Shutdown();
      mSFXBinder  = NULL;
   }

   StopAllSounds();
   FreeAllStoppedSounds( true );

   for( unsigned int ii(0L); ii < kNumSoundFiles; ii++ )
   {
      AudioManager::GetManager()->UnloadWaveFile( kSoundFile[ii] );
   }

   AudioManager::Destroy();

   RemoveSender( dtCore::System::GetSystem() );
}



void
testAudioApp::PreFrame( const double deltaFrameTime )
{
   Application::PreFrame( deltaFrameTime );
   FlushQueuedSounds();
}



void
testAudioApp::Frame( const double deltaFrameTime )
{
   Application::Frame( deltaFrameTime );
}



void
testAudioApp::PostFrame( const double deltaFrameTime )
{
   Application::PostFrame( deltaFrameTime );
   FreeAllStoppedSounds();

   MoveTheStupidBox( BOX_A );
   MoveTheStupidBox( BOX_C );
}



void
testAudioApp::KeyPressed(  dtCore::Keyboard*       keyboard,
                           Producer::KeyboardKey   key,
                           Producer::KeyCharacter  character   )
{
   dtABC::Application::KeyPressed( keyboard, key, character );
   sgVec3   pos   = { 0.0f, 0.0f, 0.0f };

   switch( key )
   {
      case  Producer::Key_A:
         LoadPlaySound( kSoundFile[0L] );
         break;

      case  Producer::Key_S:
         mFXMgr->AddDetonation( pos, dtCore::HighExplosiveDetonation );
         break;

      case  Producer::Key_D:
         LoadPlaySound( kSoundFile[2L], BOX_A );
         break;

      case  Producer::Key_F:
         LoadPlaySound( kSoundFile[3L], BOX_C );
         break;

      case  Producer::Key_0:
      case  Producer::Key_KP_Insert:
         ChangeSoundGain( 0.0f / 9.0f );
         break;

      case  Producer::Key_1:
      case  Producer::Key_KP_End:
         ChangeSoundGain( 1.0f / 9.0f );
         break;

      case  Producer::Key_2:
      case  Producer::Key_KP_Down:
         ChangeSoundGain( 2.0f / 9.0f );
         break;

      case  Producer::Key_3:
      case  Producer::Key_KP_Page_Down:
         ChangeSoundGain( 3.0f / 9.0f );
         break;

      case  Producer::Key_4:
      case  Producer::Key_KP_Left:
         ChangeSoundGain( 4.0f / 9.0f );
         break;

      case  Producer::Key_5:
      case  Producer::Key_KP_Begin:
         ChangeSoundGain( 5.0f / 9.0f );
         break;

      case  Producer::Key_6:
      case  Producer::Key_KP_Right:
         ChangeSoundGain( 6.0f / 9.0f );
         break;

      case  Producer::Key_7:
      case  Producer::Key_KP_Home:
         ChangeSoundGain( 7.0f / 9.0f );
         break;

      case  Producer::Key_8:
      case  Producer::Key_KP_Up:
         ChangeSoundGain( 8.0f / 9.0f );
         break;

      case  Producer::Key_9:
      case  Producer::Key_KP_Page_Up:
         ChangeSoundGain( 9.0f / 9.0f );
         break;

      case  Producer::Key_minus:
      case  Producer::Key_KP_Subtract:
         ChangeSoundPitch( 0.9f );
         break;

      case  Producer::Key_equal:
      case  Producer::Key_KP_Add:
         ChangeSoundPitch( 1.1f );
         break;

      case  Producer::Key_L:
         ToggleSoundLooping();
         break;

      case  Producer::Key_Pause:
         PauseAllSounds();
         break;

      case  Producer::Key_Return:
      case  Producer::Key_KP_Enter:
         RewindAllSounds();
         break;

      case  Producer::Key_space:
         StopAllSounds();
         break;

      default:
         break;
   }
}



void
testAudioApp::LoadPlaySound( const char* fname, unsigned int box /*= 0L*/ )
{
   assert( fname );

   dtCore::Notify( dtCore::ALWAYS, " LoadPlaySound( %s )", fname );

   Sound*   snd = AudioManager::GetManager()->NewSound();
   assert( snd );

   snd->LoadFile( fname );
   snd->SetGain( mSndGain );
   snd->SetPitch( mSndPitch );
   snd->SetLooping( mLooping );
   if( box )
   {
      snd->SetMinDistance( 30.0f );
      snd->SetRolloffFactor( 10.0f );
   }
   snd->Play();
   mQueued.push( snd );

   if( box )
   {
      dtCore::Object*  obj   = mGfxObj[box].get();
      assert( obj );

      obj->AddChild( snd );

      dtCore::Transform transform( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
      snd->SetTransform( &transform, dtCore::Transformable::REL_CS );

      snd->SetPlayCallback( MakeSmoke, this );
      snd->SetStopCallback( StopSmoke, this );
   }
}



void
testAudioApp::StopAllSounds( void )
{
   Sound*   snd(NULL);
   SND_ITR  iter(NULL);
   for( iter = mActive.begin(); iter != mActive.end(); iter++ )
   {
      snd   = *iter;
      if( snd == NULL )
         continue;

      if( ! snd->IsStopped() )
      {
         snd->Stop();

         dtCore::Notify( dtCore::ALWAYS, " StopAllSounds( %s )", snd->GetFilename() );
      }
   }
}



void
testAudioApp::FreeAllStoppedSounds( bool forced /*= false*/ )
{
   SND_ITR              iter(NULL);
   std::stack<SND_ITR>  stk;
   Sound*               snd(NULL);

   for( iter = mActive.begin(); iter != mActive.end(); iter++ )
   {
      snd   = *iter;
      if( snd == NULL )
         continue;

      if( snd->IsStopped() || forced )
      {
         stk.push( iter );
      }
   }

   while( stk.size() )
   {
      iter  = stk.top();
      stk.pop();

      snd   = *iter;
      if( snd == NULL )
         continue;

      dtCore::Notify( dtCore::ALWAYS, " FreeAllStoppedSounds( %s )", snd->GetFilename() );

      AudioManager::GetManager()->FreeSound( snd );
      mActive.erase( iter );
   }
}



void
testAudioApp::FlushQueuedSounds( void )
{
   Sound*   snd(NULL);

   while( mQueued.size() )
   {
      snd   = mQueued.front();
      if( snd == NULL )
      {
         mQueued.pop();
         continue;
      }

      if( ! snd->IsStopped() )
      {
         mQueued.pop();
         mActive.push_back( snd );
         continue;
      }

      break;
   }
}



void
testAudioApp::ChangeSoundGain( float gain )
{
   mSndGain = gain;

   dtCore::Notify( dtCore::ALWAYS, " ChangeSoundGain( %1.1f )", mSndGain );

   Sound*   snd(NULL);
   SND_ITR  iter(NULL);

   for( iter = mActive.begin(); iter != mActive.end(); iter++ )
   {
      snd   = *iter;
      assert( snd );

      snd->SetGain( mSndGain );
   }
}



void
testAudioApp::ChangeSoundPitch( float pitch )
{
   mSndPitch   *= pitch;

   dtCore::Notify( dtCore::ALWAYS, " ChangeSoundPitch( %1.4f )", mSndPitch );

   Sound*   snd(NULL);
   SND_ITR  iter(NULL);

   for( iter = mActive.begin(); iter != mActive.end(); iter++ )
   {
      snd   = *iter;
      assert( snd );

      snd->SetPitch( mSndPitch );
   }
}



void
testAudioApp::ToggleSoundLooping( void )
{
   mLooping =  !mLooping;

   dtCore::Notify( dtCore::ALWAYS, " ToggleSoundLooping( %s )", (mLooping)? "true": "false" );

   Sound*   snd(NULL);
   SND_ITR  iter(NULL);

   for( iter = mActive.begin(); iter != mActive.end(); iter++ )
   {
      snd   = *iter;
      assert( snd );

      snd->SetLooping( mLooping );
   }
}



void
testAudioApp::PauseAllSounds( void )
{
   Sound*   snd(NULL);
   SND_ITR  iter(NULL);
   for( iter = mActive.begin(); iter != mActive.end(); iter++ )
   {
      snd   = *iter;
      if( snd == NULL )
         continue;

      snd->Pause();

      if( snd->IsPlaying() )
      {
         dtCore::Notify( dtCore::ALWAYS, " PauseAllSounds( %s ) paused", snd->GetFilename() );
      }
      else  if( snd->IsPaused() )
      {
         dtCore::Notify( dtCore::ALWAYS, " PauseAllSounds( %s ) un-paused", snd->GetFilename() );
      }
   }
}



void
testAudioApp::RewindAllSounds( void )
{
   Sound*   snd(NULL);
   SND_ITR  iter(NULL);
   for( iter = mActive.begin(); iter != mActive.end(); iter++ )
   {
      snd   = *iter;
      if( snd == NULL )
         continue;

      snd->Rewind();

      dtCore::Notify( dtCore::ALWAYS, " RewindAllSounds( %s )", snd->GetFilename() );
   }
}



void
testAudioApp::SetUpVisuals( void )
{
   for( unsigned int ii(0L); ii < kNumGfx; ii++ )
   {
      mGfxObj[ii] = LoadGfxFile( kGfxFile[ii] );
      assert( mGfxObj[ii].get() );
   }

   mFXMgr   = LoadFxFile( kFxFile[EXPLODE] );
   assert( mFXMgr.get() );

   mPSysA   = LoadPSFile( kFxFile[SMOKE] );
   assert( mPSysA.get() );

   mGfxObj[BOX_A]->AddChild( mPSysA.get() );

   mPSysC   = LoadPSFile( kFxFile[SMOKE] );
   assert( mPSysC.get() );

   mGfxObj[BOX_C]->AddChild( mPSysC.get() );

   InitInputDevices();
   SetUpCamera();
}



dtCore::Object*
testAudioApp::LoadGfxFile( const char* fname )
{
   if( fname == NULL )
      // no file name, bail...
      return   NULL;

   std::string filename = osgDB::findDataFile( fname );
   if( filename == "" )
   {
      // still no file name, bail...
      dtCore::Notify( dtCore::WARN, "AudioManager: can't load file %s", fname );
      return   NULL;
   }


   dtCore::Object*  fileobj  = new dtCore::Object;
   assert( fileobj );


   // load the graphics file from disk
   bool fileLoaded = false;
   fileLoaded = fileobj->LoadFile( filename );

   if( ! fileLoaded )
   {
      dtCore::Notify( dtCore::WARN, "can't load gfx file '%s'", filename.c_str() );
      delete   fileobj;
      return   NULL;
   }

   // add the object to the scene
   AddDrawable( fileobj );

   osg::Node*  filenode = fileobj->GetOSGNode();
   assert( filenode );

   filenode->setNodeMask( 0xFFFFFFFF );
   return   fileobj;
}



dtCore::EffectManager*
testAudioApp::LoadFxFile( const char* fname )
{
   dtCore::EffectManager* effectManager  = new dtCore::EffectManager;
   assert( effectManager );

   effectManager->AddDetonationTypeMapping(
      dtCore::HighExplosiveDetonation,
      fname
   );

   AddDrawable( effectManager );

   return   effectManager;
}



dtCore::ParticleSystem*
testAudioApp::LoadPSFile( const char* fname )
{
   dtCore::ParticleSystem*   particlesystem = new dtCore::ParticleSystem;
   assert( particlesystem );
   
   particlesystem->LoadFile( fname );

   osg::Node*  filenode = particlesystem->GetOSGNode();
   assert( filenode );

   filenode->setNodeMask( 0x00000000 );

   AddDrawable( particlesystem );

   return   particlesystem;
}



void
testAudioApp::InitInputDevices( void )
{
   mInputDevice   = new dtCore::LogicalInputDevice;
   assert( mInputDevice );

   dtCore::Keyboard* k  = GetKeyboard();
   assert( k );

   dtCore::Mouse*    m  = GetMouse();
   assert( m );

   dtCore::Axis* leftButtonUpAndDown  =
         mInputDevice->AddAxis(
                                 "left mouse button up/down",
                                 new dtCore::ButtonAxisToAxis(
                                       m->GetButton( dtCore::LeftButton ),
                                       m->GetAxis( 1 )
                                                             )
                              );


   dtCore::Axis* leftButtonLeftAndRight  =
         mInputDevice->AddAxis(
                                 "left mouse button left/right",
                                 new dtCore::ButtonAxisToAxis(
                                       m->GetButton( dtCore::LeftButton ),
                                       m->GetAxis( 0 )
                                                             )
                              );


   dtCore::Axis* middleButtonUpAndDown   =
         mInputDevice->AddAxis(
                                 "middle mouse button up/down",
                                 new dtCore::ButtonAxisToAxis(
                                       m->GetButton( dtCore::MiddleButton ),
                                       m->GetAxis( 1 )
                                                             )
                              );


   dtCore::Axis* rightButtonUpAndDown    =
         mInputDevice->AddAxis(
                                 "right mouse button up/down",
                                 new dtCore::ButtonAxisToAxis(
                                       m->GetButton( dtCore::RightButton ),
                                       m->GetAxis( 1 )
                                                             )
                              );


   dtCore::Axis* rightButtonLeftAndRight =
         mInputDevice->AddAxis(
                                 "right mouse button left/right",
                                 new dtCore::ButtonAxisToAxis(
                                       m->GetButton( dtCore::RightButton ),
                                       m->GetAxis( 0 )
                                                             )
                              );


   dtCore::Axis* arrowKeysUpAndDown      =
         mInputDevice->AddAxis(
                                 "arrow keys up/down",
                                 new dtCore::ButtonsToAxis(
                                       k->GetButton( Producer::Key_Down ),
                                       k->GetButton( Producer::Key_Up )
                                                          )
                              );


   dtCore::Axis* arrowKeysLeftAndRight   =
         mInputDevice->AddAxis(
                                 "arrow keys left/right",
                                 new dtCore::ButtonsToAxis(
                                       k->GetButton( Producer::Key_Left ),
                                       k->GetButton( Producer::Key_Right )
                                                          )
                              );


   dtCore::Axis* wsKeysUpAndDown         =
         mInputDevice->AddAxis(
                                 "w/s keys up/down",
                                 new dtCore::ButtonsToAxis(
                                       k->GetButton( Producer::Key_S ),
                                       k->GetButton( Producer::Key_W )
                                                          )
                              );


   dtCore::Axis* adKeysLeftAndRight      =
         mInputDevice->AddAxis(
                                 "a/d keys left/right",
                                 new dtCore::ButtonsToAxis(
                                       k->GetButton( Producer::Key_A ),
                                       k->GetButton( Producer::Key_D )
                                                          )
                              );


   dtCore::Axis* primaryUpAndDown        =
         mInputDevice->AddAxis(
                                 "primary up/down",
                                 new dtCore::AxesToAxis(
                                       arrowKeysUpAndDown,
                                       leftButtonUpAndDown
                                                       )
                              );


   dtCore::Axis* secondaryUpAndDown      =
         mInputDevice->AddAxis(
                                 "secondary up/down",
                                 new dtCore::AxesToAxis(
                                       wsKeysUpAndDown,
                                       rightButtonUpAndDown
                                                       )
                              );


   dtCore::Axis* primaryLeftAndRight     =
         mInputDevice->AddAxis(
                                 "primary left/right",
                                 new dtCore::AxesToAxis(
                                       arrowKeysLeftAndRight,
                                       leftButtonLeftAndRight
                                                       )
                              );


   dtCore::Axis* secondaryLeftAndRight   =
         mInputDevice->AddAxis(
                                 "secondary left/right",
                                 new dtCore::AxesToAxis(
                                       adKeysLeftAndRight,
                                       rightButtonLeftAndRight
                                                       )
                              );


   dtCore::OrbitMotionModel* omm   = new dtCore::OrbitMotionModel;
   assert( omm );

   omm->SetAzimuthAxis( primaryLeftAndRight );
   omm->SetElevationAxis( primaryUpAndDown );
   omm->SetDistanceAxis( middleButtonUpAndDown );
   omm->SetLeftRightTranslationAxis( secondaryLeftAndRight );
   omm->SetUpDownTranslationAxis( secondaryUpAndDown );
   mMotionModel   = omm;

   mMotionModel->SetTarget( GetCamera() );
   mMotionModel->SetEnabled( true );
}



void
testAudioApp::SetUpCamera( void )
{
   sgVec3   pos      = { 0.0f, -150.0f, 30.0f };
   sgVec3   lookat   = { 0.f, 0.f, 0.f };
   sgVec3   up       = { 0.f, 0.f, 1.f };

   dtCore::Transform   xform;
   xform.SetLookAt( pos, lookat, up );

   float dist(sgDistanceVec3( lookat, pos ));

   dtCore::Camera*   cam(GetCamera());
   assert( cam );

   cam->SetTransform( &xform );


   dtCore::OrbitMotionModel*  omm   =
      static_cast<dtCore::OrbitMotionModel*>(mMotionModel);
   assert( omm );

   omm->SetDistance( dist );
}



void
testAudioApp::MoveTheStupidBox( unsigned int box )
{
   // figure out which box gets what velocity
   static   long              X(0L);
   static   const double      A(1.0f/50.0f);
   static   const double      C(1.0f/75.0f);
            double            D((box==BOX_C)? C: A);
            double            P(sin( double(X++) * D ));
            double            V(cos( double(X++) * D ));
            unsigned int      I((box==BOX_C)? 1L: 0L);
            dtCore::Transform xform;
            sgVec3            pos   = { 0.0f, 0.0f, 0.0f };
            sgVec3            vel   = { 0.0f, 0.0f, 0.0f };
            OBJ_PTR           gfx(mGfxObj[box]);

   // move the stupid little box
   assert( gfx.get() );

   gfx->GetTransform( &xform );
   xform.GetTranslation( pos );

   pos[I]   = static_cast<SGfloat>(P * 50.0f);

   xform.SetTranslation( pos );
   gfx->SetTransform( &xform );

   // don't set velocity for BOX_A
   if( box == BOX_A )
      return;

   // set the velocity for all children of the box
   vel[I]   = static_cast<ALfloat>(V * 50.0f);
   Sound*   snd(NULL);
   for( unsigned int ii(0L); ii < gfx->GetNumChildren(); ii++ )
   {
      snd   = dynamic_cast<Sound*>(gfx->GetChild( ii ));
      if( snd == NULL )
         continue;

      snd->SetVelocity( vel );
   }
}



void
testAudioApp::MakeSmoke( dtAudio::Sound* sound, void* param )
{
   assert( sound );
   assert( param );

   sound->SetPlayCallback( NULL, NULL );

   std::string    fname = sound->GetFilename();
   testAudioApp*  app   = static_cast<testAudioApp*>(param);

   if( fname == app->kSoundFile[2L] )
   {
      assert( app->mPSysA.get() );

      app->mSmokeCountA++;

      osg::Node*  node  = app->mPSysA->GetOSGNode();
      assert( node );

      node->setNodeMask( 0xFFFFFFFF );
      return;
   }

   if( fname == app->kSoundFile[3L] )
   {
      assert( app->mPSysC.get() );

      app->mSmokeCountC++;

      osg::Node*  node  = app->mPSysC->GetOSGNode();
      assert( node );

      node->setNodeMask( 0xFFFFFFFF );
      return;
   }
}



void
testAudioApp::StopSmoke( dtAudio::Sound* sound, void* param )
{
   assert( sound );
   assert( param );

   sound->SetStopCallback( NULL, NULL );

   std::string    fname = sound->GetFilename();
   testAudioApp*  app   = static_cast<testAudioApp*>(param);

   if( fname == app->kSoundFile[2L] )
   {
      assert( app->mPSysA.get() );

      app->mSmokeCountA--;

      if( app->mSmokeCountA )
         return;

      osg::Node*  node  = app->mPSysA->GetOSGNode();
      assert( node );

      node->setNodeMask( 0x00000000 );
   }

   if( fname == app->kSoundFile[3L] )
   {
      assert( app->mPSysC.get() );

      app->mSmokeCountC--;

      if( app->mSmokeCountC )
         return;

      osg::Node*  node  = app->mPSysC->GetOSGNode();
      assert( node );

      node->setNodeMask( 0x00000000 );
   }
}
