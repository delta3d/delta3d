#include <prefix/stageprefix.h>
#include <dtEditQt/shaderbrowser.h>
#include "dtEditQt/resourcetreewidget.h"

////////////////////////////////////////////////////////////////////////////////
dtEditQt::ShaderBrowser::ShaderBrowser(dtCore::DataType& type, QWidget* parent):
 ResourceAbstractBrowser(&type, parent)
{
   mGrid = new QGridLayout(this);

   mGrid->addWidget(mTree,0,0);
   mGrid->addWidget(standardButtons(QString("Resource Tools")), 2, 0, Qt::AlignCenter);

   mTree->setResourceName("Shader");

}

////////////////////////////////////////////////////////////////////////////////
dtEditQt::ShaderBrowser::~ShaderBrowser()
{

}
