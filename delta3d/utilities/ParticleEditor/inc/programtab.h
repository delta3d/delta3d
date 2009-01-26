#ifndef DELTA_PROGRAM_TAB
#define DELTA_PROGRAM_TAB

#include <QtGui/QDoubleSpinBox>
#include <QtGui/QSlider>
#include <QtGui/QListWidget>
#include <QtGui/QStackedWidget>
#include <QtGui/QPushButton>

class ProgramTab : public QObject
{
   Q_OBJECT

public:
   ProgramTab();
   ~ProgramTab();

   // Operators UI items
   void SetOperatorsList(QListWidget* operatorsList)
      { mpOperatorsList = operatorsList; }
   void SetOperatorsStackedWidget(QStackedWidget* operatorsStackedWidget)
      { mpOperatorsStackedWidget = operatorsStackedWidget; }
   void SetOperatorsNewForceButton(QPushButton* operatorsNewForceButton)
      { mpOperatorsNewForceButton = operatorsNewForceButton; }
   void SetOperatorsNewAccelerationButton(QPushButton* operatorsNewAccelerationButton)
      { mpOperatorsNewAccelerationButton = operatorsNewAccelerationButton; }
   void SetOperatorsNewFluidFrictionButton(QPushButton* operatorsNewFluidFrictionButton)
      { mpOperatorsNewFluidFrictionButton = operatorsNewFluidFrictionButton; }
   void SetOperatorsDeleteOperatorsButton(QPushButton* operatorsDeleteOperatorsButton)
      { mpOperatorsDeleteOperatorsButton = operatorsDeleteOperatorsButton; }

   // Force UI items
   void SetForceXSpinBox(QDoubleSpinBox* forceXSpinBox)
      { mpForceXSpinBox = forceXSpinBox; }
   void SetForceXSlider(QSlider* forceXSlider)
      { mpForceXSlider = forceXSlider; }
   void SetForceYSpinBox(QDoubleSpinBox* forceYSpinBox)
      { mpForceYSpinBox = forceYSpinBox; }
   void SetForceYSlider(QSlider* forceYSlider)
      { mpForceYSlider = forceYSlider; }
   void SetForceZSpinBox(QDoubleSpinBox* forceZSpinBox)
      { mpForceZSpinBox = forceZSpinBox; }
   void SetForceZSlider(QSlider* forceZSlider)
      { mpForceZSlider = forceZSlider; }

   // Acceleration UI items
   void SetAccelerationXSpinBox(QDoubleSpinBox* accelerationXSpinBox)
      { mpAccelerationXSpinBox = accelerationXSpinBox; }
   void SetAccelerationXSlider(QSlider* accelerationXSlider)
      { mpAccelerationXSlider = accelerationXSlider; }
   void SetAccelerationYSpinBox(QDoubleSpinBox* accelerationYSpinBox)
      { mpAccelerationYSpinBox = accelerationYSpinBox; }
   void SetAccelerationYSlider(QSlider* accelerationYSlider)
      { mpAccelerationYSlider = accelerationYSlider; }
   void SetAccelerationZSpinBox(QDoubleSpinBox* accelerationZSpinBox)
      { mpAccelerationZSpinBox = accelerationZSpinBox; }
   void SetAccelerationZSlider(QSlider* accelerationZSlider)
      { mpAccelerationZSlider = accelerationZSlider; }

   // Fluid Friction UI items
   void SetDensitySpinBox(QDoubleSpinBox* densitySpinBox)
      { mpDensitySpinBox = densitySpinBox; }
   void SetDensitySlider(QSlider* densitySlider)
      { mpDensitySlider = densitySlider; }
   void SetViscositySpinBox(QDoubleSpinBox* viscositySpinBox)
      { mpViscositySpinBox = viscositySpinBox; }
   void SetViscositySlider(QSlider* viscositySlider)
      { mpViscositySlider = viscositySlider; }
   void SetOverrideRadiusSpinBox(QDoubleSpinBox* overrideRadiusSpinBox)
      { mpOverrideRadiusSpinBox = overrideRadiusSpinBox; }
   void SetOverrideRadiusSlider(QSlider* overrideRadiusSlider)
      { mpOverrideRadiusSlider = overrideRadiusSlider; }

   void SetupUI();

signals:
   void OperatorSelectionUpdate(int newIndex, const QString& operatorType);

public slots:
   // Operators UI items
   void OperatorSelectionChanged(const QString& currentText);
   void ClearOperatorsList();
   void AddOperatorToOperatorsList(const QString& currentText);
   void SelectIndexOfOperatorsList(int newIndex);
   void NewForceButtonPressed();
   void NewAccelerationButtonPressed();
   void NewFluidFrictionButtonPressed();
   void DeleteOperatorButtonPressed();

   // Force UI items
   void ForceXSpinBoxValueChanged(double newValue);
   void ForceXSliderValueChanged(int newValue);
   void ForceYSpinBoxValueChanged(double newValue);
   void ForceYSliderValueChanged(int newValue);
   void ForceZSpinBoxValueChanged(double newValue);
   void ForceZSliderValueChanged(int newValue);

   // Acceleration UI items
   void AccelerationXSpinBoxValueChanged(double newValue);
   void AccelerationXSliderValueChanged(int newValue);
   void AccelerationYSpinBoxValueChanged(double newValue);
   void AccelerationYSliderValueChanged(int newValue);
   void AccelerationZSpinBoxValueChanged(double newValue);
   void AccelerationZSliderValueChanged(int newValue);

   // Fluid Friction UI items
   void DensitySpinBoxValueChanged(double newValue);
   void DensitySliderValueChanged(int newValue);
   void ViscositySpinBoxValueChanged(double newValue);
   void ViscositySliderValueChanged(int newValue);
   void OverrideRadiusSpinBoxValueChanged(double newValue);
   void OverrideRadiusSliderValueChanged(int newValue);

private:
   void SetupOperatorsConnections();
   void SetupForceConnections();
   void SetupAccelerationConnections();
   void SetupFluidFrictionConnections();

   // Operators UI items
   QListWidget* mpOperatorsList;
   QStackedWidget* mpOperatorsStackedWidget;
   QPushButton* mpOperatorsNewForceButton;
   QPushButton* mpOperatorsNewAccelerationButton;
   QPushButton* mpOperatorsNewFluidFrictionButton;
   QPushButton* mpOperatorsDeleteOperatorsButton;

   // Force UI items
   QDoubleSpinBox* mpForceXSpinBox;
   QSlider* mpForceXSlider;
   QDoubleSpinBox* mpForceYSpinBox;
   QSlider* mpForceYSlider;
   QDoubleSpinBox* mpForceZSpinBox;
   QSlider* mpForceZSlider;

   // Acceleration UI items
   QDoubleSpinBox* mpAccelerationXSpinBox;
   QSlider* mpAccelerationXSlider;
   QDoubleSpinBox* mpAccelerationYSpinBox;
   QSlider* mpAccelerationYSlider;
   QDoubleSpinBox* mpAccelerationZSpinBox;
   QSlider* mpAccelerationZSlider;

   // Fluid Friction UI items
   QDoubleSpinBox* mpDensitySpinBox;
   QSlider* mpDensitySlider;
   QDoubleSpinBox* mpViscositySpinBox;
   QSlider* mpViscositySlider;
   QDoubleSpinBox* mpOverrideRadiusSpinBox;
   QSlider* mpOverrideRadiusSlider;
};

#endif /*DELTA_PROGRAM_TAB*/
