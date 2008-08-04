/// Included above ObjectViewer.h to avoid a compile error in linux
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtGui/QMessageBox>

#include "ObjectViewer.h"
#include "ObjectViewerData.h"
#include "ObjectWorkspace.h"

#include <dtCore/system.h>
#include <dtCore/transform.h>
#include <dtCore/scene.h>
#include <dtCore/camera.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/globals.h>
#include <dtCore/light.h>
#include <dtCore/deltawin.h>
#include <dtCore/exceptionenum.h>
#include <dtCore/shadermanager.h>

#include <dtUtil/xercesparser.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/log.h>
#include <dtUtil/exception.h>

#include <osg/Geode>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include <osg/Material>
#include <osg/MatrixTransform>

#include <osgViewer/GraphicsWindow>
#include <osgViewer/CompositeViewer>

#include <assert.h>

///////////////////////////////////////////////////////////////////////////////
ObjectViewer::ObjectViewer()
{
   mShadedScene   = new osg::Group;
   mUnShadedScene = new osg::Group;

   osg::StateSet* shadedState = mShadedScene->getOrCreateStateSet();
   shadedState->setMode(GL_LIGHTING, osg::StateAttribute::ON);

   dtUtil::Log::GetInstance().SetLogLevel(dtUtil::Log::LOG_DEBUG);
}

///////////////////////////////////////////////////////////////////////////////
ObjectViewer::~ObjectViewer()
{
}

///////////////////////////////////////////////////////////////////////////////
void ObjectViewer::Config()
{
   dtABC::Application::Config();
   GetCompositeViewer()->setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);

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

   // Add the compass (3d basis axes) to the bottom left of the screen
   mCompass = new dtCore::Compass(GetCamera());
   GetScene()->GetSceneNode()->addChild(mCompass->GetOSGNode());

   mModelMotion = new dtCore::OrbitMotionModel(GetKeyboard(), GetMouse());
   mModelMotion->SetTarget(GetCamera());
   mModelMotion->SetDistance(5.f);

   dtCore::Light *l = GetScene()->GetLight(0);
   l->SetAmbient(0.2f, 0.2f, 0.2f, 1.f);  
   l->SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f); 
   l->SetSpecular(1.0f, 1.0f, 1.0f, 1.0f);

   // Infinite lights must start here, point light from the postive y axis
   l->GetLightSource()->getLight()->setPosition(osg::Vec4(-osg::Y_AXIS, 0.0f));

   mWireDecorator  = new osg::Group;
   mShadeDecorator = new osg::Group;

   mLightArrow = new dtCore::Object;
   mLightArrow->LoadFile("models/LightArrow.ive");     

   dtCore::Transform lightArrowTransform;
   lightArrowTransform.SetTranslation(0.0f, 0.0f, 0.0f);

   mLightArrowTransformable = new dtCore::Transformable;
   mLightArrowTransformable->AddChild(mLightArrow.get()); 
   mLightArrowTransformable->AddChild(l);

   mLightMotion = new dtCore::OrbitMotionModel(GetKeyboard(), GetMouse());
   mLightMotion->SetTarget(mLightArrowTransformable.get());
   mLightMotion->SetDistance(3.f);
   mLightMotion->SetFocalPoint(osg::Vec3());
   mLightMotion->SetLeftRightTranslationAxis(NULL);
   mLightMotion->SetUpDownTranslationAxis(NULL);

   GetScene()->GetSceneNode()->addChild(mShadedScene.get());
   GetScene()->GetSceneNode()->addChild(mUnShadedScene.get());

   GetScene()->AddDrawable(mLightArrowTransformable.get());
  
   InitWireDecorator(); 
   InitGridPlanes();

   OnSetShaded();
   OnToggleGrid(true);

   OnEnterObjectMode();

   dtUtil::Log::GetInstance().SetLogLevel(dtUtil::Log::LOG_DEBUG);
}

///////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnLoadShaderFile(const QString &filename)
{
   try
   {
      dtCore::ShaderManager &shaderManager = dtCore::ShaderManager::GetInstance();
      shaderManager.LoadShaderDefinitions(filename.toStdString());

      std::vector<dtCore::RefPtr<dtCore::ShaderGroup> > shaderGroupList;
      shaderManager.GetAllShaderGroupPrototypes(shaderGroupList);

      // Emit all shader groups and their individual shaders      
      {
         size_t groupIndex = shaderGroupList.size() - 1;

         std::vector<dtCore::RefPtr<dtCore::ShaderProgram> > programList;
         shaderGroupList[groupIndex]->GetAllShaders(programList);

         const std::string &groupName = shaderGroupList[groupIndex]->GetName();

         for (size_t programIndex = 0; programIndex < programList.size(); ++programIndex)
         {
            emit ShaderLoaded(groupName, programList[programIndex]->GetName());
         }
      }
   }
   catch (dtUtil::Exception &e)
   {
      QMessageBox::critical(NULL, "Error", e.ToString().c_str());
   }   
}

///////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnReloadShaderFile(const QString &filename)
{
   dtCore::ShaderManager::GetInstance().ReloadAndReassignShaderDefinitions(filename.toStdString());
}

///////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnLoadGeometryFile(const std::string &filename)
{
   // For now only allow 1 object
   if (mObject.valid())
   {
      OnUnloadGeometryFile();
   }

   mObject = new dtCore::Object; 
   mObject->LoadFile(filename);

   // set up the ObjectViewer's scene graph
   mShadeDecorator->addChild(mObject->GetOSGNode());
   mWireDecorator->addChild(mObject->GetOSGNode());

   osg::Vec3 center;   

   if (mObject.valid())
   {
      float radius;
      mObject->GetBoundingSphere(&center, &radius); 

      mLightMotion->SetDistance(radius * 0.5f);
      mLightMotion->SetFocalPoint(center); 

      // Adust the size of the light arrow
      float arrowScale = radius * 0.5f;
      mLightArrow->SetScale(osg::Vec3(arrowScale, arrowScale, arrowScale));
   }   
}

///////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnUnloadGeometryFile()
{   
   if (mObject.valid())
   {
      mShadeDecorator->removeChild(mObject->GetOSGNode());
      mWireDecorator->removeChild(mObject->GetOSGNode());

      mObject = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnApplyShader(const std::string &groupName, const std::string &programName)
{
   dtCore::ShaderManager &shaderManager = dtCore::ShaderManager::GetInstance();

   dtCore::ShaderProgram *program = shaderManager.FindShaderPrototype(programName, groupName);
   assert(program);

   shaderManager.AssignShaderFromPrototype(*program, *mShadedScene);
} 

///////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnRemoveShader()
{
   dtCore::ShaderManager &shaderManager = dtCore::ShaderManager::GetInstance();
   shaderManager.UnassignShaderFromNode(*mShadedScene);
}

///////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnSetShaded()
{
   mShadedScene->removeChild(mWireDecorator.get());
   mShadedScene->removeChild(mShadeDecorator.get());

   mShadedScene->addChild(mShadeDecorator.get());
}

///////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnSetWireframe()
{
   mShadedScene->removeChild(mWireDecorator.get());
   mShadedScene->removeChild(mShadeDecorator.get());

   mShadedScene->addChild(mWireDecorator.get());
}

///////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnSetShadedWireframe()
{
   mShadedScene->removeChild(mWireDecorator.get());
   mShadedScene->removeChild(mShadeDecorator.get());

   mShadedScene->addChild(mWireDecorator.get());
   mShadedScene->addChild(mShadeDecorator.get());
}

///////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnToggleGrid(bool shouldDisplay)
{
   if (shouldDisplay)
   {
      mUnShadedScene->addChild(mGridGeode.get());
   }
   else
   {
      mUnShadedScene->removeChild(mGridGeode.get());
   }  
}

///////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnAddLight(int id)
{
   //dtCore::Light *l = GetScene()->GetLight(0);
   //l->SetAmbient(0.7f, 0.7f, 0.7f, 1.f);  
   //l->SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f);  
}

///////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnEnterObjectMode()
{
   mModelMotion->SetEnabled(true);
   mLightMotion->SetEnabled(false);   

   mLightArrow->SetActive(false);
}

///////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnEnterLightMode()
{  
   mModelMotion->SetEnabled(false);
   mLightMotion->SetEnabled(true);  
   
   mLightArrow->SetActive(true);
}

///////////////////////////////////////////////////////////////////////////////
void ObjectViewer::InitWireDecorator()
{
   osg::StateSet *stateset = new osg::StateSet;
   osg::PolygonOffset *polyOffset = new osg::PolygonOffset;
   polyOffset->setFactor(-1.0f);
   polyOffset->setUnits(-1.0f);
   osg::PolygonMode *polyMode = new osg::PolygonMode;
   polyMode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
   stateset->setAttributeAndModes(polyOffset, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
   stateset->setAttributeAndModes(polyMode, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

   osg::Material *material = new osg::Material;
   stateset->setAttributeAndModes(material, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
   stateset->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);

   stateset->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );

   mWireDecorator->setStateSet(stateset);
}

///////////////////////////////////////////////////////////////////////////////
void ObjectViewer::InitGridPlanes()
{
   const int GRID_LINE_COUNT = 49;
   const float GRID_LINE_SPACING = 2.0f;

   const int numVerts(2 * 2 * GRID_LINE_COUNT);
   const float length(((GRID_LINE_COUNT - 1) * GRID_LINE_SPACING) / 2.f);

   osg::Vec3 verts[numVerts];
   int indx(0L);

   for(int ii(0L); ii < GRID_LINE_COUNT; ii++)
   {
      verts[indx++].set( -length + ii * GRID_LINE_SPACING,  length, 0.0f);
      verts[indx++].set( -length + ii * GRID_LINE_SPACING, -length, 0.0f);
   }

   for( int ii(0L); ii < GRID_LINE_COUNT; ii++ )
   {
      verts[indx++].set(  length, -length + ii * GRID_LINE_SPACING, 0.0f);
      verts[indx++].set( -length, -length + ii * GRID_LINE_SPACING, 0.0f);
   }

   osg::Geometry* geometry = new osg::Geometry;
   assert( geometry );

   geometry->setVertexArray(new osg::Vec3Array(numVerts, verts));
   geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, numVerts));

   assert(!mGridGeode.valid());
   mGridGeode = new osg::Geode;

   mGridGeode->addDrawable(geometry);
   mGridGeode->getOrCreateStateSet()->setMode(GL_LIGHTING, 0);   
}

///////////////////////////////////////////////////////////////////////////////
void ObjectViewer::PostFrame(const double)
{
   // Broadcast the current state of all the lights in the scene
   for (int lightIndex = 0; lightIndex < dtCore::MAX_LIGHTS; ++lightIndex)
   {
      dtCore::Light* light = GetScene()->GetLight(lightIndex);

      if (light)
      {
         dtCore::Transform arrowTransform;
         mLightArrowTransformable->GetTransform(arrowTransform);

         //osg::Vec3 lightPos = arrowTransform.GetTranslation();

         //std::ostringstream oss;
         //oss << "arrow pos: (" << lightPos.x() << ", " << lightPos.y() << ", " << lightPos.z() << ")";

         //std::cout << oss.str() << std::endl;

         LightInfo lightInfo;
         lightInfo.light = light;
         lightInfo.transform = &arrowTransform;

         emit LightUpdate(lightInfo);
         continue;
      }

      break;
   }
}


