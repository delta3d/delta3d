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
#include <dtCore/globals.h>
#include <dtCore/light.h>
#include <dtCore/deltawin.h>
#include <dtCore/exceptionenum.h>

#include <dtAnim/characterfilehandler.h>
#include <dtAnim/chardrawable.h>
#include <dtAnim/characterfilehandler.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/chardrawable.h>
#include <dtAnim/cal3ddatabase.h>
#include <dtAnim/cal3dmodeldata.h>
#include <dtAnim/posemesh.h>
#include <dtAnim/characterwrapper.h>

#include <dtUtil/xercesparser.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/log.h>
#include <dtUtil/exception.h>

#include <xercesc/sax/SAXParseException.hpp>  // for base class
#include <xercesc/util/XMLString.hpp>

#include <osg/Geode>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include <osg/Material>
#include <osg/MatrixTransform>

#include <osgViewer/GraphicsWindow>
#include <osgViewer/CompositeViewer>

#include <cal3d/animation.h>

#include <dtAnim/hotspotdriver.h>
#include <dtCore/hotspotattachment.h>
#include <dtCore/refptr.h>
#include <dtCore/pointaxis.h>

typedef std::vector<dtCore::RefPtr<dtCore::HotSpotAttachment> > VectorHotSpot;

using namespace dtUtil;
using namespace dtCore;
using namespace dtAnim;

////////////////////////////////////////////////////////////////////////////////
Viewer::Viewer()
: mCalDatabase(&Cal3DDatabase::GetInstance())
, mPoseMeshes(NULL)
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
   osg::Box* box = new osg::Box(osg::Vec3(0.f,0.f,-0.025f), 2.5f, 2.5f, 0.05f);
   osg::ShapeDrawable *shapeDrawable = new osg::ShapeDrawable(box);

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

   std::string exampleDataPath = dtCore::GetEnvironment("DELTA_ROOT");
   std::string rootDataPath    = dtCore::GetEnvironment("DELTA_DATA");
   exampleDataPath += "/examples/data;" + rootDataPath;

   dtCore::SetDataFilePathList(dtCore::GetDataFilePathList() + ";" + exampleDataPath);

   //adjust the Camera position
   dtCore::Transform camPos;
   osg::Vec3 camXYZ(0.f, -5.f, 1.f);
   osg::Vec3 lookAtXYZ (0.f, 0.f, 1.f);
   osg::Vec3 upVec (0.f, 0.f, 1.f);
   camPos.SetLookAt(camXYZ, lookAtXYZ, upVec);

   GetCamera()->SetTransform(camPos);
   GetCamera()->SetNearFarCullingMode(dtCore::Camera::NO_AUTO_NEAR_FAR);

   mMotion = new OrbitMotionModel(GetKeyboard(), GetMouse());
   mMotion->SetTarget(GetCamera());
   mMotion->SetDistance(5.f);

   Light *l = GetScene()->GetLight(0);
   l->SetAmbient(0.7f, 0.7f, 0.7f, 1.f);  
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
void Viewer::OnLoadCharFile(const QString& filename)
{
   LOG_DEBUG("Loading file: " + filename.toStdString());

   QDir dir(filename);
   dir.cdUp();

   SetDataFilePathList(dtCore::GetDataFilePathList() + ";" +
                       dir.path().toStdString() + ";");

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
      dtAnim::Cal3DModelData *modelData = mCalDatabase->GetModelData(*wrapper.get());

      if (!modelData->GetPoseMeshFilename().empty())
      {
         OnLoadPoseMeshFile(modelData->GetPoseMeshFilename());
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

   //get all data for the meshes and emit
   for (int meshID=0; meshID<wrapper->GetCoreMeshCount(); meshID++)
   {
      QString nameToSend = QString::fromStdString(wrapper->GetCoreMeshName(meshID));
      emit MeshLoaded(meshID, nameToSend);
   }

   //get all material data and emit
   for (int matID=0; matID<wrapper->GetCoreMaterialCount(); matID++)
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
 
   LOG_DEBUG("Done loading file: " + filename.toStdString());
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

   while(boneNameIter!=boneNameEnd)
   {
      //create a HotSpot
      dtUtil::HotSpotDefinition hotSpotDefinition;
      hotSpotDefinition.mName = *boneNameIter;
      hotSpotDefinition.mParentName = *boneNameIter;

      //Create the axis geometry
      dtCore::PointAxis *axis = new dtCore::PointAxis();
      axis->SetLength(dtCore::PointAxis::X, 0.025f);
      axis->SetLength(dtCore::PointAxis::Y, 0.025f);
      axis->SetLength(dtCore::PointAxis::Z, 0.025f);

      mAttachmentController->AddAttachment(*axis, hotSpotDefinition);

      mBoneBasisGroup->addChild(axis->GetOSGNode());

      ++boneNameIter;
   }
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
   if(mCharacter.valid())
   {
      Cal3DModelWrapper* wrapper = mCharacter->GetCal3DWrapper();
      wrapper->BlendCycle(id, weight, delay);

      LOG_DEBUG("Started:" + dtUtil::ToString(id) + ", weight:" + dtUtil::ToString(weight) + ", delay:" + dtUtil::ToString(delay));
   }
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::OnStopAnimation(unsigned int id, float delay)
{
   if(mCharacter.valid())
   {
      Cal3DModelWrapper* wrapper = mCharacter->GetCal3DWrapper();
      wrapper->ClearCycle(id, delay);

      LOG_DEBUG("Stopped:" + dtUtil::ToString(id) + ", delay:" + dtUtil::ToString(delay));
   }
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::OnStartAction(unsigned int id, float delayIn, float delayOut)
{
   if(mCharacter.valid())
   {
      Cal3DModelWrapper* wrapper = mCharacter->GetCal3DWrapper();
      wrapper->ExecuteAction(id, delayIn, delayOut);
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
      mCharacter->GetCal3DWrapper()->ApplyCoreModelScaleFactor(scaleFactorValue);
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
void Viewer::OnTimeout()
{
   if (mCharacter.valid())
   {   
      dtAnim::Cal3DModelWrapper *rapper = mCharacter->GetCal3DWrapper();
      assert(rapper);

      mAttachmentController->Update(*rapper);

      std::vector<CalAnimation*> animVec = rapper->GetCalModel()->getMixer()->getAnimationVector();
      std::vector<CalAnimation*>::iterator animItr = animVec.begin();

      std::vector<float> weightList;
      weightList.reserve(animVec.size());

      while (animItr != animVec.end())
      {
         CalAnimation *anim = *(animItr);
         float weight = 0.f;

         if (anim!=NULL)
         {
            weight = anim->getWeight();  
         }

         weightList.push_back(weight);
        
         ++animItr;
      }   

      emit BlendUpdate(weightList);   
   }
}

////////////////////////////////////////////////////////////////////////////////
void Viewer::InitShadeDecorator()
{
   //osg::StateSet *stateset = new osg::StateSet;  
   //osg::PolygonMode *polyMode = new osg::PolygonMode;
   //polyMode->setMode(osg::PolygonMode::FRONT, osg::PolygonMode::FILL);
   //
   //osg::Material *material = new osg::Material;
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
}



