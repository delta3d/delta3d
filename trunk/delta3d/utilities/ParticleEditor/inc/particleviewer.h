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
   ///> Layer Browser signals
   void LayerHiddenChanged(bool hidden);

   ///> Particles Tab signals
   // Particle UI
   void AlignmentUpdated(int newAlignment);
   void ShapeUpdated(int newShape);
   void EmissiveUpdated(bool enabled);
   void LightingUpdated(bool enabled);
   void LifeUpdated(double newValue);
   void RadiusUpdated(double newValue);
   void MassUpdated(double newValue);
   void SizeFromUpdated(double newValue);
   void SizeToUpdated(double newValue);

   // Texture UI
   void TextureUpdated(QString filename, bool emissive, bool lighting);

   // Color UI
   void RFromUpdated(double newValue);
   void RToUpdated(double newValue);
   void GFromUpdated(double newValue);
   void GToUpdated(double newValue);
   void BFromUpdated(double newValue);
   void BToUpdated(double newValue);
   void AFromUpdated(double newValue);
   void AToUpdated(double newValue);

   // Emitter UI
   void EmitterLifeUpdated(double newValue);
   void EmitterStartUpdated(double newValue);
   void EmitterResetUpdated(double newValue);
   void EndlessLifetimeUpdated(bool endless);

   ///> Counter Tab signals
   void CounterTypeBoxUpdated(int newCounter);
   void RandomRateMinRateUpdated(double newValue);
   void RandomRateMaxRateUpdated(double newValue);

   ///> Placer Tab signals
   void PlacerTypeBoxUpdated(int newCounter);

   // Point Placer UI
   void PointPlacerXUpdated(double newValue);
   void PointPlacerYUpdated(double newValue);
   void PointPlacerZUpdated(double newValue);

   // Sector Placer UI
   void SectorPlacerXUpdated(double newValue);
   void SectorPlacerYUpdated(double newValue);
   void SectorPlacerZUpdated(double newValue);
   void SectorPlacerMinRadiusUpdated(double newValue);
   void SectorPlacerMaxRadiusUpdated(double newValue);
   void SectorPlacerMinPhiUpdated(double newValue);
   void SectorPlacerMaxPhiUpdated(double newValue);

   // Point Placer UI
   void SegmentPlacerVertexAXUpdated(double newValue);
   void SegmentPlacerVertexAYUpdated(double newValue);
   void SegmentPlacerVertexAZUpdated(double newValue);
   void SegmentPlacerVertexBXUpdated(double newValue);
   void SegmentPlacerVertexBYUpdated(double newValue);
   void SegmentPlacerVertexBZUpdated(double newValue);

   public slots:
   void CreateNewParticleSystem();

   // Layer Browser slots
   void CreateNewParticleLayer();
   void DeleteSelectedLayer();
   void ToggleSelectedLayerHidden();
   void ResetEmitters();
   void UpdateSelectionIndex(int newIndex);

   ///> Particles Tab slots
   // Particle UI
   void AlignmentChanged(int newAlignment);
   void ShapeChanged(int newShape);
   void ToggleEmissive(bool enabled);
   void ToggleLighting(bool enabled);
   void LifeValueChanged(double newValue);
   void RadiusValueChanged(double newValue);
   void MassValueChanged(double newValue);
   void SizeFromValueChanged(double newValue);
   void SizeToValueChanged(double newValue);

   // Texture UI
   void TextureChanged(QString filename, bool emissive, bool lighting);

   // Color UI
   void RFromValueChanged(double newValue);
   void RToValueChanged(double newValue);
   void GFromValueChanged(double newValue);
   void GToValueChanged(double newValue);
   void BFromValueChanged(double newValue);
   void BToValueChanged(double newValue);
   void AFromValueChanged(double newValue);
   void AToValueChanged(double newValue);

   // Emitter UI
   void EmitterLifeValueChanged(double newValue);
   void EmitterStartValueChanged(double newValue);
   void EmitterResetValueChanged(double newValue);
   void EndlessLifetimeChanged(bool endless);

   ///> Counter Tab slots
   void CounterTypeBoxValueChanged(int newCounter);
   void RandomRateMinRateValueChanged(double newValue);
   void RandomRateMaxRateValueChanged(double newValue);

   ///> Placer Tab signals
   void PlacerTypeBoxValueChanged(int newCounter);

   // Point Placer UI
   void PointPlacerXValueChanged(double newValue);
   void PointPlacerYValueChanged(double newValue);
   void PointPlacerZValueChanged(double newValue);

   // Sector Placer UI
   void SectorPlacerXValueChanged(double newValue);
   void SectorPlacerYValueChanged(double newValue);
   void SectorPlacerZValueChanged(double newValue);
   void SectorPlacerMinRadiusValueChanged(double newValue);
   void SectorPlacerMaxRadiusValueChanged(double newValue);
   void SectorPlacerMinPhiValueChanged(double newValue);
   void SectorPlacerMaxPhiValueChanged(double newValue);

   // Placer Placer UI
   void SegmentPlacerVertexAXValueChanged(double newValue);
   void SegmentPlacerVertexAYValueChanged(double newValue);
   void SegmentPlacerVertexAZValueChanged(double newValue);
   void SegmentPlacerVertexBXValueChanged(double newValue);
   void SegmentPlacerVertexBYValueChanged(double newValue);
   void SegmentPlacerVertexBZValueChanged(double newValue);

private:
   void MakeCompass();
   void MakeGrids();
   void UpdateParticleTabsValues();
   void UpdateCounterTabsValues();
   void UpdateRandomRatesValues();
   void UpdatePlacerTabsValues();
   void UpdatePointPlacerValues();
   void UpdateSectorPlacerValues();
   void UpdateSegmentPlacerValues();
   void UpdateMultiSegmentPlacerValues();

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