#ifndef Viewer_h__
#define Viewer_h__

#include <dtABC/application.h>
#include <QObject>
#include <dtCore/system.h>
#include <QtCore/QBasicTimer>

namespace dtCore
{
   class OrbitMotionModel;
   class RefPtr;
}


namespace dtChar
{
   class CharDrawable;
   class CoreModel;
}

class Viewer : public QObject, public dtABC::Application
{
   Q_OBJECT

public:
	Viewer();
	~Viewer();

   virtual void Config();

public slots:
   void OnLoadCharFile( const QString &filename );
   void OnStartAnimation( unsigned int id, float weight, float delay );
   void OnStopAnimation( unsigned int id, float delay );
   void OnStartAction( unsigned int id, float delayIn, float delayOut );

signals:
   void OnAnimationLoaded( unsigned int, const QString & );

protected:
   virtual void timerEvent(QTimerEvent *event)
   {
      dtCore::System::GetInstance().StepWindow();
   }


private:
   QBasicTimer mTimer;
   dtCore::RefPtr<dtChar::CoreModel> mCoreModel;
   dtCore::RefPtr<dtChar::CharDrawable> mCharacter;
   dtCore::RefPtr<dtCore::OrbitMotionModel> mMotion;
};

#endif // Viewer_h__