#ifndef __PARTICLE_VIEWER_H__
#define __PARTICLE_VIEWER_H__

#include <orbitmotionmodel.h>

#include <osgParticle/ModularEmitter>
#include <osgParticle/ModularProgram>
#include <osgParticle/Particle>
#include <osgParticle/ParticleSystem>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/Program>

#include <QtCore/QObject>

#include <dtABC/application.h>

namespace osg
{
   class MatrixTransform;
}

////////////////////////////////////////////////////////////////////////////////

/**
* A single particle system in the particle system group.
*/
struct ParticleSystemLayer
{
   /**
   * The geode that holds the drawable particle system, and whose name is
   * the name of the layer.
   */
   dtCore::RefPtr<osg::Geode> mGeode;

   /**
   * The active particle system.
   */
   dtCore::RefPtr<osgParticle::ParticleSystem> mParticleSystem;

   /**
   * The active particle template.
   */
   osgParticle::Particle* mpParticle;

   /**
   * The transform that controls the position of the emitter.
   */
   dtCore::RefPtr<osg::MatrixTransform> mEmitterTransform;

   /**
   * The active emitter.
   */
   dtCore::RefPtr<osgParticle::ModularEmitter> mModularEmitter;

   /**
   * The active program.
   */
   dtCore::RefPtr<osgParticle::ModularProgram> mModularProgram;
};

////////////////////////////////////////////////////////////////////////////////
/**
 * The number of lines in each direction.
 */
const int GRID_LINE_COUNT = 49;

/**
 * The amount of space between each line.
 */
const float GRID_LINE_SPACING = 1.0f;

////////////////////////////////////////////////////////////////////////////////
class ParticleViewer : public QObject, public dtABC::Application
{
   Q_OBJECT

public:
   ParticleViewer();
   ~ParticleViewer();

   virtual void Config();

signals:
   void LayerHiddenChanged(bool hidden);

public slots:
   void CreateNewParticleSystem();
   void CreateNewParticleLayer();
   void DeleteSelectedLayer();
   void ToggleSelectedLayerHidden();
   void ResetEmitters();
   void UpdateSelectionIndex(int newIndex);

private:
   void MakeCompass();
   void MakeGrids();

   dtCore::RefPtr<OrbitMotionModel> mMotion;

   osg::Group* mpSceneGroup;
   osg::Group* mpParticleSystemGroup;
   osg::ref_ptr<osg::Node> mpReferenceModel;
   osg::MatrixTransform* mpCompassTransform;
   osg::MatrixTransform* mpXYGridTransform;
   osg::MatrixTransform* mpXZGridTransform;
   osg::MatrixTransform* mpYZGridTransform;

   std::vector<ParticleSystemLayer> mLayers;
   osgParticle::ParticleSystemUpdater* mpParticleSystemUpdater;
   std::string mParticleSystemFilename;

   int mLayerIndex;
};

#endif // __PARTICLE_VIEWER_H__