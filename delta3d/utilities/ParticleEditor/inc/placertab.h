#ifndef DELTA_PLACER_TAB
#define DELTA_PLACER_TAB

#include <QtGui/QComboBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QSlider>

class PlacerTab : public QObject
{
   Q_OBJECT

public:
   PlacerTab();
   ~PlacerTab();

   void SetPlacerTypeBox(QComboBox* placerTypeBox) { mpPlacerTypeBox = placerTypeBox; }

   // Point placer UI items
   void SetPointPlacerXSpinBox(QDoubleSpinBox* pointPlacerXSpinBox) { mpPointPlacerXSpinBox = pointPlacerXSpinBox; }
   void SetPointPlacerXSlider(QSlider* pointPlacerXSlider) { mpPointPlacerXSlider = pointPlacerXSlider; }
   void SetPointPlacerYSpinBox(QDoubleSpinBox* pointPlacerYSpinBox) { mpPointPlacerYSpinBox = pointPlacerYSpinBox; }
   void SetPointPlacerYSlider(QSlider* pointPlacerYSlider) { mpPointPlacerYSlider = pointPlacerYSlider; }
   void SetPointPlacerZSpinBox(QDoubleSpinBox* pointPlacerZSpinBox) { mpPointPlacerZSpinBox = pointPlacerZSpinBox; }
   void SetPointPlacerZSlider(QSlider* pointPlacerZSlider) { mpPointPlacerZSlider = pointPlacerZSlider; }

   void SetupUI();

public slots:
   // Point placer UI items
   void PointPlacerXSpinBoxValueChanged(double newValue);
   void PointPlacerXSliderValueChanged(int newValue);
   void PointPlacerYSpinBoxValueChanged(double newValue);
   void PointPlacerYSliderValueChanged(int newValue);
   void PointPlacerZSpinBoxValueChanged(double newValue);
   void PointPlacerZSliderValueChanged(int newValue);

private:
   void SetupPointPlacerConnections();

   QComboBox* mpPlacerTypeBox;

   // Point placer UI items
   QDoubleSpinBox* mpPointPlacerXSpinBox;
   QSlider* mpPointPlacerXSlider;
   QDoubleSpinBox* mpPointPlacerYSpinBox;
   QSlider* mpPointPlacerYSlider;
   QDoubleSpinBox* mpPointPlacerZSpinBox;
   QSlider* mpPointPlacerZSlider;
};

#endif /*DELTA_PLACER_TAB*/
