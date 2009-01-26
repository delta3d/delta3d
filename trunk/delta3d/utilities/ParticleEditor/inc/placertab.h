#ifndef DELTA_PLACER_TAB
#define DELTA_PLACER_TAB

#include <QtGui/QComboBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QSlider>
#include <QtGui/QListWidget>
#include <QtGui/QStackedWidget>
#include <QtGui/QPushButton>

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

   // Segment placer UI items
   void SetSegmentPlacerVertexAXSpinBox(QDoubleSpinBox* segmentPlacerVertexAXSpinBox)
      { mpSegmentPlacerVertexAXSpinBox = segmentPlacerVertexAXSpinBox; }
   void SetSegmentPlacerVertexAXSlider(QSlider* segmentPlacerVertexAXSlider)
      { mpSegmentPlacerVertexAXSlider = segmentPlacerVertexAXSlider; }
   void SetSegmentPlacerVertexAYSpinBox(QDoubleSpinBox* segmentPlacerVertexAYSpinBox)
      { mpSegmentPlacerVertexAYSpinBox = segmentPlacerVertexAYSpinBox; }
   void SetSegmentPlacerVertexAYSlider(QSlider* segmentPlacerVertexAYSlider)
      { mpSegmentPlacerVertexAYSlider = segmentPlacerVertexAYSlider; }
   void SetSegmentPlacerVertexAZSpinBox(QDoubleSpinBox* segmentPlacerVertexAZSpinBox)
      { mpSegmentPlacerVertexAZSpinBox = segmentPlacerVertexAZSpinBox; }
   void SetSegmentPlacerVertexAZSlider(QSlider* segmentPlacerVertexBZSlider)
      { mpSegmentPlacerVertexAZSlider = segmentPlacerVertexBZSlider; }
   void SetSegmentPlacerVertexBXSpinBox(QDoubleSpinBox* segmentPlacerVertexBXSpinBox)
      { mpSegmentPlacerVertexBXSpinBox = segmentPlacerVertexBXSpinBox; }
   void SetSegmentPlacerVertexBXSlider(QSlider* segmentPlacerVertexBXSlider)
      { mpSegmentPlacerVertexBXSlider = segmentPlacerVertexBXSlider; }
   void SetSegmentPlacerVertexBYSpinBox(QDoubleSpinBox* segmentPlacerVertexBYSpinBox)
      { mpSegmentPlacerVertexBYSpinBox = segmentPlacerVertexBYSpinBox; }
   void SetSegmentPlacerVertexBYSlider(QSlider* segmentPlacerVertexBYSlider)
      { mpSegmentPlacerVertexBYSlider = segmentPlacerVertexBYSlider; }
   void SetSegmentPlacerVertexBZSpinBox(QDoubleSpinBox* segmentPlacerVertexBZSpinBox)
      { mpSegmentPlacerVertexBZSpinBox = segmentPlacerVertexBZSpinBox; }
   void SetSegmentPlacerVertexBZSlider(QSlider* segmentPlacerVertexBZSlider)
      { mpSegmentPlacerVertexBZSlider = segmentPlacerVertexBZSlider; }

   // Multi Segment placer UI items
   void SetMultiSegmentPlacerVerticesList(QListWidget* multiSegmentPlacerVerticesList)
      { mpMultiSegmentPlacerVerticesList = multiSegmentPlacerVerticesList; }
   void SetVertexParametersStackedWidget(QStackedWidget* vertexParametersStackedWidget)
      { mpVertexParametersStackedWidget = vertexParametersStackedWidget; }
   void SetMultiSegmentPlacerAddVertexButton(QPushButton* multiSegmentPlacerAddVertexButton)
      { mpMultiSegmentPlacerAddVertexButton = multiSegmentPlacerAddVertexButton; }
   void SetMultiSegmentPlacerDeleteVertexButton(QPushButton* multiSegmentPlacerDeleteVertexButton)
      { mpMultiSegmentPlacerDeleteVertexButton = multiSegmentPlacerDeleteVertexButton; }
   void SetMultiSegmentPlacerXSpinBox(QDoubleSpinBox* multiSegmentPlacerXSpinBox)
      { mpMultiSegmentPlacerXSpinBox = multiSegmentPlacerXSpinBox; }
   void SetMultiSegmentPlacerXSlider(QSlider* multiSegmentPlacerXSlider)
      { mpMultiSegmentPlacerXSlider = multiSegmentPlacerXSlider; }
   void SetMultiSegmentPlacerYSpinBox(QDoubleSpinBox* multiSegmentPlacerYSpinBox)
      { mpMultiSegmentPlacerYSpinBox = multiSegmentPlacerYSpinBox; }
   void SetMultiSegmentPlacerYSlider(QSlider* multiSegmentPlacerYSlider)
      { mpMultiSegmentPlacerYSlider = multiSegmentPlacerYSlider; }
   void SetMultiSegmentPlacerZSpinBox(QDoubleSpinBox* multiSegmentPlacerZSpinBox)
      { mpMultiSegmentPlacerZSpinBox = multiSegmentPlacerZSpinBox; }
   void SetMultiSegmentPlacerZSlider(QSlider* multiSegmentPlacerZSlider)
      { mpMultiSegmentPlacerZSlider = multiSegmentPlacerZSlider; }

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

   // Segment placer UI items
   void SegmentPlacerVertexAXSpinBoxValueChanged(double newValue);
   void SegmentPlacerVertexAXSliderValueChanged(int newValue);
   void SegmentPlacerVertexAYSpinBoxValueChanged(double newValue);
   void SegmentPlacerVertexAYSliderValueChanged(int newValue);
   void SegmentPlacerVertexAZSpinBoxValueChanged(double newValue);
   void SegmentPlacerVertexAZSliderValueChanged(int newValue);
   void SegmentPlacerVertexBXSpinBoxValueChanged(double newValue);
   void SegmentPlacerVertexBXSliderValueChanged(int newValue);
   void SegmentPlacerVertexBYSpinBoxValueChanged(double newValue);
   void SegmentPlacerVertexBYSliderValueChanged(int newValue);
   void SegmentPlacerVertexBZSpinBoxValueChanged(double newValue);
   void SegmentPlacerVertexBZSliderValueChanged(int newValue);

   // Multi Segment placer UI items
   void MultiSegmentPlacerVertexListSelectionChanged(int newIndex);
   void MultiSegmentPlacerClearVertexList();
   void MultiSegmentPlacerAddVertexToList(double x, double y, double z);
   void MultiSegmentPlacerSelectIndexOfVertexList(int newIndex);
   void MultiSegmentPlacerAddVertexButtonPressed();
   void MultiSegmentPlacerDeleteVertexButtonPressed();
   void MultiSegmentPlacerXSpinBoxValueChanged(double newValue);
   void MultiSegmentPlacerXSliderValueChanged(int newValue);
   void MultiSegmentPlacerYSpinBoxValueChanged(double newValue);
   void MultiSegmentPlacerYSliderValueChanged(int newValue);
   void MultiSegmentPlacerZSpinBoxValueChanged(double newValue);
   void MultiSegmentPlacerZSliderValueChanged(int newValue);

private:
   void SetupPointPlacerConnections();
   void SetupSectorPlacerConnections();
   void SetupSegmentPlacerConnections();
   void SetupMultiSegmentPlacerConnections();
   void UpdateMultSegmentPlacerVerticesList();

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

   // Segment placer UI items
   QDoubleSpinBox* mpSegmentPlacerVertexAXSpinBox;
   QSlider* mpSegmentPlacerVertexAXSlider;
   QDoubleSpinBox* mpSegmentPlacerVertexAYSpinBox;
   QSlider* mpSegmentPlacerVertexAYSlider;
   QDoubleSpinBox* mpSegmentPlacerVertexAZSpinBox;
   QSlider* mpSegmentPlacerVertexAZSlider;
   QDoubleSpinBox* mpSegmentPlacerVertexBXSpinBox;
   QSlider* mpSegmentPlacerVertexBXSlider;
   QDoubleSpinBox* mpSegmentPlacerVertexBYSpinBox;
   QSlider* mpSegmentPlacerVertexBYSlider;
   QDoubleSpinBox* mpSegmentPlacerVertexBZSpinBox;
   QSlider* mpSegmentPlacerVertexBZSlider;

   // Multi Segment placer UI items
   QListWidget* mpMultiSegmentPlacerVerticesList;
   QStackedWidget* mpVertexParametersStackedWidget;
   QPushButton* mpMultiSegmentPlacerAddVertexButton;
   QPushButton* mpMultiSegmentPlacerDeleteVertexButton;
   QDoubleSpinBox* mpMultiSegmentPlacerXSpinBox;
   QSlider* mpMultiSegmentPlacerXSlider;
   QDoubleSpinBox* mpMultiSegmentPlacerYSpinBox;
   QSlider* mpMultiSegmentPlacerYSlider;
   QDoubleSpinBox* mpMultiSegmentPlacerZSpinBox;
   QSlider* mpMultiSegmentPlacerZSlider;
};

#endif /*DELTA_PLACER_TAB*/
