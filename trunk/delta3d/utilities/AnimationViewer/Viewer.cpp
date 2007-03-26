
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
#include <dtUtil/xercesparser.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/fileutils.h>

#include <osg/Geode>
#include <osg/Shape>
#include <osg/ShapeDrawable>

#include <QDir>
#include <QDebug>
#include <QString>
#include <QMessageBox>

using namespace dtUtil;
using namespace dtCore;

Viewer::Viewer():
mMotion(NULL),
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
  
   if (mCharacter.get())
   {
      RemoveDrawable(mCharacter.get());  
      mCharacter = NULL;
   }

   mCharacter = new dtAnim::CharDrawable();
   AddDrawable(mCharacter.get());

   //create an instance from the character definition file
   mCharacter->Create(filename.toStdString());

   for (int animID=0; animID<mCharacter->GetCal3DWrapper()->GetCoreAnimationCount(); animID++)
   {
      emit OnAnimationLoaded(animID, QString::number(animID) );
   }        
}

void Viewer::OnStartAnimation( unsigned int id, float weight, float delay )
{
   mCharacter->StartLoop(id, weight, delay);
   LOG_DEBUG("Started:" + dtUtil::ToString(id) + ", weight:" + dtUtil::ToString(weight) + ", delay:" + dtUtil::ToString(delay));
}

void Viewer::OnStopAnimation( unsigned int id, float delay )
{
   mCharacter->StopLoop(id, delay);
   LOG_DEBUG("Stopped:" + dtUtil::ToString(id) + ", delay:" + dtUtil::ToString(delay));
}

void Viewer::OnStartAction( unsigned int id, float delayIn, float delayOut )
{
   mCharacter->StartAction( id, delayIn, delayOut );
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
