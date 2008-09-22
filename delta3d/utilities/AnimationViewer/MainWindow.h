#ifndef DELTA_MainWindow
#define DELTA_MainWindow

#include <QtGui/QMainWindow>

class QAction;
class QTableWidgetItem;
class QToolBar;
class AnimationTableWidget;
class QListWidget;
class QListWidgetItem;
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

namespace dtQt
{
   class OSGAdapterWidget; 
}

namespace dtAnim
{
   class PoseMesh;
   class CharDrawable;
}

class MainWindow : public QMainWindow
{
   friend class Delta3DThread;
   Q_OBJECT
public:
   MainWindow();
   ~MainWindow();

   dtQt::OSGAdapterWidget* GetGLWidget() { return mGLWidget; }

   void LoadCharFile(const QString& filename);

protected:
   virtual void dragEnterEvent(QDragEnterEvent* event);
   virtual void dropEvent(QDropEvent* event);
   
signals:
   void FileToLoad(const QString&);
   void StartAnimation(unsigned int, float, float);
   void StopAnimation(unsigned int, float);
   void StartAction(unsigned int, float, float);
   void LODScale_Changed(float scaleValue);
   void SpeedChanged(float speedFactor);
   void ScaleFactorChanged(float scaleFactorValue);

   //Show the mesh on the CalModel
   void ShowMesh(int meshID);
   /// Hide the mesh on CalModel from view
   void HideMesh(int meshID);

public slots:
   void OnNewAnimation(unsigned int id, const QString& animationName, unsigned int trackCount,
                       unsigned int keyframes, float duration);
   
   void OnNewMesh(int meshID, const QString& meshName);

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
   void OnToggleScalingToolbar();
   void OnToggleLightingToolbar(); 
   void OnDisplayError(const QString& msg);
	
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

   QAction* mExitAct;
   QAction* mLoadCharAct;
   QAction* mRecentFilesAct[5];
   QAction* mWireframeAction; 
   QAction* mShadedAction;    
   QAction* mShadedWireAction;
   QAction* mBoneBasisAction;
   QAction* mDiffuseLightAction;
   QAction* mPointLightAction;

   QToolBar* mShadingToolbar;
   QToolBar* mLightingToolbar;
   QToolBar* mLODScaleToolbar;
   QToolBar* mSpeedToolbar;
   QToolBar* mScalingToolbar;

   QDoubleSpinBox* mScaleFactorSpinner;

   QTabWidget* mTabs;

   AnimationTableWidget* mAnimListWidget;
   QListWidget*          mMeshListWidget;
   
   QStandardItemModel* mMaterialModel; ///<Model for the character's materials
   QTableView*         mMaterialView;  ///<View for the character's materials

   QDockWidget*        mPoseDock;
   PoseMeshView*       mPoseMeshViewer;
   PoseMeshScene*      mPoseMeshScene;
   PoseMeshProperties* mPoseMeshProperties;

   dtQt::OSGAdapterWidget* mGLWidget;

private slots:
   void OnOpenCharFile();
   void OpenRecentFile();
   void OnItemChanged(QTableWidgetItem* item);
   void OnItemDoubleClicked(QTableWidgetItem* item);

   void OnSelectModeGrab();
   void OnSelectModeBlendPick();
   void OnSelectModeErrorPick();

   void OnToggleDisplayEdges(bool shouldDisplay);
   void OnToggleDisplayError(bool shouldDisplay);
   void OnToggleFlipVertical();
   void OnToggleFlipHorizontal();
};
#endif // DELTA_MainWindow
