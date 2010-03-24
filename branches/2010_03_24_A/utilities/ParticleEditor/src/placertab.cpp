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
   SetupSegmentPlacerConnections();
   SetupMultiSegmentPlacerConnections();
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
void PlacerTab::SegmentPlacerVertexAXSpinBoxValueChanged(double newValue)
{
   mpSegmentPlacerVertexAXSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::SegmentPlacerVertexAXSliderValueChanged(int newValue)
{
   mpSegmentPlacerVertexAXSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::SegmentPlacerVertexAYSpinBoxValueChanged(double newValue)
{
   mpSegmentPlacerVertexAYSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::SegmentPlacerVertexAYSliderValueChanged(int newValue)
{
   mpSegmentPlacerVertexAYSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::SegmentPlacerVertexAZSpinBoxValueChanged(double newValue)
{
   mpSegmentPlacerVertexAZSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::SegmentPlacerVertexAZSliderValueChanged(int newValue)
{
   mpSegmentPlacerVertexAZSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::SegmentPlacerVertexBXSpinBoxValueChanged(double newValue)
{
   mpSegmentPlacerVertexBXSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::SegmentPlacerVertexBXSliderValueChanged(int newValue)
{
   mpSegmentPlacerVertexBXSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::SegmentPlacerVertexBYSpinBoxValueChanged(double newValue)
{
   mpSegmentPlacerVertexBYSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::SegmentPlacerVertexBYSliderValueChanged(int newValue)
{
   mpSegmentPlacerVertexBYSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::SegmentPlacerVertexBZSpinBoxValueChanged(double newValue)
{
   mpSegmentPlacerVertexBZSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::SegmentPlacerVertexBZSliderValueChanged(int newValue)
{
   mpSegmentPlacerVertexBZSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::MultiSegmentPlacerVertexListSelectionChanged(int newIndex)
{
   if(newIndex != -1)
   {
      mpMultiSegmentPlacerDeleteVertexButton->setEnabled(true);
      mpVertexParametersStackedWidget->setCurrentIndex(1);
   }
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::MultiSegmentPlacerClearVertexList()
{
   mpMultiSegmentPlacerVerticesList->clear();
   mpMultiSegmentPlacerDeleteVertexButton->setEnabled(false);
   mpVertexParametersStackedWidget->setCurrentIndex(0);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::MultiSegmentPlacerSelectIndexOfVertexList(int newIndex)
{
   if(0 <= newIndex && newIndex < mpMultiSegmentPlacerVerticesList->count())
   {
      mpMultiSegmentPlacerVerticesList->setCurrentRow(newIndex);
      mpMultiSegmentPlacerVerticesList->setFocus();
   }
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::MultiSegmentPlacerAddVertexToList(double x, double y, double z)
{
   QString newVector;
   newVector.sprintf("%g, %g, %g", x, y, z);
   mpMultiSegmentPlacerVerticesList->addItem(newVector);
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::MultiSegmentPlacerAddVertexButtonPressed()
{
   if(mpMultiSegmentPlacerVerticesList->count() == 1)
   {
      mpMultiSegmentPlacerDeleteVertexButton->setEnabled(true);
      mpVertexParametersStackedWidget->setCurrentIndex(1);
   }
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::MultiSegmentPlacerDeleteVertexButtonPressed()
{
   if(mpMultiSegmentPlacerVerticesList->count() < 1)
   {
      mpMultiSegmentPlacerDeleteVertexButton->setEnabled(false);
      mpVertexParametersStackedWidget->setCurrentIndex(0);
   }
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::MultiSegmentPlacerXSpinBoxValueChanged(double newValue)
{
   mpMultiSegmentPlacerXSlider->setValue(newValue * 10);
   UpdateMultSegmentPlacerVerticesList();
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::MultiSegmentPlacerXSliderValueChanged(int newValue)
{
   mpMultiSegmentPlacerXSpinBox->setValue(newValue / 10.0f);
   UpdateMultSegmentPlacerVerticesList();
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::MultiSegmentPlacerYSpinBoxValueChanged(double newValue)
{
   mpMultiSegmentPlacerYSlider->setValue(newValue * 10);
   UpdateMultSegmentPlacerVerticesList();
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::MultiSegmentPlacerYSliderValueChanged(int newValue)
{
   mpMultiSegmentPlacerYSpinBox->setValue(newValue / 10.0f);
   UpdateMultSegmentPlacerVerticesList();
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::MultiSegmentPlacerZSpinBoxValueChanged(double newValue)
{
   mpMultiSegmentPlacerZSlider->setValue(newValue * 10);
   UpdateMultSegmentPlacerVerticesList();
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::MultiSegmentPlacerZSliderValueChanged(int newValue)
{
   mpMultiSegmentPlacerZSpinBox->setValue(newValue / 10.0f);
   UpdateMultSegmentPlacerVerticesList();
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
void PlacerTab::SetupSegmentPlacerConnections()
{
   connect(mpSegmentPlacerVertexAXSpinBox, SIGNAL(valueChanged(double)), this, SLOT(SegmentPlacerVertexAXSpinBoxValueChanged(double)));
   connect(mpSegmentPlacerVertexAXSlider, SIGNAL(sliderMoved(int)), this, SLOT(SegmentPlacerVertexAXSliderValueChanged(int)));
   connect(mpSegmentPlacerVertexAYSpinBox, SIGNAL(valueChanged(double)), this, SLOT(SegmentPlacerVertexAYSpinBoxValueChanged(double)));
   connect(mpSegmentPlacerVertexAYSlider, SIGNAL(sliderMoved(int)), this, SLOT(SegmentPlacerVertexAYSliderValueChanged(int)));
   connect(mpSegmentPlacerVertexAZSpinBox, SIGNAL(valueChanged(double)), this, SLOT(SegmentPlacerVertexAZSpinBoxValueChanged(double)));
   connect(mpSegmentPlacerVertexAZSlider, SIGNAL(sliderMoved(int)), this, SLOT(SegmentPlacerVertexAZSliderValueChanged(int)));
   connect(mpSegmentPlacerVertexBXSpinBox, SIGNAL(valueChanged(double)), this, SLOT(SegmentPlacerVertexBXSpinBoxValueChanged(double)));
   connect(mpSegmentPlacerVertexBXSlider, SIGNAL(sliderMoved(int)), this, SLOT(SegmentPlacerVertexBXSliderValueChanged(int)));
   connect(mpSegmentPlacerVertexBYSpinBox, SIGNAL(valueChanged(double)), this, SLOT(SegmentPlacerVertexBYSpinBoxValueChanged(double)));
   connect(mpSegmentPlacerVertexBYSlider, SIGNAL(sliderMoved(int)), this, SLOT(SegmentPlacerVertexBYSliderValueChanged(int)));
   connect(mpSegmentPlacerVertexBZSpinBox, SIGNAL(valueChanged(double)), this, SLOT(SegmentPlacerVertexBZSpinBoxValueChanged(double)));
   connect(mpSegmentPlacerVertexBZSlider, SIGNAL(sliderMoved(int)), this, SLOT(SegmentPlacerVertexBZSliderValueChanged(int)));
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::SetupMultiSegmentPlacerConnections()
{
   connect(mpMultiSegmentPlacerVerticesList, SIGNAL(currentRowChanged(int)), this, SLOT(MultiSegmentPlacerVertexListSelectionChanged(int)));
   connect(mpMultiSegmentPlacerAddVertexButton, SIGNAL(clicked()), this, SLOT(MultiSegmentPlacerAddVertexButtonPressed()));
   connect(mpMultiSegmentPlacerDeleteVertexButton, SIGNAL(clicked()), this, SLOT(MultiSegmentPlacerDeleteVertexButtonPressed()));
   connect(mpMultiSegmentPlacerXSpinBox, SIGNAL(valueChanged(double)), this, SLOT(MultiSegmentPlacerXSpinBoxValueChanged(double)));
   connect(mpMultiSegmentPlacerXSlider, SIGNAL(sliderMoved(int)), this, SLOT(MultiSegmentPlacerXSliderValueChanged(int)));
   connect(mpMultiSegmentPlacerYSpinBox, SIGNAL(valueChanged(double)), this, SLOT(MultiSegmentPlacerYSpinBoxValueChanged(double)));
   connect(mpMultiSegmentPlacerYSlider, SIGNAL(sliderMoved(int)), this, SLOT(MultiSegmentPlacerYSliderValueChanged(int)));
   connect(mpMultiSegmentPlacerZSpinBox, SIGNAL(valueChanged(double)), this, SLOT(MultiSegmentPlacerZSpinBoxValueChanged(double)));
   connect(mpMultiSegmentPlacerZSlider, SIGNAL(sliderMoved(int)), this, SLOT(MultiSegmentPlacerZSliderValueChanged(int)));
}

///////////////////////////////////////////////////////////////////////////////
void PlacerTab::UpdateMultSegmentPlacerVerticesList()
{
   QString newVector;
   newVector.sprintf("%g, %g, %g", mpMultiSegmentPlacerXSpinBox->value(),
      mpMultiSegmentPlacerYSpinBox->value(), mpMultiSegmentPlacerZSpinBox->value());
   mpMultiSegmentPlacerVerticesList->currentItem()->setText(newVector);
}

///////////////////////////////////////////////////////////////////////////////
