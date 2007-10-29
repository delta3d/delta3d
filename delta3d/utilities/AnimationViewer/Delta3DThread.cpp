#include "Delta3DThread.h"
#include "MainWindow.h"
#include "Viewer.h"
#include <dtCore/system.h>

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

   connect(mWin, SIGNAL(FileToLoad(const QString&)), viewer.get(), SLOT(OnLoadCharFile(const QString&)) );

   connect(viewer.get(), SIGNAL(AnimationLoaded(unsigned int,const QString &,unsigned int,unsigned int,float)),
                    mWin, SLOT(OnNewAnimation(unsigned int,const QString &,unsigned int,unsigned int,float)));

   connect(viewer.get(), SIGNAL(MeshLoaded(int,const QString&)), mWin, SLOT(OnNewMesh(int,const QString&)));

   connect(viewer.get(), SIGNAL(MaterialLoaded(int,const QString&,const QColor&,const QColor&,const QColor&,float )), 
           mWin, SLOT(OnNewMaterial(int,const QString&,const QColor&,const QColor&,const QColor&,float)));

   connect(mWin, SIGNAL(AttachMesh(int)), viewer.get(), SLOT(OnAttachMesh(int)));
   connect(mWin, SIGNAL(DetachMesh(int)), viewer.get(), SLOT(OnDetachMesh(int)));

   connect(viewer.get(), SIGNAL(ErrorOccured(const QString&)), mWin, SLOT(OnDisplayError(const QString&)) );

   connect(mWin, SIGNAL(StartAnimation(unsigned int,float,float)), viewer.get(), SLOT(OnStartAnimation(unsigned int,float,float)));
   connect(mWin, SIGNAL(StopAnimation(unsigned int,float)), viewer.get(), SLOT(OnStopAnimation(unsigned int,float)));
   connect(mWin, SIGNAL(StartAction(unsigned int,float,float)), viewer.get(), SLOT(OnStartAction(unsigned int,float,float)));
   connect(mWin, SIGNAL(LOD_Changed(float)), viewer.get(), SLOT(OnLOD_Changed(float)));
   connect(mWin, SIGNAL(SpeedChanged(float)), viewer.get(), SLOT(OnSpeedChanged(float)));

   connect((QObject*)mWin->mShadedAction, SIGNAL(triggered()), viewer.get(), SLOT(OnSetShaded()));
   connect((QObject*)mWin->mWireframeAction, SIGNAL(triggered()), viewer.get(), SLOT(OnSetWireframe()));
   connect((QObject*)mWin->mShadedWireAction, SIGNAL(triggered()), viewer.get(), SLOT(OnSetShadedWireframe()));


   dtCore::System::GetInstance().Start();

   this->exec();
}
