/// Included above Viewer.h to avoid a compile error in linux
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtGui/QColor>

#include "Viewer.h"

#include <dtUtil/log.h>
#include <dtCore/transform.h>
#include <dtCore/scene.h>
#include <dtCore/camera.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/globals.h>
#include <dtCore/light.h>

#include <dtAnim/characterfilehandler.h>
#include <dtAnim/chardrawable.h>
#include <dtAnim/characterfilehandler.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/chardrawable.h>
#include <dtAnim/cal3ddatabase.h>

#include <dtUtil/xercesparser.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/fileutils.h>

#include <xercesc/sax/SAXParseException.hpp>  // for base class
#include <xercesc/util/XMLString.hpp>

#include <osg/Geode>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include <osg/Material>

using namespace dtUtil;
using namespace dtCore;
using namespace dtAnim;

Viewer::Viewer():
   mDatabase(&Cal3DDatabase::GetInstance())
{

}

Viewer::~Viewer()
{
   mTimer.stop();
}

osg::Geode* MakePlane()
{
   osg::Geode *geode = new osg::Geode();
   osg::Box *box = new osg::Box( osg::Vec3(0.f,0.f,-0.025f), 2.5f, 2.5f, 0.05f);
   osg::ShapeDrawable *shapeDrawable = new osg::ShapeDrawable(box);

   geode->addDrawable(shapeDrawable);

   return geode;
}

void Viewer::Config()
{
   dtABC::Application::Config();

   //adjust the Camera position
   dtCore::Transform camPos;
   osg::Vec3 camXYZ( 0.f, -5.f, 1.f );
   osg::Vec3 lookAtXYZ ( 0.f, 0.f, 1.f );
   osg::Vec3 upVec ( 0.f, 0.f, 1.f );
   camPos.SetLookAt( camXYZ, lookAtXYZ, upVec );
   GetCamera()->SetTransform( camPos );

   mMotion = new OrbitMotionModel( GetKeyboard(), GetMouse() );
   mMotion->SetTarget( GetCamera() );
   mMotion->SetDistance(5.f);

   Light *l = GetScene()->GetLight(0);
   l->SetAmbient(0.7f, 0.7f, 0.7f, 1.f);  
   l->SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f);  
  
   GetScene()->GetOrCreateSceneNode()->addChild(MakePlane());

   mWireDecorator  = new osg::Group;
   mShadeDecorator = new osg::Group;

   InitWireDecorator(); 
   InitShadeDecorator();

   OnSetShaded();

   Log::GetInstance().SetLogLevel(Log::LOG_DEBUG);

   mTimer.start(10, this);
}

void Viewer::OnLoadCharFile( const QString &filename )
{
   LOG_DEBUG("loading file: " + filename.toStdString() );

   QDir dir(filename);
   dir.cdUp();

   SetDataFilePathList( GetDeltaDataPathList() + ":" +
                        dir.path().toStdString() + ":" );

   // try to clean up the scene graph
   if (mCharacter.valid())
   {        
      mShadeDecorator->removeChild(mCharacter->GetGeode());
      mWireDecorator->removeChild(mCharacter->GetGeode());
      mCharacter = NULL;
   }

   //wipe out any previously loaded characters. This will ensure we can 
   //reload the same file (which might have been modified).
   mDatabase->TruncateDatabase();
   mDatabase->PurgeLoaderCaches();

   //create an instance from the character definition file
   try
   {
      // Create a new Cal3DWrapper
      dtCore::RefPtr<Cal3DModelWrapper> wrapper = mDatabase->Load(filename.toStdString());
       
      if( mCharacter.valid() )
      {
         mCharacter->SetCal3DWrapper( wrapper.get() );
      }
      else
      {
         mCharacter = new CharDrawable( wrapper.get() );
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

      XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release( &msg );
      emit ErrorOccured(errorMsg);
      return;
   }

   // set up the viewer's scene graph
   mShadeDecorator->addChild(mCharacter->GetGeode());
   mWireDecorator->addChild(mCharacter->GetGeode());
   dtCore::RefPtr<Cal3DModelWrapper> wrapper = mCharacter->GetCal3DWrapper();

   //get all the data for animations and tell the world
   for (int animID=0; animID<wrapper->GetCoreAnimationCount(); animID++)
   {
      QString nameToSend = QString::fromStdString(wrapper->GetCoreAnimationName(animID));
      unsigned int trackCount = wrapper->GetCoreAnimationTrackCount(animID);
      unsigned int keyframes = wrapper->GetCoreAnimationKeyframeCount(animID);
      float dur = wrapper->GetCoreAnimationDuration(animID);
      emit AnimationLoaded(animID, nameToSend, trackCount, keyframes, dur );
   }

   //get all data for the meshes and emit
   for (int meshID=0; meshID<wrapper->GetCoreMeshCount(); meshID++)
   {
      QString nameToSend = QString::fromStdString( wrapper->GetCoreMeshName(meshID) );
      emit MeshLoaded(meshID, nameToSend);
   }

   //get all material data and emit
   for (int matID=0; matID<wrapper->GetCoreMaterialCount(); matID++)
   {
      QString nameToSend = QString::fromStdString(wrapper->GetCoreMaterialName(matID));

      osg::Vec4 diffuse = wrapper->GetCoreMaterialDiffuse(matID);
      QColor diffColor(diffuse[0], diffuse[1], diffuse[2], diffuse[3]);

      osg::Vec4 ambient = wrapper->GetCoreMaterialAmbient(matID);
      QColor ambColor(ambient[0], ambient[1], ambient[2], ambient[3]);

      osg::Vec4 specular = wrapper->GetCoreMaterialSpecular(matID);
      QColor specColor(specular[0], specular[1], specular[2], specular[3]);

      float shininess = wrapper->GetCoreMaterialShininess(matID);

      emit MaterialLoaded(matID, nameToSend, diffColor, ambColor, specColor, shininess);
   }
}

void Viewer::OnStartAnimation( unsigned int id, float weight, float delay )
{
   if( mCharacter.valid() )
   {
      Cal3DModelWrapper* wrapper = mCharacter->GetCal3DWrapper();
      wrapper->BlendCycle(id, weight, delay);

      LOG_DEBUG("Started:" + dtUtil::ToString(id) + ", weight:" + dtUtil::ToString(weight) + ", delay:" + dtUtil::ToString(delay));
   }
}

void Viewer::OnStopAnimation( unsigned int id, float delay )
{
   if( mCharacter.valid() )
   {
      Cal3DModelWrapper* wrapper = mCharacter->GetCal3DWrapper();
      wrapper->ClearCycle(id, delay);

      LOG_DEBUG("Stopped:" + dtUtil::ToString(id) + ", delay:" + dtUtil::ToString(delay));
   }
}

void Viewer::OnStartAction( unsigned int id, float delayIn, float delayOut )
{
   if( mCharacter.valid() )
   {
      Cal3DModelWrapper* wrapper = mCharacter->GetCal3DWrapper();
      wrapper->ExecuteAction(id, delayIn, delayOut);
   }
}

void Viewer::OnLOD_Changed( float zeroToOneValue )
{
   assert(fabs(zeroToOneValue)<= 1.0f);

   if (mCharacter.get())
   {
      dtAnim::Cal3DModelWrapper* mcHammer = mCharacter->GetCal3DWrapper();
      assert(mcHammer);

      mcHammer->SetLODLevel(zeroToOneValue);
   }  
}

void Viewer::OnSetShaded()
{
   GetScene()->GetOrCreateSceneNode()->removeChild(mWireDecorator.get());
   GetScene()->GetOrCreateSceneNode()->removeChild(mShadeDecorator.get());

   GetScene()->GetOrCreateSceneNode()->addChild(mShadeDecorator.get());
}

void Viewer::OnSetWireframe()
{
   GetScene()->GetOrCreateSceneNode()->removeChild(mWireDecorator.get());
   GetScene()->GetOrCreateSceneNode()->removeChild(mShadeDecorator.get());  

   GetScene()->GetOrCreateSceneNode()->addChild(mWireDecorator.get());
}

void Viewer::OnSetShadedWireframe()
{
   GetScene()->GetOrCreateSceneNode()->removeChild(mWireDecorator.get());
   GetScene()->GetOrCreateSceneNode()->removeChild(mShadeDecorator.get());

   GetScene()->GetOrCreateSceneNode()->addChild(mWireDecorator.get());
   GetScene()->GetOrCreateSceneNode()->addChild(mShadeDecorator.get());   
}

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

void Viewer::InitWireDecorator()
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

void Viewer::OnAttachMesh( int meshID )
{
   mMeshesToAttach.push_back(meshID);
}

void Viewer::OnDetachMesh( int meshID )
{
   mMeshesToDetach.push_back(meshID);
}

void Viewer::PostFrame( const double deltaFrameTime )
{
   {
      std::vector<int>::iterator attachItr = mMeshesToAttach.begin();
      while (attachItr != mMeshesToAttach.end())
      {
         bool success = mCharacter->GetCal3DWrapper()->AttachMesh( (*attachItr) );
         assert(success);

         ++attachItr;
      }

      mMeshesToAttach.clear();
   }
   
   {
      std::vector<int>::iterator detachItr = mMeshesToDetach.begin();
      while (detachItr != mMeshesToDetach.end())
      {
         bool success = mCharacter->GetCal3DWrapper()->DetachMesh( (*detachItr) );
         assert(success);
         ++detachItr;
      }

      mMeshesToDetach.clear();
   }

}
