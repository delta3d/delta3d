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

namespace dtQt
{
   class OSGAdapterWidget; 
}

namespace dtAnim
{   
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
   void RemoveShader();

public slots:
   
   void OnInitialization();
   void OnNewShader(const std::string &shaderGroup, const std::string &shaderProgram);  
   
   void OnToggleShadingToolbar(); 
	
private:
   void CreateMenus();
   void CreateActions();
   void CreateToolbars();
   void UpdateRecentFileActions();
   void SetCurrentFile(const QString &filename);
   void LoadObjectFile(const QString &filename);
   void LoadCharFile(const QString &filename);

   // File menu
   QAction *mLoadShaderDefAction;
   QAction *mLoadGeometryAction;
   QAction *mChangeContextAction;
   QAction *mExitAct;

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

   std::string mContextPath;

   dtQt::OSGAdapterWidget* mGLWidget;

private slots:
   void OnOpenCharFile();
   void OpenRecentFile(); 

   // File menu callbacks
   void OnLoadShaderDefinition();
   void OnLoadGeometry();
   void OnChangeContext();
 
   void OnShaderItemChanged(QTreeWidgetItem *item, int column);
   void OnDoubleclickShaderItem(QTreeWidgetItem *item, int column);

   std::string GetContextPathFromUser();
   void SaveCurrentContextPath();
};
#endif // DELTA_ObjectWorkspace
