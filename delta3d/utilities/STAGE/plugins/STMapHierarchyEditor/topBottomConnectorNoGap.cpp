#include "TopBottomConnectorNoGap.h"

TopBottomConnectorNoGap::TopBottomConnectorNoGap(QWidget* parent /*=0*/, Qt::WindowFlags f /*=0*/)
   : mUI()
   , QWidget(parent, f)
{
   //apply layout made with QtDesigner (see ui file)
   mUI.setupUi(this);
}

TopBottomConnectorNoGap::~TopBottomConnectorNoGap()
{
}
