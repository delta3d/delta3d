#ifndef __ANIMATIONTABLEWIDGET_H__
#define __ANIMATIONTABLEWIDGET_H__

#include <QtGui/QTableWidget>

class QAction;

class AnimationTableWidget : public QTableWidget
{
   Q_OBJECT
public:
	AnimationTableWidget(QWidget* parent = NULL);
   ~AnimationTableWidget() {};

public slots:
   void OnDisplayProperty();

protected:
	virtual void contextMenuEvent(QContextMenuEvent* event);

private:

   QAction* mDisplayPropertyAct;
   
   void CreateActions(); 

};
#endif // __ANIMATIONTABLEWIDGET_H__
