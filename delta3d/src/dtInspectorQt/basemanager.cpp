#include <dtInspectorQt/basemanager.h>

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::BaseManager::BaseManager(Ui::InspectorWidget& ui)
:mUI(&ui)
{
   connect(ui.baseNameText, SIGNAL(textEdited(const QString&)), this, SLOT(OnNameChange(const QString&)));
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::BaseManager::~BaseManager()
{
   mOperateOn = NULL;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::BaseManager::Update()
{
   if (mOperateOn.valid())
   {
      mUI->baseNameText->setText(QString::fromStdString(mOperateOn->GetName()));
      mUI->baseRefCountLabel->setText(QString::number(mOperateOn->referenceCount()));
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::BaseManager::OperateOn(dtCore::Base* b)
{
   mOperateOn = b;
   Update();
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::BaseManager::OnNameChange(const QString& text)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetName(text.toStdString());
      emit NameChanged(text);
   }
}
