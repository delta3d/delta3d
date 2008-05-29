/// Included above ObjectViewer.h to avoid a compile error in linux
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtGui/QColor>
#include <QtGui/QMessageBox>

#include "ObjectViewer.h"
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

#include <dtAnim/characterfilehandler.h>
#include <dtAnim/chardrawable.h>
#include <dtAnim/characterfilehandler.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/chardrawable.h>
#include <dtAnim/cal3ddatabase.h>
#include <dtAnim/cal3dmodeldata.h>

#include <dtUtil/xercesparser.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/log.h>
#include <dtUtil/exception.h>

#include <dtGUI/ceuidrawable.h>

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

#include <CEGUI/CEGUI.h>
#include <CEGUI/CEGUISystem.h>
#include <CEGUI/CEGUISchemeManager.h>

#include <dtAnim/hotspotdriver.h>
#include <dtCore/hotspotattachment.h>
#include <dtCore/refptr.h>
#include <dtCore/pointaxis.h>

typedef std::vector<dtCore::RefPtr<dtCore::HotSpotAttachment> > VectorHotSpot;

/////////////////////////////////////////////////////////////////////////////////////////
ObjectViewer::ObjectViewer()
{
   mShadedScene   = new osg::Group;
   mUnShadedScene = new osg::Group;

   dtUtil::Log::GetInstance().SetLogLevel(dtUtil::Log::LOG_DEBUG);   
}

/////////////////////////////////////////////////////////////////////////////////////////
ObjectViewer::~ObjectViewer()
{
}

/////////////////////////////////////////////////////////////////////////////////////////
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
   osg::Vec3 camXYZ( 0.f, -5.f, 1.f );
   osg::Vec3 lookAtXYZ ( 0.f, 0.f, 1.f );
   osg::Vec3 upVec ( 0.f, 0.f, 1.f );
   camPos.SetLookAt( camXYZ, lookAtXYZ, upVec );

   GetCamera()->SetTransform( camPos );
   GetCamera()->SetNearFarCullingMode(dtCore::Camera::NO_AUTO_NEAR_FAR);

   mMotion = new dtCore::OrbitMotionModel( GetKeyboard(), GetMouse() );
   mMotion->SetTarget( GetCamera() );
   mMotion->SetDistance(5.f);

   dtCore::Light *l = GetScene()->GetLight(0);
   l->SetAmbient(0.7f, 0.7f, 0.7f, 1.f);  
   l->SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f);  

   mWireDecorator  = new osg::Group;
   mShadeDecorator = new osg::Group;

   GetScene()->GetSceneNode()->addChild(mShadedScene.get());
   GetScene()->GetSceneNode()->addChild(mUnShadedScene.get());
  
   InitWireDecorator(); 
   InitShadeDecorator();
   InitGridPlanes();

   OnSetShaded();
   OnToggleGrid(true);

   dtUtil::Log::GetInstance().SetLogLevel(dtUtil::Log::LOG_DEBUG);
}

/////////////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnLoadShaderFile(const QString &filename)
{
   dtCore::ShaderManager &shaderManager = dtCore::ShaderManager::GetInstance();
   shaderManager.LoadShaderDefinitions(filename.toStdString());

   std::vector<dtCore::RefPtr<dtCore::ShaderGroup> > shaderGroupList;
   shaderManager.GetAllShaderGroupPrototypes(shaderGroupList);

   // Emit all shader groups and their individual shaders
   for (size_t groupIndex = 0; groupIndex < shaderGroupList.size(); ++groupIndex)
   {
      std::vector<dtCore::RefPtr<dtCore::ShaderProgram> > programList;
      shaderGroupList[groupIndex]->GetAllShaders(programList);

      const std::string &groupName = shaderGroupList[groupIndex]->GetName();

      for (size_t programIndex = 0; programIndex < programList.size(); ++programIndex)
      {
         emit ShaderLoaded(groupName, programList[programIndex]->GetName());
      }
   }
}

/////////////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnLoadGeometryFile(const std::string &filename)
{
   // For now only allow 1 object
   if (mObject.valid())
   {
      OnUnloadGeometryFile();
   }

   mObject = new dtCore::Object; 
   mObject->LoadFile("staticmeshes/" + filename);

   // set up the ObjectViewer's scene graph
   mShadeDecorator->addChild(mObject->GetOSGNode());
   mWireDecorator->addChild(mObject->GetOSGNode());
}

/////////////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnUnloadGeometryFile()
{   
   if (mObject.valid())
   {
      mShadeDecorator->removeChild(mObject->GetOSGNode());
      mWireDecorator->removeChild(mObject->GetOSGNode());

      mObject = NULL;
   }
}

/////////////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnApplyShader(const std::string &groupName, const std::string &programName)
{
   dtCore::ShaderManager &shaderManager = dtCore::ShaderManager::GetInstance();

   dtCore::ShaderProgram *program = shaderManager.FindShaderPrototype(programName, groupName);
   assert(program);

   shaderManager.AssignShaderFromPrototype(*program, *mShadedScene);
} 

/////////////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnRemoveShader()
{
   dtCore::ShaderManager &shaderManager = dtCore::ShaderManager::GetInstance();
   shaderManager.UnassignShaderFromNode(*mShadedScene);
}

/////////////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnSetShaded()
{
   mShadedScene->removeChild(mWireDecorator.get());
   mShadedScene->removeChild(mShadeDecorator.get());

   mShadedScene->addChild(mShadeDecorator.get());
}

/////////////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnSetWireframe()
{
   mShadedScene->removeChild(mWireDecorator.get());
   mShadedScene->removeChild(mShadeDecorator.get());

   mShadedScene->addChild(mWireDecorator.get());
}

/////////////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnSetShadedWireframe()
{
   mShadedScene->removeChild(mWireDecorator.get());
   mShadedScene->removeChild(mShadeDecorator.get());

   mShadedScene->addChild(mWireDecorator.get());
   mShadedScene->addChild(mShadeDecorator.get());
}

/////////////////////////////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::InitShadeDecorator()
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

/////////////////////////////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::PostFrame(const double)
{
   
}


