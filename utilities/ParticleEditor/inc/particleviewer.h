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
#include <QtCore/QString>

#include <dtABC/application.h>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class MatrixTransform;
}
/// @endcond

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
   void LoadFile(QString filename, bool import = false);

   virtual bool KeyPressed(const dtCore::Keyboard* keyboard, int kc);


signals:
   ///> Main Window signals
   void UpdateWindowTitle(const QString& title);
   void UpdateHistory(const QString& filename);
   void ReferenceObjectLoaded(const QString& filename);

   ///> Layer Browser signals
   void ClearLayerList();
   void AddLayerToLayerList(const QString& layerName);
   void SelectIndexOfLayersList(int newIndex);
   void LayerHiddenChanged(bool hidden);
   void LayerRenderBinChanged(int renderbin);

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
   void TextureUpdated(QString filename);
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
   // Segment Placer UI
   void SegmentPlacerVertexAXUpdated(double newValue);
   void SegmentPlacerVertexAYUpdated(double newValue);
   void SegmentPlacerVertexAZUpdated(double newValue);
   void SegmentPlacerVertexBXUpdated(double newValue);
   void SegmentPlacerVertexBYUpdated(double newValue);
   void SegmentPlacerVertexBZUpdated(double newValue);
   // Multi Segment Placer UI
   void ClearMultiSegmentPlacerVertexList();
   void AddVertexToMultiSegmentPlacerVertexList(double x, double y, double z);
   void SelectIndexOfMultiSegmentPlacerVertexList(int newIndex);
   void MultiSegmentPlacerXUpdated(double newValue);
   void MultiSegmentPlacerYUpdated(double newValue);
   void MultiSegmentPlacerZUpdated(double newValue);

   ///> Shooter Tab signals
   void ShooterTypeBoxUpdated(int newCounter);
   // Radial Shooter UI
   void RadialShooterElevationMinUpdated(double newValue);
   void RadialShooterElevationMaxUpdated(double newValue);
   void RadialShooterAzimuthMinUpdated(double newValue);
   void RadialShooterAzimuthMaxUpdated(double newValue);
   void RadialShooterInitialVelocityMinUpdated(double newValue);
   void RadialShooterInitialVelocityMaxUpdated(double newValue);
   void RadialShooterInitialMinRotationXUpdated(double newValue);
   void RadialShooterInitialMinRotationYUpdated(double newValue);
   void RadialShooterInitialMinRotationZUpdated(double newValue);
   void RadialShooterInitialMaxRotationXUpdated(double newValue);
   void RadialShooterInitialMaxRotationYUpdated(double newValue);
   void RadialShooterInitialMaxRotationZUpdated(double newValue);

   ///> Program Tab signals
   // Operators UI
   void ClearOperatorsList();
   void AddOperatorToOperatorsList(const QString &newOperator);
   void SelectIndexOfOperatorsList(int newIndex);
   // Force UI
   void OperatorsForceXUpdated(double newValue);
   void OperatorsForceYUpdated(double newValue);
   void OperatorsForceZUpdated(double newValue);
   // Acceleration UI
   void OperatorsAccelerationXUpdated(double newValue);
   void OperatorsAccelerationYUpdated(double newValue);
   void OperatorsAccelerationZUpdated(double newValue);
   // Force UI
   void OperatorsFluidFrictionDensityUpdated(double newValue);
   void OperatorsFluidFrictionViscosityUpdated(double newValue);
   void OperatorsFluidFrictionOverrideRadiusUpdated(double newValue);

public slots:
   // Menu Actions
   void CreateNewParticleSystem();
   void OpenParticleSystem();
   void OpenRecentParticleSystem();
   void ImportParticleSystem();
   void LoadReferenceObject();
   void ToggleReferenceObject(bool enabled);
   void SaveParticleToFile();
   void SaveParticleAs();
   void ToggleCompass(bool enabled);
   void ToggleXYGrid(bool enabled);
   void ToggleXZGrid(bool enabled);
   void ToggleYZGrid(bool enabled);

   // Layer Browser slots
   void CreateNewParticleLayer();
   void DeleteSelectedLayer();
   void ToggleSelectedLayerHidden();
   void ResetEmitters();
   void UpdateSelectionIndex(int newIndex);
   void RenameParticleLayer(const QString& name);
   void SetParticleLayerRenderBin(int value);

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
   void TextureChanged(QString filename);
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
   // Segment Placer UI
   void SegmentPlacerVertexAXValueChanged(double newValue);
   void SegmentPlacerVertexAYValueChanged(double newValue);
   void SegmentPlacerVertexAZValueChanged(double newValue);
   void SegmentPlacerVertexBXValueChanged(double newValue);
   void SegmentPlacerVertexBYValueChanged(double newValue);
   void SegmentPlacerVertexBZValueChanged(double newValue);
   // Multi Segment Placer UI
   void UpdateMultiSegmentPlacerSelectionIndex(int newIndex);
   void MultiSegmentPlacerAddVertex();
   void MultiSegmentPlacerDeleteVertex();
   void MultiSegmentPlacerXValueChanged(double newValue);
   void MultiSegmentPlacerYValueChanged(double newValue);
   void MultiSegmentPlacerZValueChanged(double newValue);

   ///> Shooter Tab signals
   void ShooterTypeBoxValueChanged(int newShooter);
   // Radial Shooter UI
   void RadialShooterElevationMinValueChanged(double newValue);
   void RadialShooterElevationMaxValueChanged(double newValue);
   void RadialShooterAzimuthMinValueChanged(double newValue);
   void RadialShooterAzimuthMaxValueChanged(double newValue);
   void RadialShooterInitialVelocityMinValueChanged(double newValue);
   void RadialShooterInitialVelocityMaxValueChanged(double newValue);
   void RadialShooterInitialMinRotationXValueChanged(double newValue);
   void RadialShooterInitialMinRotationYValueChanged(double newValue);
   void RadialShooterInitialMinRotationZValueChanged(double newValue);
   void RadialShooterInitialMaxRotationXValueChanged(double newValue);
   void RadialShooterInitialMaxRotationYValueChanged(double newValue);
   void RadialShooterInitialMaxRotationZValueChanged(double newValue);

   ///> Placer Tab signals
   // Operators UI
   void UpdateOperatorsSelectionIndex(int newIndex, const QString &operatorType);
   void OperatorsAddNewForce();
   void OperatorsAddNewAcceleration();
   void OperatorsAddNewFluidFriction();
   void OperatorsDeleteCurrentOperator();
   // Force UI
   void OperatorsForceXValueChanged(double newValue);
   void OperatorsForceYValueChanged(double newValue);
   void OperatorsForceZValueChanged(double newValue);
   // Acceleration UI
   void OperatorsAccelerationXValueChanged(double newValue);
   void OperatorsAccelerationYValueChanged(double newValue);
   void OperatorsAccelerationZValueChanged(double newValue);
   // Force UI
   void FluidFrictionAirButtonPressed();
   void FluidFrictionWaterButtonPressed();
   void OperatorsFluidFrictionDensityValueChanged(double newValue);
   void OperatorsFluidFrictionViscosityValueChanged(double newValue);
   void OperatorsFluidFrictionOverrideRadiusValueChanged(double newValue);

private:
   void MakeCompass();
   void MakeGrids();
   void SetParticleSystemFilename(QString filename);
   void UpdateLayersList();
   void UpdateParticleTabsValues();
   void UpdateCounterTabsValues();
   void UpdateRandomRatesValues();
   void UpdatePlacerTabsValues();
   void UpdatePointPlacerValues();
   void UpdateSectorPlacerValues();
   void UpdateSegmentPlacerValues();
   void UpdateMultiSegmentPlacerVertexList();
   void UpdateMultiSegmentPlacerValues();
   void UpdateShooterTabsValues();
   void UpdateRadialShooterValues();
   void UpdateProgramTabsValues();
   void UpdateOperatorsList();
   void UpdateForceValues();
   void UpdateAccelerationValues();
   void UpdateFluidFrictionValues();

   void SetTexturePaths(QString path, bool relativePath);

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
   QString mParticleSystemFilename;

   unsigned int mLayerIndex;
   int mMultiSegmentVertexIndex;
   int mOperatorsIndex;
};

#endif // __PARTICLE_VIEWER_H__

