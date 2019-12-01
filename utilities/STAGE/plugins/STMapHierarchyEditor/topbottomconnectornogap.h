#ifndef STAGE_MAP_HIERARCHY_TOP_BOTTOM_CONNECTOR_NO_GAP_H__
#define STAGE_MAP_HIERARCHY_TOP_BOTTOM_CONNECTOR_NO_GAP_H__

#include "export.h"
#include "ui_topbottomconnectornogap.h"

#include <QtGui/QWidget>

/**
 Connects from the top and bottom and left in the map editor tree.
 */
class STAGE_MAP_HIERARCHY_EDITOR_EXPORT TopBottomConnectorNoGap
   : public QWidget
{
   Q_OBJECT
public:

   TopBottomConnectorNoGap(QWidget* parent = 0, Qt::WindowFlags f = 0);
   ~TopBottomConnectorNoGap();

private:
   Ui_TopBottomConnectorNoGap mUI;
};

#endif //STAGE_MAP_HIERARCHY_TOP_BOTTOM_CONNECTOR_NO_GAP_H__
