#ifndef __DELTA3DTHREAD_H__
#define __DELTA3DTHREAD_H__

#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <dtCore/refptr.h>

class MainWindow;
class Viewer;

class Delta3DThread : public QThread
{
   Q_OBJECT
public:
   Delta3DThread(QObject* parent=0);
   ~Delta3DThread();
   void SetMainWindow(MainWindow* win) { mWin = win; }
   virtual void run();

   dtCore::RefPtr<Viewer> mViewer;

protected:

private:

   QTimer mTimer;
   MainWindow* mWin;

};
#endif // __DELTA3DTHREAD_H__
