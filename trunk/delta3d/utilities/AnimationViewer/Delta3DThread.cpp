#include <dtCore/system.h>
#include "Delta3DThread.h"
#include "Viewer.h"
#include "MainWindow.h"

Delta3DThread::Delta3DThread(QObject *parent):
QThread(parent)
{

}

Delta3DThread::~Delta3DThread()
{
   if (isRunning())
   {
      this->quit();
   }
   dtCore::System::GetInstance().Stop();
}

void Delta3DThread::run()
{
   dtCore::RefPtr<Viewer> viewer;
   viewer = new Viewer();
   viewer->Config();

   QObject::connect(mWin, SIGNAL(FileToLoad(const QString&)), viewer.get(), SLOT(OnLoadCharFile(const QString&)) );
   QObject::connect(viewer.get(), SIGNAL(OnAnimationLoaded(unsigned int,const QString &)), mWin, SLOT(OnNewAnimation(unsigned int,const QString &)));

   connect(mWin, SIGNAL(StartAnimation(unsigned int,float,float)), viewer.get(), SLOT(OnStartAnimation(unsigned int,float,float)));
   connect(mWin, SIGNAL(StopAnimation(unsigned int,float)), viewer.get(), SLOT(OnStopAnimation(unsigned int,float)));
   connect(mWin, SIGNAL(StartAction(unsigned int,float,float)), viewer.get(), SLOT(OnStartAction(unsigned int,float,float)));
   connect(mWin, SIGNAL(LOD_Changed(float)), viewer.get(), SLOT(OnLOD_Changed(float)));

   dtCore::System::GetInstance().Start();

   this->exec();
}