#include "topbottomconnectornogap.h"

TopBottomConnectorNoGap::TopBottomConnectorNoGap(QWidget* parent /*=0*/, Qt::WindowFlags f /*=0*/)
   : QWidget(parent, f)
   , mUI()
{
   //apply layout made with QtDesigner (see ui file)
   mUI.setupUi(this);
}

TopBottomConnectorNoGap::~TopBottomConnectorNoGap()
{
}
