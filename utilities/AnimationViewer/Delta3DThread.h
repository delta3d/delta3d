#ifndef Delta3DThread_h__
#define Delta3DThread_h__

#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <dtCore/refptr.h>

class MainWindow;
class Viewer;

class Delta3DThread : public QThread
{
   Q_OBJECT
public:
	Delta3DThread(QObject *parent=0);
	~Delta3DThread();
   void SetMainWindow(MainWindow *win) { mWin = win;}
   virtual void run();
	
   dtCore::RefPtr<Viewer> mViewer;
   
protected:

private:
   QTimer mTimer;
   MainWindow *mWin;
};
#endif // Delta3DThread_h__
