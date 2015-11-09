#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/sigslot.h>
#include <dtAnim/animationhelper.h>
#include "../dtGame/basegmtests.h"

class AnimModelLoadingTestFixture: public dtGame::BaseGMTestFixture, public sigslot::has_slots<>
{
public:
   AnimModelLoadingTestFixture()
   : mModelLoaded()
   , mModelUnloaded()
   {

   }

   void GetRequiredLibraries(NameVector& namesOut) override
   {
      dtGame::BaseGMTestFixture::GetRequiredLibraries(namesOut);
      namesOut.push_back("dtAnim");
   }

   void Connect(dtAnim::AnimationHelper* helper)
   {
      helper->ModelLoadedSignal.connect_slot(this, &AnimModelLoadingTestFixture::OnModelLoaded);
      helper->ModelUnloadedSignal.connect_slot(this, &AnimModelLoadingTestFixture::OnModelUnloaded);
   }

   void LoadModel(dtAnim::AnimationHelper* helper, const dtCore::ResourceDescriptor& rd)
   {
      mModelUnloaded = mModelLoaded = false;
      helper->LoadModel(rd);
      int count = 10;
      while (!mModelLoaded && count > 0)
      {
         helper->CheckLoadingState();
         dtCore::AppSleep(50);
         --count;
      }
      CPPUNIT_ASSERT(mModelLoaded);
      CPPUNIT_ASSERT(helper->GetModelWrapper() != NULL);
      CPPUNIT_ASSERT(helper->GetNode() != NULL);
   }

   void OnModelLoaded(dtAnim::AnimationHelper*) { mModelLoaded = true; }
   void OnModelUnloaded(dtAnim::AnimationHelper*) { mModelUnloaded = true;}
   bool mModelLoaded, mModelUnloaded;
};
