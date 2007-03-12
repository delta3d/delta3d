
#include "Viewer.h"
#include <dtUtil/log.h>
#include <dtCore/transform.h>
#include <dtCore/scene.h>
#include <dtCore/camera.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/globals.h>
#include <dtCore/light.h>
#include <dtChar/characterfilehandler.h>
#include <dtChar/chardrawable.h>
#include <dtChar/characterfilehandler.h>
#include <dtChar/coremodel.h>
#include <dtChar/model.h>
#include <dtUtil/xercesparser.h>
#include <dtUtil/stringutils.h>

#include <osg/Geode>
#include <osg/Shape>
#include <osg/ShapeDrawable>

#include <QDir>
#include <QDebug>
#include <QString>

using namespace dtUtil;
using namespace dtCore;

Viewer::Viewer():
mMotion(NULL),
mCoreModel(NULL),
mCharacter(NULL)
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
   l->SetAmbient(0.3f, 0.3f, 0.3f, 1.f);

   GetScene()->GetSceneNode()->addChild( MakePlane() );


   Log::GetInstance().SetLogLevel(Log::LOG_DEBUG);
   
   mTimer.start(10, this);

}

void Viewer::OnLoadCharFile( const QString &filename )
{
   LOG_DEBUG("loading file: " + filename.toStdString() );

   QDir dir(filename);
   dir.cdUp();

   SetDataFilePathList( GetDeltaDataPathList() + ";" +
                        dir.path().toStdString() + ";" );

   //read the character's definition file
   dtChar::CharacterFileHandler handler;
   dtUtil::XercesParser parser;
   bool result = parser.Parse(filename.toStdString(), handler ); 

   //create the coreModel ("template")
   mCoreModel = new dtChar::CoreModel("test");


   //load skeleton
   mCoreModel->LoadSkeleton( FindFileInPathList(handler.mSkeletonFilename) );

   //load animations
   unsigned int id = 0;
   std::vector<std::string>::iterator animItr = handler.mAnimationFilenames.begin();
   while (animItr != handler.mAnimationFilenames.end())
   {
      mCoreModel->LoadAnimation( FindFileInPathList(*animItr) );
      emit OnAnimationLoaded(id, QString().fromStdString(*animItr) );
      ++id;
      ++animItr;
   }

   //load meshes
   std::vector<std::string>::iterator meshItr = handler.mMeshFilenames.begin();
   while (meshItr != handler.mMeshFilenames.end())
   {
      mCoreModel->LoadMesh( FindFileInPathList(*meshItr) );
      ++meshItr;
   }

   //load materials
   std::vector<std::string>::iterator matItr = handler.mMaterialFilenames.begin();
   while (matItr != handler.mMaterialFilenames.end())
   {
      mCoreModel->LoadMaterial( FindFileInPathList(*matItr) );
      ++matItr;
   }

   //load all the required textures (tries to do this from loadMaterial as well)
   mCoreModel->LoadAllTextures(dtCore::GetDeltaRootPath() + "/examples/data/marine/");

   mCharacter = new dtChar::CharDrawable();
   AddDrawable(mCharacter.get());

   //create an instance from the coreModel template
   mCharacter->mModel->Create(mCoreModel.get());
}

void Viewer::OnStartAnimation( unsigned int id, float weight, float delay )
{
   mCharacter->mModel->StartLoop(id, weight, delay);
   LOG_DEBUG("Started:" + dtUtil::ToString(id) + ", weight:" + dtUtil::ToString(weight) + ", delay:" + dtUtil::ToString(delay));
}

void Viewer::OnStopAnimation( unsigned int id, float delay )
{
   mCharacter->mModel->StopLoop(id, delay);
   LOG_DEBUG("Stopped:" + dtUtil::ToString(id) + ", delay:" + dtUtil::ToString(delay));
}

void Viewer::OnStartAction( unsigned int id, float delayIn, float delayOut )
{
   mCharacter->mModel->StartAction( id, delayIn, delayOut );
}