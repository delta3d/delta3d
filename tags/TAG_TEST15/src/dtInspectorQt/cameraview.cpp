#include <dtInspectorQt/cameraview.h>
#include <dtInspectorQt/transformableview.h>
#include "ui_dtinspectorqt.h"

#include <dtABC/application.h>

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::CameraView::CameraView(Ui::InspectorWidget& ui)
:mUI(&ui)
{
   mFilterName = QString("Camera");

   connect(mUI->baseNameText, SIGNAL(textEdited(const QString&)), this, SLOT(OnNameChange(const QString&)));
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
   TransformableView::GetInstance(*mUI).Destroy();
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::CameraView::Build(QList<EntryData> &itemList)
{
   if (dtABC::Application::GetInstanceCount() == 0)
   {
      return;
   }

   dtABC::Application* app = dtABC::Application::GetInstance(0);
   if (app)
   {
      dtCore::Camera* camera = app->GetCamera();
      if (camera)
      {
         EntryData data;
         data.name = camera->GetName().c_str();
         data.type = "Application Camera";
         data.itemData = QVariant(camera->GetUniqueId().ToString().c_str());
         itemList.push_back(data);
      }
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::CameraView::OperateOn(const QVariant& itemData)
{
   if (dtABC::Application::GetInstanceCount() == 0)
   {
      return;
   }

   mOperateOn = NULL;

   dtABC::Application* app = dtABC::Application::GetInstance(0);
   if (app)
   {
      dtCore::Camera* camera = app->GetCamera();
      if (camera && camera->GetUniqueId().ToString() == itemData.toString().toStdString())
      {
         mOperateOn = camera;
      }
   }

   Update();
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::CameraView::Update()
{
   dtInspectorQt::TransformableView::GetInstance(*mUI).OperateOn(mOperateOn.get());

   if (mOperateOn.valid())
   {
      mUI->baseGroupBox->show();
      mUI->baseNameText->setText(QString::fromStdString(mOperateOn->GetName()));
      mUI->baseRefCountLabel->setText(QString::number(mOperateOn->referenceCount()));

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
void dtInspectorQt::CameraView::OnNameChange(const QString& text)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetName(text.toStdString());
      emit NameChanged(text);
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
