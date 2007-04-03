#ifndef Viewer_h__
#define Viewer_h__

#include <dtABC/application.h>
#include <QObject>
#include <dtCore/system.h>
#include <QtCore/QBasicTimer>
#include <vector>

namespace dtCore
{
   class OrbitMotionModel;
   class RefPtr;
}

namespace osg
{
   class Group;
}


namespace dtAnim
{
   class CharDrawable;
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
   void OnLOD_Changed( float zeroToOneValue );  
   void OnSetShaded();
   void OnSetWireframe();
   void OnSetShadedWireframe();
   
   ///attach a mesh to the CalModel
   void OnAttachMesh( int meshID );

   ///detach a mesh from the CalModel
   void OnDetachMesh( int meshID );

signals:
   void OnAnimationLoaded( unsigned int, const QString &, unsigned int trackCount,
                           unsigned int keyframes, float duration);

   void OnMeshLoaded(int meshID);

   void ErrorOccured( const QString &msg );

protected:
   virtual void timerEvent(QTimerEvent *event)
   {
      dtCore::System::GetInstance().StepWindow();
   }

   virtual void PostFrame( const double deltaFrameTime );

   void InitShadeDecorator();
   void InitWireDecorator();

private:
   QBasicTimer mTimer;
   dtCore::RefPtr<dtAnim::CharDrawable> mCharacter;
   dtCore::RefPtr<dtCore::OrbitMotionModel> mMotion;
   dtCore::RefPtr<osg::Group> mWireDecorator;
   dtCore::RefPtr<osg::Group> mShadeDecorator;

   std::vector<int> mMeshesToAttach;
   std::vector<int> mMeshesToDetach;
};

#endif // Viewer_h__