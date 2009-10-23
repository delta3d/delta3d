#include <dtEditQt/stageapplication.h>

////////////////////////////////////////////////////////////////////////////////
dtEditQt::STAGEApplication::STAGEApplication(dtCore::DeltaWin* win):
dtABC::Application("", win)
{

}

////////////////////////////////////////////////////////////////////////////////
dtEditQt::STAGEApplication::~STAGEApplication()
{

}

////////////////////////////////////////////////////////////////////////////////
void dtEditQt::STAGEApplication::Config()
{
   dtABC::Application::Config();

   //remove the default View, DeltaWin, and Camera
   this->SetWindow(NULL);
   this->SetCamera(NULL);

   this->RemoveView(*this->GetView(), true);
}
