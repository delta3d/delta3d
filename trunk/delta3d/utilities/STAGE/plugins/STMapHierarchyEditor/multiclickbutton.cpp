#include "multiclickbutton.h"

#include "nodebutton.h"

#include <QtGui/QApplication>
#include <QtGui/QMouseEvent>

#include <dtQt/typedefs.h>

MultiClickButton::MultiClickButton(QWidget* parent /*=0*/)
   : QPushButton(parent)
   , mDragStartPosition()
{
}

MultiClickButton::~MultiClickButton()
{
}

void MultiClickButton::mousePressEvent(QMouseEvent* myEvent )
{
   setDown( true );
   isInClick = true;

   if (myEvent->button() == Qt::LeftButton)
   {
      mDragStartPosition = myEvent->pos();
   }
}

void MultiClickButton::mouseMoveEvent(QMouseEvent* myEvent)
{
   if ((myEvent->buttons() & Qt::LeftButton) &&
      (myEvent->pos() - mDragStartPosition).manhattanLength() < QApplication::startDragDistance())
   {     
      NodeButton* nb = dynamic_cast<NodeButton*>(this->parent());

      QDrag* drag = new QDrag(this);

      std::string uid = nb->GetActorID();
      QByteArray itemData(uid.c_str());
      QDataStream dataStream(&itemData, QIODevice::WriteOnly);
      QMimeData *mimeData = new QMimeData;

      mimeData->setData("Node", itemData);
      drag->setMimeData(mimeData);
      
      //Qt::DropAction dropAction =
      drag->exec(Qt::MoveAction);
   }
}

void MultiClickButton::mouseReleaseEvent(QMouseEvent* myEvent )
{
   if ( isDown() )
   {
      switch( myEvent->button() )
      {
         case Qt::LeftButton:
            emit( clickedLeft() );
            break;

         case Qt::MidButton:
            emit( clickedMid() );
            break;

         case Qt::RightButton:
            emit( clickedRight() );
            break;

         default:
            break;
      }
   }

   setDown( false );
   isInClick = false;
}


void MultiClickButton::enterEvent ( QEvent * )
{
   if ( isInClick )
   {
      setDown( true );
   }
}


void MultiClickButton::leaveEvent ( QEvent * )
{
   if ( isInClick )
   {
      setDown( false );
   }
}

