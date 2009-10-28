#include <dtInspectorQt/lightview.h>
#include "ui_dtinspectorqt.h"
#include <QtGui/QColorDialog>

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::LightView::LightView(Ui::InspectorWidget& ui)
:mUI(&ui)
{
   mFilterName = QString("dtCore::Light");

   // General light connections
   connect(mUI->lightEnabledToggle, SIGNAL(stateChanged(int)), this, SLOT(OnEnabled(int)));
   connect(mUI->lightNumberEdit, SIGNAL(valueChanged(int)), this, SLOT(OnLightNumberChanged(int)));
   connect(mUI->lightModeCombo, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnModeChanged(const QString&)));
   connect(mUI->lightAmbRedEdit, SIGNAL(valueChanged(double)), this, SLOT(OnAmbientChanged(double)));
   connect(mUI->lightAmbGreenEdit, SIGNAL(valueChanged(double)), this, SLOT(OnAmbientChanged(double)));
   connect(mUI->lightAmbBlueEdit, SIGNAL(valueChanged(double)), this, SLOT(OnAmbientChanged(double)));
   connect(mUI->lightAmbPushbutton, SIGNAL(clicked()), this, SLOT(OnAmbientColorChooserClicked()));
   connect(mUI->lightDiffRedEdit, SIGNAL(valueChanged(double)), this, SLOT(OnDiffuseChanged(double)));
   connect(mUI->lightDiffGreenEdit, SIGNAL(valueChanged(double)), this, SLOT(OnDiffuseChanged(double)));
   connect(mUI->lightDiffBlueEdit, SIGNAL(valueChanged(double)), this, SLOT(OnDiffuseChanged(double)));
   connect(mUI->lightDiffPushbutton, SIGNAL(clicked()), this, SLOT(OnDiffuseColorChooserClicked()));
   connect(mUI->lightSpecRedEdit, SIGNAL(valueChanged(double)), this, SLOT(OnSpecularChanged(double)));
   connect(mUI->lightSpecGreenEdit, SIGNAL(valueChanged(double)), this, SLOT(OnSpecularChanged(double)));
   connect(mUI->lightSpecBlueEdit, SIGNAL(valueChanged(double)), this, SLOT(OnSpecularChanged(double)));
   connect(mUI->lightSpecPushbutton, SIGNAL(clicked()), this, SLOT(OnSpecularColorChooserClicked()));

   // Positional light connections
   connect(mUI->positionalLightConstAttenEdit, SIGNAL(valueChanged(double)), this, SLOT(OnAttenuationChanged(double)));
   connect(mUI->positionalLightLinearAttenEdit, SIGNAL(valueChanged(double)), this, SLOT(OnAttenuationChanged(double)));
   connect(mUI->positionalLightQuadAttenEdit, SIGNAL(valueChanged(double)), this, SLOT(OnAttenuationChanged(double)));

   // Spotlight connections
   connect(mUI->spotlightCutoffEdit, SIGNAL(valueChanged(double)), this, SLOT(OnSpotCutoffChanged(double)));
   connect(mUI->spotlightExponentEdit, SIGNAL(valueChanged(double)), this, SLOT(OnSpotExponentChanged(double)));
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::LightView::~LightView()
{
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LightView::OperateOn(dtCore::Base* b)
{
   dtCore::Light *light = dynamic_cast<dtCore::Light*>(b);

   mOperateOn = light;
   Update();
}

//////////////////////////////////////////////////////////////////////////
bool dtInspectorQt::LightView::IsOfType(QString name, dtCore::Base* object)
{
   if (name == mFilterName && dynamic_cast<dtCore::Light*>(object) != NULL)
   {
      return true;
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LightView::OnEnabled(int checked)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetEnabled(checked ? true : false);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LightView::OnLightNumberChanged(int newValue)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetNumber(newValue);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LightView::OnModeChanged(const QString& newValue)
{
   if (mOperateOn.valid())
   {
      if (newValue == "GLOBAL")
      {
         mOperateOn->SetLightingMode(dtCore::Light::GLOBAL);
      }
      else if (newValue == "LOCAL")
      {
         mOperateOn->SetLightingMode(dtCore::Light::LOCAL);
      }
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LightView::OnAmbientChanged(double newValue)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetAmbient(mUI->lightAmbRedEdit->value(),
         mUI->lightAmbGreenEdit->value(), mUI->lightAmbBlueEdit->value(), 1.0f);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LightView::OnAmbientColorChooserClicked()
{
   QColor initialColor(mUI->lightAmbRedEdit->value() * 255,
      mUI->lightAmbGreenEdit->value() * 255, mUI->lightAmbBlueEdit->value() * 255);
   QColor newColor = QColorDialog::getColor(initialColor, mUI->lightAmbPushbutton);
   if (newColor.isValid())
   {
      mUI->lightAmbRedEdit->setValue(newColor.redF());
      mUI->lightAmbGreenEdit->setValue(newColor.greenF());
      mUI->lightAmbBlueEdit->setValue(newColor.blueF());
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LightView::OnDiffuseChanged(double newValue)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetDiffuse(mUI->lightDiffRedEdit->value(),
         mUI->lightDiffGreenEdit->value(), mUI->lightDiffBlueEdit->value(), 1.0f);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LightView::OnDiffuseColorChooserClicked()
{
   QColor initialColor(mUI->lightDiffRedEdit->value() * 255,
      mUI->lightDiffGreenEdit->value() * 255, mUI->lightDiffBlueEdit->value() * 255);
   QColor newColor = QColorDialog::getColor(initialColor, mUI->lightDiffPushbutton);
   if (newColor.isValid())
   {
      mUI->lightDiffRedEdit->setValue(newColor.redF());
      mUI->lightDiffGreenEdit->setValue(newColor.greenF());
      mUI->lightDiffBlueEdit->setValue(newColor.blueF());
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LightView::OnSpecularChanged(double newValue)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetSpecular(mUI->lightSpecRedEdit->value(),
         mUI->lightSpecGreenEdit->value(), mUI->lightSpecBlueEdit->value(), 1.0f);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LightView::OnSpecularColorChooserClicked()
{
   QColor initialColor(mUI->lightSpecRedEdit->value() * 255,
      mUI->lightSpecGreenEdit->value() * 255, mUI->lightSpecBlueEdit->value() * 255);
   QColor newColor = QColorDialog::getColor(initialColor, mUI->lightSpecPushbutton);
   if (newColor.isValid())
   {
      mUI->lightSpecRedEdit->setValue(newColor.redF());
      mUI->lightSpecGreenEdit->setValue(newColor.greenF());
      mUI->lightSpecBlueEdit->setValue(newColor.blueF());
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LightView::OnAttenuationChanged(double newValue)
{
   if (mPositionalLight.valid())
   {
      mPositionalLight->SetAttenuation(mUI->positionalLightConstAttenEdit->value(),
         mUI->positionalLightLinearAttenEdit->value(), mUI->positionalLightQuadAttenEdit->value());
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LightView::OnSpotCutoffChanged(double newValue)
{
   if (mSpotLight.valid())
   {
      mSpotLight->SetSpotCutoff(mUI->spotlightCutoffEdit->value());
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LightView::OnSpotExponentChanged(double newValue)
{
   if (mSpotLight.valid())
   {
      mSpotLight->SetSpotExponent(mUI->spotlightExponentEdit->value());
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LightView::Update()
{
   if (mOperateOn.valid())
   {
      UpdateGeneralLightProperties();
      dtCore::PositionalLight* positionalLight = dynamic_cast<dtCore::PositionalLight*>(mOperateOn.get());
      if (positionalLight != NULL)
      {
         mPositionalLight = positionalLight;
         UpdatePositionalLightProperties();
      }
      else
      {
         mUI->positionalLightGroupBox->hide();
      }

      dtCore::SpotLight* spotlight = dynamic_cast<dtCore::SpotLight*>(mOperateOn.get());
      if (spotlight != NULL)
      {
         mSpotLight = spotlight;
         UpdateSpotLightProperties();
      }
      else
      {
         mUI->spotlightGroupBox->hide();
      }
   }
   else
   {
      mUI->lightGroupBox->hide();
      mUI->positionalLightGroupBox->hide();
      mUI->spotlightGroupBox->hide();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LightView::UpdateGeneralLightProperties()
{
   mUI->lightGroupBox->show();

   mUI->lightEnabledToggle->setChecked(mOperateOn->GetEnabled());
   mUI->lightNumberEdit->setValue(mOperateOn->GetNumber());

   if (mOperateOn->GetLightingMode()==dtCore::Light::GLOBAL)
   {
      mUI->lightModeCombo->setCurrentIndex(mUI->lightModeCombo->findText("GLOBAL"));
   }
   else
   {
      mUI->lightModeCombo->setCurrentIndex(mUI->lightModeCombo->findText("LOCAL"));
   }

   float r,g,b,a;
   mOperateOn->GetAmbient(r, g, b, a);
   mUI->lightAmbRedEdit->setValue(r);
   mUI->lightAmbGreenEdit->setValue(g);
   mUI->lightAmbBlueEdit->setValue(b);

   mOperateOn->GetSpecular(r, g, b, a);
   mUI->lightDiffRedEdit->setValue(r);
   mUI->lightDiffGreenEdit->setValue(g);
   mUI->lightDiffBlueEdit->setValue(b);

   mOperateOn->GetDiffuse(r, g, b, a);
   mUI->lightSpecRedEdit->setValue(r);
   mUI->lightSpecGreenEdit->setValue(g);
   mUI->lightSpecBlueEdit->setValue(b);
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LightView::UpdatePositionalLightProperties()
{
   mUI->positionalLightGroupBox->show();

   float con, lin, quad;
   mPositionalLight->GetAttenuation(con, lin, quad);
   mUI->positionalLightConstAttenEdit->setValue(con);
   mUI->positionalLightLinearAttenEdit->setValue(lin);
   mUI->positionalLightQuadAttenEdit->setValue(quad);
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LightView::UpdateSpotLightProperties()
{
   mUI->spotlightGroupBox->show();

   mUI->spotlightCutoffEdit->setValue(mSpotLight->GetSpotCutoff());
   mUI->spotlightExponentEdit->setValue(mSpotLight->GetSpotExponent());
}

//////////////////////////////////////////////////////////////////////////

