#ifndef DELTA_ShaderWorkspace
#define DELTA_ShaderWorkspace

#include <QtGui/QMainWindow>

///////////////////////////////////////////////////////////////////////////////

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
   class CharDrawable;
}

///////////////////////////////////////////////////////////////////////////////

class ShaderWorkspace : public QMainWindow
{
   friend class Delta3DThread;
   Q_OBJECT
public:
   ShaderWorkspace();
   ~ShaderWorkspace();

   dtQt::OSGAdapterWidget* GetGLWidget() { return mGLWidget; }
   
signals:
   void FileToLoad(const QString&);  
   void LoadShaderDefinition(const QString &);
   void LoadGeometry(const QString &);
   void StartAction(unsigned int, float, float);

public slots:
   
   void OnInitialization();
   void OnNewShader(const QString &shaderName);

   void OnPoseMeshesLoaded(const std::vector<dtAnim::PoseMesh*> &poseMeshList, 
                           dtAnim::CharDrawable *model);  
   
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
  
   QListWidget          *mShaderListWidget;  

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

   void OnSelectModeGrab();
   void OnSelectModeBlendPick();
   void OnSelectModeErrorPick();

   void OnToggleDisplayEdges(bool shouldDisplay);
   void OnToggleDisplayError(bool shouldDisplay);
   void OnToggleBoneBasisDisplay(bool shouldDisplay);
};
#endif // DELTA_ShaderWorkspace
