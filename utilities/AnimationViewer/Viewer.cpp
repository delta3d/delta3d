/// Included above Viewer.h to avoid a compile error in linux
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtGui/QColor>
#include <QtGui/QDialog>
#include <QtGui/QMessageBox>

#include "Viewer.h"
#include "NewCharacterDialog.h"

#include <dtCore/system.h>
#include <dtCore/transform.h>
#include <dtCore/scene.h>
#include <dtCore/camera.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/light.h>
#include <dtCore/deltawin.h>
#include <dtCore/project.h>

#include <dtAnim/basemodeldata.h>
#include <dtAnim/basemodelwrapper.h>
#include <dtAnim/characterfilehandler.h>
#include <dtAnim/chardrawable.h>
#include <dtAnim/osgmodeldata.h>
#include <dtAnim/osgmodelwrapper.h>
#include <dtAnim/posemesh.h>

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

#include <dtAnim/hotspotdriver.h>
#include <dtAnim/modeldatabase.h>
#include <dtCore/hotspotattachment.h>
#include <dtCore/refptr.h>
#include <dtCore/pointaxis.h>

#if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
#include <cal3d/morphtargetmixer.h>
#include <dtAnim/cal3dmodelwrapper.h>
#endif



////////////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////
#if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
CalMorphTargetMixer* GetMorphTargetMixer(dtAnim::BaseModelWrapper& wrapper)
{
   CalMorphTargetMixer* mixer = NULL;

   dtAnim::Cal3DModelWrapper* calWrapper = dynamic_cast<dtAnim::Cal3DModelWrapper*>(&wrapper);
   if (calWrapper != NULL)
   {
      mixer = calWrapper->GetCalModel()->getMorphTargetMixer();
   }
   
   return mixer;
}
#endif



typedef std::vector<dtCore::RefPtr<dtCore::HotSpotAttachment> > VectorHotSpot;

using namespace dtUtil;
using namespace dtCore;
using namespace dtAnim;

////////////////////////////////////////////////////////////////////////////////
Viewer::Viewer()
 : dtABC::Application()
 , mPoseMeshes(NULL)
 , mAttachmentObject(NULL)
{
   dtUtil::Log::GetInstance().SetLogLevel(dtUtil::Log::LOG_DEBUG);

   mModelLoader = new dtAnim::ModelLoader;
}

////////////////////////////////////////////////////////////////////////////////
Viewer::~Viewer()
{
}

////////////////////////////////////////////////////////////////////////////////
osg::Node* Viewer::GetRootNode()
{
   return ! mCharacter.valid() ? NULL : mCharacter->GetModelWrapper()->GetDrawableNode();
}

////////////////////////////////////////////////////////////////////////////////
dtAnim::CharDrawable* Viewer::GetCharacter()
{
   return mCharacter.get();
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::UpdateCharacter()
{
   if (mCharacter.valid())
   {
      dtCore::RefPtr<BaseModelWrapper> wrapper = mCharacter->GetModelWrapper();
      
      mCharacter->RebuildSubmeshes(true);

      wrapper->HandleModelUpdated();

      UpdateAnimationList(*wrapper);
      UpdateMorphList(*wrapper);
      UpdateMeshList(*wrapper);
      UpdateMaterialList(*wrapper);

      osg::Node* charNode = mCharacter->GetOSGNode();
      mShadeDecorator->removeChildren(0, mShadeDecorator->getNumChildren());
      mShadeDecorator->addChild(charNode);

      mWireDecorator->removeChildren(0, mWireDecorator->getNumChildren());
      mWireDecorator->addChild(charNode);

      CreateBoneBasisDisplay();
   }
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
   dtAnim::Cal3DAnimator::SetBindPoseAllowed(true);

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

   NewCharacterDialog* dialog = new NewCharacterDialog;

   int retCode = dialog->exec();
   
   dtCore::RefPtr<dtAnim::BaseModelData> modelData;
   dtCore::RefPtr<dtAnim::BaseModelWrapper> wrapper;
   if (retCode == QDialog::Accepted)
   {
      std::string name(dialog->mUI.mName->text().toStdString());

      if (dialog->mUI.mChoiceTypeCal3d->isChecked())
      {
         dtCore::RefPtr<dtAnim::Cal3DModelData> calModelData = new Cal3DModelData(name, dtCore::ResourceDescriptor::NULL_RESOURCE);
         modelData = calModelData;
         ModelDatabase::GetInstance().RegisterModelData(*modelData);
         wrapper = new Cal3DModelWrapper(*calModelData);
      }
      else
      {
         dtCore::RefPtr<dtAnim::OsgModelData> osgModelData = new OsgModelData(name, dtCore::ResourceDescriptor::NULL_RESOURCE);
         modelData = osgModelData;
         ModelDatabase::GetInstance().RegisterModelData(*modelData);
         osgModelData->GetOrCreateSkeleton();
         wrapper = new OsgModelWrapper(*osgModelData);
      }

      mCharacter = new dtAnim::CharDrawable(wrapper.get());
      UpdateCharacter();
   }

   // Delete the dialog before sending signals to the rest of the application.
   delete dialog;

   // Signal that a new character and data have been created.
   if (wrapper.valid() && modelData.valid())
   {
      emit CharacterDataLoaded(modelData, wrapper);
   }
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::OnLoadCharFile(const QString& filename)
{
   std::string fileStdStr = filename.toStdString();

   dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
   if (!fileUtils.FileExists(fileStdStr, false))
   {
      emit ErrorOccured(QString(("Unable to load file \"" + fileStdStr + "\": it does not exist.  Current Directory:" + dtUtil::FileUtils::GetInstance().CurrentDirectory()).c_str()));
      return;
   }

   dtUtil::FileInfo fi = fileUtils.GetFileInfo(fileStdStr);
   dtCore::ResourceDescriptor resource(fi.baseName);

   OnUnloadCharFile();

   dtAnim::BaseModelData* modelData = NULL;

   //create an instance from the character definition file
   dtCore::RefPtr<dtAnim::BaseModelWrapper> wrapper;
   try
   {
      dtCore::Project::GetInstance().SetContext(fileUtils.GetAbsolutePath(filename.toStdString(), true));
      mModelLoader->LoadModel(resource, false);
      if (mModelLoader->GetLoadingState() == dtAnim::ModelLoader::COMPLETE)
         wrapper = mModelLoader->CreateModel();

      if (!wrapper.valid())
      {
         emit ErrorOccured("Problem encountered loading file.  See log file.");
         return;
      }

      wrapper->HandleModelResourceUpdate(ModelResourceType::SKEL_FILE);
      wrapper->HandleModelResourceUpdate(ModelResourceType::MESH_FILE);
      wrapper->HandleModelResourceUpdate(ModelResourceType::MAT_FILE);
      wrapper->HandleModelResourceUpdate(ModelResourceType::ANIM_FILE);

      mCharacter = new CharDrawable(wrapper.get());

      // Retrieve the data to check for the inclusion of an IK pose mesh file
      modelData = wrapper->GetModelData();

      if (!modelData->GetPoseMeshFilename().empty())
      {
         OnLoadPoseMeshFile(modelData->GetPoseMeshFilename());
      }

      // This is bad.  It should be in a temp dir or something.
      // Determine where a temp file should be for the current character.
      mTempFile = dtCore::Project::GetInstance().GetContext(0);
      if (!mTempFile.empty())
      {
         mTempFile += "/_Temp.dtchar";
      }
   }
   catch (const dtUtil::Exception& ex)
   {
      QString errorMsg;
      errorMsg = QString("Exception thrown: %1").arg(ex.ToString().c_str());
      emit ErrorOccured(errorMsg);
      return;
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

   UpdateCharacter();

   emit CharacterDataLoaded(modelData, wrapper);

   LOG_DEBUG("Done loading file: " + filename.toStdString());
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::OnSaveCharFile(const QString& filename)
{
   if(mCharacter.valid())
   {
      // Acquire the model data associated with this character.
      dtAnim::BaseModelWrapper* wrapper = mCharacter->GetModelWrapper();
      dtAnim::BaseModelData* data = wrapper->GetModelData();

      if(data != NULL)
      {
         // Finalize the data from current changes in the model.
         data->SetScale(wrapper->GetScale());

         try
         {
            // Wrap and write the data.
            dtCore::RefPtr<dtCore::WriteWrapperOSGObject<dtAnim::BaseModelData> > obj
               = new dtCore::WriteWrapperOSGObject<dtAnim::BaseModelData>(*data);
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
      dtAnim::BaseModelWrapper* wrapper = mCharacter->GetModelWrapper();
      dtAnim::BaseModelData* modelData = wrapper->GetModelData();
      dtCore::ResourceDescriptor rd = modelData->GetResource();

      if ( ! rd.IsEmpty())
      {
         QString qTempFile(mTempFile.c_str());
         OnSaveCharFile(qTempFile);
         OnLoadCharFile(qTempFile);

         wrapper = mCharacter->GetModelWrapper();
         modelData = wrapper->GetModelData();
         modelData->SetResource(rd);
      }
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
void Viewer::OnLoadAttachmentFile(const QString filename)
{
   OnUnloadAttachmentFile();

   mAttachmentObject = new dtCore::Object;
   mAttachmentObject->LoadFile(filename.toStdString());
   GetScene()->AddChild(mAttachmentObject.get());

   emit SignalAttachmentLoaded();
}

//////////////////////////////////////////////////////////////////////////
void Viewer::OnAttachmentSettingsChanged(AttachmentInfo info)
{
   if(mAttachmentObject == NULL)
   {
      return;
   }

   osg::Vec3& rot = info.mRotation;

   dtUtil::HotSpotDefinition hsd;
   hsd.mName = "HotSpot";
   hsd.mParentName = info.mBone.valid() ? info.mBone->GetName() : "";
   hsd.mLocalTranslation = info.mOffset;
   hsd.mLocalRotation = osg::Quat(osg::DegreesToRadians(rot.x()), osg::Vec3(1, 0, 0),
            osg::DegreesToRadians(rot.y()), osg::Vec3(0, 1, 0),
            osg::DegreesToRadians(rot.z()), osg::Vec3(0, 0, 1));
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

   mAttachmentController = NULL;

   //wipe out any previously loaded characters. This will ensure we can
   //reload the same file (which might have been modified).
   if (dtAnim::ModelDatabase::IsAvailable())
   {
      ModelDatabase& database = dtAnim::ModelDatabase::GetInstance();
      database.TruncateDatabase();
   }

   emit ClearCharacterData();
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::CreateBoneBasisDisplay()
{
   // Destroy any previous held point axes and start fresh
   mBoneBasisGroup->removeChildren(0, mBoneBasisGroup->getNumChildren());

   VectorHotSpot hotSpotList;
   dtAnim::BaseModelWrapper* modelWrapper = mCharacter->GetModelWrapper();

   dtAnim::BoneArray bones;
   modelWrapper->GetBones(bones);

   dtAnim::BoneInterface* bone = NULL;

   //for every bone
   dtAnim::BoneArray::iterator curIter = bones.begin();
   dtAnim::BoneArray::iterator endIter = bones.end();
   while (curIter != endIter)
   {
      bone = curIter->get();

      //create a HotSpot
      dtUtil::HotSpotDefinition hotSpotDefinition;
      hotSpotDefinition.mName = bone->GetName();
      hotSpotDefinition.mParentName = bone->GetName();

      //Create the axis geometry
      dtCore::PointAxis* axis = new dtCore::PointAxis();
      axis->SetLength(dtCore::PointAxis::X, 0.025f);
      axis->SetLength(dtCore::PointAxis::Y, 0.025f);
      axis->SetLength(dtCore::PointAxis::Z, 0.025f);
      axis->SetCharacterScale(5.0f);
      axis->Enable(dtCore::PointAxis::LABEL_X);
      axis->SetLabel(dtCore::PointAxis::X, bone->GetName().c_str());
      axis->SetLabelColor(dtCore::PointAxis::X, dtCore::PointAxis::YELLOW);
      
      if ( ! mAttachmentController.valid())
      {
         mAttachmentController = new dtAnim::AttachmentController;
      }
      mAttachmentController->AddAttachment(*axis, hotSpotDefinition);

      mBoneBasisGroup->addChild(axis->GetOSGNode());

      ++curIter;
   }
}

////////////////////////////////////////////////////////////////////////////////
int Viewer::GetMaxBoneAffectedBoneIDForAnimation(int animationID)
{
   int maxID = -1;

   BaseModelWrapper* wrapper = mCharacter->GetModelWrapper();
   dtAnim::AnimationInterface* anim = wrapper->GetAnimationByIndex(animationID);
   
   if (anim != NULL)
   {
      dtAnim::TrackArray tracks;
      anim->GetTracks(tracks);

      dtAnim::TrackArray::iterator curIter = tracks.begin();
      dtAnim::TrackArray::iterator endIter = tracks.end();

      dtAnim::BoneInterface* bone = NULL;
      while(curIter != endIter)
      {
         bone = (*curIter)->GetBone();
         if (bone != NULL)
         {
            maxID = dtUtil::Max(maxID, bone->GetID());
         }
         ++curIter;
      }
   }

   return maxID;
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::OnLoadPoseMeshFile(const std::string& filename)
{
   dtAnim::BaseModelWrapper* rapper = mCharacter->GetModelWrapper();
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
      BaseModelWrapper* wrapper = mCharacter->GetModelWrapper();
      dtAnim::SkeletonInterface* skeleton = wrapper->GetSkeleton();

      int maxTrackBone = GetMaxBoneAffectedBoneIDForAnimation(id);
      int maxSkeletonBone = skeleton->GetBoneCount();

      if (maxTrackBone < maxSkeletonBone)
      {
         dtAnim::AnimationInterface* anim = wrapper->GetAnimationByIndex(id);

         if (anim->PlayCycle(weight, delay))
         {
            LOG_DEBUG("Started:" + dtUtil::ToString(id) + ", weight:" + dtUtil::ToString(weight) + ", delay:" + dtUtil::ToString(delay));
         }
         else
         {
            LOG_ERROR("Could not start animation: " + anim->GetName());
         }
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
      dtAnim::AnimationInterface* anim = mCharacter->GetModelWrapper()->GetAnimationByIndex(id);
      anim->ClearCycle(delay);

      LOG_DEBUG("Stopped:" + dtUtil::ToString(id) + ", delay:" + dtUtil::ToString(delay));
   }
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::OnStartAction(unsigned int id, float delayIn, float delayOut)
{
   if (mCharacter.valid())
   {
      dtAnim::BaseModelWrapper* wrapper = mCharacter->GetModelWrapper();
      dtAnim::SkeletonInterface* skeleton = wrapper->GetSkeleton();

      int maxTrackBone = GetMaxBoneAffectedBoneIDForAnimation(id);
      int maxSkeletonBone = skeleton->GetBoneCount();

      if (maxTrackBone < maxSkeletonBone)
      {
         dtAnim::AnimationInterface* anim = mCharacter->GetModelWrapper()->GetAnimationByIndex(id);
         anim->PlayAction(delayIn, delayOut);
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
   if (mCharacter.valid())
   {
      GetCamera()->SetLODScale(scaleValue);
   }
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::OnScaleFactorChanged(float scaleFactorValue)
{
   if (mCharacter.valid())
   {
      dtAnim::BaseModelWrapper* model = mCharacter->GetModelWrapper();
      model->SetScale(scaleFactorValue);
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
      dtAnim::BaseModelWrapper* wrapper = mCharacter->GetModelWrapper();
      assert(wrapper);

      if (mAttachmentController.valid())
      {
         mAttachmentController->Update(*wrapper);
      }

      std::vector<std::pair<float, float> > animWeightTimeList;

      dtAnim::AnimationInterface* curAnim = NULL;
      int numAnims = wrapper->GetAnimationCount();
      for (int i = 0; i < numAnims; ++i)
      {
         curAnim = wrapper->GetAnimationByIndex(i);

         float weight = curAnim->GetWeight();
         float time = curAnim->GetTime();

         animWeightTimeList.push_back(std::make_pair(weight, time));
      }

      std::vector<float> morphWeightList;

#if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
      CalMorphTargetMixer* mixer = GetMorphTargetMixer(*wrapper);
      if (mixer != NULL)
      {
         int count = mixer->getMorphTargetCount();
         morphWeightList.reserve(count);

         for (int index = 0; index < count; ++index)
         {
            float weight = mixer->getCurrentWeight(index);
            morphWeightList.push_back(weight);
         }
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
   if ( ! mMeshesToShow.empty() || ! mMeshesToHide.empty())
   {
      dtAnim::MeshArray meshes;
      mCharacter->GetModelWrapper()->GetMeshes(meshes);

      {
         std::vector<int>::iterator showItr = mMeshesToShow.begin();
         while (showItr != mMeshesToShow.end())
         {
            int id = *showItr;

            dtAnim::MeshInterface* mesh = NULL;
            dtAnim::MeshArray::iterator curIter = meshes.begin();
            dtAnim::MeshArray::iterator endIter = meshes.end();
            for (; curIter != endIter; ++curIter)
            {
               mesh = curIter->get();
               if (mesh->GetID() == id)
               {
                  mesh->SetVisible(true);
               }
            }

            ++showItr;
         }

         mMeshesToShow.clear();
      }

      {
         std::vector<int>::iterator hideItr = mMeshesToHide.begin();
         while (hideItr != mMeshesToHide.end())
         {
            int id = *hideItr;

            dtAnim::MeshInterface* mesh = NULL;
            dtAnim::MeshArray::iterator curIter = meshes.begin();
            dtAnim::MeshArray::iterator endIter = meshes.end();
            for (; curIter != endIter; ++curIter)
            {
               mesh = curIter->get();
               if (mesh->GetID() == id)
               {
                  mesh->SetVisible(false);
               }
            }

            ++hideItr;
         }

         mMeshesToHide.clear();
      }
   }

   OnTimeout();
}

//////////////////////////////////////////////////////////////////////////
void Viewer::OnMorphChanged(const QString& meshName, int submeshID, int morphID, float weight)
{
   if (mCharacter.valid())
   {
      dtAnim::MeshInterface* mesh = mCharacter->GetModelWrapper()->GetMesh(meshName.toStdString());
      dtAnim::SubmeshArray submeshes;
      if (mesh != NULL)
      {
         mesh->GetSubmeshes(submeshes);
      }
      else
      {
         LOG_ERROR("Could not access mesh \"" + meshName.toStdString() + "\"");
      }

      dtAnim::SubmeshInterface* submesh = int(submeshes.size()) > submeshID ? submeshes[submeshID] : NULL;
      if (mesh && submesh)
      {
         dtAnim::MorphTargetArray morphs;
         submesh->GetMorphTargets(morphs);

         if (morphID < int(morphs.size()))
         {
            morphs[morphID]->SetWeight(weight);
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////
void Viewer::OnPlayMorphAnimation(int morphAnimID, float weight, float delayIn, float delayOut, bool looping)
{
#if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
   if (mCharacter.valid())
   {
      CalMorphTargetMixer *mixer = GetMorphTargetMixer(*mCharacter->GetModelWrapper());
      if (mixer)
      {
         mixer->blend(morphAnimID, weight, delayIn, delayOut, looping);
      }
   }
#endif
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::OnStopMorphAnimation(int morphAnimID, float delay)
{
#if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
   if (mCharacter.valid())
   {
      CalMorphTargetMixer *mixer = GetMorphTargetMixer(*mCharacter->GetModelWrapper());
      if (mixer)
      {
         mixer->clear(morphAnimID, delay);
      }
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

////////////////////////////////////////////////////////////////////////////////
void Viewer::UpdateAnimationList(dtAnim::BaseModelWrapper& wrapper)
{
   //get all the data for animations and tell the world
   dtAnim::AnimationInterface* curAnim = NULL;
   int numAnims = wrapper.GetAnimationCount();
   for (int animID = 0; animID < numAnims; ++animID)
   {
      curAnim = wrapper.GetAnimationByIndex(animID);

      QString nameToSend = QString::fromStdString(curAnim->GetName());
      unsigned int trackCount = curAnim->GetTrackCount();
      unsigned int keyframes = curAnim->GetKeyframeCount();
      float dur = curAnim->GetDuration();
      emit AnimationLoaded(animID, nameToSend, trackCount, keyframes, dur);
   }
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::UpdateMaterialList(dtAnim::BaseModelWrapper& wrapper)
{
   //get all material data and emit
   dtAnim::MaterialArray materials;
   wrapper.GetMaterials(materials);
   
   int rgb = 255;
   QColor diffColor(rgb, rgb, rgb);
   QColor ambColor(rgb, rgb, rgb);
   QColor specColor(rgb, rgb, rgb);
   dtAnim::Cal3dMaterial* calMaterial = NULL;
   dtAnim::MaterialInterface* material = NULL;
   int numMaterials = int(materials.size());
   for (int matID = 0; matID < numMaterials; matID++)
   {
      material = materials[matID].get();
      const std::string& name = material->GetName();

      QString nameToSend = QString::fromStdString(name);

      float shininess = 0.0f;

      calMaterial = dynamic_cast<dtAnim::Cal3dMaterial*>(material);
      if (calMaterial != NULL)
      {
         osg::Vec4 diffuse = calMaterial->GetDiffuseColor();
         diffColor = QColor( (int)(diffuse[0]), (int)(diffuse[1]), (int)(diffuse[2]), (int)(diffuse[3]) );

         osg::Vec4 ambient = calMaterial->GetAmbientColor();
         ambColor = QColor( (int)(ambient[0]), (int)(ambient[1]), (int)(ambient[2]), (int)(ambient[3]) );

         osg::Vec4 specular = calMaterial->GetSpecularColor();
         specColor = QColor( (int)(specular[0]), (int)(specular[1]), (int)(specular[2]), (int)(specular[3]) );

         shininess = calMaterial->GetShininess();
      }

      emit MaterialLoaded(matID, nameToSend, diffColor, ambColor, specColor, shininess);
   }
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::UpdateMeshList(dtAnim::BaseModelWrapper& wrapper)
{
   std::vector<std::string> boneNames;
   dtAnim::BoneArray bones;
   wrapper.GetBones(bones);

   dtAnim::BoneArray::iterator curIter = bones.begin();
   dtAnim::BoneArray::iterator endIter = bones.end();
   for ( ; curIter != endIter; ++curIter)
   {
      boneNames.push_back((*curIter)->GetName());
   }

   dtAnim::MeshArray meshes;
   wrapper.GetMeshes(meshes);

   dtAnim::MeshInterface* mesh = NULL;
   dtAnim::MeshArray::iterator curMeshIter = meshes.begin();
   dtAnim::MeshArray::iterator endMeshIter = meshes.end();
   //get all data for the meshes and emit
   for ( ; curMeshIter != endMeshIter; ++curMeshIter)
   {
      mesh = curMeshIter->get();

      // If the mesh is currently loaded
      std::string name = mesh->GetName();
      QString meshName = QString::fromStdString(name);

      int meshID = mesh->GetID();
      emit MeshLoaded(meshID, meshName, boneNames,
         mesh->IsVisible(), mesh->GetVertexCount(), mesh->GetFaceCount(), mesh->GetSubmeshCount());

      dtAnim::SubmeshArray submeshes;
      mesh->GetSubmeshes(submeshes);

      dtAnim::SubmeshInterface* submesh = NULL;
      dtAnim::SubmeshArray::iterator curSubmeshIter = submeshes.begin();
      dtAnim::SubmeshArray::iterator endSubmeshIter = submeshes.end();
      for (int submeshID = 0; curSubmeshIter != endSubmeshIter; ++curSubmeshIter, ++submeshID)
      {
         submesh = curSubmeshIter->get();

         dtAnim::MorphTargetArray submorphs;
         submesh->GetMorphTargets(submorphs);
         
         dtAnim::MorphTargetArray::iterator curMorphIter = submorphs.begin();
         dtAnim::MorphTargetArray::iterator endMorphIter = submorphs.end();
         for (size_t morphID = 0; curMorphIter != endMorphIter; ++curMorphIter, ++morphID)
         {
#if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
            QString nameToSend = QString::fromStdString(curMorphIter->get()->GetName());
#else
            QString nameToSend = QString::number(morphID);
#endif
            emit SubMorphTargetLoaded(meshName, submeshID, morphID, nameToSend);
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::UpdateMorphList(dtAnim::BaseModelWrapper& wrapper)
{
#if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
   Cal3DModelWrapper* calWrapper = dynamic_cast<Cal3DModelWrapper*>(&wrapper);
   if (calWrapper != NULL)
   {
      CalMorphTargetMixer *mixer = calWrapper->GetCalModel()->getMorphTargetMixer();
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
   }
#endif
}


