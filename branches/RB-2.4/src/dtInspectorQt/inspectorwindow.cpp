#include <dtInspectorQt/inspectorwindow.h>
#include <dtInspectorQt/baseview.h>
#include <dtInspectorQt/cameraview.h>
#include <dtInspectorQt/clouddomeview.h>
#include <dtInspectorQt/deltawinview.h>
#include <dtInspectorQt/drawableview.h>
#include <dtInspectorQt/environmentview.h>
#include <dtInspectorQt/infiniteterrainview.h>
#include <dtInspectorQt/labelactorview.h>
#include <dtInspectorQt/lightview.h>
#include <dtInspectorQt/objectview.h>
#include <dtInspectorQt/particlesystemview.h>
#include <dtInspectorQt/physicalview.h>
#include <dtInspectorQt/sceneview.h>
#include <dtInspectorQt/skydomeview.h>
#include <dtInspectorQt/systemview.h>
#include <dtInspectorQt/transformableview.h>
#include <dtInspectorQt/viewview.h>
#include <dtInspectorQt/weatherview.h>

#include "ui_dtinspectorqt.h"

#include <dtCore/base.h>
#include <dtCore/uniqueid.h>


dtInspectorQt::InspectorWindow::InspectorWindow(QWidget* parent /* = NULL */)
: QMainWindow(parent)
{
   ui = new Ui::InspectorWidget();
   ui->setupUi(this);

   BaseView* baseMgr = new BaseView(*ui);
   mViewContainer.push_back(baseMgr);
   mViewContainer.push_back(new CameraView(*ui));
   mViewContainer.push_back(new CloudDomeView(*ui));
   mViewContainer.push_back(new DeltaWinView(*ui));
   mViewContainer.push_back(new DrawableView(*ui));
   mViewContainer.push_back(new EnvironmentView(*ui));
   mViewContainer.push_back(new InfiniteTerrainView(*ui));
   mViewContainer.push_back(new LabelActorView(*ui));
   mViewContainer.push_back(new LightView(*ui));
   mViewContainer.push_back(new ObjectView(*ui));
   mViewContainer.push_back(new ParticleSystemView(*ui));
   mViewContainer.push_back(new PhysicalView(*ui));
   mViewContainer.push_back(new SceneView(*ui));
   mViewContainer.push_back(new SkyDomeView(*ui));
   mViewContainer.push_back(new SystemView(*ui));
   mViewContainer.push_back(new TransformableView(*ui));
   mViewContainer.push_back(new ViewView(*ui));
   mViewContainer.push_back(new WeatherView(*ui));

   RefreshFilters();

   mFilterName = baseMgr->mFilterName;

   UpdateInstances();
   ui->itemList->setCurrentRow(0);
   RefreshCurrentItem();

   connect(ui->itemList, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(OnSelection(QListWidgetItem*,QListWidgetItem*)));
   connect(ui->actionRefresh_Item, SIGNAL(triggered()), this, SLOT(RefreshCurrentItem()));
   connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(close()));
   connect(ui->actionRebuild_List, SIGNAL(triggered()), this, SLOT(UpdateInstances()));
   connect(ui->actionSort_List, SIGNAL(toggled(bool)), this, SLOT(SortList(bool)));
   connect(baseMgr, SIGNAL(NameChanged(const QString&)), this, SLOT(OnNameChanged(const QString&)));
   connect(ui->filterBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(FilterSelected(const QString&)));
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::InspectorWindow::~InspectorWindow()
{
   while (!mViewContainer.empty())
   {
      delete mViewContainer.takeFirst();
   }
   delete ui;
}

//////////////////////////////////////////////////////////////////////////
QWidget* dtInspectorQt::InspectorWindow::GetPropertyContainerWidget()
{
   return ui->scrollAreaWidgetContents;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InspectorWindow::AddCustomView(IView* customView)
{
   mViewContainer.push_back(customView);
   RefreshFilters();
   UpdateInstances();
   RefreshCurrentItem();
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InspectorWindow::OnSelection(QListWidgetItem* current, QListWidgetItem* prev)
{
   RefreshCurrentItem();
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InspectorWindow::RefreshCurrentItem()
{
   QListWidgetItem* currentItem = ui->itemList->currentItem();
   if (currentItem == NULL) { return; }

   dtCore::Base* b = dtCore::Base::GetInstance(currentItem->text().toStdString());

   for (int i = 0; i < mViewContainer.size(); ++i)
   {
      mViewContainer.at(i)->OperateOn(b);
   }
}
//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InspectorWindow::OnNameChanged(const QString& text)
{
   QListWidgetItem* item = ui->itemList->currentItem();
   item->setText(text);
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InspectorWindow::UpdateInstances()
{
   ui->itemList->clear(); //remove any previous entries

   for (int i = 0; i < dtCore::Base::GetInstanceCount(); i++)
   {
      dtCore::Base *o = dtCore::Base::GetInstance(i);

      for (int j = 0; j < mViewContainer.size(); ++j)
      {
         if (mViewContainer.at(j)->IsOfType(mFilterName, o))
         {
            QListWidgetItem* item = new QListWidgetItem();
            item->setText(QString::fromStdString(o->GetName()));

            ui->itemList->addItem(item);
            break;
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InspectorWindow::SortList(bool sorted)
{
   ui->itemList->setSortingEnabled(sorted);
   UpdateInstances();
   ui->itemList->setCurrentRow(0);
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InspectorWindow::RefreshFilters()
{
   ui->filterBox->clear();

   for (int i = 0; i < mViewContainer.size(); ++i)
   {
      ui->filterBox->addItem(mViewContainer.at(i)->mFilterName);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InspectorWindow::FilterSelected(const QString& text)
{
   mFilterName = text;
   UpdateInstances();
   ui->itemList->setCurrentRow(0);
}

//////////////////////////////////////////////////////////////////////////

