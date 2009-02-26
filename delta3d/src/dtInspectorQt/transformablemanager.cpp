#include <dtInspectorQt/transformablemanager.h>
#include <dtCore/odegeomwrap.h>

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::TransformableManager::TransformableManager(Ui::InspectorWidget &ui)
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
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::TransformableManager::~TransformableManager()
{
   mOperateOn = NULL;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::TransformableManager::OperateOn(dtCore::Base* b)
{
   dtCore::Transformable *trans = dynamic_cast<dtCore::Transformable*>(b);
   
   mOperateOn = trans;
   Update();
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::TransformableManager::Update()
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
   }
   else
   {
      mUI->transformableGroupBox->hide();
   } 
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::TransformableManager::OnXYZHPRChanged(double val)
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
void dtInspectorQt::TransformableManager::OnCollisionDetection(int checked)
{
   if (mOperateOn.valid())
   {
      mOperateOn->GetGeomWrapper()->SetCollisionDetection(checked ? true : false);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::TransformableManager::OnRenderCollision(int checked)
{
   if (mOperateOn.valid())
   {
      mOperateOn->RenderCollisionGeometry(checked ? true : false);
   }
}
