#ifndef MainWindow_h__
#define MainWindow_h__

#include <QMainWindow>

class QAction;
class QTableWidget;
class QTableWidgetItem;

class MainWindow : public QMainWindow
{
   Q_OBJECT
public:
	MainWindow();
	~MainWindow();

signals:
   void FileToLoad(const QString&);
   void StartAnimation(unsigned int, float, float);
   void StopAnimation(unsigned int, float);
   void StartAction(unsigned int, float, float);

public slots:
   void OnNewAnimation(unsigned int id, const QString &filename);
   void OnAnimationClicked( QTableWidgetItem *item);
	
private:
   void CreateMenus();
   void CreateActions();
   void UpdateRecentFileActions();
   void SetCurrentFile( const QString &filename );
   void LoadCharFile(const QString &filename);
   void OnStartAnimation(int row);
   void OnStopAnimation(int row);
   void OnStartAction( int row );

   QAction *mExitAct;
   QAction *mLoadCharAct;
   QAction *mRecentFilesAct[5];
   QTableWidget *mAnimListWidget;

private slots:
   void OnOpenCharFile();
   void OpenRecentFile();
   void OnItemChanged( QTableWidgetItem *item );
   void OnItemDoubleClicked(QTableWidgetItem *item);


};
#endif // MainWindow_h__