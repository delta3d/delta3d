#include <QtGui/QApplication>
#include <dtInspectorQt/inspectorqt.h>
#include <dtInspectorQt/inspectorwindow.h>
#include <dtInspectorQt/iview.h>
#include <dtCore/system.h>

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::InspectorQt::InspectorQt(int& argc, char **argv)
{
   mApp = new QApplication(argc, argv);

   mInspector = new InspectorWindow();
   mInspector->show();

   AddSender(&dtCore::System::GetInstance());
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::InspectorQt::~InspectorQt()
{
   mInspector->hide();
   mApp->exit();
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InspectorQt::OnMessage(MessageData* data)
{
   if (data->message == dtCore::System::MESSAGE_POST_FRAME)
   {
      mApp->processEvents();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InspectorQt::AddCustomView(IView* customView)
{
   mInspector->AddCustomView(customView);
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InspectorQt::Show()
{
   SetVisible(true);
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InspectorQt::Hide()
{
   SetVisible(false);
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InspectorQt::SetVisible(bool isVisible)
{
   mInspector->setVisible(isVisible);

   // Refresh the inspector if we're showing it
   if (isVisible)
   {
      mInspector->UpdateInstances();
      mInspector->RefreshCurrentItem();
   }
}

//////////////////////////////////////////////////////////////////////////

