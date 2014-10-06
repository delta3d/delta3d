#ifndef __ANIMATION_VIEWER_H__
#define __ANIMATION_VIEWER_H__

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
#include <dtAnim/modelloader.h>

#include <vector>

#include "AttachmentPanel.h"

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
   class BaseModelData;
   class BaseModelWrapper;
   class CharDrawable;
   class PoseMeshDatabase;
}


class Viewer : public QObject, public dtABC::Application
{
   Q_OBJECT

public:
   Viewer();

   virtual void Config();

   osg::Node* GetRootNode();

   dtAnim::CharDrawable* GetCharacter();

   void UpdateCharacter();

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
   void OnLoadAttachmentFile  (const QString filename);
   void OnAttachmentSettingsChanged(AttachmentInfo info);

   void OnTimeout();

   /// Show the mesh on the CalModel
   void OnShowMesh(int meshID);

   /// Hide the mesh on CalModel from view
   void OnHideMesh(int meshID);

   void OnMorphChanged(const QString& meshName, int subMeshID, int morphID, float weight);
   void OnPlayMorphAnimation(int morphAnimID, float weight, float delayIn, float delayOut, bool looping);
   void OnStopMorphAnimation(int morphAnimID, float delay);

   void OnClearTempFile();

signals:
   void SignalAttachmentLoaded();

   void AnimationLoaded(unsigned int, const QString&, unsigned int trackCount,
                        unsigned int keyframes, float duration);

   void MorphAnimationLoaded(unsigned int, const QString&, unsigned int trackCount,
      unsigned int keyframes, float duration);

   void ClearCharacterData();

   void MeshLoaded(int meshID, const QString& meshName, const std::vector<std::string>& boneNames,
      bool visible, int vertCount, int faceCount, int submeshCount);

   void SubMorphTargetLoaded(const QString& meshName, int subMeshID, int morphID, const QString& morphName);

   void PoseMeshLoaded(const dtAnim::PoseMesh& poseMesh);

   void PoseMeshesLoaded(const std::vector<dtAnim::PoseMesh*> &poseMeshes,
                         dtAnim::CharDrawable* character);

   void MaterialLoaded(int materialID, const QString& name,
                       const QColor& diffuse, const QColor& ambient, const QColor& specular,
                       float shininess);
   
   void CharacterDataLoaded(dtAnim::BaseModelData* modelData, dtAnim::BaseModelWrapper* wrapper);

   void ErrorOccured(const QString& msg);

   void BlendUpdate(const std::vector<std::pair<float, float> >& animWeightTimeList, const std::vector<float>& morphWeightList);

   void SignalError(const std::string& title, const std::string& message);

protected:
   virtual ~Viewer();

   virtual void PostFrame(const double deltaFrameTime);

   void UpdateAnimationList(dtAnim::BaseModelWrapper& wrapper);
   void UpdateMaterialList(dtAnim::BaseModelWrapper& wrapper);
   void UpdateMeshList(dtAnim::BaseModelWrapper& wrapper);
   void UpdateMorphList(dtAnim::BaseModelWrapper& wrapper);

private:
   dtCore::RefPtr<dtAnim::ModelLoader>          mModelLoader;
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

   std::vector<dtAnim::PoseMesh*>* mPoseMeshes;

   void InitShadeDecorator();
   void InitWireDecorator();
   void CreateBoneBasisDisplay();

   int GetMaxBoneAffectedBoneIDForAnimation(int animationID);

   dtCore::RefPtr<dtCore::Object> mAttachmentObject;

   std::string mTempFile;
};

#endif // Viewer_h__
