#include <dtInspectorQt/osgview.h>
#include "ui_dtinspectorqt.h"

#include <dtCore/scene.h>
#include <dtGame/gameapplication.h>
#include <dtGame/gamemanager.h>

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::OSGView::OSGView(Ui::InspectorWidget& ui)
   : mUI(&ui)
{
   mFilterName = QString("OSG");
   connect(ui.baseNameText, SIGNAL(textEdited(const QString&)), this, SLOT(OnNameChange(const QString&)));
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::OSGView::~OSGView()
{
   mOperateOn = NULL;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::OSGView::Build(QList<EntryData>& itemList)
{
   if (!mGameManager)
   {
      return;
   }

   osg::Node* sceneNode = mGameManager->GetScene().GetSceneNode();

   while (sceneNode->getNumParents() == 1)
   {
      sceneNode = sceneNode->getParent(0);
   }

   EntryData data;
   data.name = sceneNode->getName().c_str();
   data.type = sceneNode->className();
   data.itemData = qVariantFromValue((void *)sceneNode); // QVariant(reinterpret_cast<int>(sceneNode));
   BuildChildren(sceneNode->asGroup(), data.children);
   itemList.push_back(data);

   //dtCore::ActorPtrVector actorList;
   //mGameManager->GetAllActors(actorList);

   //int count = (int)actorList.size();
   //for (int index = 0; index < count; ++index)
   //{
   //   dtCore::BaseActorObject* actor = actorList[index];
   //   if (actor)
   //   {
   //      EntryData data;
   //      data.name = actor->GetName().c_str();
   //      data.type = actor->GetActorType().GetFullName().c_str();
   //      data.itemData = QVariant(actor->GetId().ToString().c_str());
   //      itemList.push_back(data);
   //   }
   //}
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::OSGView::OperateOn(const QVariant& itemData)
{
   if (!mGameManager)
   {
      return;
   }

   //mOperateOn = NULL;
   //dtCore::UniqueId id;
   //id = itemData.toString().toStdString();

   //dtCore::BaseActorObject* actor = mGameManager->FindActorById(id);
   //if (actor)
   //{
   //   mOperateOn = actor;
   //}

   Update();
}

////////////////////////////////////////////////////////////////////////////////
void dtInspectorQt::OSGView::BuildChildren(osg::Group* parent, QList<EntryData>& childList)
{
   if (parent)
   {
      for (size_t childIndex = 0; childIndex < parent->getNumChildren(); ++childIndex)
      {
         osg::Node* currentNode = parent->getChild(childIndex);

         EntryData data;
         data.name = currentNode->getName().c_str();
         data.type = currentNode->className();
         data.itemData = qVariantFromValue((void *)currentNode);
         BuildChildren(currentNode->asGroup(), data.children);
         childList.push_back(data);
      }
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::OSGView::Update()
{
   if (mOperateOn.valid())
   {
      //mUI->baseGroupBox->show();
      //mUI->baseNameText->setText(QString::fromStdString(mOperateOn->GetName()));
      //mUI->baseRefCountLabel->setText(QString::number(mOperateOn->referenceCount()));

      //std::vector<dtCore::RefPtr<dtCore::PropertyContainer> > actorList;
      //actorList.push_back(mOperateOn.get());
      //mUI->propertyEditor->HandlePropertyContainersSelected(actorList);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::OSGView::OnNameChange(const QString& text)
{
   //if (mOperateOn.valid())
   //{
   //   mOperateOn->SetName(text.toStdString());
   //   emit NameChanged(text);
   //}
}

//////////////////////////////////////////////////////////////////////////
