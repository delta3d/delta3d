#include <dtInspectorQt/skydomemanager.h>
#include "ui_dtinspectorqt.h"
#include <QtGui/QColorDialog>

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::SkyDomeManager::SkyDomeManager(Ui::InspectorWidget& ui)
:mUI(&ui)
{
   connect(mUI->envSkyRedEdit, SIGNAL(valueChanged(double)), this, SLOT(OnColorChanged(double)));
   connect(mUI->envSkyGreenEdit, SIGNAL(valueChanged(double)), this, SLOT(OnColorChanged(double)));
   connect(mUI->envSkyBlueEdit, SIGNAL(valueChanged(double)), this, SLOT(OnColorChanged(double)));
   connect(mUI->skyDomeColorButton, SIGNAL(clicked()), this, SLOT(OnColorPickerClicked()));
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::SkyDomeManager::~SkyDomeManager()
{
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::SkyDomeManager::OperateOn(dtCore::Base* b)
{
   dtCore::SkyDome *skydome = dynamic_cast<dtCore::SkyDome*>(b);

   mOperateOn = skydome;
   Update();
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::SkyDomeManager::Update()
{
   if (mOperateOn.valid())
   {
      mUI->skyDomeGroupBox->show();

      osg::Vec3 color;
      mOperateOn->GetBaseColor(color);
      mUI->envSkyRedEdit->setValue(color[0]);
      mUI->envSkyGreenEdit->setValue(color[1]);
      mUI->envSkyBlueEdit->setValue(color[2]);
   }
   else
   {
      mUI->skyDomeGroupBox->hide();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::SkyDomeManager::OnColorChanged(double val)
{
   if (mOperateOn.valid())
   {
      osg::Vec3 color(mUI->skyDomeRedEdit->value(),
                      mUI->skyDomeGreenEdit->value(),
                      mUI->skyDomeBlueEdit->value());

      mOperateOn->SetBaseColor(color);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::SkyDomeManager::OnColorPickerClicked()
{
   QColor initialColor(mUI->skyDomeRedEdit->value() * 255,
      mUI->skyDomeGreenEdit->value() * 255, mUI->skyDomeBlueEdit->value() * 255);
   QColor newColor = QColorDialog::getColor(initialColor, mUI->skyDomeColorButton);
   if (newColor.isValid())
   {
      mUI->skyDomeRedEdit->setValue(newColor.redF());
      mUI->skyDomeGreenEdit->setValue(newColor.greenF());
      mUI->skyDomeBlueEdit->setValue(newColor.blueF());
   }
}

//////////////////////////////////////////////////////////////////////////

