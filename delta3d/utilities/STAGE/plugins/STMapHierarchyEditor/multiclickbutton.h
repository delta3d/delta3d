#ifndef STAGE_MAP_HIERARCHY_MULTICLICK_BUTTON_H__
#define STAGE_MAP_HIERARCHY_MULTICLICK_BUTTON_H__

#include "export.h"

#include <QtGui/QPushButton>

class STAGE_MAP_HIERARCHY_EDITOR_EXPORT MultiClickButton : public QPushButton
{
   Q_OBJECT

public:
   MultiClickButton(QWidget* parent = 0);
   ~MultiClickButton();

signals:
   void clickedLeft();
   void clickedMid();
   void clickedRight();

protected:
   QPoint                                       mDragStartPosition;

   void mousePressEvent(QMouseEvent* );
   void mouseMoveEvent(QMouseEvent* );
   void mouseReleaseEvent(QMouseEvent* );
   void enterEvent(QEvent* );
   void leaveEvent(QEvent* );

   bool isInClick;

};

#endif //STAGE_MAP_HIERARCHY_MULTICLICK_BUTTON_H__

