/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2008, MOVES Institute
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
* Erik Johnson
*/


#include <cppunit/extensions/HelperMacros.h>
#include <cal3d/coremodel.h>
#include <cal3d/model.h>
#include <cal3d/error.h>
#include <cal3d/corebone.h>
#include <cal3d/coretrack.h>
#include <cal3d/coresubmesh.h>
#include <cal3d/mesh.h>
#include <cal3d/submesh.h>
#include <cal3d/physique.h>
#include <dtCore/globals.h>
#include <cal3d/coresubmorphtarget.h>
#include <cal3d/morphtargetmixer.h>

#if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
   #define TEST_MORPH_TARGET
#endif


#ifdef TEST_MORPH_TARGET
   #include <cal3d/coreanimatedmorph.h>
#endif



class CAL3DLoadingTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE( CAL3DLoadingTests );
   CPPUNIT_TEST( TestLoadSkeleton );
   CPPUNIT_TEST( CorrectNumberOfBones );
   CPPUNIT_TEST( CorrectBoneIdAndName );
   CPPUNIT_TEST( CorrectBoneData );
   CPPUNIT_TEST( TestLoadAnimation );
   CPPUNIT_TEST( CorrectAnimationData );
   CPPUNIT_TEST( CorrectTrackData );
   CPPUNIT_TEST( TestLoadMesh );
   CPPUNIT_TEST( CorrectMeshData );
   CPPUNIT_TEST( TestLoadMaterial );
   CPPUNIT_TEST( CorrectMaterialData );


#ifdef TEST_MORPH_TARGET
   CPPUNIT_TEST( SceneAmbientColor );
   CPPUNIT_TEST( TestLoadMorphAnimation );
   CPPUNIT_TEST( CorrectMorphAnimationData );
   CPPUNIT_TEST( TestLoadMorphMesh );
   CPPUNIT_TEST( TestMorphWeights );
   CPPUNIT_TEST( TestScaleMorphedMesh );
   CPPUNIT_TEST( TestPlayAnimatedMorph );
#endif

   CPPUNIT_TEST_SUITE_END();

public:

   CAL3DLoadingTests():
      kNumBones(87)
      {};

   ~CAL3DLoadingTests() {};

   virtual void setUp();
   void TestLoadSkeleton();
   void CorrectNumberOfBones();
   void CorrectBoneIdAndName();
   void CorrectBoneData();
   void TestLoadAnimation();
   void CorrectAnimationData();
   void CorrectTrackData();
   void TestLoadMesh();
   void CorrectMeshData();
   void TestLoadMaterial();
   void CorrectMaterialData();

#ifdef TEST_MORPH_TARGET
   void SceneAmbientColor();
   void TestLoadMorphAnimation();
   void CorrectMorphAnimationData();
   void TestLoadMorphMesh();
   void TestMorphWeights();
   void TestScaleMorphedMesh();
   void TestPlayAnimatedMorph();
#endif

private:
   std::string mMorphSkelFile;
   std::string mAnimFile;
   std::string mMeshFile;
   std::string mMorphMeshFile;
   std::string mMaterialFile;
   std::string mMorphAnimationFile;
   const unsigned int kNumBones;
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( CAL3DLoadingTests );

//////////////////////////////////////////////////////////////////////////
void CAL3DLoadingTests::setUp()
{
   const std::string dataPath = dtCore::GetDeltaRootPath() + "/tests/data/ProjectContext/";
   mMorphSkelFile = dtCore::FindFileInPathList(dataPath + "SkeletalMeshes/Skeleton.xsf");
   mAnimFile = dtCore::FindFileInPathList(dataPath + "SkeletalMeshes/ERightIn.xaf");
   mMeshFile = dtCore::FindFileInPathList(dataPath + "SkeletalMeshes/Helmet.xmf");
   mMorphMeshFile = dtCore::FindFileInPathList(dataPath + "SkeletalMeshes/test_head.xmf");
   mMaterialFile = dtCore::FindFileInPathList(dataPath + "SkeletalMeshes/Helmet.xrf");
   mMorphAnimationFile = dtCore::FindFileInPathList(dataPath + "SkeletalMeshes/talking_head.xpf");
}


//////////////////////////////////////////////////////////////////////////
void CAL3DLoadingTests::TestLoadSkeleton()
{
   CalCoreModel* model = new CalCoreModel("testskel");

   bool result = model->loadCoreSkeleton(mMorphSkelFile);

   std::ostringstream ss;
   ss << "Skeleton file did not load : " << CalError::getLastErrorDescription() << " " <<
         CalError::getLastErrorFile() << " " << CalError::getLastErrorLine();

   CPPUNIT_ASSERT_EQUAL_MESSAGE(ss.str(),
                                true, result);

   delete model;
}

//////////////////////////////////////////////////////////////////////////
void CAL3DLoadingTests::CorrectNumberOfBones()
{
   CalCoreModel* model = new CalCoreModel("testskel");

   CPPUNIT_ASSERT(model->loadCoreSkeleton(mMorphSkelFile));

   CalCoreSkeleton* skel = model->getCoreSkeleton();

#ifdef TEST_MORPH_TARGET
   CPPUNIT_ASSERT_EQUAL_MESSAGE("CalCoreSkeleton doesn't have the right number of bones.",
                                kNumBones, skel->getNumCoreBones() );
#else
   CPPUNIT_ASSERT_EQUAL_MESSAGE("CalCoreSkeleton doesn't have the right number of bones.",
      size_t(kNumBones), skel->getVectorCoreBone().size() );
#endif
   delete model;
}


//////////////////////////////////////////////////////////////////////////
void CAL3DLoadingTests::CorrectBoneIdAndName()
{
   CalCoreModel* model = new CalCoreModel("testskel");

   CPPUNIT_ASSERT(model->loadCoreSkeleton(mMorphSkelFile));

   CalCoreSkeleton* skel = model->getCoreSkeleton();
   CalCoreBone* bone = skel->getCoreBone(3);

   CPPUNIT_ASSERT_MESSAGE("CalCoreSkeleton doesn't have bone '3'.",
                          bone != NULL );

   CPPUNIT_ASSERT_EQUAL_MESSAGE("CalCoreBone doesn't have the right name",
      std::string("lfThigh"), bone->getName());

   delete model;
}

//////////////////////////////////////////////////////////////////////////
void CAL3DLoadingTests::CorrectBoneData()
{
   CalCoreModel* model = new CalCoreModel("testskel");

   CPPUNIT_ASSERT(model->loadCoreSkeleton(mMorphSkelFile));

   CalCoreSkeleton* skel = model->getCoreSkeleton();
   CalCoreBone* bone = skel->getCoreBone(17);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Bone 17 doesn't have the right number of children",
                                size_t(3), bone->getListChildId().size());

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Bone 17 doesn't have the right parent id",
                                16, bone->getParentId());
   delete model;
}

//////////////////////////////////////////////////////////////////////////
void CAL3DLoadingTests::TestLoadAnimation()
{
   CalCoreModel* model = new CalCoreModel("testskel");

   CPPUNIT_ASSERT(model->loadCoreSkeleton(mMorphSkelFile)); //need a skeleton to load an animation

   const int animId = model->loadCoreAnimation(mAnimFile);

   std::ostringstream ss;
   ss << "Animation file did not load : " << CalError::getLastErrorDescription() << " " <<
          CalError::getLastErrorFile() << " " << CalError::getLastErrorLine();

   CPPUNIT_ASSERT_EQUAL_MESSAGE(ss.str(), 0, animId);

#ifdef TEST_MORPH_TARGET
   CPPUNIT_ASSERT_EQUAL_MESSAGE("CalCoreModel doesn't have the correct number of loaded animations.",
      1, model->getNumCoreAnimations() );
#else
   CPPUNIT_ASSERT_EQUAL_MESSAGE("CalCoreModel doesn't have the correct number of loaded animations.",
      1, model->getCoreAnimationCount() );

#endif

   CPPUNIT_ASSERT_MESSAGE("CalCoreModel didn't return back the loaded animation.",
                           model->getCoreAnimation(animId) != NULL );

   delete model;
}

//////////////////////////////////////////////////////////////////////////
void CAL3DLoadingTests::CorrectAnimationData()
{
   CalCoreModel* model = new CalCoreModel("testskel");

   CPPUNIT_ASSERT(model->loadCoreSkeleton(mMorphSkelFile)); //need a skeleton to load an animation

   const int animId = model->loadCoreAnimation(mAnimFile);

   CalCoreAnimation* anim = model->getCoreAnimation(animId);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("CalCoreAnimation doesn't have the correct num tracks.",
                                 unsigned (1), anim->getTrackCount());

   CPPUNIT_ASSERT_EQUAL_MESSAGE("CalCoreAnimation doesn't have the correct duration.",
                                2.f, anim->getDuration());

   delete model;

}

//////////////////////////////////////////////////////////////////////////
void CAL3DLoadingTests::CorrectTrackData()
{
   CalCoreModel* model = new CalCoreModel("testskel");

   CPPUNIT_ASSERT(model->loadCoreSkeleton(mMorphSkelFile)); //need a skeleton to load an animation
   const int animId = model->loadCoreAnimation(mAnimFile);
   CalCoreAnimation* anim = model->getCoreAnimation(animId);
   CalCoreTrack* track = anim->getCoreTrack(12);

   CPPUNIT_ASSERT_MESSAGE("CalCoreAnimation didn't return back the CalCoreTrack",
                          track != NULL);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("CalCoreTrack doesn't have the correct number of keyframes.",
      62, track->getCoreKeyframeCount());



   delete model;

}

//////////////////////////////////////////////////////////////////////////
void CAL3DLoadingTests::TestLoadMesh()
{
   CalCoreModel* model = new CalCoreModel("testskel");
   CPPUNIT_ASSERT(model->loadCoreSkeleton(mMorphSkelFile)); //need a skeleton to load an animation
   const int meshID = model->loadCoreMesh(mMeshFile);

   std::ostringstream ss;
   ss << "Mesh file did not load : " << CalError::getLastErrorDescription() << " " <<
      CalError::getLastErrorFile() << " " << CalError::getLastErrorLine();

   CPPUNIT_ASSERT_EQUAL_MESSAGE(ss.str(), 0, meshID);

   delete model;
}

//////////////////////////////////////////////////////////////////////////
void CAL3DLoadingTests::CorrectMeshData()
{
   CalCoreModel* model = new CalCoreModel("testskel");
   CPPUNIT_ASSERT(model->loadCoreSkeleton(mMorphSkelFile)); //need a skeleton to load an animation
   const int meshID = model->loadCoreMesh(mMeshFile);

   CPPUNIT_ASSERT_MESSAGE("CalCoreModel did not return the correct mesh.",
                          model->getCoreMesh(meshID) != NULL);


   CPPUNIT_ASSERT_EQUAL_MESSAGE("CalCoreModel did not return the correct number of meshes.",
                                1, model->getCoreMeshCount());

   CalCoreMesh* mesh = model->getCoreMesh(meshID);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("CalCoreModel did not return the correct number of submeshes.",
                                1, mesh->getCoreSubmeshCount());

   CalCoreSubmesh* submesh = mesh->getCoreSubmesh(0);
   CPPUNIT_ASSERT_MESSAGE("CalCoreMesh did not return the correct submesh.",
                           submesh != NULL);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("CalCoreSubMesh did not return the correct number of vertices.",
                                 77, submesh->getVertexCount());

   CPPUNIT_ASSERT_EQUAL_MESSAGE("CalCoreSubMesh did not return the correct number of faces.",
      141, submesh->getFaceCount());

   CPPUNIT_ASSERT_EQUAL_MESSAGE("CalCoreSubMesh did not return the correct number of LODs.",
      0, submesh->getLodCount());

   CPPUNIT_ASSERT_EQUAL_MESSAGE("CalCoreSubMesh did not return the correct number of springs.",
      0, submesh->getSpringCount());

   CPPUNIT_ASSERT_EQUAL_MESSAGE("CalCoreSubMesh did not return the correct number of SubMorphTargets.",
      0, submesh->getCoreSubMorphTargetCount());

   const std::vector<CalCoreSubmesh::Vertex> vertices = submesh->getVectorVertex();
   CPPUNIT_ASSERT_EQUAL_MESSAGE("CalCoreSubMesh did not return a correctly sized Vertex vector",
                                size_t(77), vertices.size());

   const CalCoreSubmesh::Vertex vert = vertices[0];
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Vertex did not return a correctly sized influence vector",
                                size_t(1), vert.vectorInfluence.size());

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Influence boneID is incorrect.",
      int(8), vert.vectorInfluence[0].boneId);

   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Influence weight is incorrect.",
      float(1.f), vert.vectorInfluence[0].weight, 0.0001f);


   delete model;
}

//////////////////////////////////////////////////////////////////////////
void CAL3DLoadingTests::TestLoadMaterial()
{
   CalCoreModel* model = new CalCoreModel("testskel");
   CPPUNIT_ASSERT(model->loadCoreSkeleton(mMorphSkelFile)); //need a skeleton to load an animation

   const int matID = model->loadCoreMaterial(mMaterialFile);

   std::ostringstream ss;
   ss << "Material file did not load : " << CalError::getLastErrorDescription() << " " <<
      CalError::getLastErrorFile() << " " << CalError::getLastErrorLine();

   CPPUNIT_ASSERT_EQUAL_MESSAGE(ss.str(), 0, matID);

   delete model;
}

//////////////////////////////////////////////////////////////////////////
void CAL3DLoadingTests::CorrectMaterialData()
{
   CalCoreModel* model = new CalCoreModel("testskel");
   CPPUNIT_ASSERT(model->loadCoreSkeleton(mMorphSkelFile)); //need a skeleton to load an animation

   const int matID = model->loadCoreMaterial(mMaterialFile);

   model->getCoreMaterial(matID);

   delete model;
}



#ifdef TEST_MORPH_TARGET

//////////////////////////////////////////////////////////////////////////
void CAL3DLoadingTests::SceneAmbientColor()
{
   CalCoreModel* model = new CalCoreModel("testskel");

   bool result = model->loadCoreSkeleton(mMorphSkelFile);
   CalVector kSceneAmbientColor(0.1f, 0.2f, 0.3f);

   const CalCoreSkeleton* skel = model->getCoreSkeleton();
   CalVector color;
   skel->getSceneAmbientColor(color);

   CPPUNIT_ASSERT_MESSAGE("CalCoreSkeleton doesn't have the right scene ambient color.",
      kSceneAmbientColor == color );
   delete model;
}


//////////////////////////////////////////////////////////////////////////
void CAL3DLoadingTests::TestLoadMorphAnimation()
{
   CalCoreModel* model = new CalCoreModel("testskel");
   bool result = model->loadCoreSkeleton(mMorphSkelFile); //need a skeleton to load an animation

   const int morphID = model->loadCoreAnimatedMorph(mMorphAnimationFile);

   std::ostringstream ss;
   ss << "AnimatedMorph file did not load : " << CalError::getLastErrorDescription() << " " <<
      CalError::getLastErrorFile() << " " << CalError::getLastErrorLine();

   CPPUNIT_ASSERT_EQUAL_MESSAGE(ss.str(), 0, morphID);

   delete model;
}

//////////////////////////////////////////////////////////////////////////
void CAL3DLoadingTests::CorrectMorphAnimationData()
{
   CalCoreModel* model = new CalCoreModel("testskel");
   bool result = model->loadCoreSkeleton(mMorphSkelFile); //need a skeleton to load an animation

   const int morphID = model->loadCoreAnimatedMorph(mMorphAnimationFile);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("CoreModel doesn't have the correct number of morph animations.",
                                1, model->getNumCoreAnimatedMorphs());

   CalCoreAnimatedMorph* morph = model->getCoreAnimatedMorph(morphID);
   CPPUNIT_ASSERT_MESSAGE("CalModel didn't return back the correct morph animation.",
                           morph != NULL);

   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("CalCoreAnimatedMorph didn't have the correct duration",
                        11.3333, morph->getDuration(), 0.0001f);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("CalCoreAnimatedMorph didn't have the correct number of tracks.",
      unsigned (9), morph->getListCoreTrack().size());

   const CalCoreMorphTrack* track = morph->getCoreTrack("aah_Head.Exclusive");
   CPPUNIT_ASSERT_MESSAGE("CalCoreAnimatedMorph didn't return back the correct CalCoreMorphTrack.",
                          track!=NULL);

   const CalCoreMorphTrack* track2 = morph->getCoreTrack("E_Head.Exclusive");
   CPPUNIT_ASSERT_MESSAGE("CalCoreAnimatedMorph didn't return back the correct CalCoreMorphTrack.",
                          track2!=NULL);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("CalCoreMorphTrack didn't have the correct number of keyframes.",
      341, track->getCoreMorphKeyframeCount());

   const CalCoreMorphKeyframe* keyframe = track->getCoreMorphKeyframe(1);
   CPPUNIT_ASSERT_MESSAGE("CalCoreMorphTrack didn't return the correct keyframe.",
                         keyframe != NULL);

   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("CalCoreMorphKeyFrame doesn't have the correct time.",
      0.0333333, keyframe->getTime(), 0.00001f );

   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("CalCoreMorphKeyFrame doesn't have the correct weight.",
      0.123, keyframe->getWeight(), 0.00001f );


   delete model;

}

//////////////////////////////////////////////////////////////////////////
void CAL3DLoadingTests::TestLoadMorphMesh()
{
   CalCoreModel* model = new CalCoreModel("testskel");
   bool result = model->loadCoreSkeleton(mMorphSkelFile); //need a skeleton to load an animation
   const int meshID = model->loadCoreMesh(mMorphMeshFile);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("CalCoreModel didn't load the morph mesh file.",
                                0, meshID);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("CalCoreModel did not return the correct number of meshes.",
                               1, model->getCoreMeshCount());

   const CalCoreMesh* mesh = model->getCoreMesh(meshID);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("CalCoreModel did not return the correct number of submeshes.",
                               1, mesh->getCoreSubmeshCount());

   const CalCoreSubmesh* submesh = mesh->getCoreSubmesh(0);
   CPPUNIT_ASSERT_MESSAGE("CalCoreMesh did not return the correct submesh.",
                          submesh != NULL);

   const std::vector<CalCoreSubmesh::Vertex> verts = submesh->getVectorVertex();

   CPPUNIT_ASSERT_EQUAL_MESSAGE("CalCoreSubmesh didn't return back the correct number of verts",
                                size_t(64), verts.size());

   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("CalCoreSubMesh did not return the correct vertex x.",
                                        -19.1491f, verts[2].position.x, 0.0001f);
   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("CalCoreSubMesh did not return the correct vertex y.",
                                        26.3569f, verts[2].position.y, 0.0001f);
   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("CalCoreSubMesh did not return the correct vertex z.",
                                        1084.84f, verts[2].position.z, 0.0001f);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("CalCoreSubMesh did not return the correct number of SubMorphTargets.",
                                2, submesh->getCoreSubMorphTargetCount());

   const CalCoreSubMorphTarget* morphTarget = submesh->getCoreSubMorphTarget(0);
   CPPUNIT_ASSERT_MESSAGE("CalCoreMesh did not return the correct CalCoreSubMorphTarget.",
                          morphTarget != NULL);

   //CPPUNIT_ASSERT_EQUAL_MESSAGE("CalCoreSubMorphTarget did not return the correct number of blended vertices.",
   //   1559, morphTarget->getBlendVertexCount());
   //TODO why is this failing?

   CPPUNIT_ASSERT_EQUAL_MESSAGE("CalCoreSubMorphTarget did not return the correct name.",
                                std::string("aah_Head.Exclusive"), morphTarget->name());

   CPPUNIT_ASSERT_EQUAL_MESSAGE("CalCoreSubMorphTarget did not return the correct type.",
                                 CalMorphTargetTypeExclusive, morphTarget->morphTargetType());


   const CalCoreSubMorphTarget::BlendVertex *vert = morphTarget->getBlendVertex(26);
   CPPUNIT_ASSERT_MESSAGE("CalCoreSubMorphTarget did not return the correct BlendVertex.",
                           vert != NULL);

   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("BlendVertex doesn't have the correct X",
                                        0.0f, vert->position.x, 0.0001f);

   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("BlendVertex doesn't have the correct Y",
                                        -52.7146f, vert->position.y, 0.0001f);

   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("BlendVertex doesn't have the correct Z",
                                        1077.13f, vert->position.z, 0.0001f);

   delete model;
}

//////////////////////////////////////////////////////////////////////////
void CAL3DLoadingTests::TestMorphWeights()
{
   CalCoreModel* coreModel = new CalCoreModel("testskel");
   bool result = coreModel->loadCoreSkeleton(mMorphSkelFile); //need a skeleton to load an animation
   const int meshID = coreModel->loadCoreMesh(mMorphMeshFile);

   CalModel* model = new CalModel(coreModel);
   model->attachMesh(0);
   CalMesh* mesh = model->getMesh(0);

   CPPUNIT_ASSERT_MESSAGE("CalModel didn't have the correct mesh.",
                          mesh!= NULL);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("CalMesh didn't have the correct number of submesh.",
      1, mesh->getSubmeshCount());

   CalSubmesh* subMesh = mesh->getSubmesh(0);

   const int weightCount = subMesh->getMorphTargetWeightCount();

   float baseWeight = subMesh->getBaseWeight();

   const std::vector<float> weights = subMesh->getVectorMorphTargetWeight();

   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("CalSubMesh doesn't have the correct morph target weight",
      0.f, subMesh->getMorphTargetWeight(0), 0.0001f);

   model->update(0.1f);


   float vertBuff[1000][3];
   const int numverts = model->getPhysique()->calculateVertices(subMesh, &vertBuff[0][0]);

   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("CalModel's unmoved vert is in the incorrect position",
      1057.13f, vertBuff[26][2], 0.001f);

   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("CalModel's unmoved vert is in the incorrect position",
      1057.13f, vertBuff[27][2], 0.001f);




   subMesh->clearMorphTargetScales();
   subMesh->clearMorphTargetState("aah_Head.Exclusive");

   const float setWeight = 1.f;
   subMesh->setMorphTargetWeight("aah_Head.Exclusive", setWeight);

   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("CalSubMesh doesn't have the correct morph target weight",
      setWeight, subMesh->getMorphTargetWeight(0), 0.0001f);


   float vertBuff2[1000][3];
   model->getPhysique()->calculateVertices(subMesh, &vertBuff2[0][0]);

   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("CalModel's moved vert is in the incorrect position",
      1077.13f, vertBuff2[26][2], 0.001f);

   // get vertex vector of the core submesh
   std::vector<CalCoreSubmesh::Vertex>& vectorVertex = subMesh->getCoreSubmesh()->getVectorVertex();


   delete model;
   delete coreModel;
}

//////////////////////////////////////////////////////////////////////////
void CAL3DLoadingTests::TestScaleMorphedMesh()
{
   CalCoreModel* coreModel = new CalCoreModel("testskel");
   bool result = coreModel->loadCoreSkeleton(mMorphSkelFile); //need a skeleton to load an animation
   const int meshID = coreModel->loadCoreMesh(mMorphMeshFile);
   coreModel->scale(0.001f);

   CalModel* model = new CalModel(coreModel);
   model->attachMesh(0);
   CalMesh* mesh = model->getMesh(0);

   model->update(0.1f);

   CalSubmesh* subMesh = mesh->getSubmesh(0);
   std::vector<CalCoreSubmesh::Vertex>& vectorVertex = subMesh->getCoreSubmesh()->getVectorVertex();

   float vertBuff[1000][3];
   const int numverts = model->getPhysique()->calculateVertices(subMesh, &vertBuff[0][0]);


   const float setWeight = 1.f;
   subMesh->setMorphTargetWeight("aah_Head.Exclusive", setWeight);


   float vertBuff2[1000][3];
   model->getPhysique()->calculateVertices(subMesh, &vertBuff2[0][0]);


   delete model;
   delete coreModel;
}

//////////////////////////////////////////////////////////////////////////
void CAL3DLoadingTests::TestPlayAnimatedMorph()
{
   CalCoreModel* coreModel = new CalCoreModel("testskel");
   bool result = coreModel->loadCoreSkeleton(mMorphSkelFile); //need a skeleton to load an animation
   const int coreMeshID = coreModel->loadCoreMesh(mMorphMeshFile);

   const int morphID = coreModel->loadCoreAnimatedMorph(mMorphAnimationFile);

   CalModel* model = new CalModel(coreModel);
   model->attachMesh(coreMeshID);

   model->getMorphTargetMixer()->blend(morphID, 1.f, 0.f);

   model->update(1.18f);

   delete model;
   delete coreModel;
}
#endif  //TEST_MORPH_TARGET
