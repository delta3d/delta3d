#ifndef STAGE_MAP_HIERARCHY_BUTTON_CONNECTOR_H__
#define STAGE_MAP_HIERARCHY_BUTTON_CONNECTOR_H__

#include "export.h"
#include "ui_buttonconnector.h"

#include <QtGui/QWidget>

/**
 Connects from the top and bottom and left in the map editor tree.
 */
class STAGE_MAP_HIERARCHY_EDITOR_EXPORT ButtonConnector
   : public QWidget
{
   Q_OBJECT
public:

   enum Type
   {
      /*TOP_BOTTOM = 0, */
      TOP_BOTTOM_RIGHT = 0,
      BOTTOM_LEFT,
      BOTTOM_LEFT_RIGHT,
      LEFT_RIGHT,
      UNSPECIFIED
   };

   ButtonConnector(Type t, QWidget* parent = 0, Qt::WindowFlags f = 0);
   ~ButtonConnector();

   Type GetType() const { return mType; }
   void SetType(Type t);

private:
   void SetLineWidths(int top, int bottom, int left, int right);
   void SetLineWidth(QFrame* line, int width);

   Ui_ButtonConnector                                mUI;
   Type                                              mType;
};

#endif //STAGE_MAP_HIERARCHY_BUTTON_CONNECTOR_H__
