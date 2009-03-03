#include <dtInspectorQt/weathermanager.h>
#include "ui_dtinspectorqt.h"
#include <dtCore/environment.h>

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::WeatherManager::WeatherManager(Ui::InspectorWidget& ui)
:mUI(&ui)
{
   connect(mUI->weatherThemeCombo, SIGNAL(activated(int)), this, SLOT(OnThemeChanged(int)));
   connect(mUI->weatherCloudCombo, SIGNAL(activated(int)), this, SLOT(OnCloudTypeChanged(int)));
   connect(mUI->weatherWindCombo, SIGNAL(activated(int)), this, SLOT(OnWindTypeChanged(int)));
   connect(mUI->weatherVisibilityCombo, SIGNAL(activated(int)), this, SLOT(OnVisibilityTypeChanged(int)));
   connect(mUI->weatherTimeCombo, SIGNAL(activated(int)), this, SLOT(OnTimeSeasonChanged(int)));
   connect(mUI->weatherSeasonCombo, SIGNAL(activated(int)), this, SLOT(OnTimeSeasonChanged(int)));
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::WeatherManager::~WeatherManager()
{
   mOperateOn = NULL;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::WeatherManager::OperateOn(dtCore::Base* b)
{
   dtABC::Weather *weather = dynamic_cast<dtABC::Weather*>(b);

   mOperateOn = weather;
   Update();
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::WeatherManager::Update()
{
   if (mOperateOn.valid())
   {
      mUI->weatherGroupBox->show();

      mUI->weatherThemeCombo->setCurrentIndex(mOperateOn->GetTheme());
      mUI->weatherCloudCombo->setCurrentIndex(mOperateOn->GetBasicCloudType());
      mUI->weatherWindCombo->setCurrentIndex(mOperateOn->GetBasicWindType());
      mUI->weatherVisibilityCombo->setCurrentIndex(mOperateOn->GetBasicVisibilityType());

      dtABC::Weather::TimePeriod time;
      dtABC::Weather::Season season;
      mOperateOn->GetTimePeriodAndSeason(&time, &season);
      mUI->weatherTimeCombo->setCurrentIndex(time);
      mUI->weatherSeasonCombo->setCurrentIndex(season);

      const dtCore::Environment* env = mOperateOn->GetEnvironment();
      if (env)
      {
         mUI->weatherEnvEdit->setText(QString::fromStdString(env->GetName()));
      } 
      else
      {
         mUI->weatherEnvEdit->setText("");
      }
   } 
   else
   {
      mUI->weatherGroupBox->hide();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::WeatherManager::OnThemeChanged(int index)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetTheme(dtABC::Weather::WeatherTheme(index));
      Update();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::WeatherManager::OnCloudTypeChanged(int index)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetBasicCloudType(dtABC::Weather::CloudType(index));
      Update();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::WeatherManager::OnWindTypeChanged(int index)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetBasicWindType(dtABC::Weather::WindType(index));
      Update();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::WeatherManager::OnVisibilityTypeChanged(int index)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetBasicVisibilityType(dtABC::Weather::VisibilityType(index));
      Update();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::WeatherManager::OnTimeSeasonChanged(int)
{
   if (mOperateOn.valid())
   {
      dtABC::Weather::TimePeriod time = dtABC::Weather::TimePeriod(mUI->weatherTimeCombo->currentIndex());
      dtABC::Weather::Season season = dtABC::Weather::Season(mUI->weatherSeasonCombo->currentIndex());

      mOperateOn->SetTimePeriodAndSeason(time, season);
      Update();
   }
}
