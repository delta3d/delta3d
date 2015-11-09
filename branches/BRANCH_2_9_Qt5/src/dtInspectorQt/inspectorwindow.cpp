#include <dtInspectorQt/inspectorwindow.h>
#include <dtInspectorQt/actorview.h>
#include <dtInspectorQt/baseview.h>
#include <dtInspectorQt/cameraview.h>
#include <dtInspectorQt/clouddomeview.h>
#include <dtInspectorQt/deltawinview.h>
#include <dtInspectorQt/directorview.h>
#include <dtInspectorQt/drawableview.h>
#include <dtInspectorQt/environmentview.h>
#include <dtInspectorQt/infiniteterrainview.h>
#include <dtInspectorQt/labelactorview.h>
#include <dtInspectorQt/lightview.h>
#include <dtInspectorQt/objectview.h>
#include <dtInspectorQt/osgview.h>
#include <dtInspectorQt/particlesystemview.h>
#include <dtInspectorQt/sceneview.h>
#include <dtInspectorQt/skydomeview.h>
#include <dtInspectorQt/systemview.h>
#include <dtInspectorQt/transformableview.h>
#include <dtInspectorQt/viewview.h>
#include <dtInspectorQt/weatherview.h>

#include "ui_dtinspectorqt.h"

#include <dtCore/base.h>
#include <dtCore/uniqueid.h>

////////////////////////////////////////////////////////////////////////////////
dtInspectorQt::InspectorWindow::InspectorWindow(QWidget* parent /* = NULL */)
   : QMainWindow(parent)
   , mGameManager(NULL)
   , mFilterIndex(0)
{
   ui = new Ui::InspectorWidget();
   ui->setupUi(this);

   ActorView* actorView = new ActorView(*ui);
   CameraView* cameraView = new CameraView(*ui);
   DirectorView* directorView = new DirectorView(*ui);
   OSGView* osgView = new OSGView(*ui);
   mViewContainer.push_back(actorView);
   mViewContainer.push_back(cameraView);
   mViewContainer.push_back(directorView);
   mViewContainer.push_back(osgView);

   //mViewContainer.push_back(baseMgr);
   //mViewContainer.push_back(new ActorView(*ui));
   //mViewContainer.push_back(new CameraView(*ui));
   //mViewContainer.push_back(new CloudDomeView(*ui));
   //mViewContainer.push_back(new DeltaWinView(*ui));
   //mViewContainer.push_back(new DirectorView(*ui));
   //mViewContainer.push_back(new DrawableView(*ui));
   //mViewContainer.push_back(new EnvironmentView(*ui));
   //mViewContainer.push_back(new InfiniteTerrainView(*ui));
   //mViewContainer.push_back(new LabelActorView(*ui));
   //mViewContainer.push_back(new LightView(*ui));
   //mViewContainer.push_back(new ObjectView(*ui));
   //mViewContainer.push_back(new ParticleSystemView(*ui));
   //mViewContainer.push_back(new SceneView(*ui));
   //mViewContainer.push_back(new SkyDomeView(*ui));
   //mViewContainer.push_back(new SystemView(*ui));
   //mViewContainer.push_back(new TransformableView(*ui));
   //mViewContainer.push_back(new ViewView(*ui));
   //mViewContainer.push_back(new WeatherView(*ui));

   RefreshFilters();

   UpdateInstances();
   ui->itemList->setCurrentItem(ui->itemList->itemAt(0,0));
   RefreshCurrentItem();

   connect(actorView, SIGNAL(NameChanged(const QString&)), this, SLOT(OnNameChanged(const QString&)));
   connect(cameraView, SIGNAL(NameChanged(const QString&)), this, SLOT(OnNameChanged(const QString&)));
   connect(directorView, SIGNAL(NameChanged(const QString&)), this, SLOT(OnNameChanged(const QString&)));

   connect(ui->itemList, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(OnSelection(QTreeWidgetItem*,QTreeWidgetItem*)));
   connect(ui->actionRefresh_Item, SIGNAL(triggered()), this, SLOT(RefreshCurrentItem()));
   connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(close()));
   connect(ui->actionRebuild_List, SIGNAL(triggered()), this, SLOT(UpdateInstances()));
   connect(ui->actionSort_List, SIGNAL(toggled(bool)), this, SLOT(SortList(bool)));
   connect(ui->filterBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(FilterSelected(const QString&)));
}

////////////////////////////////////////////////////////////////////////////////
dtInspectorQt::InspectorWindow::~InspectorWindow()
{
   while (!mViewContainer.empty())
   {
      delete mViewContainer.takeFirst();
   }
   delete ui;
}

////////////////////////////////////////////////////////////////////////////////
QWidget* dtInspectorQt::InspectorWindow::GetPropertyContainerWidget()
{
   return ui->scrollAreaWidgetContents;
}

////////////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InspectorWindow::AddCustomView(IView* customView)
{
   mViewContainer.push_back(customView);
   customView->SetGameManager(mGameManager);
   RefreshFilters();
   UpdateInstances();
   RefreshCurrentItem();
}

////////////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InspectorWindow::SetGameManager(dtGame::GameManager* gm)
{
   mGameManager = gm;

   int count = mViewContainer.size();
   for (int index = 0; index < count; ++index)
   {
      IView* view = mViewContainer[index];
      if (view)
      {
         view->SetGameManager(gm);
      }
   }

   UpdateInstances();
   ui->itemList->setCurrentItem(ui->itemList->itemAt(0,0));
   RefreshCurrentItem();
}

////////////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InspectorWindow::OnSelection(QTreeWidgetItem* current, QTreeWidgetItem* prev)
{
   RefreshCurrentItem();
}

////////////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InspectorWindow::RefreshCurrentItem()
{
   ui->baseGroupBox->hide();
   ui->drawableGroupBox->hide();
   ui->transformableGroupBox->hide();
   ui->winGroupBox->hide();
   ui->cameraGroupBox->hide();
   ui->objectGroupBox->hide();
   ui->sceneGroupBox->hide();
   ui->viewGroupBox->hide();
   ui->envGroupBox->hide();
   ui->particleSystemGroupBox->hide();
   ui->lightGroupBox->hide();
   ui->positionalLightGroupBox->hide();
   ui->spotlightGroupBox->hide();
   ui->sysGroupBox->hide();
   ui->infTerGroupBox->hide();
   ui->weatherGroupBox->hide();
   ui->cloudDomeGroupBox->hide();
   ui->skyDomeGroupBox->hide();
   ui->labelActorGroupBox->hide();
   ui->directorScriptGroupBox->hide();

   QTreeWidgetItem* currentItem = ui->itemList->currentItem();
   if (currentItem == NULL) { return; }

   QVariant itemData = currentItem->data(0, 100);

   int count = mViewContainer.size();
   for (int index = 0; index < count; ++index)
   {
      IView* view = mViewContainer[index];
      if (view)
      {
         view->OperateOn(itemData);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InspectorWindow::OnNameChanged(const QString& text)
{
   QTreeWidgetItem* item = ui->itemList->currentItem();
   item->setText(0, text);
}

////////////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InspectorWindow::UpdateInstances()
{
   ui->itemList->clear(); //remove any previous entries

   QList<IView::EntryData> itemList;

   if (mFilterIndex > -1 && mFilterIndex < mViewContainer.size())
   {
      IView* view = mViewContainer[mFilterIndex];
      if (view)
      {
         view->Build(itemList);
      }
   }
   else
   {
      int count = mViewContainer.size();
      for (int index = 0; index < count; ++index)
      {
         IView* view = mViewContainer[index];
         if (view)
         {
            view->Build(itemList);
         }
      }
   }

   int count = itemList.size();
   for (int index = 0; index < count; ++index)
   {
      IView::EntryData& data = itemList[index];
      BuildItemTree(NULL, data);
   }

   ui->itemList->resizeColumnToContents(0);
   ui->itemList->resizeColumnToContents(1);
}

////////////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InspectorWindow::SortList(bool sorted)
{
   ui->itemList->setSortingEnabled(sorted);

   if (sorted)
   {
      ui->itemList->sortByColumn(0, Qt::AscendingOrder);
   }

   UpdateInstances();
   ui->itemList->setCurrentItem(ui->itemList->itemAt(0,0));
   RefreshCurrentItem();
}

////////////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InspectorWindow::RefreshFilters()
{
   ui->filterBox->clear();

   ui->filterBox->addItem("All");

   for (int i = 0; i < mViewContainer.size(); ++i)
   {
      ui->filterBox->addItem(mViewContainer.at(i)->mFilterName, QVariant(i));
   }
}

////////////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InspectorWindow::FilterSelected(const QString& text)
{
   mFilterIndex = ui->filterBox->currentIndex() - 1;
   UpdateInstances();
   ui->itemList->setCurrentItem(ui->itemList->itemAt(0,0));
   RefreshCurrentItem();
}

////////////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InspectorWindow::BuildItemTree(QTreeWidgetItem* parent, IView::EntryData& data)
{
   QTreeWidgetItem* item = NULL;
   if (!parent)
   {
      item = new QTreeWidgetItem(ui->itemList);
   }
   else
   {
      item = new QTreeWidgetItem(parent);
   }

   if (item)
   {
      item->setText(0, data.name);
      item->setText(1, data.type);
      item->setData(0, 100, data.itemData);

      int count = data.children.count();
      for (int index = 0; index < count; ++index)
      {
         BuildItemTree(item, data.children[index]);
      }
   }
}

//////////////////////////////////////////////////////////////////////////

