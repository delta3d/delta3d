#ifndef DELTA_ANIMVIEW_VIEWER
#define DELTA_ANIMVIEW_VIEWER

#include <QtCore/QObject>
#include <QtCore/QTimer>

#include <dtCore/object.h>
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


/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Group;
}
/// @endcond


namespace dtAnim
{
   class Cal3DDatabase;
   class Cal3DModelData;
   class Cal3DModelWrapper;
   class CharDrawable;
   class PoseMeshDatabase;
}


class Viewer : public QObject, public dtABC::Application
{
   Q_OBJECT

public:
   Viewer();

   virtual void Config();

public slots:

   void OnNewCharFile();
   void OnLoadCharFile        (const QString& filename);
   void OnSaveCharFile        (const QString& filename);
   void OnReloadCharFile();
   void OnUnloadCharFile();
   void OnLoadPoseMeshFile    (const std::string& filename);
   void OnStartAnimation      (unsigned int id, float weight, float delay);
   void OnStopAnimation       (unsigned int id, float delay);
   void OnStartAction         (unsigned int id, float delayIn, float delayOut);
   void OnLODScale_Changed    (float scaleValue);
   void OnSpeedChanged        (float speedFactor);
   void OnScaleFactorChanged  (float scaleFactorValue);
   void OnSetShaded();
   void OnSetWireframe();
   void OnSetShadedWireframe();
   void OnSetBoneBasisDisplay (bool shouldDisplay);
   void OnSetBoneLabelDisplay (bool shouldDisplay);

   void OnUnloadAttachmentFile();
   void OnLoadAttachmentFile  (const QString& filename);
   void OnAttachmentSettingsChanged(const std::string& bone, float offsx, float offsy, float offsz, float rotx, float roty, float rotz);

   void OnTimeout();

   /// Show the mesh on the CalModel
   void OnShowMesh(int meshID);

   /// Hide the mesh on CalModel from view
   void OnHideMesh(int meshID);

   void OnMorphChanged(int meshID, int subMeshID, int morphID, float weight);
   void OnPlayMorphAnimation(int morphAnimID, float weight, float delayIn, float delayOut, bool looping);
   void OnStopMorphAnimation(int morphAnimID, float delay);

   void OnClearTempFile();

signals:
   void AnimationLoaded(unsigned int, const QString&, unsigned int trackCount,
                        unsigned int keyframes, float duration);

   void MorphAnimationLoaded(unsigned int, const QString&, unsigned int trackCount,
      unsigned int keyframes, float duration);

   void ClearCharacterData();

   void MeshLoaded(int meshID, const QString& meshName, const std::vector<std::string>& boneNames);

   void SubMorphTargetLoaded(int meshID, int subMeshID, int morphID, const QString& morphName);

   void PoseMeshLoaded(const dtAnim::PoseMesh& poseMesh);

   void PoseMeshesLoaded(const std::vector<dtAnim::PoseMesh*> &poseMeshes,
                         dtAnim::CharDrawable* character);

   void MaterialLoaded(int materialID, const QString& name,
                       const QColor& diffuse, const QColor& ambient, const QColor& specular,
                       float shininess);
   
   void CharacterDataLoaded(dtAnim::Cal3DModelData* modelData, dtAnim::Cal3DModelWrapper* wrapper);

   void ErrorOccured(const QString& msg);

   void BlendUpdate(const std::vector<std::pair<float, float> >& animWeightTimeList, const std::vector<float>& morphWeightList);

   void SignalError(const std::string& title, const std::string& message);

protected:
   virtual ~Viewer();

   virtual void PostFrame(const double deltaFrameTime);

private:

   dtCore::RefPtr<dtAnim::CharDrawable>         mCharacter;
   dtCore::RefPtr<dtAnim::AttachmentController> mAttachmentController;

   dtCore::RefPtr<dtCore::OrbitMotionModel> mMotion;

   dtCore::RefPtr<osg::Group> mWireDecorator;
   dtCore::RefPtr<osg::Group> mShadeDecorator;
   dtCore::RefPtr<osg::Group> mBoneBasisGroup;

   std::vector<int> mMeshesToShow;
   std::vector<int> mMeshesToHide;

   dtCore::RefPtr<dtAnim::PoseMeshDatabase> mPoseDatabase;
   dtCore::RefPtr<dtAnim::PoseMeshUtility>  mPoseUtility;
   dtCore::RefPtr<dtAnim::Cal3DDatabase>    mCalDatabase; ///<Need to keep this around since it holds our textures

   std::vector<dtAnim::PoseMesh*>* mPoseMeshes;

   void InitShadeDecorator();
   void InitWireDecorator();
   void CreateBoneBasisDisplay();

   int GetMaxBoneAffectedBoneIDForAnimation(int animationID);

   dtCore::RefPtr<dtCore::Object> mAttachmentObject;

   std::string mTempFile;
};

#endif // Viewer_h__
