#include <dtInspectorQt/transformableview.h>
#include <dtCore/transform.h>
#include <dtCore/odegeomwrap.h>
#include "ui_dtinspectorqt.h"

//static
dtInspectorQt::TransformableView* dtInspectorQt::TransformableView::mInstance(NULL);

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::TransformableView::TransformableView(Ui::InspectorWidget &ui)
:mUI(&ui)
{

   connect(mUI->transXEdit, SIGNAL(valueChanged(double)), this, SLOT(OnXYZHPRChanged(double)));
   connect(mUI->transYEdit, SIGNAL(valueChanged(double)), this, SLOT(OnXYZHPRChanged(double)));
   connect(mUI->transZEdit, SIGNAL(valueChanged(double)), this, SLOT(OnXYZHPRChanged(double)));
   connect(mUI->transHEdit, SIGNAL(valueChanged(double)), this, SLOT(OnXYZHPRChanged(double)));
   connect(mUI->transPEdit, SIGNAL(valueChanged(double)), this, SLOT(OnXYZHPRChanged(double)));
   connect(mUI->transREdit, SIGNAL(valueChanged(double)), this, SLOT(OnXYZHPRChanged(double)));
   connect(mUI->transABSRadioButton, SIGNAL(clicked()), this, SLOT(Update()));
   connect(mUI->transRELRadioButton, SIGNAL(clicked()), this, SLOT(Update()));
   connect(mUI->transCollisionToggle, SIGNAL(stateChanged(int)), this, SLOT(OnCollisionDetection(int)));
   connect(mUI->transRenderToggle, SIGNAL(stateChanged(int)), this, SLOT(OnRenderCollision(int)));
   connect(mUI->transCategoryEdit, SIGNAL(textChanged(const QString&)), this, SLOT(OnCategoryBits(const QString&)));
   connect(mUI->transCollideEdit, SIGNAL(textChanged(const QString&)), this, SLOT(OnCollideBits(const QString&)));
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::TransformableView::~TransformableView()
{
   mOperateOn = NULL;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::TransformableView::OperateOn(dtCore::Transformable* trans)
{
   mOperateOn = trans;
   Update();
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::TransformableView::Update()
{
   if (mOperateOn.valid())
   {
      mUI->transformableGroupBox->show();

      dtCore::Transform xform;
      if (mUI->transABSRadioButton->isChecked())
      {
         mOperateOn->GetTransform(xform, dtCore::Transformable::ABS_CS);
      }
      else
      {
         mOperateOn->GetTransform(xform, dtCore::Transformable::REL_CS);
      }

      const osg::Vec3 xyz = xform.GetTranslation();
      mUI->transXEdit->setValue(xyz[0]);
      mUI->transYEdit->setValue(xyz[1]);
      mUI->transZEdit->setValue(xyz[2]);

      osg::Vec3 hpr;
      xform.GetRotation(hpr);
      mUI->transHEdit->setValue(hpr[0]);
      mUI->transPEdit->setValue(hpr[1]);
      mUI->transREdit->setValue(hpr[2]);

      //collision detection
      mUI->transCollisionToggle->setChecked(mOperateOn->GetGeomWrapper()->GetCollisionDetection());
      mUI->transRenderToggle->setChecked(mOperateOn->GetRenderCollisionGeometry());

      //bits
      mUI->transCategoryEdit->setText(QString::number(mOperateOn->GetGeomWrapper()->GetCollisionCategoryBits()));
      mUI->transCollideEdit->setText(QString::number(mOperateOn->GetGeomWrapper()->GetCollisionCollideBits()));
   }
   else
   {
      mUI->transformableGroupBox->hide();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::TransformableView::OnXYZHPRChanged(double val)
{
   if (mOperateOn.valid())
   {
      dtCore::Transform xform(mUI->transXEdit->value(),
                              mUI->transYEdit->value(),
                              mUI->transZEdit->value(),
                              mUI->transHEdit->value(),
                              mUI->transPEdit->value(),
                              mUI->transREdit->value());

      if (mUI->transABSRadioButton->isChecked())
      {
         mOperateOn->SetTransform(xform, dtCore::Transformable::ABS_CS);
      }
      else
      {
         mOperateOn->SetTransform(xform, dtCore::Transformable::REL_CS);
      }
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::TransformableView::OnCollisionDetection(int checked)
{
   if (mOperateOn.valid())
   {
      mOperateOn->GetGeomWrapper()->SetCollisionDetection(checked ? true : false);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::TransformableView::OnRenderCollision(int checked)
{
   if (mOperateOn.valid())
   {
      mOperateOn->RenderCollisionGeometry(checked ? true : false);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::TransformableView::OnCategoryBits(const QString& text)
{
   if (mOperateOn.valid())
   {
      mOperateOn->GetGeomWrapper()->SetCollisionCategoryBits(text.toULong());
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::TransformableView::OnCollideBits(const QString& text)
{
   if (mOperateOn.valid())
   {
      mOperateOn->GetGeomWrapper()->SetCollisionCollideBits(text.toULong());
   }
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::TransformableView& dtInspectorQt::TransformableView::GetInstance(Ui::InspectorWidget& ui)
{
   if (mInstance == NULL)
   {
      mInstance = new TransformableView(ui);
   }

   return *mInstance;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::TransformableView::Destroy()
{
   if (mInstance)
   {
      delete mInstance;
      mInstance = NULL;
   }
}
