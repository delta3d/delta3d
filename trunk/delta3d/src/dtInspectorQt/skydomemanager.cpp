#include <dtInspectorQt/skydomemanager.h>
#include "ui_dtinspectorqt.h"

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::SkyDomeManager::SkyDomeManager(Ui::InspectorWidget& ui)
:mUI(&ui)
{
   connect(mUI->envSkyRedEdit, SIGNAL(valueChanged(double)), this, SLOT(OnColorChanged(double)));
   connect(mUI->envSkyGreenEdit, SIGNAL(valueChanged(double)), this, SLOT(OnColorChanged(double)));
   connect(mUI->envSkyBlueEdit, SIGNAL(valueChanged(double)), this, SLOT(OnColorChanged(double)));
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
