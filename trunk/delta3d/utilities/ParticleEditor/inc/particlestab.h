#ifndef DELTA_PARTICLES_TAB
#define DELTA_PARTICLES_TAB

#include <texturepreview.h>
#include <QtGui/QComboBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QSlider>
#include <QtGui/QLineEdit>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QCheckBox>

class ParticlesTab : public QObject
{
   Q_OBJECT

public:
   ParticlesTab();
   ~ParticlesTab();

   // Particle UI
   void SetAlignmentBox(QComboBox* alignmentBox) { mpAlignmentBox = alignmentBox; }
   void SetShapeBox(QComboBox* shapeBox) { mpShapeBox = shapeBox; }
   void SetEmissiveBox(QCheckBox* emissiveBox) { mpEmissiveBox = emissiveBox; }
   void SetLightingBox(QCheckBox* lightingBox) { mpLightingBox = lightingBox; }
   void SetLifeSpinBox(QDoubleSpinBox* lifeSpinBox) { mpLifeSpinBox = lifeSpinBox; }
   void SetLifeSlider(QSlider* lifeSlider) { mpLifeSlider = lifeSlider; }
   void SetRadiusSpinBox(QDoubleSpinBox* radiusSpinBox) { mpRadiusSpinBox = radiusSpinBox; }
   void SetRadiusSlider(QSlider* radiusSlider) { mpRadiusSlider = radiusSlider; }
   void SetMassSpinBox(QDoubleSpinBox* massSpinBox) { mpMassSpinBox = massSpinBox; }
   void SetMassSlider(QSlider* massSlider) { mpMassSlider = massSlider; }
   void SetSizeFromSpinBox(QDoubleSpinBox* sizeFromSpinBox) { mpSizeFromSpinBox = sizeFromSpinBox; }
   void SetSizeFromSlider(QSlider* sizeFromSlider) { mpSizeFromSlider = sizeFromSlider; }
   void SetSizeToSpinBox(QDoubleSpinBox* sizeToSpinBox) { mpSizeToSpinBox = sizeToSpinBox; }
   void SetSizeToSlider(QSlider* sizeToSlider) { mpSizeToSlider = sizeToSlider; }

   // Texture UI
   void SetTextureLineEdit(QLineEdit* TextureLineEdit) { mpTextureLineEdit = TextureLineEdit; }
   void SetTextureButton(QPushButton* textureButton) { mpTextureButton = textureButton; }
   void SetTexturePreview(TexturePreview* texturePreview) { mpTexturePreview = texturePreview; }

   // Color UI
   void SetRFromSpinBox(QDoubleSpinBox* rFromSpinBox) { mpRFromSpinBox = rFromSpinBox; }
   void SetRFromSlider(QSlider* rFromSlider) { mpRFromSlider = rFromSlider; }
   void SetRToSpinBox(QDoubleSpinBox* rToSpinBox) { mpRToSpinBox = rToSpinBox; }
   void SetRToSlider(QSlider* rToSlider) { mpRToSlider = rToSlider; }
   void SetGFromSpinBox(QDoubleSpinBox* gFromSpinBox) { mpGFromSpinBox = gFromSpinBox; }
   void SetGFromSlider(QSlider* gFromSlider) { mpGFromSlider = gFromSlider; }
   void SetGToSpinBox(QDoubleSpinBox* gToSpinBox) { mpGToSpinBox = gToSpinBox; }
   void SetGToSlider(QSlider* gToSlider) { mpGToSlider = gToSlider; }
   void SetBFromSpinBox(QDoubleSpinBox* bFromSpinBox) { mpBFromSpinBox = bFromSpinBox; }
   void SetBFromSlider(QSlider* bFromSlider) { mpBFromSlider = bFromSlider; }
   void SetBToSpinBox(QDoubleSpinBox* bToSpinBox) { mpBToSpinBox = bToSpinBox; }
   void SetBToSlider(QSlider* bToSlider) { mpBToSlider = bToSlider; }
   void SetAFromSpinBox(QDoubleSpinBox* aFromSpinBox) { mpAFromSpinBox = aFromSpinBox; }
   void SetAFromSlider(QSlider* aFromSlider) { mpAFromSlider = aFromSlider; }
   void SetAToSpinBox(QDoubleSpinBox* aToSpinBox) { mpAToSpinBox = aToSpinBox; }
   void SetAToSlider(QSlider* aToSlider) { mpAToSlider = aToSlider; }
   void SetCustomColorFromButton(QPushButton* customColorFromButton) { mpCustomColorFromButton = customColorFromButton; }
   void SetCustomColorToButton(QPushButton* customColorToButton) { mpCustomColorToButton = customColorToButton; }

   // Emitter UI
   void SetEmitterLifeSpinBox(QDoubleSpinBox* emitterLifeSpinBox) { mpEmitterLifeSpinBox = emitterLifeSpinBox; }
   void SetEmitterLifeSlider(QSlider* emitterLifeSlider) { mpEmitterLifeSlider = emitterLifeSlider; }
   void SetEmitterStartSpinBox(QDoubleSpinBox* emitterStartSpinBox) { mpEmitterStartSpinBox = emitterStartSpinBox; }
   void SetEmitterStartSlider(QSlider* emitterStartSlider) { mpEmitterStartSlider = emitterStartSlider; }
   void SetEmitterResetSpinBox(QDoubleSpinBox* emitterResetSpinBox) { mpEmitterResetSpinBox = emitterResetSpinBox; }
   void SetEmitterResetSlider(QSlider* emitterResetSlider) { mpEmitterResetSlider = emitterResetSlider; }
   void SetForeverBox(QCheckBox* foreverBox) { mpForeverBox = foreverBox; }

   void SetupUI();

signals:
   void TextureChanged(QString filename);

public slots:
   // Particle UI
   void LifeSpinBoxValueChanged(double newValue);
   void LifeSliderValueChanged(int newValue);
   void RadiusSpinBoxValueChanged(double newValue);
   void RadiusSliderValueChanged(int newValue);
   void MassSpinBoxValueChanged(double newValue);
   void MassSliderValueChanged(int newValue);
   void SizeFromSpinBoxValueChanged(double newValue);
   void SizeFromSliderValueChanged(int newValue);
   void SizeToSpinBoxValueChanged(double newValue);
   void SizeToSliderValueChanged(int newValue);

   // Texture UI
   void TextureLineEditValueChanged(QString newText);
   void TextureBrowserButtonPressed();
   void TextureUpdated(QString filename);

   // Color UI
   void RFromSpinBoxValueChanged(double newValue);
   void RFromSliderValueChanged(int newValue);
   void RToSpinBoxValueChanged(double newValue);
   void RToSliderValueChanged(int newValue);
   void GFromSpinBoxValueChanged(double newValue);
   void GFromSliderValueChanged(int newValue);
   void GToSpinBoxValueChanged(double newValue);
   void GToSliderValueChanged(int newValue);
   void BFromSpinBoxValueChanged(double newValue);
   void BFromSliderValueChanged(int newValue);
   void BToSpinBoxValueChanged(double newValue);
   void BToSliderValueChanged(int newValue);
   void AFromSpinBoxValueChanged(double newValue);
   void AFromSliderValueChanged(int newValue);
   void AToSpinBoxValueChanged(double newValue);
   void AToSliderValueChanged(int newValue);
   void CustomColorFromButtonPushed();
   void CustomColorToButtonPushed();

   // Emitter UI
   void EmitterLifeSpinBoxValueChanged(double newValue);
   void EmitterLifeSliderValueChanged(int newValue);
   void EmitterStartSpinBoxValueChanged(double newValue);
   void EmitterStartSliderValueChanged(int newValue);
   void EmitterResetSpinBoxValueChanged(double newValue);
   void EmitterResetSliderValueChanged(int newValue);

private:
   void SetupParticleConnections();
   void SetupTextureConnections();
   void SetupColorConnections();
   void SetupEmitterConnections();

   void ChangeCustomColorFromColor();
   void ChangeCustomColorToColor();

   // Particle UI
   QComboBox* mpAlignmentBox;
   QComboBox* mpShapeBox;
   QCheckBox* mpEmissiveBox;
   QCheckBox* mpLightingBox;
   QDoubleSpinBox* mpLifeSpinBox;
   QSlider* mpLifeSlider;
   QDoubleSpinBox* mpRadiusSpinBox;
   QSlider* mpRadiusSlider;
   QDoubleSpinBox* mpMassSpinBox;
   QSlider* mpMassSlider;
   QDoubleSpinBox* mpSizeFromSpinBox;
   QSlider* mpSizeFromSlider;
   QDoubleSpinBox* mpSizeToSpinBox;
   QSlider* mpSizeToSlider;

   // Texture UI
   QLineEdit* mpTextureLineEdit;
   QPushButton* mpTextureButton;
   TexturePreview* mpTexturePreview;

   // Color UI
   QDoubleSpinBox* mpRFromSpinBox;
   QSlider* mpRFromSlider;
   QDoubleSpinBox* mpRToSpinBox;
   QSlider* mpRToSlider;
   QDoubleSpinBox* mpGFromSpinBox;
   QSlider* mpGFromSlider;
   QDoubleSpinBox* mpGToSpinBox;
   QSlider* mpGToSlider;
   QDoubleSpinBox* mpBFromSpinBox;
   QSlider* mpBFromSlider;
   QDoubleSpinBox* mpBToSpinBox;
   QSlider* mpBToSlider;
   QDoubleSpinBox* mpAFromSpinBox;
   QSlider* mpAFromSlider;
   QDoubleSpinBox* mpAToSpinBox;
   QSlider* mpAToSlider;
   QPushButton* mpCustomColorFromButton;
   QPushButton* mpCustomColorToButton;

   // Emitter UI
   QDoubleSpinBox* mpEmitterLifeSpinBox;
   QSlider* mpEmitterLifeSlider;
   QDoubleSpinBox* mpEmitterStartSpinBox;
   QSlider* mpEmitterStartSlider;
   QDoubleSpinBox* mpEmitterResetSpinBox;
   QSlider* mpEmitterResetSlider;
   QCheckBox* mpForeverBox;
};

#endif /*DELTA_PARTICLES_TAB*/
