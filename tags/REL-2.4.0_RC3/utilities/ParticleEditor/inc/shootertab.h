#ifndef DELTA_SHOOTER_TAB
#define DELTA_SHOOTER_TAB

#include <QtGui/QComboBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QSlider>

class ShooterTab : public QObject
{
   Q_OBJECT

public:
   ShooterTab();
   ~ShooterTab();

   void SetShooterTypeBox(QComboBox* shooterTypeBox) { mpShooterTypeBox = shooterTypeBox; }

   // Point placer UI items
   void SetRadialShooterElevationMinSpinBox(QDoubleSpinBox* radialShooterElevationMinSpinBox)
      { mpRadialShooterElevationMinSpinBox = radialShooterElevationMinSpinBox; }
   void SetRadialShooterElevationMinSlider(QSlider* radialShooterElevationMinSlider)
      { mpRadialShooterElevationMinSlider = radialShooterElevationMinSlider; }
   void SetRadialShooterElevationMaxSpinBox(QDoubleSpinBox* radialShooterElevationMaxSpinBox)
      { mpRadialShooterElevationMaxSpinBox = radialShooterElevationMaxSpinBox; }
   void SetRadialShooterElevationMaxSlider(QSlider* radialShooterElevationMaxSlider)
      { mpRadialShooterElevationMaxSlider = radialShooterElevationMaxSlider; }
   void SetRadialShooterAzimuthMinSpinBox(QDoubleSpinBox* radialShooterAzimuthMinSpinBox)
      { mpRadialShooterAzimuthMinSpinBox = radialShooterAzimuthMinSpinBox; }
   void SetRadialShooterAzimuthMinSlider(QSlider* radialShooterAzimuthMinSlider)
      { mpRadialShooterAzimuthMinSlider = radialShooterAzimuthMinSlider; }
   void SetRadialShooterAzimuthMaxSpinBox(QDoubleSpinBox* radialShooterAzimuthMaxSpinBox)
      { mpRadialShooterAzimuthMaxSpinBox = radialShooterAzimuthMaxSpinBox; }
   void SetRadialShooterAzimuthMaxSlider(QSlider* radialShooterAzimuthMaxSlider)
      { mpRadialShooterAzimuthMaxSlider = radialShooterAzimuthMaxSlider; }
   void SetRadialShooterInitialVelocityMinSpinBox(QDoubleSpinBox* radialShooterInitialVelocityMinSpinBox)
      { mpRadialShooterInitialVelocityMinSpinBox = radialShooterInitialVelocityMinSpinBox; }
   void SetRadialShooterInitialVelocityMinSlider(QSlider* radialShooterInitialVelocityMinSlider)
      { mpRadialShooterInitialVelocityMinSlider = radialShooterInitialVelocityMinSlider; }
   void SetRadialShooterInitialVelocityMaxSpinBox(QDoubleSpinBox* radialShooterInitialVelocityMaxSpinBox)
      { mpRadialShooterInitialVelocityMaxSpinBox = radialShooterInitialVelocityMaxSpinBox; }
   void SetRadialShooterInitialVelocityMaxSlider(QSlider* radialShooterInitialVelocityMaxSlider)
      { mpRadialShooterInitialVelocityMaxSlider = radialShooterInitialVelocityMaxSlider; }
   void SetRadialShooterInitialMinRotationXSpinBox(QDoubleSpinBox* radialShooterInitialMinRotationXSpinBox)
      { mpRadialShooterInitialMinRotationXSpinBox = radialShooterInitialMinRotationXSpinBox; }
   void SetRadialShooterInitialMinRotationXSlider(QSlider* radialShooterInitialMinRotationXSlider)
      { mpRadialShooterInitialMinRotationXSlider = radialShooterInitialMinRotationXSlider; }
   void SetRadialShooterInitialMinRotationYSpinBox(QDoubleSpinBox* radialShooterInitialMinRotationYSpinBox)
      { mpRadialShooterInitialMinRotationYSpinBox = radialShooterInitialMinRotationYSpinBox; }
   void SetRadialShooterInitialMinRotationYSlider(QSlider* radialShooterInitialMinRotationYSlider)
      { mpRadialShooterInitialMinRotationYSlider = radialShooterInitialMinRotationYSlider; }
   void SetRadialShooterInitialMinRotationZSpinBox(QDoubleSpinBox* radialShooterInitialMinRotationZSpinBox)
      { mpRadialShooterInitialMinRotationZSpinBox = radialShooterInitialMinRotationZSpinBox; }
   void SetRadialShooterInitialMinRotationZSlider(QSlider* radialShooterInitialMinRotationZSlider)
      { mpRadialShooterInitialMinRotationZSlider = radialShooterInitialMinRotationZSlider; }
   void SetRadialShooterInitialMaxRotationXSpinBox(QDoubleSpinBox* radialShooterInitialMaxRotationXSpinBox)
      { mpRadialShooterInitialMaxRotationXSpinBox = radialShooterInitialMaxRotationXSpinBox; }
   void SetRadialShooterInitialMaxRotationXSlider(QSlider* radialShooterInitialMaxRotationXSlider)
      { mpRadialShooterInitialMaxRotationXSlider = radialShooterInitialMaxRotationXSlider; }
   void SetRadialShooterInitialMaxRotationYSpinBox(QDoubleSpinBox* radialShooterInitialMaxRotationYSpinBox)
      { mpRadialShooterInitialMaxRotationYSpinBox = radialShooterInitialMaxRotationYSpinBox; }
   void SetRadialShooterInitialMaxRotationYSlider(QSlider* radialShooterInitialMaxRotationYSlider)
      { mpRadialShooterInitialMaxRotationYSlider = radialShooterInitialMaxRotationYSlider; }
   void SetRadialShooterInitialMaxRotationZSpinBox(QDoubleSpinBox* radialShooterInitialMaxRotationZSpinBox)
      { mpRadialShooterInitialMaxRotationZSpinBox = radialShooterInitialMaxRotationZSpinBox; }
   void SetRadialShooterInitialMaxRotationZSlider(QSlider* radialShooterInitialMaxRotationZSlider)
      { mpRadialShooterInitialMaxRotationZSlider = radialShooterInitialMaxRotationZSlider; }

   void SetupUI();

public slots:
   // Point placer UI items
   void RadialShooterElevationMinSpinBoxValueChanged(double newValue);
   void RadialShooterElevationMinSliderValueChanged(int newValue);
   void RadialShooterElevationMaxSpinBoxValueChanged(double newValue);
   void RadialShooterElevationMaxSliderValueChanged(int newValue);
   void RadialShooterAzimuthMinSpinBoxValueChanged(double newValue);
   void RadialShooterAzimuthMinSliderValueChanged(int newValue);
   void RadialShooterAzimuthMaxSpinBoxValueChanged(double newValue);
   void RadialShooterAzimuthMaxSliderValueChanged(int newValue);
   void RadialShooterInitialVelocityMinSpinBoxValueChanged(double newValue);
   void RadialShooterInitialVelocityMinSliderValueChanged(int newValue);
   void RadialShooterInitialVelocityMaxSpinBoxValueChanged(double newValue);
   void RadialShooterInitialVelocityMaxSliderValueChanged(int newValue);
   void RadialShooterInitialMinRotationXSpinBoxValueChanged(double newValue);
   void RadialShooterInitialMinRotationXSliderValueChanged(int newValue);
   void RadialShooterInitialMinRotationYSpinBoxValueChanged(double newValue);
   void RadialShooterInitialMinRotationYSliderValueChanged(int newValue);
   void RadialShooterInitialMinRotationZSpinBoxValueChanged(double newValue);
   void RadialShooterInitialMinRotationZSliderValueChanged(int newValue);
   void RadialShooterInitialMaxRotationXSpinBoxValueChanged(double newValue);
   void RadialShooterInitialMaxRotationXSliderValueChanged(int newValue);
   void RadialShooterInitialMaxRotationYSpinBoxValueChanged(double newValue);
   void RadialShooterInitialMaxRotationYSliderValueChanged(int newValue);
   void RadialShooterInitialMaxRotationZSpinBoxValueChanged(double newValue);
   void RadialShooterInitialMaxRotationZSliderValueChanged(int newValue);

private:
   void SetupRadialShooterConnections();

   QComboBox* mpShooterTypeBox;

   // Radial Shooter UI items
   QDoubleSpinBox* mpRadialShooterElevationMinSpinBox;
   QSlider* mpRadialShooterElevationMinSlider;
   QDoubleSpinBox* mpRadialShooterElevationMaxSpinBox;
   QSlider* mpRadialShooterElevationMaxSlider;
   QDoubleSpinBox* mpRadialShooterAzimuthMinSpinBox;
   QSlider* mpRadialShooterAzimuthMinSlider;
   QDoubleSpinBox* mpRadialShooterAzimuthMaxSpinBox;
   QSlider* mpRadialShooterAzimuthMaxSlider;
   QDoubleSpinBox* mpRadialShooterInitialVelocityMinSpinBox;
   QSlider* mpRadialShooterInitialVelocityMinSlider;
   QDoubleSpinBox* mpRadialShooterInitialVelocityMaxSpinBox;
   QSlider* mpRadialShooterInitialVelocityMaxSlider;
   QDoubleSpinBox* mpRadialShooterInitialMinRotationXSpinBox;
   QSlider* mpRadialShooterInitialMinRotationXSlider;
   QDoubleSpinBox* mpRadialShooterInitialMinRotationYSpinBox;
   QSlider* mpRadialShooterInitialMinRotationYSlider;
   QDoubleSpinBox* mpRadialShooterInitialMinRotationZSpinBox;
   QSlider* mpRadialShooterInitialMinRotationZSlider;
   QDoubleSpinBox* mpRadialShooterInitialMaxRotationXSpinBox;
   QSlider* mpRadialShooterInitialMaxRotationXSlider;
   QDoubleSpinBox* mpRadialShooterInitialMaxRotationYSpinBox;
   QSlider* mpRadialShooterInitialMaxRotationYSlider;
   QDoubleSpinBox* mpRadialShooterInitialMaxRotationZSpinBox;
   QSlider* mpRadialShooterInitialMaxRotationZSlider;
};

#endif /*DELTA_SHOOTER_TAB*/
