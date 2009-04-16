#include <dtInspectorQt/systemview.h>
#include "ui_dtinspectorqt.h"

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::SystemView::SystemView(Ui::InspectorWidget& ui)
:mUI(&ui)
{
   mFilterName = QString("dtCore::System");

   connect(mUI->sysPausedToggle, SIGNAL(stateChanged(int)), this, SLOT(OnPaused(int)));
   connect(mUI->sysTimeScaleEdit, SIGNAL(valueChanged(double)), this, SLOT(OnTimeScaleChanged(double)));
   connect(mUI->sysFixedTimeStepToggle, SIGNAL(stateChanged(int)), this, SLOT(OnFixedTimeStepsToggled(int)));
   connect(mUI->sysFrameRateEdit, SIGNAL(valueChanged(double)), this, SLOT(OnSetFrameRateChanged(double)));
   connect(mUI->sysMaxDrawEdit, SIGNAL(valueChanged(double)), this, SLOT(OnMaxBetweenDrawsChanged(double)));
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::SystemView::~SystemView()
{
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::SystemView::OperateOn(dtCore::Base* b)
{
   dtCore::System *system = dynamic_cast<dtCore::System*>(b);

   mOperateOn = system;
   Update();
}

//////////////////////////////////////////////////////////////////////////
bool dtInspectorQt::SystemView::IsOfType(QString name, dtCore::Base* object)
{
   if (name == mFilterName && dynamic_cast<dtCore::System*>(object) != NULL)
   {
      return true;
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::SystemView::OnPaused(int checked)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetPause(checked ? true : false);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::SystemView::OnTimeScaleChanged(double newValue)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetTimeScale(newValue);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::SystemView::OnFixedTimeStepsToggled(int checked)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetUseFixedTimeStep(checked ? true : false);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::SystemView::OnSetFrameRateChanged(double newValue)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetFrameRate(newValue);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::SystemView::OnMaxBetweenDrawsChanged(double newValue)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetMaxTimeBetweenDraws(newValue);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::SystemView::Update()
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

