#ifndef MainWindow_h__
#define MainWindow_h__

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

namespace dtQt
{
   class OSGAdapterWidget; 
}

namespace dtAnim
{
   class PoseMesh;
}

class MainWindow : public QMainWindow
{
   friend class Delta3DThread;
   Q_OBJECT
public:
   MainWindow();
   ~MainWindow();

   dtQt::OSGAdapterWidget* GetGLWidget() { return mGLWidget; }
   
signals:
   void FileToLoad(const QString&);
   void StartAnimation(unsigned int, float, float);
   void StopAnimation(unsigned int, float);
   void StartAction(unsigned int, float, float);
   void LODScale_Changed(float scaleValue);
   void SpeedChanged(float speedFactor);
   void AttachMesh(int meshID);
   void DetachMesh(int meshID);

public slots:
   void OnNewAnimation(unsigned int id, const QString &animationName, unsigned int trackCount,
                       unsigned int keyframes, float duration);
   
   void OnNewMesh(int meshID, const QString &meshName);

   void OnNewPoseMesh(const dtAnim::PoseMesh &poseMesh);

   void OnNewMaterial(int matID, const QString &name,
                      const QColor &diff, const QColor &amb, const QColor &spec,
                      float shininess );

   void OnBlendUpdate(const std::vector<float> &weightList);

   void OnAnimationClicked( QTableWidgetItem *item);
   void OnMeshActivated( QListWidgetItem *item );
   void OnLODScale_Changed(double newValue);
   void OnSpeedChanged(double newValue);
   void OnToggleHardwareSkinning();
   void OnToggleShadingToolbar();
   void OnToggleLODScaleToolbar();
   void OnToggleLightingToolbar(); 
   void OnDisplayError( const QString &msg );
	
private:
   void CreateMenus();
   void CreateActions();
   void CreateToolbars();
   void UpdateRecentFileActions();
   void SetCurrentFile( const QString &filename );
   void LoadCharFile(const QString &filename);
   void OnStartAnimation(int row);
   void OnStopAnimation(int row);
   void OnStartAction( int row );

   QAction *mExitAct;
   QAction *mLoadCharAct;
   QAction *mRecentFilesAct[5];
   QAction *mWireframeAction; 
   QAction *mShadedAction;    
   QAction *mShadedWireAction;
   QAction *mDiffuseLightAction;
   QAction *mPointLightAction;
   QAction *mMixerViewerAction;

   QToolBar *mShadingToolbar;
   QToolBar *mLightingToolbar;
   QToolBar *mLODScaleToolbar;
   QToolBar *mSpeedToolbar;

   QTabWidget  *mTabs;

   AnimationTableWidget *mAnimListWidget;
   QListWidget          *mMeshListWidget;
   
   QStandardItemModel   *mMaterialModel; ///<Model for the character's materials
   QTableView           *mMaterialView;  ///<View for the character's materials

   PoseMeshView         *mPoseMeshViewer;
   PoseMeshScene        *mPoseMeshScene;
   PoseMeshProperties   *mPoseMeshProperties;

   dtQt::OSGAdapterWidget* mGLWidget;
   
private slots:
   void OnOpenCharFile();
   void OpenRecentFile();
   void OnItemChanged( QTableWidgetItem *item );
   void OnItemDoubleClicked(QTableWidgetItem *item);
};
#endif // MainWindow_h__
