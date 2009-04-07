#ifndef DELTA_COUNTER_TAB
#define DELTA_COUNTER_TAB

#include <QtGui/QComboBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QSlider>

class CounterTab : public QObject
{
   Q_OBJECT

public:
   CounterTab();
   ~CounterTab();

   void SetCounterTypeBox(QComboBox* counterTypeBox) { mpCounterTypeBox = counterTypeBox; }
   void SetRandomRateMinRateSpinBox(QDoubleSpinBox* randomRateMinRateSpinBox) { mpRandomRateMinRateSpinBox = randomRateMinRateSpinBox; }
   void SetRandomRateMinRateSlider(QSlider* randomRateMinRateSlider) { mpRandomRateMinRateSlider = randomRateMinRateSlider; }
   void SetRandomRateMaxRateSpinBox(QDoubleSpinBox* randomRateMaxRateSpinBox) { mpRandomRateMaxRateSpinBox = randomRateMaxRateSpinBox; }
   void SetRandomRateMaxRateSlider(QSlider* randomRateMaxRateSlider) { mpRandomRateMaxRateSlider = randomRateMaxRateSlider; }

   void SetupUI();

public slots:
   void RandomRateMinRateSpinBoxValueChanged(double newValue);
   void RandomRateMinRateSliderValueChanged(int newValue);
   void RandomRateMaxRateSpinBoxValueChanged(double newValue);
   void RandomRateMaxRateSliderValueChanged(int newValue);

private:
   void SetupRandomRateConnections();

   QComboBox* mpCounterTypeBox;

   QDoubleSpinBox* mpRandomRateMinRateSpinBox;
   QSlider* mpRandomRateMinRateSlider;
   QDoubleSpinBox* mpRandomRateMaxRateSpinBox;
   QSlider* mpRandomRateMaxRateSlider;
};

#endif /*DELTA_COUNTER_TAB*/
