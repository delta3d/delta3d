#include <dtInspectorQt/drawablemanager.h>

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::DrawableManager::DrawableManager(Ui::InspectorWidget& ui)
:mUI(&ui)
{

}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::DrawableManager::~DrawableManager()
{
   mOperateOn = NULL;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DrawableManager::OperateOn(dtCore::Base* b)
{
   dtCore::DeltaDrawable* drawable = dynamic_cast<dtCore::DeltaDrawable*>(b);

   mOperateOn = drawable;
   Update();
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DrawableManager::Update()
{
   if (mOperateOn.valid())
   {
      mUI->drawableGroupBox->show();

      //children
      mUI->drawableChildList->clear();
      for (unsigned int childIdx=0; childIdx<mOperateOn->GetNumChildren(); childIdx++)
      {
         dtCore::RefPtr<dtCore::DeltaDrawable> child = mOperateOn->GetChild(childIdx);
         mUI->drawableChildList->addItem(QString::fromStdString(child->GetName()));
      }

      //parent
      std::string parentName;
      if (mOperateOn->GetParent() != NULL)
      {
         parentName = mOperateOn->GetParent()->GetName();
      }
      mUI->drawableParentLabel->setText(QString::fromStdString(parentName));

   }
   else
   {
      mUI->drawableGroupBox->hide();
   }
}
