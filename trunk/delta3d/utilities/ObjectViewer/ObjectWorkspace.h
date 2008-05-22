#ifndef DELTA_ObjectWorkspace
#define DELTA_ObjectWorkspace

#include <QtGui/QMainWindow>

///////////////////////////////////////////////////////////////////////////////

class QAction;
class QTableWidgetItem;
class QToolBar;
class AnimationTableWidget;
class QTreeWidget;
class QTreeWidgetItem;
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
   class CharDrawable;
}

///////////////////////////////////////////////////////////////////////////////

class ObjectWorkspace : public QMainWindow
{
   friend class Delta3DThread;
   Q_OBJECT
public:
   ObjectWorkspace();
   ~ObjectWorkspace();

   dtQt::OSGAdapterWidget* GetGLWidget() { return mGLWidget; }
   
signals:
   void FileToLoad(const QString&);  
   void LoadShaderDefinition(const QString &);
   void LoadGeometry(const QString &);
   void StartAction(unsigned int, float, float);
   void ApplyShader(const std::string &group, const std::string &name);

public slots:
   
   void OnInitialization();
   void OnNewShader(const std::string &shaderGroup, const std::string &shaderProgram);  
   
   void OnToggleShadingToolbar(); 
	
private:
   void CreateMenus();
   void CreateActions();
   void CreateToolbars();
   void DestroyPoseResources();
   void UpdateRecentFileActions();
   void SetCurrentFile(const QString &filename);
   void LoadObjectFile(const QString &filename);
   void LoadCharFile(const QString &filename);

   QAction *mExitAct;
   QAction *mLoadShaderDefAction;
   QAction *mLoadGeometryAction;
   QAction *mRecentFilesAct[5];
   QAction *mWireframeAction; 
   QAction *mShadedAction;    
   QAction *mShadedWireAction;
   QAction *mDiffuseLightAction;
   QAction *mPointLightAction;

   QToolBar *mShadingToolbar; 
   QTabWidget *mTabs;
  
   QTreeWidget          *mShaderTreeWidget;  

   QDockWidget          *mPoseDock;
   PoseMeshView         *mPoseMeshViewer;
   PoseMeshScene        *mPoseMeshScene;
   PoseMeshProperties   *mPoseMeshProperties;

   std::string mContextPath;

   dtQt::OSGAdapterWidget* mGLWidget;

private slots:
   void OnOpenCharFile();
   void OpenRecentFile(); 

   void OnLoadShaderDefinition();
   void OnLoadGeometry();
 
   void OnSelectShaderItem();
   void OnDoubleclickShaderItem(QTreeWidgetItem *item, int column);
};
#endif // DELTA_ObjectWorkspace
