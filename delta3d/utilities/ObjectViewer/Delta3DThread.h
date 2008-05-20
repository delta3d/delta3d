#ifndef Delta3DThread_h__
#define Delta3DThread_h__

#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <dtCore/refptr.h>

class ObjectWorkspace;
class ObjectViewer;

class Delta3DThread : public QThread
{
   Q_OBJECT
public:
	Delta3DThread(QObject *parent=0);
	~Delta3DThread();
   void SetObjectWorkspace(ObjectWorkspace *win) { mWin = win;}
   virtual void run();
	
   dtCore::RefPtr<ObjectViewer> mViewer;
   
protected:

private:
   QTimer mTimer;
   ObjectWorkspace *mWin;
};
#endif // Delta3DThread_h__
