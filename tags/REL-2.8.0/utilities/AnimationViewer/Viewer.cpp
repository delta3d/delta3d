/// Included above Viewer.h to avoid a compile error in linux
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtGui/QColor>
#include <QtGui/QMessageBox>

#include "Viewer.h"

#include <dtCore/system.h>
#include <dtCore/transform.h>
#include <dtCore/scene.h>
#include <dtCore/camera.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/light.h>
#include <dtCore/deltawin.h>

#include <dtAnim/characterfilehandler.h>
#include <dtAnim/chardrawable.h>
#include <dtAnim/characterfilehandler.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/chardrawable.h>
#include <dtAnim/cal3ddatabase.h>
#include <dtAnim/cal3dmodeldata.h>
#include <dtAnim/posemesh.h>
#include <dtAnim/characterwrapper.h>

#include <dtCore/basexmlreaderwriter.h>

#include <dtUtil/datapathutils.h>
#include <dtUtil/xercesparser.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/log.h>
#include <dtUtil/mathdefines.h>

#include <xercesc/sax/SAXParseException.hpp>  // for base class
#include <xercesc/util/XMLString.hpp>

#include <osg/Geode>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include <osg/Material>
#include <osg/MatrixTransform>

#include <osgDB/WriteFile>

#include <osgViewer/GraphicsWindow>
#include <osgViewer/CompositeViewer>

#include <cal3d/animation.h>
#include <cal3d/coresubmorphtarget.h>
#include <cal3d/mesh.h>
#include <cal3d/submesh.h>
#include <cal3d/coremodel.h>
#include <cal3d/coresubmesh.h>
#include <cal3d/coretrack.h>
#include <cal3d/morphtargetmixer.h>

#include <dtAnim/hotspotdriver.h>
#include <dtCore/hotspotattachment.h>
#include <dtCore/refptr.h>
#include <dtCore/pointaxis.h>

#include <dtAnim/macros.h>
#include <dtAnim/cal3danimator.h>
#include <cal3d/model.h>
#include <cal3d/mixer.h>
#include <cal3d/skeleton.h>
#include <cal3d/bone.h>

typedef std::vector<dtCore::RefPtr<dtCore::HotSpotAttachment> > VectorHotSpot;

using namespace dtUtil;
using namespace dtCore;
using namespace dtAnim;

////////////////////////////////////////////////////////////////////////////////
Viewer::Viewer()
 : dtABC::Application()
 , mCalDatabase(&Cal3DDatabase::GetInstance())
 , mPoseMeshes(NULL)
 , mAttachmentObject(NULL)
{
   dtUtil::Log::GetInstance().SetLogLevel(dtUtil::Log::LOG_DEBUG);
}

////////////////////////////////////////////////////////////////////////////////
Viewer::~Viewer()
{
}

////////////////////////////////////////////////////////////////////////////////
osg::Geode* MakePlane()
{
   osg::Geode* geode = new osg::Geode();
   osg::Box* box = new osg::Box(osg::Vec3(0.0f,0.0f,-0.025f), 2.5f, 2.5f, 0.01f);
   osg::ShapeDrawable* shapeDrawable = new osg::ShapeDrawable(box);

   osg::Material* material = new osg::Material;
   material->setTransparency(osg::Material::FRONT, 0.5f);
   material->setAlpha(osg::Material::FRONT, 0.5f);

   osg::StateSet* ss = geode->getOrCreateStateSet();
   ss->setAttributeAndModes(material, osg::StateAttribute::ON);
   ss->setMode(GL_BLEND, osg::StateAttribute::ON);

   geode->addDrawable(shapeDrawable);

   return geode;
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::Config()
{
   dtABC::Application::Config();
   GetCompositeViewer()->setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);
   //osg::Camera* camera = GetCamera()->GetOSGCamera();
   //camera->setViewport(new osg::Viewport(0,0,mGLWidget->width(),mGLWidget->height()));
   //camera->setProjectionMatrixAsPerspective(30.0f,
   //         static_cast<double>(mGLWidget->width())/static_cast<double>(mGLWidget->height()), 1.0f, 10000.0f);

   std::string exampleDataPath = dtUtil::GetEnvironment("DELTA_ROOT");
   std::string rootDataPath    = dtUtil::GetEnvironment("DELTA_DATA");
   exampleDataPath += "/examples/data;" + rootDataPath;

   dtUtil::SetDataFilePathList(dtUtil::GetDataFilePathList() + ";" + exampleDataPath);

   // Ensure bind poses are allowed by default.
   Cal3DModelWrapper::SetAllowBindPose(true);

   //adjust the Camera position
   dtCore::Transform camPos;
   osg::Vec3 camXYZ(0.0f, 5.0f, 1.0f);
   osg::Vec3 lookAtXYZ (0.0f, 0.0f, 1.0f);
   osg::Vec3 upVec (0.0f, 0.0f, 1.0f);
   camPos.Set(camXYZ, lookAtXYZ, upVec);

   GetCamera()->SetTransform(camPos);
   GetCamera()->SetNearFarCullingMode(dtCore::Camera::NO_AUTO_NEAR_FAR);

   double vfov, aspectRatio, nearClip, farClip;
   GetCamera()->GetPerspectiveParams(vfov, aspectRatio, nearClip, farClip);
   GetCamera()->SetPerspectiveParams(vfov, aspectRatio, 0.25f, farClip);

   mMotion = new OrbitMotionModel(GetKeyboard(), GetMouse());
   mMotion->SetTarget(GetCamera());
   mMotion->SetDistance(5.0f);

   Light* l = GetScene()->GetLight(0);
   l->SetAmbient(0.7f, 0.7f, 0.7f, 1.0f);
   l->SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f);

   GetScene()->GetSceneNode()->addChild(MakePlane());

   mWireDecorator  = new osg::Group;
   mShadeDecorator = new osg::Group;
   mBoneBasisGroup = new osg::Group;

   InitWireDecorator();
   InitShadeDecorator();

   OnSetShaded();

   Log::GetInstance().SetLogLevel(Log::LOG_DEBUG);
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::OnNewCharFile()
{
   OnUnloadCharFile();

   // TODO:
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::OnLoadCharFile(const QString& filename)
{
   LOG_DEBUG("Loading file: " + filename.toStdString());

   QDir dir(filename);
   dir.cdUp();

   dtUtil::SetDataFilePathList(dtUtil::GetDataFilePathList() + ";" +
                       dir.path().toStdString() + ";");

   OnUnloadCharFile();

   dtAnim::Cal3DModelData* modelData = NULL;

   //create an instance from the character definition file
   try
   {
      // Create a new Cal3DWrapper
      dtCore::RefPtr<Cal3DModelWrapper> wrapper = mCalDatabase->Load(filename.toStdString());
      if (wrapper.valid() == false)
      {
         emit ErrorOccured("Problem encountered loading file.  See log file.");
         return;
      }

      mCharacter = new CharDrawable(wrapper.get());
      mAttachmentController = new dtAnim::AttachmentController;

      // Retrieve the data to check for the inclusion of an IK pose mesh file
      modelData = mCalDatabase->GetModelData(*wrapper.get());

      if (!modelData->GetPoseMeshFilename().empty())
      {
         OnLoadPoseMeshFile(modelData->GetPoseMeshFilename());
      }

      // Determine where a temp file should be for the current character.
      mTempFile = osgDB::getFilePath(modelData->GetFilename());
      if (!mTempFile.empty())
      {
         mTempFile += "/_Temp.dtchar";
      }
   }
   catch (const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& e)
   {
      //something bad happened while parsing, we should get out of here
      char* msg = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(e.getMessage());

      QString errorMsg;
      errorMsg = QString("Parsing error at line %1 : %2")
                        .arg(e.getLineNumber())
                        .arg(msg);

      XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&msg);
      emit ErrorOccured(errorMsg);
      return;
   }

   // set up the viewer's scene graph
   mShadeDecorator->addChild(mCharacter->GetOSGNode());
   mWireDecorator->addChild(mCharacter->GetOSGNode());

   dtCore::RefPtr<Cal3DModelWrapper> wrapper = mCharacter->GetCal3DWrapper();

   //get all the data for animations and tell the world
   for (int animID=0; animID<wrapper->GetCoreAnimationCount(); animID++)
   {
      QString nameToSend = QString::fromStdString(wrapper->GetCoreAnimationName(animID));
      unsigned int trackCount = wrapper->GetCoreAnimationTrackCount(animID);
      unsigned int keyframes = wrapper->GetCoreAnimationKeyframeCount(animID);
      float dur = wrapper->GetCoreAnimationDuration(animID);
      emit AnimationLoaded(animID, nameToSend, trackCount, keyframes, dur);
   }

#ifdef CAL3D_VERSION_DEVELOPMENT
   CalMorphTargetMixer *mixer = wrapper->GetCalModel()->getMorphTargetMixer();
   if (mixer)
   {
      for (int animID = 0; animID < mixer->getMorphTargetCount(); animID++)
      {
         std::string name = mixer->getMorphName(animID).c_str();
         QString nameToSend = name.c_str();
         unsigned int trackCount = mixer->getTrackCount(animID);
         unsigned int keyframes = mixer->getKeyframeCount(animID);
         float dur = mixer->getDuration(animID);
         emit MorphAnimationLoaded(animID, nameToSend, trackCount, keyframes, dur);
      }
   }
#endif

   std::vector<std::string> bones;
   wrapper->GetCoreBoneNames(bones);

   //get all data for the meshes and emit
   for (int meshID = 0; meshID < wrapper->GetCoreMeshCount(); ++meshID)
   {
      CalCoreMesh* currentMesh = wrapper->GetCalModel()->getCoreModel()->getCoreMesh(meshID);

      // If the mesh is currently loaded
      if (currentMesh)
      {
         QString nameToSend = QString::fromStdString(currentMesh->getName());

         emit MeshLoaded(meshID, nameToSend, bones);

         const std::vector<CalCoreSubmesh *> subMeshVec = currentMesh->getVectorCoreSubmesh();
         for (size_t subMeshID = 0; subMeshID < subMeshVec.size(); ++subMeshID)
         {
            const std::vector<CalCoreSubMorphTarget *> morphVec = subMeshVec[subMeshID]->getVectorCoreSubMorphTarget();
            for (size_t morphID = 0; morphID < morphVec.size(); ++morphID)
            {
#ifdef CAL3D_VERSION_DEVELOPMENT
               QString nameToSend = QString::fromStdString(morphVec[morphID]->name());
#else
               QString nameToSend = QString::number(morphID);
#endif
               emit SubMorphTargetLoaded(meshID, subMeshID, morphID, nameToSend);
            }
         }
      }

   }

   //get all material data and emit
   for (int matID = 0; matID < wrapper->GetCoreMaterialCount(); matID++)
   {
      QString nameToSend = QString::fromStdString(wrapper->GetCoreMaterialName(matID));

      osg::Vec4 diffuse = wrapper->GetCoreMaterialDiffuse(matID);
      QColor diffColor( (int)(diffuse[0]), (int)(diffuse[1]), (int)(diffuse[2]), (int)(diffuse[3]) );

      osg::Vec4 ambient = wrapper->GetCoreMaterialAmbient(matID);
      QColor ambColor( (int)(ambient[0]), (int)(ambient[1]), (int)(ambient[2]), (int)(ambient[3]) );

      osg::Vec4 specular = wrapper->GetCoreMaterialSpecular(matID);
      QColor specColor( (int)(specular[0]), (int)(specular[1]), (int)(specular[2]), (int)(specular[3]) );

      float shininess = wrapper->GetCoreMaterialShininess(matID);

      emit MaterialLoaded(matID, nameToSend, diffColor, ambColor, specColor, shininess);
   }

   CreateBoneBasisDisplay();

   emit CharacterDataLoaded(modelData, wrapper);

   LOG_DEBUG("Done loading file: " + filename.toStdString());
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::OnSaveCharFile(const QString& filename)
{
   if(mCharacter.valid())
   {
      // Acquire the model data associated with this character.
      const Cal3DModelWrapper* wrapper = mCharacter->GetCal3DWrapper();
      Cal3DModelData* data = mCalDatabase->GetModelData(*wrapper);

      if(data != NULL)
      {
         // Finalize the data from current changes in the model.
         data->SetScale(wrapper->GetScale());

         try
         {
            // Wrap and write the data.
            dtCore::RefPtr<dtCore::WriteWrapperOSGObject<Cal3DModelData> > obj
               = new dtCore::WriteWrapperOSGObject<Cal3DModelData>(*data);
            osgDB::writeObjectFile(*obj, filename.toStdString());
         }
         catch(std::exception& e)
         {
            std::string title("Error saving.");
            std::ostringstream oss;
            oss << "Exception saving file \""
               << filename.toStdString().c_str()
               << "\": " << e.what() << std::endl;
            emit SignalError(title, oss.str());
         }
         catch(...)
         {
            std::string title("Error saving.");
            std::ostringstream oss;
            oss << "Unknown exception saving file \""
               << filename.toStdString().c_str() << std::endl;
            emit SignalError(title, oss.str());
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::OnReloadCharFile()
{
   if(mCharacter.valid())
   {
      dtAnim::Cal3DModelWrapper* wrapper = mCharacter->GetCal3DWrapper();
      dtAnim::Cal3DModelData* modelData = mCalDatabase->GetModelData(*wrapper);
      std::string currentFilePath(modelData->GetFilename());

      QString qTempFile(mTempFile.c_str());
      OnSaveCharFile(qTempFile);
      OnLoadCharFile(qTempFile);

      wrapper = mCharacter->GetCal3DWrapper();
      modelData = mCalDatabase->GetModelData(*wrapper);
      modelData->SetFilename(currentFilePath);
   }
}


//////////////////////////////////////////////////////////////////////////
void Viewer::OnUnloadAttachmentFile()
{
   if(mAttachmentObject != NULL)
   {
      GetScene()->RemoveChild(mAttachmentObject.get());
      mAttachmentObject = NULL;
   }
}

//////////////////////////////////////////////////////////////////////////
void Viewer::OnLoadAttachmentFile(const QString& filename)
{
   OnUnloadAttachmentFile();

   mAttachmentObject = new dtCore::Object;
   mAttachmentObject->LoadFile(filename.toStdString());
   GetScene()->AddChild(mAttachmentObject.get());
}

//////////////////////////////////////////////////////////////////////////
void Viewer::OnAttachmentSettingsChanged(const std::string& bone, float x, float y, float z, float rotx, float roty, float rotz)
{
   if(mAttachmentObject == NULL)
   {
      return;
   }

   dtUtil::HotSpotDefinition hsd;
   hsd.mName = "HotSpot";
   hsd.mParentName = bone;
   hsd.mLocalTranslation = osg::Vec3(x, y, z);
   hsd.mLocalRotation = osg::Quat(osg::DegreesToRadians(rotx), osg::Vec3(1, 0, 0),
            osg::DegreesToRadians(roty), osg::Vec3(0, 1, 0),
            osg::DegreesToRadians(rotz), osg::Vec3(0, 0, 1));
   mAttachmentController->RemoveAttachment(*mAttachmentObject.get());
   mAttachmentController->AddAttachment(*mAttachmentObject.get(), hsd);
}

//////////////////////////////////////////////////////////////////////////
void Viewer::OnUnloadCharFile()
{
   // try to clean up the scene graph
   if (mCharacter.valid())
   {
      mShadeDecorator->removeChild(mCharacter->GetOSGNode());
      mWireDecorator->removeChild(mCharacter->GetOSGNode());
      mCharacter = NULL;
   }

   //wipe out any previously loaded characters. This will ensure we can
   //reload the same file (which might have been modified).
   mCalDatabase->TruncateDatabase();
   mCalDatabase->PurgeLoaderCaches();

   emit ClearCharacterData();
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::CreateBoneBasisDisplay()
{
   // Destroy any previous held point axes and start fresh
   mBoneBasisGroup->removeChildren(0, mBoneBasisGroup->getNumChildren());

   VectorHotSpot hotSpotList;
   dtAnim::Cal3DModelWrapper* modelWrapper = mCharacter->GetCal3DWrapper();

   std::vector<int> boneList;
   modelWrapper->GetCoreBoneChildrenIDs(0, boneList);

   std::vector<std::string> boneVec;
   modelWrapper->GetCoreBoneNames(boneVec);

   //for every bone
   std::vector<std::string>::const_iterator boneNameIter = boneVec.begin();
   std::vector<std::string>::const_iterator boneNameEnd  = boneVec.end();

   while (boneNameIter!=boneNameEnd)
   {
      //create a HotSpot
      dtUtil::HotSpotDefinition hotSpotDefinition;
      hotSpotDefinition.mName = *boneNameIter;
      hotSpotDefinition.mParentName = *boneNameIter;

      //Create the axis geometry
      dtCore::PointAxis* axis = new dtCore::PointAxis();
      axis->SetLength(dtCore::PointAxis::X, 0.025f);
      axis->SetLength(dtCore::PointAxis::Y, 0.025f);
      axis->SetLength(dtCore::PointAxis::Z, 0.025f);
      axis->SetCharacterScale(5.0f);
      axis->Enable(dtCore::PointAxis::LABEL_X);
      axis->SetLabel(dtCore::PointAxis::X, boneNameIter->c_str());
      axis->SetLabelColor(dtCore::PointAxis::X, dtCore::PointAxis::YELLOW);

      mAttachmentController->AddAttachment(*axis, hotSpotDefinition);

      mBoneBasisGroup->addChild(axis->GetOSGNode());

      ++boneNameIter;
   }
}

////////////////////////////////////////////////////////////////////////////////
int Viewer::GetMaxBoneAffectedBoneIDForAnimation(int animationID)
{
   Cal3DModelWrapper* wrapper = mCharacter->GetCal3DWrapper();
   std::list<CalCoreTrack*>& trackList =
      wrapper->GetCalModel()->getCoreModel()->getCoreAnimation(animationID)->getListCoreTrack();

   std::list<CalCoreTrack*>::iterator trackIter = trackList.begin();

   int maxID = -1;
   while(trackIter != trackList.end())
   {
      maxID = dtUtil::Max(maxID, (*trackIter)->getCoreBoneId());
      ++trackIter;
   }

   return maxID;
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::OnLoadPoseMeshFile(const std::string& filename)
{
   dtAnim::Cal3DModelWrapper* rapper = mCharacter->GetCal3DWrapper();
   assert(rapper);

   // Delete any previous data
   if (mPoseDatabase.valid())
   {
      mPoseDatabase = NULL;
      mPoseUtility  = NULL;
   }

   // Create the database to store loaded data
   mPoseDatabase = new dtAnim::PoseMeshDatabase(rapper);
   mPoseUtility  = new dtAnim::PoseMeshUtility;

   if (mPoseDatabase->LoadFromFile(filename))
   {
      mPoseMeshes = &mPoseDatabase->GetMeshes();
      emit PoseMeshesLoaded(*mPoseMeshes, mCharacter.get());
   }
   else
   {
      // Unable to load pose mesh (this will probably induce a timer callback to timeout())
      QMessageBox::warning(NULL, "Error", "Unable to load pose meshes!", QMessageBox::Ok);
   }
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::OnStartAnimation(unsigned int id, float weight, float delay)
{
   if (mCharacter.valid())
   {
      Cal3DModelWrapper* wrapper = mCharacter->GetCal3DWrapper();
      CalCoreSkeleton* skeleton = wrapper->GetCalModel()->getCoreModel()->getCoreSkeleton();

      int maxTrackBone = GetMaxBoneAffectedBoneIDForAnimation(id);
      int maxSkeletonBone = skeleton->getVectorCoreBone().size();

      if (maxTrackBone < maxSkeletonBone)
      {
         wrapper->BlendCycle(id, weight, delay);
         LOG_DEBUG("Started:" + dtUtil::ToString(id) + ", weight:" + dtUtil::ToString(weight) + ", delay:" + dtUtil::ToString(delay));
      }
      else
      {
          QMessageBox::warning(NULL, "Error", "This animation is incompatible with current skeleton.", QMessageBox::Ok);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::OnStopAnimation(unsigned int id, float delay)
{
   if (mCharacter.valid())
   {
      Cal3DModelWrapper* wrapper = mCharacter->GetCal3DWrapper();
      wrapper->ClearCycle(id, delay);

      LOG_DEBUG("Stopped:" + dtUtil::ToString(id) + ", delay:" + dtUtil::ToString(delay));
   }
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::OnStartAction(unsigned int id, float delayIn, float delayOut)
{
   if (mCharacter.valid())
   {
      Cal3DModelWrapper* wrapper = mCharacter->GetCal3DWrapper();
      CalCoreSkeleton* skeleton = wrapper->GetCalModel()->getCoreModel()->getCoreSkeleton();

      int maxTrackBone = GetMaxBoneAffectedBoneIDForAnimation(id);
      int maxSkeletonBone = skeleton->getVectorCoreBone().size();

      if (maxTrackBone < maxSkeletonBone)
      {
         Cal3DModelWrapper* wrapper = mCharacter->GetCal3DWrapper();
         wrapper->ExecuteAction(id, delayIn, delayOut);
      }
      else
      {
         QMessageBox::warning(NULL, "Error", "This animation is incompatible with current skeleton.", QMessageBox::Ok);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::OnLODScale_Changed(float scaleValue)
{
   if (mCharacter.get())
   {
      GetCamera()->SetLODScale(scaleValue);
   }
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::OnScaleFactorChanged(float scaleFactorValue)
{
   if (mCharacter.get())
   {
      mCharacter->GetCal3DWrapper()->SetScale(mCharacter->GetCal3DWrapper()->GetScale() * scaleFactorValue);
      mCharacter->RebuildSubmeshes();//need to rebuild our geometry to match the new mesh size
   }
}

/////////////////////////////////////////////////////////////////////////////////////////
void Viewer::OnSpeedChanged(float speedFactor)
{
   dtCore::System::GetInstance().SetTimeScale(speedFactor);
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::OnSetShaded()
{
   GetScene()->GetSceneNode()->removeChild(mWireDecorator.get());
   GetScene()->GetSceneNode()->removeChild(mShadeDecorator.get());

   GetScene()->GetSceneNode()->addChild(mShadeDecorator.get());
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::OnSetWireframe()
{
   GetScene()->GetSceneNode()->removeChild(mWireDecorator.get());
   GetScene()->GetSceneNode()->removeChild(mShadeDecorator.get());

   GetScene()->GetSceneNode()->addChild(mWireDecorator.get());
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::OnSetShadedWireframe()
{
   GetScene()->GetSceneNode()->removeChild(mWireDecorator.get());
   GetScene()->GetSceneNode()->removeChild(mShadeDecorator.get());

   GetScene()->GetSceneNode()->addChild(mWireDecorator.get());
   GetScene()->GetSceneNode()->addChild(mShadeDecorator.get());
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::OnSetBoneBasisDisplay(bool shouldDisplay)
{
   if (shouldDisplay)
   {
      GetScene()->GetSceneNode()->addChild(mBoneBasisGroup.get());
   }
   else
   {
      GetScene()->GetSceneNode()->removeChild(mBoneBasisGroup.get());
   }
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::OnSetBoneLabelDisplay(bool shouldDisplay)
{
   int numberOfAxes = dtCore::PointAxis::GetInstanceCount();

   for (int axisIndex = 0; axisIndex < numberOfAxes; ++axisIndex)
   {
      dtCore::PointAxis* axis = dtCore::PointAxis::GetInstance(axisIndex);

      // Labels are on the X axis
      if (shouldDisplay)
      {
         axis->Enable(dtCore::PointAxis::LABEL_X);
      }
      else
      {
         axis->Disable(dtCore::PointAxis::LABEL_X);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::OnTimeout()
{
   if (mCharacter.valid())
   {
      dtAnim::Cal3DModelWrapper* rapper = mCharacter->GetCal3DWrapper();
      assert(rapper);

      mAttachmentController->Update(*rapper);

      std::vector<CalAnimation*> animVec = rapper->GetCalModel()->getMixer()->getAnimationVector();
      std::vector<CalAnimation*>::iterator animItr = animVec.begin();

      std::vector<std::pair<float, float> > animWeightTimeList;
      animWeightTimeList.reserve(animVec.size());

      while (animItr != animVec.end())
      {
         CalAnimation* anim = *(animItr);
         float weight = 0.f;
         float time = 0.f;

         if (anim!=NULL)
         {
            weight = anim->getWeight();
            time = anim->getTime();
         }

         animWeightTimeList.push_back(std::make_pair(weight, time));

         ++animItr;
      }

      std::vector<float> morphWeightList;

#ifdef CAL3D_VERSION_DEVELOPMENT
      int count = rapper->GetCalModel()->getMorphTargetMixer()->getMorphTargetCount();
      morphWeightList.reserve(count);

      for (int index = 0; index < count; ++index)
      {
         float weight = rapper->GetCalModel()->getMorphTargetMixer()->getCurrentWeight(index);
         morphWeightList.push_back(weight);
      }
#endif

      emit BlendUpdate(animWeightTimeList, morphWeightList);
   }
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::InitShadeDecorator()
{
   //osg::StateSet* stateset = new osg::StateSet;
   //osg::PolygonMode* polyMode = new osg::PolygonMode;
   //polyMode->setMode(osg::PolygonMode::FRONT, osg::PolygonMode::FILL);
   //
   //osg::Material* material = new osg::Material;
   //stateset->setAttributeAndModes(material, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
   //stateset->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);

   //mShadeDecorator->setStateSet(stateset);
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::InitWireDecorator()
{
   osg::StateSet* stateset = new osg::StateSet;

   osg::PolygonOffset* polyOffset = new osg::PolygonOffset;
   polyOffset->setFactor(-1.0f);
   polyOffset->setUnits(-1.0f);

   osg::PolygonMode* polyMode = new osg::PolygonMode;
   polyMode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);

   stateset->setAttributeAndModes(polyOffset, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
   stateset->setAttributeAndModes(polyMode, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

   osg::Material* material = new osg::Material;
   stateset->setAttributeAndModes(material, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
   stateset->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);

   stateset->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);

   mWireDecorator->setStateSet(stateset);
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::OnShowMesh(int meshID)
{
   mMeshesToShow.push_back(meshID);
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::OnHideMesh(int meshID)
{
   mMeshesToHide.push_back(meshID);
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::PostFrame(const double)
{
   {
      std::vector<int>::iterator showItr = mMeshesToShow.begin();
      while (showItr != mMeshesToShow.end())
      {
         mCharacter->GetCal3DWrapper()->ShowMesh((*showItr));

         ++showItr;
      }

      mMeshesToShow.clear();
   }

   {
      std::vector<int>::iterator hideItr = mMeshesToHide.begin();
      while (hideItr != mMeshesToHide.end())
      {
         mCharacter->GetCal3DWrapper()->HideMesh((*hideItr));
         ++hideItr;
      }

      mMeshesToHide.clear();
   }

   OnTimeout();
}

//////////////////////////////////////////////////////////////////////////
void Viewer::OnMorphChanged(int meshID, int subMeshID, int morphID, float weight)
{
   CalMesh* mesh = mCharacter->GetCal3DWrapper()->GetCalModel()->getMesh(meshID);
   if (mesh)
   {
      CalSubmesh *subMesh = mesh->getSubmesh(subMeshID);
      if (subMesh)
      {
         subMesh->setMorphTargetWeight(morphID, weight);
      }
   }
}

//////////////////////////////////////////////////////////////////////////
void Viewer::OnPlayMorphAnimation(int morphAnimID, float weight, float delayIn, float delayOut, bool looping)
{
#ifdef CAL3D_VERSION_DEVELOPMENT
   CalMorphTargetMixer *mixer = mCharacter->GetCal3DWrapper()->GetCalModel()->getMorphTargetMixer();
   if (mixer)
   {
      mixer->blend(morphAnimID, weight, delayIn, delayOut, looping);
   }
#endif
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::OnStopMorphAnimation(int morphAnimID, float delay)
{
#ifdef CAL3D_VERSION_DEVELOPMENT
   CalMorphTargetMixer *mixer = mCharacter->GetCal3DWrapper()->GetCalModel()->getMorphTargetMixer();
   if (mixer)
   {
      mixer->clear(morphAnimID, delay);
   }
#endif
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::OnClearTempFile()
{
   if (!mTempFile.empty())
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      if (fileUtils.FileExists(mTempFile))
      {
         fileUtils.FileDelete(mTempFile);
      }
   }
}


