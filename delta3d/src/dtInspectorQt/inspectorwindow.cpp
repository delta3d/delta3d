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

#include <dtCore/base.h>
#include <dtCore/uniqueid.h>


dtInspectorQt::InspectorWindow::InspectorWindow(QWidget* parent /* = NULL */)
: QMainWindow(parent)
{
   ui.setupUi(this);

   BaseView* baseMgr = new BaseView(ui);
   mViewContainer.push_back(baseMgr);
   mViewContainer.push_back(new CameraView(ui));
   mViewContainer.push_back(new CloudDomeView(ui));
   mViewContainer.push_back(new DeltaWinView(ui));
   mViewContainer.push_back(new DrawableView(ui));
   mViewContainer.push_back(new EnvironmentView(ui));
   mViewContainer.push_back(new InfiniteTerrainView(ui));
   mViewContainer.push_back(new LabelActorView(ui));
   mViewContainer.push_back(new LightView(ui));
   mViewContainer.push_back(new ObjectView(ui));
   mViewContainer.push_back(new ParticleSystemView(ui));
   mViewContainer.push_back(new PhysicalView(ui));
   mViewContainer.push_back(new SceneView(ui));
   mViewContainer.push_back(new SkyDomeView(ui));
   mViewContainer.push_back(new SystemView(ui));
   mViewContainer.push_back(new TransformableView(ui));
   mViewContainer.push_back(new ViewView(ui));
   mViewContainer.push_back(new WeatherView(ui));

   UpdateInstances();

   connect(ui.itemList, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(OnSelection(QListWidgetItem*,QListWidgetItem*)));
   connect(baseMgr, SIGNAL(NameChanged(const QString&)), this, SLOT(OnNameChanged(const QString&)));
}

dtInspectorQt::InspectorWindow::~InspectorWindow()
{
   while (!mViewContainer.empty())
   {
      delete mViewContainer.takeFirst();
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

   for (int i=0; i<mViewContainer.size(); ++i)
   {
      mViewContainer.at(i)->OperateOn(b);
   }
}



//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InspectorWindow::OnNameChanged(const QString& text)
{
   QListWidgetItem* item = ui.itemList->currentItem();
   item->setText(text);
}

