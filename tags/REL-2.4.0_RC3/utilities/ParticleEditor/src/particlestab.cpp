#include <particlestab.h>
#include <QtGui/QColorDialog>
#include <QtGui/QFileDialog>
#include <QtGui/QRgb>
#include <QtGui/QPalette>
#include <QtGui/QPixmap>

///////////////////////////////////////////////////////////////////////////////
ParticlesTab::ParticlesTab()
{
}

///////////////////////////////////////////////////////////////////////////////
ParticlesTab::~ParticlesTab()
{
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::SetupUI()
{
   SetupParticleConnections();
   SetupTextureConnections();
   SetupColorConnections();
   SetupEmitterConnections();

   ChangeCustomColorFromColor();
   ChangeCustomColorToColor();
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::LifeSpinBoxValueChanged(double newValue)
{
   mpLifeSlider->setValue(newValue * 1);
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::LifeSliderValueChanged(int newValue)
{
   mpLifeSpinBox->setValue(newValue / 1.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::RadiusSpinBoxValueChanged(double newValue)
{
   mpRadiusSlider->setValue(newValue * 100);
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::RadiusSliderValueChanged(int newValue)
{
   mpRadiusSpinBox->setValue(newValue / 100.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::MassSpinBoxValueChanged(double newValue)
{
   mpMassSlider->setValue(newValue * 100);
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::MassSliderValueChanged(int newValue)
{
   mpMassSpinBox->setValue(newValue / 100.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::SizeFromSpinBoxValueChanged(double newValue)
{
   mpSizeFromSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::SizeFromSliderValueChanged(int newValue)
{
   mpSizeFromSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::SizeToSpinBoxValueChanged(double newValue)
{
   mpSizeToSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::SizeToSliderValueChanged(int newValue)
{
   mpSizeToSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::TextureLineEditValueChanged(QString newText)
{
   mpTexturePreview->SetTexture(newText);

   emit TextureChanged(mpTextureLineEdit->text());
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::TextureUpdated(QString filename)
{
   mpTextureLineEdit->setText(filename);
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::TextureBrowserButtonPressed()
{
   QString filename = QFileDialog::getOpenFileName(NULL, tr("Choose Texture"),
      mpTextureLineEdit->text(), tr("Images (*.bmp *.dds *.gif *.jpg *.pic *.png *.rgb *.tga *.tiff)"));

   if(!filename.isEmpty())
   {
      mpTextureLineEdit->setText(filename);
   }
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::RFromSpinBoxValueChanged(double newValue)
{
   mpRFromSlider->setValue(newValue * 100);
   ChangeCustomColorFromColor();
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::RFromSliderValueChanged(int newValue)
{
   mpRFromSpinBox->setValue(newValue / 100.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::RToSpinBoxValueChanged(double newValue)
{
   mpRToSlider->setValue(newValue * 100);
   ChangeCustomColorToColor();
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::RToSliderValueChanged(int newValue)
{
   mpRToSpinBox->setValue(newValue / 100.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::GFromSpinBoxValueChanged(double newValue)
{
   mpGFromSlider->setValue(newValue * 100);
   ChangeCustomColorFromColor();
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::GFromSliderValueChanged(int newValue)
{
   mpGFromSpinBox->setValue(newValue / 100.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::GToSpinBoxValueChanged(double newValue)
{
   mpGToSlider->setValue(newValue * 100);
   ChangeCustomColorToColor();
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::GToSliderValueChanged(int newValue)
{
   mpGToSpinBox->setValue(newValue / 100.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::BFromSpinBoxValueChanged(double newValue)
{
   mpBFromSlider->setValue(newValue * 100);
   ChangeCustomColorFromColor();
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::BFromSliderValueChanged(int newValue)
{
   mpBFromSpinBox->setValue(newValue / 100.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::BToSpinBoxValueChanged(double newValue)
{
   mpBToSlider->setValue(newValue * 100);
   ChangeCustomColorToColor();
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::BToSliderValueChanged(int newValue)
{
   mpBToSpinBox->setValue(newValue / 100.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::AFromSpinBoxValueChanged(double newValue)
{
   mpAFromSlider->setValue(newValue * 100);
   ChangeCustomColorFromColor();
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::AFromSliderValueChanged(int newValue)
{
   mpAFromSpinBox->setValue(newValue / 100.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::AToSpinBoxValueChanged(double newValue)
{
   mpAToSlider->setValue(newValue * 100);
   ChangeCustomColorToColor();
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::AToSliderValueChanged(int newValue)
{
   mpAToSpinBox->setValue(newValue / 100.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::CustomColorFromButtonPushed()
{
   QColor initialColor(mpRFromSpinBox->value() * 255, mpGFromSpinBox->value() * 255,
      mpBFromSpinBox->value() * 255, mpAFromSpinBox->value() * 255);
   QRgb newColor = QColorDialog::getRgba(initialColor.rgba(), 0, mpCustomColorFromButton);
   mpRFromSpinBox->setValue(qRed(newColor) / 255.0);
   mpGFromSpinBox->setValue(qGreen(newColor) / 255.0);
   mpBFromSpinBox->setValue(qBlue(newColor) / 255.0);
   mpAFromSpinBox->setValue(qAlpha(newColor) / 255.0);
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::CustomColorToButtonPushed()
{
   QColor initialColor(mpRToSpinBox->value() * 255, mpGToSpinBox->value() * 255,
      mpBToSpinBox->value() * 255, mpAToSpinBox->value() * 255);
   QRgb newColor = QColorDialog::getRgba(initialColor.rgba(), 0, mpCustomColorToButton);
   mpRToSpinBox->setValue(qRed(newColor) / 255.0);
   mpGToSpinBox->setValue(qGreen(newColor) / 255.0);
   mpBToSpinBox->setValue(qBlue(newColor) / 255.0);
   mpAToSpinBox->setValue(qAlpha(newColor) / 255.0);
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::EmitterLifeSpinBoxValueChanged(double newValue)
{
   mpEmitterLifeSlider->setValue(newValue * 100);
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::EmitterLifeSliderValueChanged(int newValue)
{
   mpEmitterLifeSpinBox->setValue(newValue / 100.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::EmitterStartSpinBoxValueChanged(double newValue)
{
   mpEmitterStartSlider->setValue(newValue * 100);
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::EmitterStartSliderValueChanged(int newValue)
{
   mpEmitterStartSpinBox->setValue(newValue / 100.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::EmitterResetSpinBoxValueChanged(double newValue)
{
   mpEmitterResetSlider->setValue(newValue * 100);
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::EmitterResetSliderValueChanged(int newValue)
{
   mpEmitterResetSpinBox->setValue(newValue / 100.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::SetupParticleConnections()
{
   connect(mpLifeSpinBox, SIGNAL(valueChanged(double)), this, SLOT(LifeSpinBoxValueChanged(double)));
   connect(mpLifeSlider, SIGNAL(sliderMoved(int)), this, SLOT(LifeSliderValueChanged(int)));
   connect(mpRadiusSpinBox, SIGNAL(valueChanged(double)), this, SLOT(RadiusSpinBoxValueChanged(double)));
   connect(mpRadiusSlider, SIGNAL(sliderMoved(int)), this, SLOT(RadiusSliderValueChanged(int)));
   connect(mpMassSpinBox, SIGNAL(valueChanged(double)), this, SLOT(MassSpinBoxValueChanged(double)));
   connect(mpMassSlider, SIGNAL(sliderMoved(int)), this, SLOT(MassSliderValueChanged(int)));
   connect(mpSizeFromSpinBox, SIGNAL(valueChanged(double)), this, SLOT(SizeFromSpinBoxValueChanged(double)));
   connect(mpSizeFromSlider, SIGNAL(sliderMoved(int)), this, SLOT(SizeFromSliderValueChanged(int)));
   connect(mpSizeToSpinBox, SIGNAL(valueChanged(double)), this, SLOT(SizeToSpinBoxValueChanged(double)));
   connect(mpSizeToSlider, SIGNAL(sliderMoved(int)), this, SLOT(SizeToSliderValueChanged(int)));
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::SetupTextureConnections()
{
   connect(mpTextureLineEdit, SIGNAL(textChanged(QString)), this, SLOT(TextureLineEditValueChanged(QString)));
   connect(mpTextureButton, SIGNAL(clicked()), this, SLOT(TextureBrowserButtonPressed()));
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::SetupColorConnections()
{
   connect(mpRFromSpinBox, SIGNAL(valueChanged(double)), this, SLOT(RFromSpinBoxValueChanged(double)));
   connect(mpRFromSlider, SIGNAL(sliderMoved(int)), this, SLOT(RFromSliderValueChanged(int)));
   connect(mpRToSpinBox, SIGNAL(valueChanged(double)), this, SLOT(RToSpinBoxValueChanged(double)));
   connect(mpRToSlider, SIGNAL(sliderMoved(int)), this, SLOT(RToSliderValueChanged(int)));
   connect(mpGFromSpinBox, SIGNAL(valueChanged(double)), this, SLOT(GFromSpinBoxValueChanged(double)));
   connect(mpGFromSlider, SIGNAL(sliderMoved(int)), this, SLOT(GFromSliderValueChanged(int)));
   connect(mpGToSpinBox, SIGNAL(valueChanged(double)), this, SLOT(GToSpinBoxValueChanged(double)));
   connect(mpGToSlider, SIGNAL(sliderMoved(int)), this, SLOT(GToSliderValueChanged(int)));
   connect(mpBFromSpinBox, SIGNAL(valueChanged(double)), this, SLOT(BFromSpinBoxValueChanged(double)));
   connect(mpBFromSlider, SIGNAL(sliderMoved(int)), this, SLOT(BFromSliderValueChanged(int)));
   connect(mpBToSpinBox, SIGNAL(valueChanged(double)), this, SLOT(BToSpinBoxValueChanged(double)));
   connect(mpBToSlider, SIGNAL(sliderMoved(int)), this, SLOT(BToSliderValueChanged(int)));
   connect(mpAFromSpinBox, SIGNAL(valueChanged(double)), this, SLOT(AFromSpinBoxValueChanged(double)));
   connect(mpAFromSlider, SIGNAL(sliderMoved(int)), this, SLOT(AFromSliderValueChanged(int)));
   connect(mpAToSpinBox, SIGNAL(valueChanged(double)), this, SLOT(AToSpinBoxValueChanged(double)));
   connect(mpAToSlider, SIGNAL(sliderMoved(int)), this, SLOT(AToSliderValueChanged(int)));
   connect(mpCustomColorFromButton, SIGNAL(clicked()), this, SLOT(CustomColorFromButtonPushed()));
   connect(mpCustomColorToButton, SIGNAL(clicked()), this, SLOT(CustomColorToButtonPushed()));
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::SetupEmitterConnections()
{
   connect(mpEmitterLifeSpinBox, SIGNAL(valueChanged(double)), this, SLOT(EmitterLifeSpinBoxValueChanged(double)));
   connect(mpEmitterLifeSlider, SIGNAL(sliderMoved(int)), this, SLOT(EmitterLifeSliderValueChanged(int)));
   connect(mpEmitterStartSpinBox, SIGNAL(valueChanged(double)), this, SLOT(EmitterStartSpinBoxValueChanged(double)));
   connect(mpEmitterStartSlider, SIGNAL(sliderMoved(int)), this, SLOT(EmitterStartSliderValueChanged(int)));
   connect(mpEmitterResetSpinBox, SIGNAL(valueChanged(double)), this, SLOT(EmitterResetSpinBoxValueChanged(double)));
   connect(mpEmitterResetSlider, SIGNAL(sliderMoved(int)), this, SLOT(EmitterResetSliderValueChanged(int)));
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::ChangeCustomColorFromColor()
{
   QColor newColor(mpRFromSpinBox->value() * 255, mpGFromSpinBox->value() * 255,
      mpBFromSpinBox->value() * 255);

   QPalette palette;
   palette.setColor(mpCustomColorFromButton->backgroundRole(), newColor);
   mpCustomColorFromButton->setPalette(palette);
   mpCustomColorFromButton->setAutoFillBackground(true);
}

///////////////////////////////////////////////////////////////////////////////
void ParticlesTab::ChangeCustomColorToColor()
{
   QColor newColor(mpRToSpinBox->value() * 255, mpGToSpinBox->value() * 255,
      mpBToSpinBox->value() * 255);

   QPalette palette;
   palette.setColor(mpCustomColorToButton->backgroundRole(), newColor);
   mpCustomColorToButton->setPalette(palette);
   mpCustomColorToButton->setAutoFillBackground(true);
}

///////////////////////////////////////////////////////////////////////////////
