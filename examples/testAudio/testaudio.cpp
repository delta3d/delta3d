#include <cassert>
#include <stack>
#include <iostream>

#include "testaudio.h"

#include <dtAudio/audiomanager.h>
#include <dtAudio/sound.h>
#include <dtAudio/soundeffectbinder.h>
#include <dtCore/camera.h>
#include <dtCore/effectmanager.h>
#include <dtCore/object.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/particlesystem.h>
#include <dtCore/transform.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/nodemask.h>
#include <osgGA/GUIEventAdapter>

// name spaces
using namespace dtCore;
using namespace dtABC;
using namespace dtAudio;
using namespace dtUtil;

IMPLEMENT_MANAGEMENT_LAYER(TestAudioApp)

// static member variables
unsigned int   TestAudioApp::kNumSoundFiles(4L);
const char*    TestAudioApp::kSoundFile[] =
               {
                  "sounds/pow.wav",
                  "sounds/bang.wav",
                  "sounds/helo.wav",
                  "sounds/carhorn.wav"
               };
const char*    TestAudioApp::kGfxFile[kNumGfx] =
               {
                  "StaticMeshes/flatdirt.ive",
                  "StaticMeshes/brdm.ive",
                  "StaticMeshes/uh-1n.ive"
               };
const char*    TestAudioApp::kFxFile[kNumFx] =
               {
                  "Particles/explosion.osg",
                  "Particles/smoke.osg"
               };

const char*    TestAudioApp::kFxDetonationType[kNumFx] =
               {
                  "HighExplosiveDetonation",
                  "SmokeDetonation"
               };

TestAudioApp::TestAudioApp(const std::string& configFilename /*= "config.xml"*/)
   : Application(configFilename)
   , mSndGain(1.0f)
   , mSndPitch(1.0f)
   , mLooping(false)
   , mMic(NULL)
   , mOribitMotionModel(NULL)
   , mSmokeCountA(0L)
   , mSmokeCountC(0L)
   , mRecorder(new SoundRecorder())
{
   AudioManager::Instantiate();

   for (unsigned int ii(0L); ii < kNumSoundFiles; ii++)
   {
      AudioManager::GetInstance().LoadFile(kSoundFile[ii]);
   }

   mMic = AudioManager::GetListener();
   assert(mMic);

   SetUpVisuals();

   Camera* cam = GetCamera();
   assert(cam);

   cam->AddChild(mMic);

   Transform transform(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
   mMic->SetTransform(transform, Transformable::REL_CS);

   mSFXBinder  = new dtAudio::SoundEffectBinder;
   assert(mSFXBinder.get());

   if (mFXMgr.get())
   {
      mSFXBinder->Initialize(mFXMgr.get());
      mSFXBinder->AddEffectTypeMapping(kFxDetonationType[EXPLODE], kSoundFile[1L]);
      mSFXBinder->AddEffectTypeRange(kFxDetonationType[EXPLODE], 35.0f);
   }

   CreateHelpLabel();
}

TestAudioApp::~TestAudioApp()
{
   if (mSFXBinder.get())
   {
      mSFXBinder->RemoveEffectTypeRange(kFxDetonationType[EXPLODE]);
      mSFXBinder->RemoveEffectTypeMapping(kFxDetonationType[EXPLODE]);
      mSFXBinder->Shutdown();
      mSFXBinder = NULL;
   }

   StopAllSounds();
   FreeAllStoppedSounds(true);

   for (unsigned int ii(0L); ii < kNumSoundFiles; ii++)
   {
      AudioManager::GetInstance().UnloadFile(kSoundFile[ii]);
   }

   AudioManager::Destroy();

   dtCore::System::GetInstance().TickSignal.disconnect(this);

}



void TestAudioApp::PreFrame(const double deltaFrameTime)
{
   Application::PreFrame(deltaFrameTime);
   FlushQueuedSounds();
}



void
TestAudioApp::Frame(const double deltaFrameTime)
{
   Application::Frame(deltaFrameTime);
}



void
TestAudioApp::PostFrame(const double deltaFrameTime)
{
   Application::PostFrame(deltaFrameTime);
   FreeAllStoppedSounds();

   MoveTheObject(TRUCK, deltaFrameTime);
   MoveTheObject(HELO, deltaFrameTime);
}


bool TestAudioApp::KeyPressed(const Keyboard* keyboard, int key)
{
   bool verdict = dtABC::Application::KeyPressed(keyboard, key);
   if (verdict == true)
   {
      return verdict;
   }
   osg::Vec3 pos(0.0f, 0.0f, 0.0f);

   switch (key)
   {
      case 'a':
         LoadPlaySound(kSoundFile[0L]);
         verdict = true;
         break;

      case 's':
         mFXMgr->AddDetonation(pos, kFxDetonationType[EXPLODE]);
         verdict = true;
         break;

      case 'd':
         LoadPlaySound(kSoundFile[2L], TRUCK);
         verdict = true;
         break;

      case 'f':
         LoadPlaySound(kSoundFile[3L], HELO);
         verdict = true;
         break;

      case '0':
         ChangeSoundGain(0.0f / 9.0f);
         verdict = true;
         break;

      case '1':
         ChangeSoundGain(1.0f / 9.0f);
         verdict = true;
         break;

      case '2':
         ChangeSoundGain(2.0f / 9.0f);
         verdict = true;
         break;

      case '3':
         ChangeSoundGain(3.0f / 9.0f);
         verdict = true;
         break;

      case '4':
         ChangeSoundGain(4.0f / 9.0f);
         verdict = true;
         break;

      case '5':
         ChangeSoundGain(5.0f / 9.0f);
         verdict = true;
         break;

      case '6':
         ChangeSoundGain(6.0f / 9.0f);
         verdict = true;
         break;

      case '7':
         ChangeSoundGain(7.0f / 9.0f);
         verdict = true;
         break;

      case '8':
         ChangeSoundGain(8.0f / 9.0f);
         verdict = true;
         break;

      case '9':
         ChangeSoundGain(9.0f / 9.0f);
         verdict = true;
         break;

      case '-':
      case osgGA::GUIEventAdapter::KEY_KP_Subtract:
         ChangeSoundPitch(0.9f);
         verdict = true;
         break;

      case '=':
      case osgGA::GUIEventAdapter::KEY_KP_Add:
         ChangeSoundPitch(1.1f);
         verdict = true;
         break;

      case 'l':
         ToggleSoundLooping();
         verdict = true;
         break;

      case osgGA::GUIEventAdapter::KEY_Pause:
      case 'p':
      case 'P':
         PauseAllSounds();
         verdict = true;
         break;

      case osgGA::GUIEventAdapter::KEY_Return:
      case osgGA::GUIEventAdapter::KEY_KP_Enter:
         RewindAllSounds();
         verdict = true;
         break;

      case ' ':
         StopAllSounds();
         verdict = true;
         break;

      case 'r':
         {
            if (mRecorder->GetState() == SoundRecorder::Recording)
            {
               StopRecording();
            }
            else
            {
               StartRecording();
            }
         verdict = true;
         } break;

      case 'y':
         {
            std::string sfile("soundrecord.xml");
            LOG_INFO("Saving to file: "+ sfile)
            mRecorder->SaveFile(sfile);
            LOG_INFO("...Done saving file: " + sfile)
            verdict = true;
         } break;

      case osgGA::GUIEventAdapter::KEY_F1:
      {
         mLabel->SetActive(!mLabel->GetActive());
         break;
      }

      case '~':
         this->SetNextStatisticsType();
         break;

      default:
         break;
   }

   return verdict;
}

void TestAudioApp::LoadPlaySound(const char* fname, unsigned int box /*= 0L*/)
{
   assert( fname );

   Log::GetInstance().LogMessage(Log::LOG_ALWAYS, __FUNCTION__,
      " LoadPlaySound( %s )", fname);

   Sound* snd = AudioManager::GetInstance().NewSound();
   assert(snd);

   snd->LoadFile(fname);
   snd->SetGain(mSndGain);
   snd->SetPitch(mSndPitch);
   snd->SetLooping(mLooping);
   if (box)
   {
      snd->SetRolloffFactor(0.2f);
   }
   snd->Play();
   mQueued.push(snd);

   if (box)
   {
      Object* obj   = mGfxObj[box].get();
      assert(obj);

      obj->AddChild(snd);

      Transform transform(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
      snd->SetTransform(transform, Transformable::REL_CS);

      snd->SetPlayCallback(MakeSmoke, this);
      snd->SetStopCallback(StopSmoke, this);
   }
}

void TestAudioApp::StopAllSounds(void)
{
   Sound* snd(NULL);
   SND_ITR iter;
   for (iter = mActive.begin(); iter != mActive.end(); iter++)
   {
      snd = *iter;
      if (snd == NULL)
      {
         continue;
      }

      if (!snd->IsStopped())
      {
         snd->Stop();

         Log::GetInstance().LogMessage(Log::LOG_ALWAYS, __FUNCTION__,
            " StopAllSounds( %s )", snd->GetFilename() );
      }
   }
}

void
TestAudioApp::FreeAllStoppedSounds(bool forced /*= false*/)
{
   SND_ITR              iter;
   std::stack<SND_ITR>  stk;
   Sound*               snd(NULL);

   for (iter = mActive.begin(); iter != mActive.end(); iter++)
   {
      snd = *iter;
      if (snd == NULL)
      {
         continue;
      }

      if (snd->IsStopped() || forced)
      {
         stk.push(iter);
      }
   }

   while (stk.size())
   {
      iter = stk.top();
      stk.pop();

      snd  = *iter;
      if (snd == NULL)
      {
         continue;
      }

      Log::GetInstance().LogMessage(Log::LOG_ALWAYS, __FUNCTION__,
         " FreeAllStoppedSounds( %s )", snd->GetFilename());

      AudioManager::GetInstance().FreeSound(snd);
      mActive.erase(iter);
   }
}



void
TestAudioApp::FlushQueuedSounds()
{
   Sound* snd(NULL);

   while (mQueued.size())
   {
      snd = mQueued.front();
      if (snd == NULL)
      {
         mQueued.pop();
         continue;
      }

      if (!snd->IsStopped())
      {
         mQueued.pop();
         mActive.push_back(snd);
         continue;
      }

      break;
   }
}



void
TestAudioApp::ChangeSoundGain(float gain)
{
   mSndGain = gain;

   Log::GetInstance().LogMessage(Log::LOG_ALWAYS, __FUNCTION__,
      " ChangeSoundGain( %1.1f )", mSndGain );

   Sound* snd(NULL);
   SND_ITR iter;

   for (iter = mActive.begin(); iter != mActive.end(); iter++)
   {
      snd = *iter;
      assert(snd);

      snd->SetGain(mSndGain);
   }
}



void
TestAudioApp::ChangeSoundPitch(float pitch)
{
   mSndPitch *= pitch;

   Log::GetInstance().LogMessage(Log::LOG_ALWAYS, __FUNCTION__,
      " ChangeSoundPitch( %1.4f )", mSndPitch);

   Sound*  snd(NULL);
   SND_ITR iter;

   for (iter = mActive.begin(); iter != mActive.end(); iter++)
   {
      snd = *iter;
      assert(snd);

      snd->SetPitch(mSndPitch);
   }
}



void
TestAudioApp::ToggleSoundLooping()
{
   mLooping = !mLooping;

   Log::GetInstance().LogMessage(Log::LOG_ALWAYS, __FUNCTION__,
      " ToggleSoundLooping( %d )", (mLooping));

   Sound*  snd(NULL);
   SND_ITR iter;

   for (iter = mActive.begin(); iter != mActive.end(); iter++)
   {
      snd = *iter;
      assert(snd);

      if(! snd->IsStopped())
      {
         snd->SetLooping(mLooping);
      }
   }
}


void
TestAudioApp::PauseAllSounds()
{
   Sound*  snd(NULL);
   SND_ITR iter;
   for (iter = mActive.begin(); iter != mActive.end(); iter++)
   {
      snd = *iter;
      if (snd == NULL)
      {
         continue;
      }

      snd->Pause();

      if (snd->IsPlaying())
      {
         Log::GetInstance().LogMessage(Log::LOG_ALWAYS, __FUNCTION__,
            " PauseAllSounds( %s ) paused", snd->GetFilename());
      }
      else if (snd->IsPaused())
      {
         Log::GetInstance().LogMessage(Log::LOG_ALWAYS, __FUNCTION__,
            " PauseAllSounds( %s ) un-paused", snd->GetFilename());
      }
   }
}


void
TestAudioApp::RewindAllSounds()
{
   Sound*  snd(NULL);
   SND_ITR iter;
   for (iter = mActive.begin(); iter != mActive.end(); iter++)
   {
      snd = *iter;
      if (snd == NULL)
      {
         continue;
      }

      snd->Rewind();

      Log::GetInstance().LogMessage(Log::LOG_ALWAYS, __FUNCTION__,
         " RewindAllSounds( %s )", snd->GetFilename());
   }
}


void
TestAudioApp::SetUpVisuals()
{
   for (unsigned int ii(0L); ii < kNumGfx; ii++)
   {
      mGfxObj[ii] = LoadGfxFile(kGfxFile[ii]).get();
      assert(mGfxObj[ii].get());
   }

   mFXMgr = LoadFxFile( kFxFile[EXPLODE] ).get();
   assert(mFXMgr.get());

   mPSysA = LoadPSFile( kFxFile[SMOKE] ).get();
   assert(mPSysA.get());

   mGfxObj[TRUCK]->AddChild(mPSysA.get());

   mPSysC = LoadPSFile(kFxFile[SMOKE]).get();
   assert(mPSysC.get());

   mGfxObj[HELO]->AddChild(mPSysC.get());

   InitInputDevices();
   SetUpCamera();
}



dtCore::RefPtr<Object>
TestAudioApp::LoadGfxFile(const char* fname)
{
   if (fname == NULL)
   {
      // no file name, bail...
      return NULL;
   }

   std::string filename = dtUtil::FindFileInPathList(fname);
   if (filename == "")
   {
      // still no file name, bail...
      Log::GetInstance().LogMessage( Log::LOG_WARNING,__FUNCTION__,
         "AudioManager: can't load file %s", fname );
      return NULL;
   }


   dtCore::RefPtr<Object> fileobj  = new Object;
   assert(fileobj.valid());


   if (fileobj->LoadFile(filename) == NULL)
   {
      Log::GetInstance().LogMessage(Log::LOG_WARNING,__FUNCTION__,
         "can't load gfx file '%s'", filename.c_str());
      fileobj = 0;
      return 0;
   }

   if (std::string(fname) == kGfxFile[1])
   {
      //rotate BRDM 90 degrees
      Transform trans = Transform(0.0f, 0.0f, 0.0f, 90.0f, 0.0f, 0.0f);
      fileobj->SetTransform(trans);
   }

   // add the object to the scene
   AddDrawable(fileobj.get());

   dtCore::RefPtr<osg::Node> filenode = fileobj->GetOSGNode();
   assert(filenode.valid());

   filenode->setNodeMask(dtUtil::NodeMask::EVERYTHING);
   return fileobj;
}



dtCore::RefPtr<EffectManager>
TestAudioApp::LoadFxFile(const char* fname)
{
   dtCore::RefPtr<EffectManager> effectManager = new EffectManager;
   assert(effectManager.valid());

   effectManager->AddDetonationTypeMapping(
      kFxDetonationType[EXPLODE],
      fname
   );

   AddDrawable(effectManager.get());

   return effectManager;
}



dtCore::RefPtr<ParticleSystem>
TestAudioApp::LoadPSFile(const char* fname)
{
   dtCore::RefPtr<ParticleSystem> particlesystem = new ParticleSystem;

   assert(particlesystem.valid());

   particlesystem->LoadFile(fname, false);

   particlesystem->SetEnabled(false);

   return particlesystem;
}



void
TestAudioApp::InitInputDevices()
{
   mOribitMotionModel = new OrbitMotionModel(GetKeyboard(), GetMouse());
   mOribitMotionModel->SetTarget(GetCamera());
   mOribitMotionModel->SetEnabled(true);
}



void
TestAudioApp::SetUpCamera()
{
   osg::Vec3   pos    (0.0f, -150.0f, 30.0f);
   osg::Vec3   lookat (0.0f,     0.0f,   0.0f);
   osg::Vec3   up     (0.0f,     0.0f,   1.0f);

   Transform   xform;
   xform.Set(pos, lookat, up);

   float dist((lookat - pos).length());

   Camera* cam(GetCamera());
   assert(cam);

   cam->SetTransform(xform);

   mOribitMotionModel->SetDistance(dist);
}



inline void TestAudioApp::MoveTheObject(unsigned int obj, const double deltaFrameTime)
{
   // figure out which vehicle gets what velocity
   static double X(0);
   X += deltaFrameTime * 0.2; //slow down the velocity a little
   double P(sin(X));
   double V(cos(X));
   unsigned int I((obj==TRUCK)? 1L: 0L);
   Transform xform;
   osg::Vec3 pos(0.0f, 0.0f, 0.0f);
   osg::Vec3 vel(0.0f, 0.0f, 0.0f);
   OBJ_PTR gfx(mGfxObj[obj]);

   // move the vehicle
   assert(gfx.get());

   gfx->GetTransform(xform);
   xform.GetTranslation(pos);

   pos[I] = P * 50.0; //scale position to cover the terrain

   xform.SetTranslation(pos);
   gfx->SetTransform(xform);

   // don't set velocity for TRUCK
   if (obj == TRUCK)
   {
      return;
   }

   // set the velocity for all children of the vehicle
   vel[I] = static_cast<ALfloat>(V * 50.0f);
   Sound* snd(NULL);
   for (unsigned int ii(0L); ii < gfx->GetNumChildren(); ii++)
   {
      snd = dynamic_cast<Sound*>(gfx->GetChild(ii));
      if (snd == NULL)
      {
         continue;
      }

      snd->SetVelocity(vel);
   }
}



void
TestAudioApp::MakeSmoke(dtAudio::Sound* sound, void* param)
{
   assert(sound);
   assert(param);

   sound->SetPlayCallback(NULL, NULL);

   std::string    fname = sound->GetFilename();
   TestAudioApp*  app   = static_cast<TestAudioApp*>(param);

   if (fname == app->kSoundFile[2L])
   {
      assert(app->mPSysA.get());

      app->mSmokeCountA++;

      app->mPSysA->SetEnabled(true);
      return;
   }

   if (fname == app->kSoundFile[3L])
   {
      assert(app->mPSysC.get());

      app->mSmokeCountC++;

      app->mPSysC->SetEnabled(true);
      return;
   }
}



void
TestAudioApp::StopSmoke(dtAudio::Sound* sound, void* param)
{
   assert(sound);
   assert(param);

   sound->SetStopCallback(NULL, NULL);

   std::string    fname = sound->GetFilename();
   TestAudioApp*  app   = static_cast<TestAudioApp*>(param);

   if (fname == app->kSoundFile[2L])
   {
      assert(app->mPSysA.get());

      app->mSmokeCountA--;

      if (app->mSmokeCountA)
      {
         return;
      }

      app->mPSysA->SetEnabled(false);
   }

   if (fname == app->kSoundFile[3L])
   {
      assert(app->mPSysC.get());

      app->mSmokeCountC--;

      if (app->mSmokeCountC)
      {
         return;
      }

      app->mPSysC->SetEnabled(false);
   }
}

void TestAudioApp::StartRecording()
{
   LOG_INFO("Starting Recording.")
   mRecorder->Record();
}

void TestAudioApp::StopRecording()
{
   LOG_INFO("Stopping Recording.")
   mRecorder->Stop();
}

void TestAudioApp::CreateHelpLabel()
{
   mLabel = new LabelActor();
   osg::Vec2 testSize(21.0f, 11.0f);
   mLabel->SetBackSize(testSize);
   mLabel->SetFontSize(0.8f);
   mLabel->SetTextAlignment(LabelActor::AlignmentEnum::LEFT_CENTER);
   mLabel->SetText(CreateHelpLabelText());
   mLabel->SetEnableDepthTesting(false);
   mLabel->SetEnableLighting(false);

   GetCamera()->AddChild(mLabel.get());
   Transform labelOffset(-17.0f, 50.0f, 7.5f, 0.0f, 90.0f, 0.0f);
   mLabel->SetTransform(labelOffset, Transformable::REL_CS);
   AddDrawable(GetCamera());
}

std::string TestAudioApp::CreateHelpLabelText()
{
   std::string testString("");
   testString += "F1: Toggle Help Screen\n";
   testString += "\n";
   testString += "0-9: Change volume\n";
   testString += "+/-: Increase/Decrease pitch\n";
   testString += "a: Play gunfire sound\n";
   testString += "s: Play explosion sound\n";
   testString += "d: Play helicopter hum sound\n";
   testString += "f: Play vehicle horn sound\n";
   testString += "l: Set sounds to loop\n";
   testString += "r: Record sounds\n";
   testString += "y: Save soundrecord.xml\n";
   testString += "Space: Stops all sounds\n";
   testString += "Pause: Pause all sounds\n";

   return testString;
}

int main()
{
   std::string dataPath = dtUtil::GetDeltaDataPathList();
   dtUtil::SetDataFilePathList(dataPath + ";" +
      dtUtil::GetDeltaRootPath() + "/examples/data" + ";");

   RefPtr<TestAudioApp> app = new TestAudioApp("config.xml");

   std::cout << "Key you can push: \n\n";
   std::cout << "0-9 set the gain (volume) of sounds.\n";
   std::cout << "+ and - increase/decrease pitch.\n";
   std::cout << "A plays a 'pow' sound.\n";
   std::cout << "S plays a 'boom' sound.\n";
   std::cout << "D plays a helicopter hum that is attached to the helicopter.\n";
   std::cout << "F plays a horn sounds that is attached to the vehicle.\n";
   std::cout << "L sets sounds to looping.\n";
   std::cout << "Spacebar stops all sounds.\n";
   std::cout << "Esc exits application.\n";

   app->Run();

   return 0;
}
