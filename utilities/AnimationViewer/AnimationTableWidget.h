#ifndef AnimationTableWidget_h__
#define AnimationTableWidget_h__

#include <QtGui/QTableWidget>

class QAction;

class AnimationTableWidget : public QTableWidget
{
   Q_OBJECT
public:
	AnimationTableWidget(QWidget *parent = NULL);
   ~AnimationTableWidget() {};

public slots:
   void OnDisplayProperty();

protected:
	virtual void contextMenuEvent(QContextMenuEvent *event );

private:
   QAction *mDisplayPropertyAct;
   
   void CreateActions();

   

};
#endif // AnimationTableWidget_h__
