#include <dtInspectorQt/deltawinview.h>
#include "ui_dtinspectorqt.h"

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::DeltaWinView::DeltaWinView(Ui::InspectorWidget& ui)
:mUI(&ui)
{
   mFilterName = QString("dtCore::DeltaWin");

   connect(mUI->winFullscreenToggle, SIGNAL(stateChanged(int)), this, SLOT(OnFullScreenToggle(int)));
   connect(mUI->winCursorToggle, SIGNAL(stateChanged(int)), this, SLOT(OnCursorToggle(int)));
   connect(mUI->winTitleEdit, SIGNAL(textChanged(const QString&)), this, SLOT(OnTitleChanged(const QString&)));
   connect(mUI->winXEdit, SIGNAL(editingFinished()), this, SLOT(OnPositionChanged()));
   connect(mUI->winYEdit, SIGNAL(editingFinished()), this, SLOT(OnPositionChanged()));
   connect(mUI->winWidthEdit, SIGNAL(editingFinished()), this, SLOT(OnPositionChanged()));
   connect(mUI->winHeightEdit, SIGNAL(editingFinished()), this, SLOT(OnPositionChanged()));
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::DeltaWinView::~DeltaWinView()
{
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DeltaWinView::OperateOn(dtCore::Base* b)
{
   dtCore::DeltaWin *deltawin = dynamic_cast<dtCore::DeltaWin*>(b);

   mOperateOn = deltawin;
   Update();
}

//////////////////////////////////////////////////////////////////////////
bool dtInspectorQt::DeltaWinView::IsOfType(QString name, dtCore::Base* object)
{
   if (name == mFilterName && dynamic_cast<dtCore::DeltaWin*>(object) != NULL)
   {
      return true;
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DeltaWinView::OnFullScreenToggle(int checked)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetFullScreenMode(checked ? true : false);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DeltaWinView::OnCursorToggle(int checked)
{
   if (mOperateOn.valid())
   {
      mOperateOn->ShowCursor(checked ? true : false);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DeltaWinView::OnTitleChanged(const QString& title)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetWindowTitle(title.toStdString());
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DeltaWinView::OnPositionChanged()
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
void dtInspectorQt::DeltaWinView::Update()
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

