#ifndef DELTA_ANIMVIEW_VIEWER
#define DELTA_ANIMVIEW_VIEWER

#include <QtCore/QObject>
#include <QtCore/QTimer>

#include <dtCore/refptr.h>
#include <dtCore/system.h>
#include <dtABC/application.h>

#include <dtAnim/posemeshdatabase.h>
#include <dtAnim/posemeshutility.h>
#include <dtAnim/posemesh.h>
#include <dtAnim/attachmentcontroller.h>

#include <vector>

class QColor;

namespace dtCore
{
   class OrbitMotionModel;
}

namespace dtGUI
{
   class CEUIDrawable;
}

namespace osg
{
   class Group;
}


namespace dtAnim
{
   class CharDrawable;
   class Cal3DDatabase;
   class PoseMeshDatabase;
}

namespace CEGUI
{
   class Window;
}


class Viewer : public QObject, public dtABC::Application
{
   Q_OBJECT

public:
	Viewer();
	~Viewer();

   virtual void Config();

public slots:

   void OnLoadCharFile    ( const QString &filename );
   void OnLoadPoseMeshFile( const std::string &filename );
   void OnStartAnimation  ( unsigned int id, float weight, float delay );
   void OnStopAnimation   ( unsigned int id, float delay );
   void OnStartAction     ( unsigned int id, float delayIn, float delayOut );  
   void OnLODScale_Changed( float scaleValue );
   void OnSpeedChanged    ( float speedFactor );
   void OnSetShaded();
   void OnSetWireframe();
   void OnSetShadedWireframe();
   void OnSetBoneBasisDisplay(bool shouldDisplay);
   
   void OnTimeout();
   
   ///attach a mesh to the CalModel
   void OnAttachMesh( int meshID );

   ///detach a mesh from the CalModel
   void OnDetachMesh( int meshID );

signals:
   void AnimationLoaded( unsigned int, const QString &, unsigned int trackCount,
                           unsigned int keyframes, float duration);

   void MeshLoaded(int meshID, const QString &meshName);

   void PoseMeshLoaded(const dtAnim::PoseMesh &poseMesh);

   void PoseMeshesLoaded(const std::vector<dtAnim::PoseMesh*> &poseMeshes,
                         dtAnim::CharDrawable *character);

   void MaterialLoaded(int materialID, const QString &name, 
                       const QColor &diffuse, const QColor &ambient, const QColor &specular,
                       float shininess);

   void ErrorOccured( const QString &msg );

   void BlendUpdate(const std::vector<float> &weightList);

protected:
   virtual void PostFrame( const double deltaFrameTime );  

private:
  
   dtCore::RefPtr<dtAnim::CharDrawable> mCharacter;
   dtCore::RefPtr<dtAnim::AttachmentController> mAttachmentController;
   dtCore::RefPtr<dtCore::OrbitMotionModel> mMotion;
   dtCore::RefPtr<osg::Group> mWireDecorator;
   dtCore::RefPtr<osg::Group> mShadeDecorator;
   dtCore::RefPtr<osg::Group> mBoneBasisGroup;

   std::vector<int> mMeshesToAttach;
   std::vector<int> mMeshesToDetach;

   dtCore::RefPtr<dtAnim::PoseMeshDatabase> mPoseDatabase;
   dtCore::RefPtr<dtAnim::PoseMeshUtility>  mPoseUtility;
   dtCore::RefPtr<dtAnim::Cal3DDatabase>    mCalDatabase; ///<Need to keep this around since it holds our textures   

   std::vector<dtAnim::PoseMesh*> *mPoseMeshes;

   void InitShadeDecorator();
   void InitWireDecorator();
   void CreateBoneBasisDisplay();
};

#endif // Viewer_h__
