#include <dtInspectorQt/directorview.h>
#include "ui_dtinspectorqt.h"

#include <dtInspectorQt/propertyeditor.h>
#include <dtDirectorQt/editornotifier.h>

#include <dtCore/uniqueid.h>

#include <dtDAL/project.h>
#include <dtDAL/map.h>
#include <dtDAL/baseactorobject.h>


//////////////////////////////////////////////////////////////////////////
dtInspectorQt::DirectorView::DirectorView(Ui::InspectorWidget& ui)
   :mUI(&ui)
{
   mFilterName = QString("Director");

   connect(mUI->directorScriptViewButton, SIGNAL(clicked()), this, SLOT(OnViewButtonClicked()));
   connect(ui.baseNameText, SIGNAL(textEdited(const QString&)), this, SLOT(OnNameChange(const QString&)));
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::DirectorView::~DirectorView()
{
   mOperateOn = NULL;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DirectorView::Build(QList<EntryData>& itemList)
{
   int count = dtDirector::DirectorInstance::GetInstanceCount();
   for (int index = 0; index < count; ++index)
   {
      dtDirector::DirectorInstance* item = dtDirector::DirectorInstance::GetInstance(index);
      if (item && !item->mDirector->GetParent())
      {
         EntryData data;
         data.name = item->GetName().c_str();
         data.type = "Director Script";
         data.itemData = QVariant(item->GetUniqueId().ToString().c_str());
         itemList.push_back(data);
      }
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DirectorView::OperateOn(const QVariant& itemData)
{
   mOperateOn = NULL;

   int count = dtDirector::DirectorInstance::GetInstanceCount();
   for (int index = 0; index < count; ++index)
   {
      dtDirector::DirectorInstance* item = dtDirector::DirectorInstance::GetInstance(index);
      if (item && itemData.toString().toStdString() == item->GetUniqueId().ToString())
      {
         mOperateOn = item;
         break;
      }
   }

   Update();
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DirectorView::Update()
{
   if (mOperateOn.valid())
   {
      mUI->baseGroupBox->show();
      mUI->baseNameText->setText(QString::fromStdString(mOperateOn->GetName()));
      mUI->baseRefCountLabel->setText(QString::number(mOperateOn->referenceCount()));

      mUI->directorScriptGroupBox->show();

      dtDAL::BaseActorObject* player = mOperateOn->mDirector->GetPlayerActor();
      if (player)
      {
         mUI->directorScriptPlayerEdit->setText(player->GetName().c_str());
      }
      else
      {
         mUI->directorScriptPlayerEdit->setText("<None>");
      }

      dtDAL::BaseActorObject* owner = mOperateOn->mDirector->GetScriptOwnerActor();
      if (owner)
      {
         mUI->directorScriptOwnerEdit->setText(owner->GetName().c_str());
      }
      else
      {
         mUI->directorScriptOwnerEdit->setText("<None>");
      }

      std::vector<dtCore::RefPtr<dtDAL::PropertyContainer> > actorList;
      actorList.push_back(mOperateOn->mDirector);
      mUI->propertyEditor->HandlePropertyContainersSelected(actorList);
   }
   else
   {
      mUI->directorScriptGroupBox->hide();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DirectorView::OnNameChange(const QString& text)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetName(text.toStdString());
      emit NameChanged(text);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DirectorView::OnViewButtonClicked()
{
   dtDirector::DirectorEditor* editor =
      new dtDirector::DirectorEditor();

   if (editor)
   {
      dtCore::RefPtr<dtDirector::EditorNotifier> notifier =
         dynamic_cast<dtDirector::EditorNotifier*>(
         mOperateOn->mDirector->GetNotifier());

      if (!notifier)
      {
         notifier = new dtDirector::EditorNotifier();
      }

      notifier->AddEditor(editor);

      mOperateOn->mDirector->SetNotifier(notifier);

      editor->SetDirector(mOperateOn->mDirector);
      editor->show();
   }
}

////////////////////////////////////////////////////////////////////////////////
