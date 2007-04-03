#ifndef MainWindow_h__
#define MainWindow_h__

#include <QMainWindow>

class QAction;
class QTableWidgetItem;
class QToolBar;
class AnimationTableWidget;
class QListWidget;
class QListWidgetItem;

class MainWindow : public QMainWindow
{
   friend class Delta3DThread;
   Q_OBJECT
public:
	MainWindow();
	~MainWindow();

signals:
   void FileToLoad(const QString&);
   void StartAnimation(unsigned int, float, float);
   void StopAnimation(unsigned int, float);
   void StartAction(unsigned int, float, float);
   void LOD_Changed(float zeroToOneValue);
   void AttachMesh(int meshID);
   void DetachMesh(int meshID);

public slots:
   void OnNewAnimation(unsigned int id, const QString &filename, unsigned int trackCount,
                       unsigned int keyframes, float duration);
   
   void OnNewMesh(int meshID);

   void OnAnimationClicked( QTableWidgetItem *item);
   void OnMeshActivated( QListWidgetItem *item );
   void OnLOD_Changed(double newValue);
   void OnToggleShadingToolbar();
   void OnToggleTempToolbar();
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

   QToolBar *mShadingToolbar;
   QToolBar *mLightingToolbar;
   QToolBar *mTempToolbar;

   AnimationTableWidget *mAnimListWidget;
   QListWidget          *mMeshListWidget;

private slots:
   void OnOpenCharFile();
   void OpenRecentFile();
   void OnItemChanged( QTableWidgetItem *item );
   void OnItemDoubleClicked(QTableWidgetItem *item);
};
#endif // MainWindow_h__