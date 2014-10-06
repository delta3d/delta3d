#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/sigslot.h>
#include <dtAnim/animationhelper.h>

class AnimModelLoadingTestFixture: public CPPUNIT_NS::TestFixture, public sigslot::has_slots<>
{
public:
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
         helper->Update(0.016f);
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
