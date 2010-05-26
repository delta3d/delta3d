#include <countertab.h>

///////////////////////////////////////////////////////////////////////////////
CounterTab::CounterTab()
{
}

///////////////////////////////////////////////////////////////////////////////
CounterTab::~CounterTab()
{
}

///////////////////////////////////////////////////////////////////////////////
void CounterTab::SetupUI()
{
   SetupRandomRateConnections();
}

///////////////////////////////////////////////////////////////////////////////
void CounterTab::RandomRateMinRateSpinBoxValueChanged(double newValue)
{
   mpRandomRateMinRateSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void CounterTab::RandomRateMinRateSliderValueChanged(int newValue)
{
   mpRandomRateMinRateSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void CounterTab::RandomRateMaxRateSpinBoxValueChanged(double newValue)
{
   mpRandomRateMaxRateSlider->setValue(newValue * 10);
}

///////////////////////////////////////////////////////////////////////////////
void CounterTab::RandomRateMaxRateSliderValueChanged(int newValue)
{
   mpRandomRateMaxRateSpinBox->setValue(newValue / 10.0f);
}

///////////////////////////////////////////////////////////////////////////////
void CounterTab::SetupRandomRateConnections()
{
   connect(mpRandomRateMinRateSpinBox, SIGNAL(valueChanged(double)), this, SLOT(RandomRateMinRateSpinBoxValueChanged(double)));
   connect(mpRandomRateMinRateSlider, SIGNAL(sliderMoved(int)), this, SLOT(RandomRateMinRateSliderValueChanged(int)));
   connect(mpRandomRateMaxRateSpinBox, SIGNAL(valueChanged(double)), this, SLOT(RandomRateMaxRateSpinBoxValueChanged(double)));
   connect(mpRandomRateMaxRateSlider, SIGNAL(sliderMoved(int)), this, SLOT(RandomRateMaxRateSliderValueChanged(int)));
}

///////////////////////////////////////////////////////////////////////////////
