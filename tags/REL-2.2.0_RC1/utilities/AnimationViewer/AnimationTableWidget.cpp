#include "AnimationTableWidget.h"
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QMessageBox>

////////////////////////////////////////////////////////////////////////////////
AnimationTableWidget::AnimationTableWidget(QWidget* parent /*= NULL*/)
   : QTableWidget(parent)
{
   CreateActions();  
}

////////////////////////////////////////////////////////////////////////////////
void AnimationTableWidget::contextMenuEvent(QContextMenuEvent* event)
{
   QMenu menu(this);
   menu.addAction(mDisplayPropertyAct);
   menu.exec( event->globalPos() );
}

////////////////////////////////////////////////////////////////////////////////
void AnimationTableWidget::CreateActions()
{
   mDisplayPropertyAct = new QAction( tr("&Properties"), this);
   mDisplayPropertyAct->setStatusTip(tr("Display animation properties"));
   connect(mDisplayPropertyAct, SIGNAL(triggered()), this, SLOT(OnDisplayProperty()));
}

////////////////////////////////////////////////////////////////////////////////
void AnimationTableWidget::OnDisplayProperty()
{
   QTableWidgetItem* item = this->item(currentRow(), 0);

   if (item==NULL) return;

   //animation ID
   //track count
   //total keyframes
   //duration

   QMessageBox::about(this, tr("AnimationViewer"),
      tr("Animation ID: %1\n"
      "Number of tracks: %2\n"
      "Number of keyframes: %3\n"
      "Duration: %4")
      .arg(item->data(Qt::UserRole).toUInt())
      .arg(item->data(Qt::UserRole+1).toUInt())
      .arg(item->data(Qt::UserRole+2).toUInt())
      .arg(item->data(Qt::UserRole+3).toDouble())
      );   
}
