#ifndef MainWindow_h__
#define MainWindow_h__

#include <QMainWindow>

class QAction;
class QTableWidget;
class QTableWidgetItem;
class QToolBar;

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

public slots:
   void OnNewAnimation(unsigned int id, const QString &filename);
   void OnAnimationClicked( QTableWidgetItem *item);
   void OnLOD_Changed(double newValue);
   void OnToggleShadingToolbar();
   void OnToggleTempToolbar();
   void OnToggleLightingToolbar();
	
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

   QTableWidget *mAnimListWidget;

private slots:
   void OnOpenCharFile();
   void OpenRecentFile();
   void OnItemChanged( QTableWidgetItem *item );
   void OnItemDoubleClicked(QTableWidgetItem *item);
};
#endif // MainWindow_h__