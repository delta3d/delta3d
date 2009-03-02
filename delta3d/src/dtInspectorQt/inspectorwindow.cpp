#include <dtInspectorQt/inspectorwindow.h>
#include <dtInspectorQt/basemanager.h>
#include <dtInspectorQt/cameramanager.h>
#include <dtInspectorQt/deltawinmanager.h>
#include <dtInspectorQt/drawablemanager.h>
#include <dtInspectorQt/environmentmanager.h>
#include <dtInspectorQt/particlesystemmanager.h>
#include <dtInspectorQt/physicalmanager.h>
#include <dtInspectorQt/scenemanager.h>
#include <dtInspectorQt/transformablemanager.h>
#include <dtInspectorQt/viewmanager.h>

#include <dtCore/base.h>
#include <dtCore/uniqueid.h>


dtInspectorQt::InspectorWindow::InspectorWindow(QWidget* parent /* = NULL */)
: QMainWindow(parent)
{
   ui.setupUi(this);

   BaseManager* baseMgr = new BaseManager(ui);
   mManagerContainer.push_back(baseMgr);
   mManagerContainer.push_back(new CameraManager(ui));
   mManagerContainer.push_back(new DeltaWinManager(ui));
   mManagerContainer.push_back(new DrawableManager(ui));
   mManagerContainer.push_back(new EnvironmentManager(ui));
   mManagerContainer.push_back(new ParticleSystemManager(ui));
   mManagerContainer.push_back(new PhysicalManager(ui));
   mManagerContainer.push_back(new SceneManager(ui));
   mManagerContainer.push_back(new TransformableManager(ui));
   mManagerContainer.push_back(new ViewManager(ui));

   UpdateInstances();

   connect(ui.itemList, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(OnSelection(QListWidgetItem*,QListWidgetItem*)));
   connect(baseMgr, SIGNAL(NameChanged(const QString&)), this, SLOT(OnNameChanged(const QString&)));
}

dtInspectorQt::InspectorWindow::~InspectorWindow()
{
   while (!mManagerContainer.empty())
   {
      delete mManagerContainer.takeFirst();
   }
}

void dtInspectorQt::InspectorWindow::UpdateInstances()
{
   for (int i=0; i<dtCore::Base::GetInstanceCount(); i++)
   {
      dtCore::Base *o = dtCore::Base::GetInstance(i);
      QListWidgetItem* item = new QListWidgetItem();
      item->setText(QString::fromStdString(o->GetName()));

      ui.itemList->addItem(item);
   }

}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InspectorWindow::OnSelection(QListWidgetItem* current, QListWidgetItem* prev)
{
   int row = ui.itemList->currentRow();

   dtCore::Base* b = dtCore::Base::GetInstance(row);

   for (int i=0; i<mManagerContainer.size(); ++i)
   {
      mManagerContainer.at(i)->OperateOn(b);
   }
}



//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InspectorWindow::OnNameChanged(const QString& text)
{
   QListWidgetItem* item = ui.itemList->currentItem();
   item->setText(text);
}

