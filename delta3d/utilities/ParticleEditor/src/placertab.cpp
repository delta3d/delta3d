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
   SetupSectorPlacerConnections();
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
void PlacerTab::SectorPlacerXSpinBoxValueChanged(double newValue)
{
   mpSectorPlacerXSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::SectorPlacerXSliderValueChanged(int newValue)
{
   mpSectorPlacerXSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::SectorPlacerYSpinBoxValueChanged(double newValue)
{
   mpSectorPlacerYSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::SectorPlacerYSliderValueChanged(int newValue)
{
   mpSectorPlacerYSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::SectorPlacerZSpinBoxValueChanged(double newValue)
{
   mpSectorPlacerZSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::SectorPlacerZSliderValueChanged(int newValue)
{
   mpSectorPlacerZSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::SectorPlacerMinRadiusSpinBoxValueChanged(double newValue)
{
   mpSectorPlacerMinRadiusSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::SectorPlacerMinRadiusSliderValueChanged(int newValue)
{
   mpSectorPlacerMinRadiusSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::SectorPlacerMaxRadiusSpinBoxValueChanged(double newValue)
{
   mpSectorPlacerMaxRadiusSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::SectorPlacerMaxRadiusSliderValueChanged(int newValue)
{
   mpSectorPlacerMaxRadiusSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::SectorPlacerMinPhiSpinBoxValueChanged(double newValue)
{
   mpSectorPlacerMinPhiSlider->setValue(newValue * 100);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::SectorPlacerMinPhiSliderValueChanged(int newValue)
{
   mpSectorPlacerMinPhiSpinBox->setValue(newValue / 100.0f);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::SectorPlacerMaxPhiSpinBoxValueChanged(double newValue)
{
   mpSectorPlacerMaxPhiSlider->setValue(newValue * 100);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::SectorPlacerMaxPhiSliderValueChanged(int newValue)
{
   mpSectorPlacerMaxPhiSpinBox->setValue(newValue / 100.0f);
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
void PlacerTab::SetupSectorPlacerConnections()
{
   connect(mpSectorPlacerXSpinBox, SIGNAL(valueChanged(double)), this, SLOT(SectorPlacerXSpinBoxValueChanged(double)));
   connect(mpSectorPlacerXSlider, SIGNAL(sliderMoved(int)), this, SLOT(SectorPlacerXSliderValueChanged(int)));
   connect(mpSectorPlacerYSpinBox, SIGNAL(valueChanged(double)), this, SLOT(SectorPlacerYSpinBoxValueChanged(double)));
   connect(mpSectorPlacerYSlider, SIGNAL(sliderMoved(int)), this, SLOT(SectorPlacerYSliderValueChanged(int)));
   connect(mpSectorPlacerZSpinBox, SIGNAL(valueChanged(double)), this, SLOT(SectorPlacerZSpinBoxValueChanged(double)));
   connect(mpSectorPlacerZSlider, SIGNAL(sliderMoved(int)), this, SLOT(SectorPlacerZSliderValueChanged(int)));
   connect(mpSectorPlacerMinRadiusSpinBox, SIGNAL(valueChanged(double)), this, SLOT(SectorPlacerMinRadiusSpinBoxValueChanged(double)));
   connect(mpSectorPlacerMinRadiusSlider, SIGNAL(sliderMoved(int)), this, SLOT(SectorPlacerMinRadiusSliderValueChanged(int)));
   connect(mpSectorPlacerMaxRadiusSpinBox, SIGNAL(valueChanged(double)), this, SLOT(SectorPlacerMaxRadiusSpinBoxValueChanged(double)));
   connect(mpSectorPlacerMaxRadiusSlider, SIGNAL(sliderMoved(int)), this, SLOT(SectorPlacerMaxRadiusSliderValueChanged(int)));
   connect(mpSectorPlacerMinPhiSpinBox, SIGNAL(valueChanged(double)), this, SLOT(SectorPlacerMinPhiSpinBoxValueChanged(double)));
   connect(mpSectorPlacerMinPhiSlider, SIGNAL(sliderMoved(int)), this, SLOT(SectorPlacerMinPhiSliderValueChanged(int)));
   connect(mpSectorPlacerMaxPhiSpinBox, SIGNAL(valueChanged(double)), this, SLOT(SectorPlacerMaxPhiSpinBoxValueChanged(double)));
   connect(mpSectorPlacerMaxPhiSlider, SIGNAL(sliderMoved(int)), this, SLOT(SectorPlacerMaxPhiSliderValueChanged(int)));
}

///////////////////////////////////////////////////////////////////////////////
