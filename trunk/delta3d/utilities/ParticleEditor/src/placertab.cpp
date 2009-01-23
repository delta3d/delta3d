#include <placertab.h>

///////////////////////////////////////////////////////////////////////////////
PlacerTab::PlacerTab()
{
}

///////////////////////////////////////////////////////////////////////////////
PlacerTab::~PlacerTab()
{
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::SetupUI()
{
   SetupPointPlacerConnections();
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::PointPlacerXSpinBoxValueChanged(double newValue)
{
   mpPointPlacerXSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::PointPlacerXSliderValueChanged(int newValue)
{
   mpPointPlacerXSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::PointPlacerYSpinBoxValueChanged(double newValue)
{
   mpPointPlacerYSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::PointPlacerYSliderValueChanged(int newValue)
{
   mpPointPlacerYSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::PointPlacerZSpinBoxValueChanged(double newValue)
{
   mpPointPlacerZSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::PointPlacerZSliderValueChanged(int newValue)
{
   mpPointPlacerZSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::SetupPointPlacerConnections()
{
   connect(mpPointPlacerXSpinBox, SIGNAL(valueChanged(double)), this, SLOT(PointPlacerXSpinBoxValueChanged(double)));
   connect(mpPointPlacerXSlider, SIGNAL(sliderMoved(int)), this, SLOT(PointPlacerXSliderValueChanged(int)));
   connect(mpPointPlacerYSpinBox, SIGNAL(valueChanged(double)), this, SLOT(PointPlacerYSpinBoxValueChanged(double)));
   connect(mpPointPlacerYSlider, SIGNAL(sliderMoved(int)), this, SLOT(PointPlacerYSliderValueChanged(int)));
   connect(mpPointPlacerZSpinBox, SIGNAL(valueChanged(double)), this, SLOT(PointPlacerZSpinBoxValueChanged(double)));
   connect(mpPointPlacerZSlider, SIGNAL(sliderMoved(int)), this, SLOT(PointPlacerZSliderValueChanged(int)));
}

///////////////////////////////////////////////////////////////////////////////
