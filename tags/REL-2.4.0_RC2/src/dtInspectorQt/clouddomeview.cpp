#include <dtInspectorQt/clouddomeview.h>
#include "ui_dtinspectorqt.h"
#include <QtGui/QColorDialog>

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::CloudDomeView::CloudDomeView(Ui::InspectorWidget& ui)
:mUI(&ui)
{
   mFilterName = QString("dtCore::CloudDome");
   connect(mUI->cloudDomeEnableToggle, SIGNAL(stateChanged(int)), this, SLOT(OnEnabled(int)));
   connect(mUI->cloudDomeRedEdit, SIGNAL(valueChanged(double)), this, SLOT(OnColorChanged(double)));
   connect(mUI->cloudDomeGreenEdit, SIGNAL(valueChanged(double)), this, SLOT(OnColorChanged(double)));
   connect(mUI->cloudDomeBlueEdit, SIGNAL(valueChanged(double)), this, SLOT(OnColorChanged(double)));
   connect(mUI->cloudDomeColorButton, SIGNAL(clicked()), this, SLOT(OnPickColor()));

   connect(mUI->cloudDomeScaleEdit, SIGNAL(valueChanged(double)), this, SLOT(OnParametersChanged(double)));
   connect(mUI->cloudDomeCutoffEdit, SIGNAL(valueChanged(double)), this, SLOT(OnParametersChanged(double)));
   connect(mUI->cloudDomeExpEdit, SIGNAL(valueChanged(double)), this, SLOT(OnParametersChanged(double)));
   connect(mUI->cloudDomeBiasEdit, SIGNAL(valueChanged(double)), this, SLOT(OnParametersChanged(double)));
   connect(mUI->cloudDomeXEdit, SIGNAL(valueChanged(double)), this, SLOT(OnParametersChanged(double)));
   connect(mUI->cloudDomeYEdit, SIGNAL(valueChanged(double)), this, SLOT(OnParametersChanged(double)));
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::CloudDomeView::~CloudDomeView()
{
   mOperateOn = NULL;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::CloudDomeView::OperateOn(dtCore::Base* b)
{
   dtCore::CloudDome *clouddome = dynamic_cast<dtCore::CloudDome*>(b);

   mOperateOn = clouddome;
   Update();
}

//////////////////////////////////////////////////////////////////////////
bool dtInspectorQt::CloudDomeView::IsOfType(QString name, dtCore::Base* object)
{
   if (name == mFilterName && dynamic_cast<dtCore::CloudDome*>(object) != NULL)
   {
      return true;
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::CloudDomeView::Update()
{
   if (mOperateOn.valid())
   {
      mUI->cloudDomeGroupBox->show();

      //enable
      mUI->cloudDomeEnableToggle->setChecked(mOperateOn->GetEnable());     

      //cloud color
      const osg::Vec3 color = mOperateOn->GetCloudColor();
      mUI->cloudDomeRedEdit->setValue(color[0]);
      mUI->cloudDomeGreenEdit->setValue(color[1]);
      mUI->cloudDomeBlueEdit->setValue(color[2]);

      //scale
      mUI->cloudDomeScaleEdit->setValue(mOperateOn->GetScale());

      //cutoff
      mUI->cloudDomeCutoffEdit->setValue(mOperateOn->GetCutoff());

      //exponent
      mUI->cloudDomeExpEdit->setValue(mOperateOn->GetExponent());

      //bias
      mUI->cloudDomeBiasEdit->setValue(mOperateOn->GetBias());

      //speed
      mUI->cloudDomeXEdit->setValue(mOperateOn->GetSpeedX());
      mUI->cloudDomeYEdit->setValue(mOperateOn->GetSpeedY());
   }
   else
   {
      mUI->cloudDomeGroupBox->hide();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::CloudDomeView::OnEnabled(int state)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetShaderEnable(state ? true : false);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::CloudDomeView::OnColorChanged(double)
{
   if (mOperateOn.valid())
   {
      osg::Vec3 color(mUI->cloudDomeRedEdit->value(),
                      mUI->cloudDomeGreenEdit->value(),
                      mUI->cloudDomeBlueEdit->value());
      mOperateOn->SetCloudColor(color);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::CloudDomeView::OnPickColor()
{
   QColor initialColor(mUI->cloudDomeRedEdit->value() * 255,
                       mUI->cloudDomeGreenEdit->value() * 255,
                       mUI->cloudDomeBlueEdit->value() * 255);
   QColor newColor = QColorDialog::getColor(initialColor, mUI->cloudDomeColorButton);
   if (newColor.isValid())
   {
      mUI->cloudDomeRedEdit->setValue(newColor.redF());
      mUI->cloudDomeGreenEdit->setValue(newColor.greenF());
      mUI->cloudDomeBlueEdit->setValue(newColor.blueF());
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::CloudDomeView::OnParametersChanged(double val)
{
  if (mOperateOn.valid())
  {
     mOperateOn->SetScale(mUI->cloudDomeScaleEdit->value());
     mOperateOn->SetCutoff(mUI->cloudDomeCutoffEdit->value());
     mOperateOn->SetExponent(mUI->cloudDomeExpEdit->value());
     mOperateOn->SetBias(mUI->cloudDomeBiasEdit->value());
     mOperateOn->SetSpeedX(mUI->cloudDomeXEdit->value());
     mOperateOn->SetSpeedY(mUI->cloudDomeYEdit->value());
  } 
}
