#include <dtInspectorQt/actorview.h>
#include "ui_dtinspectorqt.h"

#include <dtGame/gameapplication.h>
#include <dtGame/gamemanager.h>

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::ActorView::ActorView(Ui::InspectorWidget& ui)
: mUI(&ui)
{
   mFilterName = QString("Actors");
   connect(ui.baseNameText, SIGNAL(textEdited(const QString&)), this, SLOT(OnNameChange(const QString&)));
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::ActorView::~ActorView()
{
   mOperateOn = NULL;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::ActorView::Build(QList<EntryData>& itemList)
{
   if (!mGameManager)
   {
      return;
   }

   dtCore::ActorPtrVector actorList;
   mGameManager->GetAllActors(actorList);

   int count = (int)actorList.size();
   for (int index = 0; index < count; ++index)
   {
      dtCore::BaseActorObject* actor = actorList[index];
      if (actor)
      {
         EntryData data;
         data.name = actor->GetName().c_str();
         data.type = actor->GetActorType().GetFullName().c_str();
         data.itemData = QVariant(actor->GetId().ToString().c_str());
         itemList.push_back(data);
      }
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::ActorView::OperateOn(const QVariant& itemData)
{
   if (!mGameManager)
   {
      return;
   }

   mOperateOn = NULL;
   dtCore::UniqueId id;
   id = itemData.toString().toStdString();

   dtCore::BaseActorObject* actor = mGameManager->FindActorById(id);
   if (actor)
   {
      mOperateOn = actor;
   }

   Update();
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::ActorView::Update()
{
   if (mOperateOn.valid())
   {
      mUI->baseGroupBox->show();
      mUI->baseNameText->setText(QString::fromStdString(mOperateOn->GetName()));
      mUI->baseRefCountLabel->setText(QString::number(mOperateOn->referenceCount()));

      std::vector<dtCore::RefPtr<dtCore::PropertyContainer> > actorList;
      actorList.push_back(mOperateOn.get());
      mUI->propertyEditor->HandlePropertyContainersSelected(actorList);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::ActorView::OnNameChange(const QString& text)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetName(text.toStdString());
      emit NameChanged(text);
   }
}

//////////////////////////////////////////////////////////////////////////
