#include <dtInspectorQt/baseview.h>
#include "ui_dtinspectorqt.h"


//////////////////////////////////////////////////////////////////////////
dtInspectorQt::BaseView::BaseView(Ui::InspectorWidget& ui)
:mUI(&ui)
{
   connect(ui.baseNameText, SIGNAL(textEdited(const QString&)), this, SLOT(OnNameChange(const QString&)));
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::BaseView::~BaseView()
{
   mOperateOn = NULL;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::BaseView::Update()
{
   if (mOperateOn.valid())
   {
      mUI->baseNameText->setText(QString::fromStdString(mOperateOn->GetName()));
      mUI->baseRefCountLabel->setText(QString::number(mOperateOn->referenceCount()));
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::BaseView::OperateOn(dtCore::Base* b)
{
   mOperateOn = b;
   Update();
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::BaseView::OnNameChange(const QString& text)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetName(text.toStdString());
      emit NameChanged(text);
   }
}
