#include <dtInspectorQt/deltawinmanager.h>
#include "ui_dtinspectorqt.h"

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::DeltaWinManager::DeltaWinManager(Ui::InspectorWidget& ui)
:mUI(&ui)
{
   connect(mUI->winFullscreenToggle, SIGNAL(stateChanged(int)), this, SLOT(OnFullScreenToggle(int)));
   connect(mUI->winCursorToggle, SIGNAL(stateChanged(int)), this, SLOT(OnCursorToggle(int)));
   connect(mUI->winTitleEdit, SIGNAL(textChanged(const QString&)), this, SLOT(OnTitleChanged(const QString&)));
   connect(mUI->winXEdit, SIGNAL(editingFinished()), this, SLOT(OnPositionChanged()));
   connect(mUI->winYEdit, SIGNAL(editingFinished()), this, SLOT(OnPositionChanged()));
   connect(mUI->winWidthEdit, SIGNAL(editingFinished()), this, SLOT(OnPositionChanged()));
   connect(mUI->winHeightEdit, SIGNAL(editingFinished()), this, SLOT(OnPositionChanged()));
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::DeltaWinManager::~DeltaWinManager()
{

}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DeltaWinManager::OperateOn(dtCore::Base* b)
{
   dtCore::DeltaWin *deltawin = dynamic_cast<dtCore::DeltaWin*>(b);

   mOperateOn = deltawin;
   Update();
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DeltaWinManager::OnFullScreenToggle(int checked)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetFullScreenMode(checked);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DeltaWinManager::OnCursorToggle(int checked)
{
   if (mOperateOn.valid())
   {
      mOperateOn->ShowCursor(checked);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DeltaWinManager::OnTitleChanged(const QString& title)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetWindowTitle(title.toStdString());
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DeltaWinManager::OnPositionChanged()
{
   if (mOperateOn.valid())
   {
      int x = mUI->winXEdit->value();
      int y = mUI->winYEdit->value();
      int width = mUI->winWidthEdit->value();
      int height = mUI->winHeightEdit->value();
      mOperateOn->SetPosition(x, y, width, height);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DeltaWinManager::Update()
{
   if (mOperateOn.valid())
   {
      mUI->winGroupBox->show();

      mUI->winFullscreenToggle->setChecked(mOperateOn->GetFullScreenMode());
      mUI->winCursorToggle->setChecked(mOperateOn->GetShowCursor());

      mUI->winTitleEdit->setText(mOperateOn->GetWindowTitle().c_str());

      int x,y,width,height;
      mOperateOn->GetPosition(x, y, width, height);
      mUI->winXEdit->setValue(x);
      mUI->winYEdit->setValue(y);
      mUI->winWidthEdit->setValue(width);
      mUI->winHeightEdit->setValue(height);
   }
   else
   {
      mUI->winGroupBox->hide();
   }
}

//////////////////////////////////////////////////////////////////////////

