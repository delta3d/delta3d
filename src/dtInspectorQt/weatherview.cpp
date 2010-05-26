#include <dtInspectorQt/weatherview.h>
#include "ui_dtinspectorqt.h"
#include <dtCore/environment.h>

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::WeatherView::WeatherView(Ui::InspectorWidget& ui)
:mUI(&ui)
{
   mFilterName = QString("dtABC::Weather");

   connect(mUI->weatherThemeCombo, SIGNAL(activated(int)), this, SLOT(OnThemeChanged(int)));
   connect(mUI->weatherCloudCombo, SIGNAL(activated(int)), this, SLOT(OnCloudTypeChanged(int)));
   connect(mUI->weatherWindCombo, SIGNAL(activated(int)), this, SLOT(OnWindTypeChanged(int)));
   connect(mUI->weatherVisibilityCombo, SIGNAL(activated(int)), this, SLOT(OnVisibilityTypeChanged(int)));
   connect(mUI->weatherTimeCombo, SIGNAL(activated(int)), this, SLOT(OnTimeSeasonChanged(int)));
   connect(mUI->weatherSeasonCombo, SIGNAL(activated(int)), this, SLOT(OnTimeSeasonChanged(int)));
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::WeatherView::~WeatherView()
{
   mOperateOn = NULL;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::WeatherView::OperateOn(dtCore::Base* b)
{
   dtABC::Weather *weather = dynamic_cast<dtABC::Weather*>(b);

   mOperateOn = weather;
   Update();
}

//////////////////////////////////////////////////////////////////////////
bool dtInspectorQt::WeatherView::IsOfType(QString name, dtCore::Base* object)
{
   if (name == mFilterName && dynamic_cast<dtABC::Weather*>(object) != NULL)
   {
      return true;
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::WeatherView::Update()
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
void dtInspectorQt::WeatherView::OnThemeChanged(int index)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetTheme(dtABC::Weather::WeatherTheme(index));
      Update();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::WeatherView::OnCloudTypeChanged(int index)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetBasicCloudType(dtABC::Weather::CloudType(index));
      Update();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::WeatherView::OnWindTypeChanged(int index)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetBasicWindType(dtABC::Weather::WindType(index));
      Update();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::WeatherView::OnVisibilityTypeChanged(int index)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetBasicVisibilityType(dtABC::Weather::VisibilityType(index));
      Update();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::WeatherView::OnTimeSeasonChanged(int)
{
   if (mOperateOn.valid())
   {
      dtABC::Weather::TimePeriod time = dtABC::Weather::TimePeriod(mUI->weatherTimeCombo->currentIndex());
      dtABC::Weather::Season season = dtABC::Weather::Season(mUI->weatherSeasonCombo->currentIndex());

      mOperateOn->SetTimePeriodAndSeason(time, season);
      Update();
   }
}
