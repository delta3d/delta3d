#ifndef Delta3DThread_h__
#define Delta3DThread_h__

#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <dtCore/refptr.h>

class ShaderWorkspace;
class ShaderViewer;

class Delta3DThread : public QThread
{
   Q_OBJECT
public:
	Delta3DThread(QObject *parent=0);
	~Delta3DThread();
   void SetShaderWorkspace(ShaderWorkspace *win) { mWin = win;}
   virtual void run();
	
   dtCore::RefPtr<ShaderViewer> mViewer;
   
protected:

private:
   QTimer mTimer;
   ShaderWorkspace *mWin;
};
#endif // Delta3DThread_h__
