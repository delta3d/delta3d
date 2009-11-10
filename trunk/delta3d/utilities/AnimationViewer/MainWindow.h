#ifndef DELTA_MainWindow
#define DELTA_MainWindow

#include <QtGui/QMainWindow>

class QAction;
class QTableWidgetItem;
class QToolBar;
class AnimationTableWidget;
class QListWidget;
class QListWidgetItem;
class QTableWidget;
class QGraphicsView;
class QGraphicsScene;
class QTabWidget;
class QGridLayout;
class QStandardItemModel;
class QTableView;
class PoseMeshView;
class PoseMeshScene;
class PoseMeshProperties;
class QDoubleSpinBox;
class QComboBox;
class QHBoxLayout;


namespace dtAnim
{
   class PoseMesh;
   class CharDrawable;
}

class Viewer; ///<The animation viewer application

class MainWindow : public QMainWindow
{
   friend class Delta3DThread;
   Q_OBJECT
public:
   MainWindow();
   ~MainWindow();

   void LoadCharFile(const QString& filename);
   void SetViewer(Viewer* viewer);

   void LoadAttachment(const QString& filename);

protected:
   virtual void dragEnterEvent(QDragEnterEvent* event);
   virtual void dropEvent(QDropEvent* event);

signals:
   void FileToLoad(const QString&);
   void UnloadFile();
   void StartAnimation(unsigned int, float, float);
   void StopAnimation(unsigned int, float);
   void StartAction(unsigned int, float, float);
   void LODScale_Changed(float scaleValue);
   void SpeedChanged(float speedFactor);
   void ScaleFactorChanged(float scaleFactorValue);
   void SetBoneLabeling(bool shouldShow);

   //Show the mesh on the CalModel
   void ShowMesh(int meshID);
   /// Hide the mesh on CalModel from view
   void HideMesh(int meshID);

   void SubMorphTargetChanged(int meshID, int subMeshID,
                              int morphID, float weight);

   void PlayMorphAnimation(int morphAnimID);

   void AttachmentToLoad(const QString&);

   void AttachmentSettingsChanged(const std::string& boneName, float offsetX, float offsetY, float offsetZ, float rotx, float roty, float rotz);

public slots:
   void OnNewAnimation(unsigned int id, const QString& animationName, unsigned int trackCount,
                       unsigned int keyframes, float duration);

   void OnNewMesh(int meshID, const QString& meshName, const std::vector<std::string>& boneNames);

   void OnNewSubMorphTarget(int meshID, int subMeshID, 
                            int morphID, const QString& morphName);

   void OnPoseMeshesLoaded(const std::vector<dtAnim::PoseMesh*>& poseMeshList,
                           dtAnim::CharDrawable* model);

   void OnNewMaterial(int matID, const QString& name,
                      const QColor& diff, const QColor& amb, const QColor& spec,
                      float shininess);

   void OnBlendUpdate(const std::vector<float>& weightList);

   void OnAnimationClicked(QTableWidgetItem* item);
   void OnMeshActivated(QListWidgetItem* item);
   void OnLODScale_Changed(double newValue);
   void OnSpeedChanged(double newValue);
   void OnChangeScaleFactor();
   void OnToggleHardwareSkinning();
   void OnToggleShadingToolbar();
   void OnToggleLODScaleToolbar();
   void OnToggleSpeedScaleToolbar();
   void OnToggleScalingToolbar();
   void OnToggleAttachmentToolbar();
   void OnToggleLightingToolbar();
   void OnDisplayError(const QString& msg);
   void OnConfiged(); ///<call when everything is up and running

   void OnClearCharacterData();

   void OnLoadAttachment();
   void OnChangeAttachmentSettings();

private:
   void CreateMenus();
   void CreateActions();
   void CreateToolbars();
   void DestroyPoseResources();
   void UpdateRecentFileActions();
   void SetCurrentFile(const QString& filename);
   void OnStartAnimation(int row);
   void OnStopAnimation(int row);
   void OnStartAction(int row);
   bool IsAnimNodeBuildingUsingHW() const;
   void SetupConnectionsWithViewer();

   ///turns color into "R:rrr "G:ggg B:bbb A:aaa" format
   QString MakeColorString(const QColor& color) const;

   QAction* mExitAct;
   QAction* mLoadCharAct;
   QAction* mCloseCharAction;
   QAction* mRecentFilesAct[5];
   QAction* mWireframeAction;
   QAction* mShadedAction;
   QAction* mShadedWireAction;
   QAction* mBoneBasisAction;
   QAction* mDiffuseLightAction;
   QAction* mPointLightAction;
   QAction* mHardwareSkinningAction;
   QAction* mBoneLabelAction;

   QToolBar* mShadingToolbar;
   QToolBar* mLightingToolbar;
   QToolBar* mLODScaleToolbar;
   QToolBar* mSpeedToolbar;
   QToolBar* mScalingToolbar;
   QToolBar* mAttachmentToolbar;

   QDoubleSpinBox* mScaleFactorSpinner;

   QDoubleSpinBox* mAttachmentOffsetXSpinner;
   QDoubleSpinBox* mAttachmentOffsetYSpinner;
   QDoubleSpinBox* mAttachmentOffsetZSpinner;
   QDoubleSpinBox* mAttachmentRotXSpinner;
   QDoubleSpinBox* mAttachmentRotYSpinner;
   QDoubleSpinBox* mAttachmentRotZSpinner;
   QComboBox* mAttachmentParent;
   QString mCurrentAttachment;   // the currently loaded attachment mesh
   
   QTabWidget* mTabs;

   AnimationTableWidget* mAnimListWidget;
   QListWidget*          mMeshListWidget;
   QTableWidget*         mSubMorphTargetListWidget;

   QStandardItemModel* mMaterialModel; ///<Model for the character's materials
   QTableView*         mMaterialView;  ///<View for the character's materials

   QDockWidget*        mPoseDock;
   PoseMeshView*       mPoseMeshViewer;
   PoseMeshScene*      mPoseMeshScene;
   PoseMeshProperties* mPoseMeshProperties;

   Viewer*             mViewer;
   QHBoxLayout*        mCentralLayout;

private slots:
   void OnOpenCharFile();
   void OpenRecentFile();
      
   void OnCloseCharFile();

   void OnItemChanged(QTableWidgetItem* item);
   void OnItemDoubleClicked(QTableWidgetItem* item);

   void OnSelectModeGrab();
   void OnSelectModeBlendPick();
   void OnSelectModeErrorPick();

   void OnToggleDisplayEdges(bool shouldDisplay);
   void OnToggleDisplayError(bool shouldDisplay);
   void OnToggleFlipVertical();
   void OnToggleFlipHorizontal();

   void OnSubMorphChanged(QTableWidgetItem* item);
   void OnSubMorphPlay(QTableWidgetItem* item);
};
#endif // DELTA_MainWindow
