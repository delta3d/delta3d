#include <shootertab.h>

///////////////////////////////////////////////////////////////////////////////
ShooterTab::ShooterTab()
{
}

///////////////////////////////////////////////////////////////////////////////
ShooterTab::~ShooterTab()
{
}

///////////////////////////////////////////////////////////////////////////////
void ShooterTab::SetupUI()
{
   SetupRadialShooterConnections();
}

///////////////////////////////////////////////////////////////////////////////
void ShooterTab::RadialShooterElevationMinSpinBoxValueChanged(double newValue)
{
   mpRadialShooterElevationMinSlider->setValue(newValue * 100);
}

///////////////////////////////////////////////////////////////////////////////
void ShooterTab::RadialShooterElevationMinSliderValueChanged(int newValue)
{
   mpRadialShooterElevationMinSpinBox->setValue(newValue / 100.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ShooterTab::RadialShooterElevationMaxSpinBoxValueChanged(double newValue)
{
   mpRadialShooterElevationMaxSlider->setValue(newValue * 100);
}

///////////////////////////////////////////////////////////////////////////////
void ShooterTab::RadialShooterElevationMaxSliderValueChanged(int newValue)
{
   mpRadialShooterElevationMaxSpinBox->setValue(newValue / 100.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ShooterTab::RadialShooterAzimuthMinSpinBoxValueChanged(double newValue)
{
   mpRadialShooterAzimuthMinSlider->setValue(newValue * 100);
}

///////////////////////////////////////////////////////////////////////////////
void ShooterTab::RadialShooterAzimuthMinSliderValueChanged(int newValue)
{
   mpRadialShooterAzimuthMinSpinBox->setValue(newValue / 100.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ShooterTab::RadialShooterAzimuthMaxSpinBoxValueChanged(double newValue)
{
   mpRadialShooterAzimuthMaxSlider->setValue(newValue * 100);
}

///////////////////////////////////////////////////////////////////////////////
void ShooterTab::RadialShooterAzimuthMaxSliderValueChanged(int newValue)
{
   mpRadialShooterAzimuthMaxSpinBox->setValue(newValue / 100.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ShooterTab::RadialShooterInitialVelocityMinSpinBoxValueChanged(double newValue)
{
   mpRadialShooterInitialVelocityMinSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void ShooterTab::RadialShooterInitialVelocityMinSliderValueChanged(int newValue)
{
   mpRadialShooterInitialVelocityMinSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ShooterTab::RadialShooterInitialVelocityMaxSpinBoxValueChanged(double newValue)
{
   mpRadialShooterInitialVelocityMaxSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void ShooterTab::RadialShooterInitialVelocityMaxSliderValueChanged(int newValue)
{
   mpRadialShooterInitialVelocityMaxSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ShooterTab::RadialShooterInitialMinRotationXSpinBoxValueChanged(double newValue)
{
   mpRadialShooterInitialMinRotationXSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void ShooterTab::RadialShooterInitialMinRotationXSliderValueChanged(int newValue)
{
   mpRadialShooterInitialMinRotationXSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ShooterTab::RadialShooterInitialMinRotationYSpinBoxValueChanged(double newValue)
{
   mpRadialShooterInitialMinRotationYSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void ShooterTab::RadialShooterInitialMinRotationYSliderValueChanged(int newValue)
{
   mpRadialShooterInitialMinRotationYSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ShooterTab::RadialShooterInitialMinRotationZSpinBoxValueChanged(double newValue)
{
   mpRadialShooterInitialMinRotationZSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void ShooterTab::RadialShooterInitialMinRotationZSliderValueChanged(int newValue)
{
   mpRadialShooterInitialMinRotationZSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ShooterTab::RadialShooterInitialMaxRotationXSpinBoxValueChanged(double newValue)
{
   mpRadialShooterInitialMaxRotationXSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void ShooterTab::RadialShooterInitialMaxRotationXSliderValueChanged(int newValue)
{
   mpRadialShooterInitialMaxRotationXSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ShooterTab::RadialShooterInitialMaxRotationYSpinBoxValueChanged(double newValue)
{
   mpRadialShooterInitialMaxRotationYSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void ShooterTab::RadialShooterInitialMaxRotationYSliderValueChanged(int newValue)
{
   mpRadialShooterInitialMaxRotationYSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ShooterTab::RadialShooterInitialMaxRotationZSpinBoxValueChanged(double newValue)
{
   mpRadialShooterInitialMaxRotationZSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void ShooterTab::RadialShooterInitialMaxRotationZSliderValueChanged(int newValue)
{
   mpRadialShooterInitialMaxRotationZSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ShooterTab::SetupRadialShooterConnections()
{
   connect(mpRadialShooterElevationMinSpinBox, SIGNAL(valueChanged(double)), this, SLOT(RadialShooterElevationMinSpinBoxValueChanged(double)));
   connect(mpRadialShooterElevationMinSlider, SIGNAL(sliderMoved(int)), this, SLOT(RadialShooterElevationMinSliderValueChanged(int)));
   connect(mpRadialShooterElevationMaxSpinBox, SIGNAL(valueChanged(double)), this, SLOT(RadialShooterElevationMaxSpinBoxValueChanged(double)));
   connect(mpRadialShooterElevationMaxSlider, SIGNAL(sliderMoved(int)), this, SLOT(RadialShooterElevationMaxSliderValueChanged(int)));
   connect(mpRadialShooterAzimuthMinSpinBox, SIGNAL(valueChanged(double)), this, SLOT(RadialShooterAzimuthMinSpinBoxValueChanged(double)));
   connect(mpRadialShooterAzimuthMinSlider, SIGNAL(sliderMoved(int)), this, SLOT(RadialShooterAzimuthMinSliderValueChanged(int)));
   connect(mpRadialShooterAzimuthMaxSpinBox, SIGNAL(valueChanged(double)), this, SLOT(RadialShooterAzimuthMaxSpinBoxValueChanged(double)));
   connect(mpRadialShooterAzimuthMaxSlider, SIGNAL(sliderMoved(int)), this, SLOT(RadialShooterAzimuthMaxSliderValueChanged(int)));
   connect(mpRadialShooterInitialVelocityMinSpinBox, SIGNAL(valueChanged(double)), this, SLOT(RadialShooterInitialVelocityMinSpinBoxValueChanged(double)));
   connect(mpRadialShooterInitialVelocityMinSlider, SIGNAL(sliderMoved(int)), this, SLOT(RadialShooterInitialVelocityMinSliderValueChanged(int)));
   connect(mpRadialShooterInitialVelocityMaxSpinBox, SIGNAL(valueChanged(double)), this, SLOT(RadialShooterInitialVelocityMaxSpinBoxValueChanged(double)));
   connect(mpRadialShooterInitialVelocityMaxSlider, SIGNAL(sliderMoved(int)), this, SLOT(RadialShooterInitialVelocityMaxSliderValueChanged(int)));
   connect(mpRadialShooterInitialMinRotationXSpinBox, SIGNAL(valueChanged(double)), this, SLOT(RadialShooterInitialMinRotationXSpinBoxValueChanged(double)));
   connect(mpRadialShooterInitialMinRotationXSlider, SIGNAL(sliderMoved(int)), this, SLOT(RadialShooterInitialMinRotationXSliderValueChanged(int)));
   connect(mpRadialShooterInitialMinRotationYSpinBox, SIGNAL(valueChanged(double)), this, SLOT(RadialShooterInitialMinRotationYSpinBoxValueChanged(double)));
   connect(mpRadialShooterInitialMinRotationYSlider, SIGNAL(sliderMoved(int)), this, SLOT(RadialShooterInitialMinRotationYSliderValueChanged(int)));
   connect(mpRadialShooterInitialMinRotationZSpinBox, SIGNAL(valueChanged(double)), this, SLOT(RadialShooterInitialMinRotationZSpinBoxValueChanged(double)));
   connect(mpRadialShooterInitialMinRotationZSlider, SIGNAL(sliderMoved(int)), this, SLOT(RadialShooterInitialMinRotationZSliderValueChanged(int)));
   connect(mpRadialShooterInitialMaxRotationXSpinBox, SIGNAL(valueChanged(double)), this, SLOT(RadialShooterInitialMaxRotationXSpinBoxValueChanged(double)));
   connect(mpRadialShooterInitialMaxRotationXSlider, SIGNAL(sliderMoved(int)), this, SLOT(RadialShooterInitialMaxRotationXSliderValueChanged(int)));
   connect(mpRadialShooterInitialMaxRotationYSpinBox, SIGNAL(valueChanged(double)), this, SLOT(RadialShooterInitialMaxRotationYSpinBoxValueChanged(double)));
   connect(mpRadialShooterInitialMaxRotationYSlider, SIGNAL(sliderMoved(int)), this, SLOT(RadialShooterInitialMaxRotationYSliderValueChanged(int)));
   connect(mpRadialShooterInitialMaxRotationZSpinBox, SIGNAL(valueChanged(double)), this, SLOT(RadialShooterInitialMaxRotationZSpinBoxValueChanged(double)));
   connect(mpRadialShooterInitialMaxRotationZSlider, SIGNAL(sliderMoved(int)), this, SLOT(RadialShooterInitialMaxRotationZSliderValueChanged(int)));
}

///////////////////////////////////////////////////////////////////////////////
