#include <dtInspectorQt/cameraview.h>
#include "ui_dtinspectorqt.h"

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::CameraView::CameraView(Ui::InspectorWidget& ui)
:mUI(&ui)
{
   mFilterName = QString("dtCore::Camera");

   connect(mUI->cameraEnabledToggle, SIGNAL(stateChanged(int)), this, SLOT(OnEnabled(int)));
   connect(mUI->cameraVertEdit, SIGNAL(valueChanged(double)), this, SLOT(OnPerspectiveChanged(double)));
   connect(mUI->cameraAspectEdit, SIGNAL(valueChanged(double)), this, SLOT(OnPerspectiveChanged(double)));
   connect(mUI->cameraNearClipEdit, SIGNAL(valueChanged(double)), this, SLOT(OnPerspectiveChanged(double)));
   connect(mUI->cameraFarClipEdit, SIGNAL(valueChanged(double)), this, SLOT(OnPerspectiveChanged(double)));
   connect(mUI->cameraRedClearEdit, SIGNAL(valueChanged(double)), this, SLOT(OnClearColorChanged(double)));
   connect(mUI->cameraGreenClearEdit, SIGNAL(valueChanged(double)), this, SLOT(OnClearColorChanged(double)));
   connect(mUI->cameraBlueClearEdit, SIGNAL(valueChanged(double)), this, SLOT(OnClearColorChanged(double)));
   connect(mUI->cameraAlphaClearEdit, SIGNAL(valueChanged(double)), this, SLOT(OnClearColorChanged(double)));
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::CameraView::~CameraView()
{
   mOperateOn = NULL;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::CameraView::OperateOn(dtCore::Base* b)
{
   dtCore::Camera *camera = dynamic_cast<dtCore::Camera*>(b);

   mOperateOn = camera;
   Update();
}

//////////////////////////////////////////////////////////////////////////
bool dtInspectorQt::CameraView::IsOfType(QString name, dtCore::Base* object)
{
   if (name == mFilterName && dynamic_cast<dtCore::Camera*>(object) != NULL)
   {
      return true;
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::CameraView::Update()
{
   if (mOperateOn.valid())
   {
      mUI->cameraGroupBox->show();

      mUI->cameraEnabledToggle->setChecked(mOperateOn->GetEnabled());

      double vfov, ar, nearClip, farClip;
      mOperateOn->GetPerspectiveParams(vfov, ar, nearClip, farClip);
      mUI->cameraVertEdit->setValue(vfov);
      mUI->cameraAspectEdit->setValue(ar);
      mUI->cameraNearClipEdit->setValue(nearClip);
      mUI->cameraFarClipEdit->setValue(farClip);

      float red, green, blue, alpha;
      mOperateOn->GetClearColor(red, green, blue, alpha);
      mUI->cameraRedClearEdit->setValue(red);
      mUI->cameraGreenClearEdit->setValue(green);
      mUI->cameraBlueClearEdit->setValue(blue);
      mUI->cameraAlphaClearEdit->setValue(alpha);
   }
   else
   {
      mUI->cameraGroupBox->hide();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::CameraView::OnEnabled(int state)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetEnabled(state ? true : false);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::CameraView::OnPerspectiveChanged(double value)
{
   if (mOperateOn.valid())
   {
      double aspectRatio = mUI->cameraAspectEdit->value();
      double vertFOV = mUI->cameraVertEdit->value();
      double nearClip = mUI->cameraNearClipEdit->value();
      double farClip = mUI->cameraFarClipEdit->value();
      mOperateOn->SetPerspectiveParams(vertFOV, aspectRatio, nearClip, farClip); //near/far auto calculated
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::CameraView::OnClearColorChanged(double value)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetClearColor(mUI->cameraRedClearEdit->value(), mUI->cameraGreenClearEdit->value(),
         mUI->cameraBlueClearEdit->value(), mUI->cameraAlphaClearEdit->value());
   }
}

//////////////////////////////////////////////////////////////////////////
