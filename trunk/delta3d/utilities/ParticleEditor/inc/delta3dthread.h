#ifndef __DELTA3DTHREAD_H__
#define __DELTA3DTHREAD_H__

#include <particleviewer.h>
#include <mainwindow.h>
#include <viewwindow.h>

#include <QtOpenGL/QGLContext>
#include <QtCore/QThread>
#include <QtCore/QTimer>

#include <dtCore/refptr.h>
#include <dtCore/system.h>
#include <dtCore/deltawin.h>

class Delta3DThread : public QThread
{
   Q_OBJECT
public:
   Delta3DThread(QObject* parent=0);
   ~Delta3DThread();
   void SetMainWindow(psEditor::MainWindow* win) { mWin = win; }
   virtual void run();

   ParticleViewer* mpViewer;

private:
   QTimer mTimer;
   psEditor::MainWindow* mWin;

};
#endif // __DELTA3DTHREAD_H__
