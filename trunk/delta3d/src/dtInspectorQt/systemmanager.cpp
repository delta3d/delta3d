#include <dtInspectorQt/systemmanager.h>
#include "ui_dtinspectorqt.h"

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::SystemManager::SystemManager(Ui::InspectorWidget& ui)
:mUI(&ui)
{
   connect(mUI->sysPausedToggle, SIGNAL(stateChanged(int)), this, SLOT(OnPaused(int)));
   connect(mUI->sysTimeScaleEdit, SIGNAL(valueChanged(double)), this, SLOT(OnTimeScaleChanged(double)));
   connect(mUI->sysFixedTimeStepToggle, SIGNAL(stateChanged(int)), this, SLOT(OnFixedTimeStepsToggled(int)));
   connect(mUI->sysFrameRateEdit, SIGNAL(valueChanged(double)), this, SLOT(OnSetFrameRateChanged(double)));
   connect(mUI->sysMaxDrawEdit, SIGNAL(valueChanged(double)), this, SLOT(OnMaxBetweenDrawsChanged(double)));
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::SystemManager::~SystemManager()
{
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::SystemManager::OperateOn(dtCore::Base* b)
{
   dtCore::System *system = dynamic_cast<dtCore::System*>(b);

   mOperateOn = system;
   Update();
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::SystemManager::OnPaused(int checked)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetPause(checked ? true : false);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::SystemManager::OnTimeScaleChanged(double newValue)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetTimeScale(newValue);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::SystemManager::OnFixedTimeStepsToggled(int checked)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetUseFixedTimeStep(checked ? true : false);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::SystemManager::OnSetFrameRateChanged(double newValue)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetFrameRate(newValue);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::SystemManager::OnMaxBetweenDrawsChanged(double newValue)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetMaxTimeBetweenDraws(newValue);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::SystemManager::Update()
{
   if (mOperateOn.valid())
   {
      mUI->sysGroupBox->show();

      mUI->sysRunningToggle->setChecked(mOperateOn->IsRunning());
      mUI->sysPausedToggle->setChecked(mOperateOn->GetPause());
      mUI->sysTimeScaleEdit->setValue(mOperateOn->GetTimeScale());
      mUI->sysFixedTimeStepToggle->setChecked(mOperateOn->GetUsesFixedTimeStep());
      mUI->sysFrameRateEdit->setValue(mOperateOn->GetFrameRate());
      mUI->sysMaxDrawEdit->setValue(mOperateOn->GetMaxTimeBetweenDraws());
   }
   else
   {
      mUI->sysGroupBox->hide();
   }
}

//////////////////////////////////////////////////////////////////////////

