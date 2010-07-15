#include <programtab.h>

///////////////////////////////////////////////////////////////////////////////
ProgramTab::ProgramTab()
{
}

///////////////////////////////////////////////////////////////////////////////
ProgramTab::~ProgramTab()
{
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::SetupUI()
{
   SetupOperatorsConnections();
   SetupForceConnections();
   SetupAccelerationConnections();
   SetupFluidFrictionConnections();
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::OperatorSelectionChanged(const QString& currentText)
{
   if(currentText == "Force")
   {
      mpOperatorsStackedWidget->setCurrentIndex(1);
      mpOperatorsDeleteOperatorsButton->setEnabled(true);
   }
   else if(currentText == "Acceleration")
   {
      mpOperatorsStackedWidget->setCurrentIndex(2);
      mpOperatorsDeleteOperatorsButton->setEnabled(true);
   }
   else if(currentText == "Fluid Friction")
   {
      mpOperatorsStackedWidget->setCurrentIndex(3);
      mpOperatorsDeleteOperatorsButton->setEnabled(true);
   }
   else
   {
      mpOperatorsStackedWidget->setCurrentIndex(0);
      mpOperatorsDeleteOperatorsButton->setEnabled(false);
   }

   emit OperatorSelectionUpdate(mpOperatorsList->currentRow(), currentText);
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::ClearOperatorsList()
{
   mpOperatorsList->clear();
   mpOperatorsStackedWidget->setCurrentIndex(0);
   mpOperatorsDeleteOperatorsButton->setEnabled(false);
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::AddOperatorToOperatorsList(const QString& currentText)
{
   mpOperatorsList->addItem(currentText);
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::SelectIndexOfOperatorsList(int newIndex)
{
   if(0 <= newIndex && newIndex < mpOperatorsList->count())
   {
      mpOperatorsList->setCurrentRow(newIndex);
      mpOperatorsList->setFocus();
   }
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::NewForceButtonPressed()
{
   if(mpOperatorsList->count() == 1)
   {
      mpOperatorsDeleteOperatorsButton->setEnabled(true);
   }
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::NewAccelerationButtonPressed()
{
   if(mpOperatorsList->count() == 1)
   {
      mpOperatorsDeleteOperatorsButton->setEnabled(true);
   }
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::NewFluidFrictionButtonPressed()
{
   if(mpOperatorsList->count() == 1)
   {
      mpOperatorsDeleteOperatorsButton->setEnabled(true);
   }
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::DeleteOperatorButtonPressed()
{
   if(mpOperatorsList->count() < 1)
   {
      mpOperatorsDeleteOperatorsButton->setEnabled(false);
   }
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::ForceXSpinBoxValueChanged(double newValue)
{
   mpForceXSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::ForceXSliderValueChanged(int newValue)
{
   mpForceXSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::ForceYSpinBoxValueChanged(double newValue)
{
   mpForceYSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::ForceYSliderValueChanged(int newValue)
{
   mpForceYSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::ForceZSpinBoxValueChanged(double newValue)
{
   mpForceZSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::ForceZSliderValueChanged(int newValue)
{
   mpForceZSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::AccelerationXSpinBoxValueChanged(double newValue)
{
   mpAccelerationXSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::AccelerationXSliderValueChanged(int newValue)
{
   mpAccelerationXSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::AccelerationYSpinBoxValueChanged(double newValue)
{
   mpAccelerationYSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::AccelerationYSliderValueChanged(int newValue)
{
   mpAccelerationYSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::AccelerationZSpinBoxValueChanged(double newValue)
{
   mpAccelerationZSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::AccelerationZSliderValueChanged(int newValue)
{
   mpAccelerationZSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::DensitySpinBoxValueChanged(double newValue)
{
   mpDensitySlider->setValue(newValue * 1000);
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::DensitySliderValueChanged(int newValue)
{
   mpDensitySpinBox->setValue(newValue / 1000.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::ViscositySpinBoxValueChanged(double newValue)
{
   mpViscositySlider->setValue(newValue * 1000);
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::ViscositySliderValueChanged(int newValue)
{
   mpViscositySpinBox->setValue(newValue / 1000.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::OverrideRadiusSpinBoxValueChanged(double newValue)
{
   mpOverrideRadiusSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::OverrideRadiusSliderValueChanged(int newValue)
{
   mpOverrideRadiusSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::SetupOperatorsConnections()
{
   connect(mpOperatorsList, SIGNAL(currentTextChanged(const QString&)), this, SLOT(OperatorSelectionChanged(const QString&)));
   connect(mpOperatorsNewForceButton, SIGNAL(clicked()), this, SLOT(NewForceButtonPressed()));
   connect(mpOperatorsNewAccelerationButton, SIGNAL(clicked()), this, SLOT(NewAccelerationButtonPressed()));
   connect(mpOperatorsNewFluidFrictionButton, SIGNAL(clicked()), this, SLOT(NewFluidFrictionButtonPressed()));
   connect(mpOperatorsDeleteOperatorsButton, SIGNAL(clicked()), this, SLOT(DeleteOperatorButtonPressed()));
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::SetupForceConnections()
{
   connect(mpForceXSpinBox, SIGNAL(valueChanged(double)), this, SLOT(ForceXSpinBoxValueChanged(double)));
   connect(mpForceXSlider, SIGNAL(sliderMoved(int)), this, SLOT(ForceXSliderValueChanged(int)));
   connect(mpForceYSpinBox, SIGNAL(valueChanged(double)), this, SLOT(ForceYSpinBoxValueChanged(double)));
   connect(mpForceYSlider, SIGNAL(sliderMoved(int)), this, SLOT(ForceYSliderValueChanged(int)));
   connect(mpForceZSpinBox, SIGNAL(valueChanged(double)), this, SLOT(ForceZSpinBoxValueChanged(double)));
   connect(mpForceZSlider, SIGNAL(sliderMoved(int)), this, SLOT(ForceZSliderValueChanged(int)));
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::SetupAccelerationConnections()
{
   connect(mpAccelerationXSpinBox, SIGNAL(valueChanged(double)), this, SLOT(AccelerationXSpinBoxValueChanged(double)));
   connect(mpAccelerationXSlider, SIGNAL(sliderMoved(int)), this, SLOT(AccelerationXSliderValueChanged(int)));
   connect(mpAccelerationYSpinBox, SIGNAL(valueChanged(double)), this, SLOT(AccelerationYSpinBoxValueChanged(double)));
   connect(mpAccelerationYSlider, SIGNAL(sliderMoved(int)), this, SLOT(AccelerationYSliderValueChanged(int)));
   connect(mpAccelerationZSpinBox, SIGNAL(valueChanged(double)), this, SLOT(AccelerationZSpinBoxValueChanged(double)));
   connect(mpAccelerationZSlider, SIGNAL(sliderMoved(int)), this, SLOT(AccelerationZSliderValueChanged(int)));
}

///////////////////////////////////////////////////////////////////////////////
void ProgramTab::SetupFluidFrictionConnections()
{
   connect(mpDensitySpinBox, SIGNAL(valueChanged(double)), this, SLOT(DensitySpinBoxValueChanged(double)));
   connect(mpDensitySlider, SIGNAL(sliderMoved(int)), this, SLOT(DensitySliderValueChanged(int)));
   connect(mpViscositySpinBox, SIGNAL(valueChanged(double)), this, SLOT(ViscositySpinBoxValueChanged(double)));
   connect(mpViscositySlider, SIGNAL(sliderMoved(int)), this, SLOT(ViscositySliderValueChanged(int)));
   connect(mpOverrideRadiusSpinBox, SIGNAL(valueChanged(double)), this, SLOT(OverrideRadiusSpinBoxValueChanged(double)));
   connect(mpOverrideRadiusSlider, SIGNAL(sliderMoved(int)), this, SLOT(OverrideRadiusSliderValueChanged(int)));
}

///////////////////////////////////////////////////////////////////////////////
