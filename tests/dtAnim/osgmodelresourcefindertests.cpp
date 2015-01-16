
////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtAnim/osgmodelresourcefinder.h>
#include <osg/Geode>
#include <osg/Group>
#include <osgAnimation/Animation>
#include <osgAnimation/BasicAnimationManager>
#include <osgAnimation/Bone>
#include <osgAnimation/MorphGeometry>
#include <osgAnimation/RigGeometry>
#include <osgAnimation/Skeleton>



namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class OsgModelResourceFinderTests : public CPPUNIT_NS::TestFixture
   {
      // Tests ordered in order of progression.
      CPPUNIT_TEST_SUITE( OsgModelResourceFinderTests );
      CPPUNIT_TEST( TestSearchSkeleton );
      CPPUNIT_TEST( TestSearchBones );
      CPPUNIT_TEST( TestSearchMeshes );
      CPPUNIT_TEST( TestSearchMaterials );
      CPPUNIT_TEST( TestSearchAnimations );
      CPPUNIT_TEST( TestSearchMorphs );
      CPPUNIT_TEST( TestSearchAll );
      CPPUNIT_TEST( TestClear );
      CPPUNIT_TEST( TestReset );
      CPPUNIT_TEST_SUITE_END();
      
   public:
      void setUp();
      void tearDown();

      void BuildTestModel();

      void TestClear();
      void TestReset();
      void TestSearchAll();
      void TestSearchAnimations();
      void TestSearchBones();
      void TestSearchMaterials();
      void TestSearchMeshes();
      void TestSearchMorphs();
      void TestSearchSkeleton();

   private:
      dtCore::RefPtr<osg::Group> mTestModel;
      dtCore::RefPtr<dtAnim::OsgModelResourceFinder> mFinder;

      // Character nodes
      static const std::string NAME_GEODE_A;
      static const std::string NAME_GEODE_B;
      static const std::string NAME_GEODE_C;
      dtCore::RefPtr<osgAnimation::Animation> mAnimA;
      dtCore::RefPtr<osgAnimation::Animation> mAnimB;
      dtCore::RefPtr<osgAnimation::Animation> mAnimC;
      dtCore::RefPtr<osgAnimation::BasicAnimationManager> mAnimManager;
      dtCore::RefPtr<osgAnimation::Bone> mBoneRoot;
      dtCore::RefPtr<osgAnimation::Bone> mBoneR1;
      dtCore::RefPtr<osgAnimation::Bone> mBoneR2;
      dtCore::RefPtr<osgAnimation::Bone> mBoneL1;
      dtCore::RefPtr<osgAnimation::Bone> mBoneL2;
      dtCore::RefPtr<osg::Geode> mGeodeA;
      dtCore::RefPtr<osg::Geode> mGeodeB;
      dtCore::RefPtr<osg::Geode> mGeodeC;
      dtCore::RefPtr<osg::StateSet> mMatA;
      dtCore::RefPtr<osg::StateSet> mMatB;
      dtCore::RefPtr<osg::StateSet> mMatC;
      dtCore::RefPtr<osgAnimation::RigGeometry> mMeshA;
      dtCore::RefPtr<osgAnimation::RigGeometry> mMeshB;
      dtCore::RefPtr<osgAnimation::RigGeometry> mMeshC;
      dtCore::RefPtr<osgAnimation::MorphGeometry> mMorphA;
      dtCore::RefPtr<osgAnimation::MorphGeometry> mMorphB;
      dtCore::RefPtr<osgAnimation::MorphGeometry> mMorphC;
      dtCore::RefPtr<osgAnimation::UpdateMorph> mMorphManager;
      dtCore::RefPtr<osgAnimation::Skeleton> mSkel;
   };

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION( OsgModelResourceFinderTests );
   const std::string OsgModelResourceFinderTests::NAME_GEODE_A("Geode_A");
   const std::string OsgModelResourceFinderTests::NAME_GEODE_B("Geode_B");
   const std::string OsgModelResourceFinderTests::NAME_GEODE_C("Geode_C");

   

   /////////////////////////////////////////////////////////////////////////////
   void OsgModelResourceFinderTests::setUp()
   {
      BuildTestModel();

      mFinder = new dtAnim::OsgModelResourceFinder;
   }

   /////////////////////////////////////////////////////////////////////////////
   void OsgModelResourceFinderTests::tearDown()
   {
      mFinder->Clear();
      mFinder = NULL;

      mTestModel = NULL;

      mSkel = NULL;
      mBoneRoot = NULL;
      mBoneR1 = NULL;
      mBoneR2 = NULL;
      mBoneL1 = NULL;
      mBoneL2 = NULL;
      mAnimA = NULL;
      mAnimB = NULL;
      mAnimC = NULL;
      mAnimManager = NULL;
      mGeodeA = NULL;
      mGeodeB = NULL;
      mGeodeC = NULL;
      mMeshA = NULL;
      mMeshB = NULL;
      mMeshC = NULL;
      mMorphA = NULL;
      mMorphB = NULL;
      mMorphC = NULL;
   }

   /////////////////////////////////////////////////////////////////////////////
   void OsgModelResourceFinderTests::BuildTestModel()
   {
      mTestModel = new osg::Group;
      mSkel = new osgAnimation::Skeleton;
      mBoneRoot = new osgAnimation::Bone("Root");
      mBoneR1 = new osgAnimation::Bone("R1");
      mBoneR2 = new osgAnimation::Bone("R2");
      mBoneL1 = new osgAnimation::Bone("L1");
      mBoneL2 = new osgAnimation::Bone("L2");
      mAnimA = new osgAnimation::Animation;
      mAnimB = new osgAnimation::Animation;
      mAnimC = new osgAnimation::Animation;
      mAnimManager = new osgAnimation::BasicAnimationManager;
      mGeodeA = new osg::Geode;
      mGeodeB = new osg::Geode;
      mGeodeC = new osg::Geode;
      mMeshA = new osgAnimation::RigGeometry;
      mMeshB = new osgAnimation::RigGeometry;
      mMeshC = new osgAnimation::RigGeometry;
      mMorphA = new osgAnimation::MorphGeometry;
      mMorphB = new osgAnimation::MorphGeometry;
      mMorphC = new osgAnimation::MorphGeometry;
      mMorphManager = new osgAnimation::UpdateMorph;

      mGeodeA->setName(NAME_GEODE_A);
      mGeodeA->addDrawable(mMeshA.get());
      mGeodeA->addDrawable(mMorphA.get());
      mGeodeB->setName(NAME_GEODE_B);
      mGeodeB->addDrawable(mMeshB.get());
      mGeodeB->addDrawable(mMorphB.get());
      mGeodeC->setName(NAME_GEODE_C);
      mGeodeC->addDrawable(mMeshC.get());
      mGeodeC->addDrawable(mMorphC.get());
      mMatA = mGeodeA->getOrCreateStateSet();
      mMatB = mGeodeB->getOrCreateStateSet();
      mMatC = mGeodeC->getOrCreateStateSet();
      mBoneL1->addChild(mBoneL2.get());
      mBoneR1->addChild(mBoneR2.get());
      mBoneRoot->addChild(mBoneL1.get());
      mBoneRoot->addChild(mBoneR1.get());
      mSkel->addChild(mBoneRoot.get());
      mSkel->addChild(mGeodeA.get());
      mSkel->addChild(mGeodeB.get());
      mSkel->addChild(mGeodeC.get());
      mSkel->setUpdateCallback(mAnimManager.get());
      mSkel->addUpdateCallback(mMorphManager.get());
      mAnimManager->registerAnimation(mAnimA.get());
      mAnimManager->registerAnimation(mAnimB.get());
      mAnimManager->registerAnimation(mAnimC.get());
      mTestModel->addChild(mSkel.get());
   }

   /////////////////////////////////////////////////////////////////////////////
   void OsgModelResourceFinderTests::TestClear()
   {
      mTestModel->accept(*mFinder);

      CPPUNIT_ASSERT( ! mFinder->mAnimNodes.empty());
      CPPUNIT_ASSERT( ! mFinder->mAnimManagers.empty());
      CPPUNIT_ASSERT( ! mFinder->mBones.empty());
      CPPUNIT_ASSERT( ! mFinder->mMaterials.empty());
      CPPUNIT_ASSERT( ! mFinder->mMaterialToObjectMap.empty());
      CPPUNIT_ASSERT( ! mFinder->mMeshes.empty());
      CPPUNIT_ASSERT( ! mFinder->mMorphs.empty());
      CPPUNIT_ASSERT( ! mFinder->mMorphManagers.empty());
      CPPUNIT_ASSERT(mFinder->mSkeleton.valid());
      
      // Clear should only remove gathered references but not change the search mode.
      mFinder->mMode = dtAnim::OsgModelResourceFinder::SEARCH_BONES;
      mFinder->Clear();
      CPPUNIT_ASSERT(mFinder->mMode == dtAnim::OsgModelResourceFinder::SEARCH_BONES);

      CPPUNIT_ASSERT(mFinder->mAnimNodes.empty());
      CPPUNIT_ASSERT(mFinder->mAnimManagers.empty());
      CPPUNIT_ASSERT(mFinder->mBones.empty());
      CPPUNIT_ASSERT(mFinder->mMaterials.empty());
      CPPUNIT_ASSERT(mFinder->mMaterialToObjectMap.empty());
      CPPUNIT_ASSERT(mFinder->mMeshes.empty());
      CPPUNIT_ASSERT(mFinder->mMorphs.empty());
      CPPUNIT_ASSERT(mFinder->mMorphManagers.empty());
      CPPUNIT_ASSERT( ! mFinder->mSkeleton.valid());
   }

   /////////////////////////////////////////////////////////////////////////////
   void OsgModelResourceFinderTests::TestReset()
   {
      mTestModel->accept(*mFinder);

      CPPUNIT_ASSERT( ! mFinder->mAnimNodes.empty());
      CPPUNIT_ASSERT( ! mFinder->mAnimManagers.empty());
      CPPUNIT_ASSERT( ! mFinder->mBones.empty());
      CPPUNIT_ASSERT( ! mFinder->mMaterials.empty());
      CPPUNIT_ASSERT( ! mFinder->mMaterialToObjectMap.empty());
      CPPUNIT_ASSERT( ! mFinder->mMeshes.empty());
      CPPUNIT_ASSERT( ! mFinder->mMorphs.empty());
      CPPUNIT_ASSERT( ! mFinder->mMorphManagers.empty());
      CPPUNIT_ASSERT(mFinder->mSkeleton.valid());

      // Reset should act like Clear but reset the search mode as well.
      mFinder->mMode = dtAnim::OsgModelResourceFinder::SEARCH_BONES;
      mFinder->Reset();
      CPPUNIT_ASSERT(mFinder->mMode == dtAnim::OsgModelResourceFinder::SEARCH_ALL);

      CPPUNIT_ASSERT(mFinder->mAnimNodes.empty());
      CPPUNIT_ASSERT(mFinder->mAnimManagers.empty());
      CPPUNIT_ASSERT(mFinder->mBones.empty());
      CPPUNIT_ASSERT(mFinder->mMaterials.empty());
      CPPUNIT_ASSERT(mFinder->mMaterialToObjectMap.empty());
      CPPUNIT_ASSERT(mFinder->mMeshes.empty());
      CPPUNIT_ASSERT(mFinder->mMorphs.empty());
      CPPUNIT_ASSERT(mFinder->mMorphManagers.empty());
      CPPUNIT_ASSERT( ! mFinder->mSkeleton.valid());
   }

   /////////////////////////////////////////////////////////////////////////////
   void OsgModelResourceFinderTests::TestSearchAll()
   {
      CPPUNIT_ASSERT(mFinder->mMode == dtAnim::OsgModelResourceFinder::SEARCH_ALL);
      
      mTestModel->accept(*mFinder);

      CPPUNIT_ASSERT(mFinder->mAnimNodes.size() == 1);
      CPPUNIT_ASSERT(mFinder->mAnimManagers.size() == 1);
      CPPUNIT_ASSERT(mFinder->mBones.size() == 5);
      CPPUNIT_ASSERT(mFinder->mMaterials.size() == 3);
      CPPUNIT_ASSERT(mFinder->mMaterialToObjectMap.size() == 3);
      CPPUNIT_ASSERT(mFinder->mMeshes.size() == 3);
      CPPUNIT_ASSERT(mFinder->mMorphs.size() == 3);
      CPPUNIT_ASSERT(mFinder->mMorphManagers.size() == 1);
      CPPUNIT_ASSERT(mFinder->mSkeleton.valid());
      
      dtAnim::OsgModelResourceFinder::OsgAnimationArray anims;
      CPPUNIT_ASSERT(mFinder->GetAnimations(anims));
      CPPUNIT_ASSERT(anims.size() == 3);

      // Ensure that the search mode did not change.
      CPPUNIT_ASSERT(mFinder->mMode == dtAnim::OsgModelResourceFinder::SEARCH_ALL);
   }

   /////////////////////////////////////////////////////////////////////////////
   void OsgModelResourceFinderTests::TestSearchAnimations()
   {
      mFinder->mMode = dtAnim::OsgModelResourceFinder::SEARCH_ANIMATIONS;
      mTestModel->accept(*mFinder);
      CPPUNIT_ASSERT(mFinder->mAnimManagers.size() == 1);
      CPPUNIT_ASSERT(mFinder->mAnimNodes.size() == 1);

      dtAnim::OsgModelResourceFinder::OsgAnimationArray anims;
      CPPUNIT_ASSERT(mFinder->GetAnimations(anims));
      CPPUNIT_ASSERT(anims.size() == 3);

      // Verify no other references were gathered.
      CPPUNIT_ASSERT(mFinder->mBones.empty());
      CPPUNIT_ASSERT(mFinder->mMaterials.empty());
      CPPUNIT_ASSERT(mFinder->mMaterialToObjectMap.empty());
      CPPUNIT_ASSERT(mFinder->mMeshes.empty());
      CPPUNIT_ASSERT(mFinder->mMorphs.empty());
      CPPUNIT_ASSERT(mFinder->mMorphManagers.empty());
      CPPUNIT_ASSERT( ! mFinder->mSkeleton.valid());

      // Ensure that the search mode did not change.
      CPPUNIT_ASSERT(mFinder->mMode == dtAnim::OsgModelResourceFinder::SEARCH_ANIMATIONS);
   }

   /////////////////////////////////////////////////////////////////////////////
   void OsgModelResourceFinderTests::TestSearchBones()
   {
      mFinder->mMode = dtAnim::OsgModelResourceFinder::SEARCH_BONES;
      mTestModel->accept(*mFinder);
      CPPUNIT_ASSERT(mFinder->mSkeleton.valid());
      CPPUNIT_ASSERT(mFinder->mBones.size() == 5);

      // Verify no other references were gathered.
      CPPUNIT_ASSERT(mFinder->mAnimNodes.empty());
      CPPUNIT_ASSERT(mFinder->mAnimManagers.empty());
      CPPUNIT_ASSERT(mFinder->mMaterials.empty());
      CPPUNIT_ASSERT(mFinder->mMaterialToObjectMap.empty());
      CPPUNIT_ASSERT(mFinder->mMeshes.empty());
      CPPUNIT_ASSERT(mFinder->mMorphs.empty());
      CPPUNIT_ASSERT(mFinder->mMorphManagers.empty());

      // Ensure that the search mode did not change.
      CPPUNIT_ASSERT(mFinder->mMode == dtAnim::OsgModelResourceFinder::SEARCH_BONES);
   }

   /////////////////////////////////////////////////////////////////////////////
   void OsgModelResourceFinderTests::TestSearchMaterials()
   {
      mFinder->mMode = dtAnim::OsgModelResourceFinder::SEARCH_MATERIALS;
      mTestModel->accept(*mFinder);
      CPPUNIT_ASSERT(mFinder->mMaterials.size() == 3);
      CPPUNIT_ASSERT(mFinder->mMaterialToObjectMap.size() == 3);

      // Verify no other references were gathered.
      CPPUNIT_ASSERT(mFinder->mAnimNodes.empty());
      CPPUNIT_ASSERT(mFinder->mAnimManagers.empty());
      CPPUNIT_ASSERT(mFinder->mBones.empty());
      CPPUNIT_ASSERT(mFinder->mMeshes.empty());
      CPPUNIT_ASSERT(mFinder->mMorphs.empty());
      CPPUNIT_ASSERT(mFinder->mMorphManagers.empty());
      CPPUNIT_ASSERT( ! mFinder->mSkeleton.valid());

      // Ensure that the search mode did not change.
      CPPUNIT_ASSERT(mFinder->mMode == dtAnim::OsgModelResourceFinder::SEARCH_MATERIALS);
   }

   /////////////////////////////////////////////////////////////////////////////
   void OsgModelResourceFinderTests::TestSearchMeshes()
   {
      mFinder->mMode = dtAnim::OsgModelResourceFinder::SEARCH_MESHES;
      mTestModel->accept(*mFinder);
      CPPUNIT_ASSERT(mFinder->mMeshes.size() == 3);

      CPPUNIT_ASSERT(mFinder->GetGeodeByName(NAME_GEODE_A) == mGeodeA.get());
      CPPUNIT_ASSERT(mFinder->GetGeodeByName(NAME_GEODE_B) == mGeodeB.get());
      CPPUNIT_ASSERT(mFinder->GetGeodeByName(NAME_GEODE_C) == mGeodeC.get());

      // Verify no other references were gathered.
      CPPUNIT_ASSERT(mFinder->mAnimNodes.empty());
      CPPUNIT_ASSERT(mFinder->mAnimManagers.empty());
      CPPUNIT_ASSERT(mFinder->mBones.empty());
      CPPUNIT_ASSERT(mFinder->mMaterials.empty());
      CPPUNIT_ASSERT(mFinder->mMaterialToObjectMap.empty());
      CPPUNIT_ASSERT(mFinder->mMorphs.empty());
      CPPUNIT_ASSERT(mFinder->mMorphManagers.empty());
      CPPUNIT_ASSERT( ! mFinder->mSkeleton.valid());

      // Ensure that the search mode did not change.
      CPPUNIT_ASSERT(mFinder->mMode == dtAnim::OsgModelResourceFinder::SEARCH_MESHES);
   }

   /////////////////////////////////////////////////////////////////////////////
   void OsgModelResourceFinderTests::TestSearchMorphs()
   {
      mFinder->mMode = dtAnim::OsgModelResourceFinder::SEARCH_MORPHS;
      mTestModel->accept(*mFinder);
      CPPUNIT_ASSERT(mFinder->mMorphs.size() == 3);
      CPPUNIT_ASSERT(mFinder->mMorphManagers.size() == 1);

      // Verify no other references were gathered.
      CPPUNIT_ASSERT(mFinder->mAnimNodes.empty());
      CPPUNIT_ASSERT(mFinder->mAnimManagers.empty());
      CPPUNIT_ASSERT(mFinder->mBones.empty());
      CPPUNIT_ASSERT(mFinder->mMaterials.empty());
      CPPUNIT_ASSERT(mFinder->mMaterialToObjectMap.empty());
      CPPUNIT_ASSERT(mFinder->mMeshes.empty());
      CPPUNIT_ASSERT( ! mFinder->mSkeleton.valid());

      // Ensure that the search mode did not change.
      CPPUNIT_ASSERT(mFinder->mMode == dtAnim::OsgModelResourceFinder::SEARCH_MORPHS);
   }

   /////////////////////////////////////////////////////////////////////////////
   void OsgModelResourceFinderTests::TestSearchSkeleton()
   {
      mFinder->mMode = dtAnim::OsgModelResourceFinder::SEARCH_SKELETON;
      mTestModel->accept(*mFinder);
      CPPUNIT_ASSERT(mFinder->mSkeleton.valid());

      // Verify no other references were gathered.
      CPPUNIT_ASSERT(mFinder->mAnimNodes.empty());
      CPPUNIT_ASSERT(mFinder->mAnimManagers.empty());
      CPPUNIT_ASSERT(mFinder->mBones.empty());
      CPPUNIT_ASSERT(mFinder->mMaterials.empty());
      CPPUNIT_ASSERT(mFinder->mMaterialToObjectMap.empty());
      CPPUNIT_ASSERT(mFinder->mMeshes.empty());
      CPPUNIT_ASSERT(mFinder->mMorphs.empty());
      CPPUNIT_ASSERT(mFinder->mMorphManagers.empty());

      // Ensure that the search mode did not change.
      CPPUNIT_ASSERT(mFinder->mMode == dtAnim::OsgModelResourceFinder::SEARCH_SKELETON);
   }

} // END - namespace dtAnim
