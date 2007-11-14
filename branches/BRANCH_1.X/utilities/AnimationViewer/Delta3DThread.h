#ifndef Delta3DThread_h__
#define Delta3DThread_h__

#include <QtCore/QThread>
#include <dtCore/refptr.h>

class MainWindow;

class Delta3DThread : public QThread
{
   Q_OBJECT
public:
	Delta3DThread(QObject *parent=0);
	~Delta3DThread();
   void SetMainWindow(MainWindow *win) { mWin = win;}
	
protected:
   virtual void run();

private:
   MainWindow *mWin;   
};
#endif // Delta3DThread_h__
