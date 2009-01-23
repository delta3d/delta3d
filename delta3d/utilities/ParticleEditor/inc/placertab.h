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
   void SetPointPlacerXSpinBox(QDoubleSpinBox* pointPlacerXSpinBox)
      { mpPointPlacerXSpinBox = pointPlacerXSpinBox; }
   void SetPointPlacerXSlider(QSlider* pointPlacerXSlider)
      { mpPointPlacerXSlider = pointPlacerXSlider; }
   void SetPointPlacerYSpinBox(QDoubleSpinBox* pointPlacerYSpinBox)
      { mpPointPlacerYSpinBox = pointPlacerYSpinBox; }
   void SetPointPlacerYSlider(QSlider* pointPlacerYSlider)
      { mpPointPlacerYSlider = pointPlacerYSlider; }
   void SetPointPlacerZSpinBox(QDoubleSpinBox* pointPlacerZSpinBox)
      { mpPointPlacerZSpinBox = pointPlacerZSpinBox; }
   void SetPointPlacerZSlider(QSlider* pointPlacerZSlider)
      { mpPointPlacerZSlider = pointPlacerZSlider; }

   // Sector placer UI items
   void SetSectorPlacerXSpinBox(QDoubleSpinBox* SectorPlacerXSpinBox)
      { mpSectorPlacerXSpinBox = SectorPlacerXSpinBox; }
   void SetSectorPlacerXSlider(QSlider* SectorPlacerXSlider)
      { mpSectorPlacerXSlider = SectorPlacerXSlider; }
   void SetSectorPlacerYSpinBox(QDoubleSpinBox* SectorPlacerYSpinBox)
      { mpSectorPlacerYSpinBox = SectorPlacerYSpinBox; }
   void SetSectorPlacerYSlider(QSlider* SectorPlacerYSlider)
      { mpSectorPlacerYSlider = SectorPlacerYSlider; }
   void SetSectorPlacerZSpinBox(QDoubleSpinBox* SectorPlacerZSpinBox)
      { mpSectorPlacerZSpinBox = SectorPlacerZSpinBox; }
   void SetSectorPlacerZSlider(QSlider* SectorPlacerZSlider)
      { mpSectorPlacerZSlider = SectorPlacerZSlider; }
   void SetSectorPlacerMinRadiusSpinBox(QDoubleSpinBox* SectorPlacerMinRadiusSpinBox)
      { mpSectorPlacerMinRadiusSpinBox = SectorPlacerMinRadiusSpinBox; }
   void SetSectorPlacerMinRadiusSlider(QSlider* SectorPlacerMinRadiusSlider)
      { mpSectorPlacerMinRadiusSlider = SectorPlacerMinRadiusSlider; }
   void SetSectorPlacerMaxRadiusSpinBox(QDoubleSpinBox* SectorPlacerMaxRadiusSpinBox)
      { mpSectorPlacerMaxRadiusSpinBox = SectorPlacerMaxRadiusSpinBox; }
   void SetSectorPlacerMaxRadiusSlider(QSlider* SectorPlacerMaxRadiusSlider)
      { mpSectorPlacerMaxRadiusSlider = SectorPlacerMaxRadiusSlider; }
   void SetSectorPlacerMinPhiSpinBox(QDoubleSpinBox* SectorPlacerMinPhiSpinBox)
      { mpSectorPlacerMinPhiSpinBox = SectorPlacerMinPhiSpinBox; }
   void SetSectorPlacerMinPhiSlider(QSlider* SectorPlacerMinPhiSlider)
      { mpSectorPlacerMinPhiSlider = SectorPlacerMinPhiSlider; }
   void SetSectorPlacerMaxPhiSpinBox(QDoubleSpinBox* SectorPlacerMaxPhiSpinBox)
      { mpSectorPlacerMaxPhiSpinBox = SectorPlacerMaxPhiSpinBox; }
   void SetSectorPlacerMaxPhiSlider(QSlider* SectorPlacerMaxPhiSlider)
      { mpSectorPlacerMaxPhiSlider = SectorPlacerMaxPhiSlider; }

   void SetupUI();

public slots:
   // Point placer UI items
   void PointPlacerXSpinBoxValueChanged(double newValue);
   void PointPlacerXSliderValueChanged(int newValue);
   void PointPlacerYSpinBoxValueChanged(double newValue);
   void PointPlacerYSliderValueChanged(int newValue);
   void PointPlacerZSpinBoxValueChanged(double newValue);
   void PointPlacerZSliderValueChanged(int newValue);

   // Sector placer UI items
   void SectorPlacerXSpinBoxValueChanged(double newValue);
   void SectorPlacerXSliderValueChanged(int newValue);
   void SectorPlacerYSpinBoxValueChanged(double newValue);
   void SectorPlacerYSliderValueChanged(int newValue);
   void SectorPlacerZSpinBoxValueChanged(double newValue);
   void SectorPlacerZSliderValueChanged(int newValue);
   void SectorPlacerMinRadiusSpinBoxValueChanged(double newValue);
   void SectorPlacerMinRadiusSliderValueChanged(int newValue);
   void SectorPlacerMaxRadiusSpinBoxValueChanged(double newValue);
   void SectorPlacerMaxRadiusSliderValueChanged(int newValue);
   void SectorPlacerMinPhiSpinBoxValueChanged(double newValue);
   void SectorPlacerMinPhiSliderValueChanged(int newValue);
   void SectorPlacerMaxPhiSpinBoxValueChanged(double newValue);
   void SectorPlacerMaxPhiSliderValueChanged(int newValue);

private:
   void SetupPointPlacerConnections();
   void SetupSectorPlacerConnections();

   QComboBox* mpPlacerTypeBox;

   // Point placer UI items
   QDoubleSpinBox* mpPointPlacerXSpinBox;
   QSlider* mpPointPlacerXSlider;
   QDoubleSpinBox* mpPointPlacerYSpinBox;
   QSlider* mpPointPlacerYSlider;
   QDoubleSpinBox* mpPointPlacerZSpinBox;
   QSlider* mpPointPlacerZSlider;

   // Sector placer UI items
   QDoubleSpinBox* mpSectorPlacerXSpinBox;
   QSlider* mpSectorPlacerXSlider;
   QDoubleSpinBox* mpSectorPlacerYSpinBox;
   QSlider* mpSectorPlacerYSlider;
   QDoubleSpinBox* mpSectorPlacerZSpinBox;
   QSlider* mpSectorPlacerZSlider;
   QDoubleSpinBox* mpSectorPlacerMinRadiusSpinBox;
   QSlider* mpSectorPlacerMinRadiusSlider;
   QDoubleSpinBox* mpSectorPlacerMaxRadiusSpinBox;
   QSlider* mpSectorPlacerMaxRadiusSlider;
   QDoubleSpinBox* mpSectorPlacerMinPhiSpinBox;
   QSlider* mpSectorPlacerMinPhiSlider;
   QDoubleSpinBox* mpSectorPlacerMaxPhiSpinBox;
   QSlider* mpSectorPlacerMaxPhiSlider;
};

#endif /*DELTA_PLACER_TAB*/
