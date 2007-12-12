#ifndef DELTA_ANIMVIEW_VIEWER
#define DELTA_ANIMVIEW_VIEWER

#include <QtCore/QObject>
#include <QtCore/QBasicTimer>

#include <dtCore/refptr.h>
#include <dtABC/application.h>
#include <dtCore/system.h>

#include <vector>

class QColor;

namespace dtCore
{
   class OrbitMotionModel;
}

namespace osg
{
   class Group;
}


namespace dtAnim
{
   class CharDrawable;
   class Cal3DDatabase;
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
   void OnSpeedChanged( float speedFactor );
   void OnSetShaded();
   void OnSetWireframe();
   void OnSetShadedWireframe();
   
   ///attach a mesh to the CalModel
   void OnAttachMesh( int meshID );

   ///detach a mesh from the CalModel
   void OnDetachMesh( int meshID );

signals:
   void AnimationLoaded( unsigned int, const QString &, unsigned int trackCount,
                           unsigned int keyframes, float duration);

   void MeshLoaded(int meshID, const QString &meshName);

   void MaterialLoaded(int materialID, const QString &name, 
                       const QColor &diffuse, const QColor &ambient, const QColor &specular,
                       float shininess);

   void ErrorOccured( const QString &msg );

protected:
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

   dtCore::RefPtr<dtAnim::Cal3DDatabase> mDatabase; ///<Need to keep this around since it holds our textures
};

#endif // Viewer_h__
