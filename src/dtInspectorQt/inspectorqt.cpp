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

   dtCore::System::GetInstance().TickSignal.connect_slot(this, &InspectorQt::OnSystem);
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::InspectorQt::~InspectorQt()
{
   dtCore::System::GetInstance().TickSignal.disconnect(this);


   mInspector->hide();
   mApp->exit();

   delete mInspector;
   mInspector = NULL;

   delete mApp;
   mApp = NULL;
}

/////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InspectorQt::OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)
{
   if (str == dtCore::System::MESSAGE_POST_FRAME)
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
void dtInspectorQt::InspectorQt::SetGameManager(dtGame::GameManager* gm)
{
   mInspector->SetGameManager(gm);
}

//////////////////////////////////////////////////////////////////////////

