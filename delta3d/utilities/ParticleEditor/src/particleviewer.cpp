#include <particleviewer.h>

#include <osg/Vec3f>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>

//#include <osgParticle/AccelOperator>
//#include <osgParticle/FluidFrictionOperator>
//#include <osgParticle/ForceOperator>
#include <osgParticle/MultiSegmentPlacer>
#include <osgParticle/PointPlacer>
#include <osgParticle/RadialShooter>
#include <osgParticle/RandomRateCounter>
#include <osgParticle/SectorPlacer>
#include <osgParticle/SegmentPlacer>

#include <osgViewer/GraphicsWindow>
#include <osgViewer/CompositeViewer>

#include <dtCore/refptr.h>
#include <dtCore/system.h>
#include <dtCore/globals.h>
#include <dtCore/transform.h>
#include <dtCore/scene.h>
#include <dtCore/camera.h>
#include <dtCore/light.h>
#include <dtCore/compass.h>

////////////////////////////////////////////////////////////////////////////////
ParticleViewer::ParticleViewer()
: mpParticleSystemGroup(NULL)
, mpParticleSystemUpdater(NULL)
, mLayerIndex(0)
{
}

////////////////////////////////////////////////////////////////////////////////
ParticleViewer::~ParticleViewer()
{
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::Config()
{
   dtABC::Application::Config();
   GetCompositeViewer()->setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);

   mMotion = new OrbitMotionModel(GetMouse(), GetCamera());

   MakeCompass();
   MakeGrids();

   GetScene()->GetSceneNode()->addChild(mpXYGridTransform);
   GetScene()->GetSceneNode()->addChild(mpXZGridTransform);
   GetScene()->GetSceneNode()->addChild(mpYZGridTransform);

   mMotion->SetCompassTransform(mpCompassTransform);

   CreateNewParticleSystem();
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::CreateNewParticleSystem()
{
   if(mpParticleSystemGroup != NULL)
   {
      GetScene()->GetSceneNode()->removeChild(mpParticleSystemGroup);
   }

   mpParticleSystemGroup = new osg::Group();
   mpParticleSystemUpdater = new osgParticle::ParticleSystemUpdater();
   mpParticleSystemGroup->addChild(mpParticleSystemUpdater);
   GetScene()->GetSceneNode()->addChild(mpParticleSystemGroup);
   //setParticleSystemFilename("");
   mLayers.clear();
   //UpdateLayers();
   CreateNewParticleLayer();
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::CreateNewParticleLayer()
{
   ParticleSystemLayer layer;
   layer.mParticleSystem = new osgParticle::ParticleSystem();
   layer.mParticleSystem->setDefaultAttributes("", true, false);
   layer.mpParticle = new osgParticle::Particle();
   layer.mParticleSystem->setDefaultParticleTemplate(*layer.mpParticle);
   layer.mGeode = new osg::Geode();

   char buf[256];
   sprintf(buf, "Layer %u", unsigned(mLayers.size()));
   layer.mGeode->setName(buf);
   layer.mGeode->addDrawable(layer.mParticleSystem.get());
   mpParticleSystemGroup->addChild(layer.mGeode.get());

   layer.mEmitterTransform = new osg::MatrixTransform();
   layer.mModularEmitter = new osgParticle::ModularEmitter();
   layer.mModularEmitter->setParticleSystem(layer.mParticleSystem.get());
   osgParticle::RandomRateCounter* rrc = new osgParticle::RandomRateCounter();
   rrc->setRateRange(20, 30);
   layer.mModularEmitter->setCounter(rrc);
   layer.mModularEmitter->setPlacer(new osgParticle::PointPlacer());
   layer.mModularEmitter->setShooter(new osgParticle::RadialShooter());
   layer.mEmitterTransform->addChild(layer.mModularEmitter.get());
   mpParticleSystemGroup->addChild(layer.mEmitterTransform.get());

   layer.mModularProgram = new osgParticle::ModularProgram();
   layer.mModularProgram->setParticleSystem(layer.mParticleSystem.get());
   mpParticleSystemGroup->addChild(layer.mModularProgram.get());
   mpParticleSystemUpdater->addParticleSystem(layer.mParticleSystem.get());
   mLayers.push_back(layer);

   ResetEmitters();
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::DeleteSelectedLayer()
{
   mpParticleSystemGroup->removeChild(mLayers[mLayerIndex].mGeode.get());
   mpParticleSystemGroup->removeChild(mLayers[mLayerIndex].mEmitterTransform.get());
   mpParticleSystemGroup->removeChild(mLayers[mLayerIndex].mModularProgram.get());

   mpParticleSystemUpdater->removeParticleSystem(mLayers[mLayerIndex].mParticleSystem.get());

   mLayers.erase(mLayers.begin() + mLayerIndex);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::ToggleSelectedLayerHidden()
{
   if(mLayers[mLayerIndex].mModularEmitter->isEnabled())
   {
      mLayers[mLayerIndex].mModularEmitter->setEnabled(false);
   }
   else
   {
      mLayers[mLayerIndex].mModularEmitter->setEnabled(true);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::ResetEmitters()
{
   for (unsigned int i = 0; i < mLayers.size(); ++i)
   {
      mLayers[i].mModularEmitter->setCurrentTime(0.0);
      mLayers[i].mModularProgram->setCurrentTime(0.0);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::UpdateSelectionIndex(int newIndex)
{
   mLayerIndex = newIndex;
   if(0 <= mLayerIndex && mLayerIndex < static_cast<int>(mLayers.size()))
   {
      emit LayerHiddenChanged(mLayers[mLayerIndex].mModularEmitter->isEnabled());
   }
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::MakeCompass()
{
   dtCore::Compass* compass = new dtCore::Compass(GetCamera());
   GetScene()->AddDrawable(compass);
   mpCompassTransform = (osg::MatrixTransform*)compass->GetOSGNode();
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::MakeGrids()
{
   const int numVertices = 2 * 2 * GRID_LINE_COUNT;
   osg::Vec3 vertices[numVertices];
   float length = (GRID_LINE_COUNT - 1) * GRID_LINE_SPACING;
   int ptr = 0;

   for(int i = 0; i < GRID_LINE_COUNT; ++i)
   {
      vertices[ptr++].set(-length / 2 + i * GRID_LINE_SPACING, length / 2, 0.0f);
      vertices[ptr++].set(-length / 2 + i * GRID_LINE_SPACING, -length / 2, 0.0f);
   }

   for (int i = 0; i < GRID_LINE_COUNT; ++i)
   {
      vertices[ptr++].set(length / 2, -length / 2 + i * GRID_LINE_SPACING, 0.0f);
      vertices[ptr++].set(-length / 2, -length / 2 + i * GRID_LINE_SPACING, 0.0f);
   }

   osg::Geometry* geometry = new osg::Geometry;
   geometry->setVertexArray(new osg::Vec3Array(numVertices, vertices));
   geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, numVertices));

   osg::Geode* geode = new osg::Geode;
   geode->addDrawable(geometry);
   geode->getOrCreateStateSet()->setMode(GL_LIGHTING, 0);

   mpXYGridTransform = new osg::MatrixTransform;
   mpXYGridTransform->addChild(geode);

   mpXZGridTransform = new osg::MatrixTransform;
   mpXZGridTransform->setMatrix(osg::Matrix::rotate(osg::PI_2, 1, 0, 0));

   mpXZGridTransform->addChild(geode);
   mpXZGridTransform->setNodeMask(0x0);

   mpYZGridTransform = new osg::MatrixTransform;
   mpYZGridTransform->setMatrix(osg::Matrix::rotate(osg::PI_2, 0, 1, 0));

   mpYZGridTransform->addChild(geode);
   mpYZGridTransform->setNodeMask(0x0);
}

////////////////////////////////////////////////////////////////////////////////
