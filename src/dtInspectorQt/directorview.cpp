#include <dtInspectorQt/directorview.h>
#include "ui_dtinspectorqt.h"

#include <dtInspectorQt/propertyeditor.h>
#include <dtDirectorQt/editornotifier.h>

#include <dtDirector/directortypefactory.h>

#include <dtCore/uniqueid.h>

#include <dtCore/project.h>
#include <dtCore/map.h>
#include <dtCore/baseactorobject.h>

#include <osgDB/FileNameUtils>


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
   dtDirector::DirectorTypeFactory* factory = dtDirector::DirectorTypeFactory::GetInstance();
   if (factory)
   {
      const std::vector<dtDirector::Director*>& scriptList = factory->GetScriptInstances();

      int count = (int)scriptList.size();
      for (int index = 0; index < count; ++index)
      {
         dtDirector::Director* director = scriptList[index];
         if (director && director->IsVisibleInInspector())
         {
            std::string fileName = director->GetScriptName();
            std::string contextDir = osgDB::convertFileNameToNativeStyle(dtCore::Project::GetInstance().GetContext()+"/directors/");
            contextDir = osgDB::getRealPath(contextDir);
            if (!fileName.empty())
            {
               fileName = osgDB::getRealPath(fileName);
            }
            fileName = dtUtil::FileUtils::GetInstance().RelativePath(contextDir, fileName);

            EntryData data;
            data.name = fileName.c_str();
            data.type = "Director Script";
            data.itemData = QVariant(director->GetID().ToString().c_str());
            BuildChildren(director, data.children);
            itemList.push_back(data);
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DirectorView::OperateOn(const QVariant& itemData)
{
   mOperateOn = NULL;

   dtDirector::DirectorTypeFactory* factory = dtDirector::DirectorTypeFactory::GetInstance();
   if (factory)
   {
      const std::vector<dtDirector::Director*>& scriptList = factory->GetScriptInstances();

      int count = (int)scriptList.size();
      for (int index = 0; index < count; ++index)
      {
         dtDirector::Director* director = scriptList[index];
         if (director && itemData.toString().toStdString() == director->GetID().ToString())
         {
            mOperateOn = director;
            break;
         }
      }
   }

   Update();
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DirectorView::OnNameChange(const QString& text)
{
   if (mOperateOn.valid())
   {
      //mOperateOn->SetName(text.toStdString());
      //emit NameChanged(text);
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
         mOperateOn->GetNotifier());

      if (!notifier)
      {
         notifier = new dtDirector::EditorNotifier();
      }

      notifier->AddEditor(editor);

      mOperateOn->SetNotifier(notifier);

      editor->SetDirector(mOperateOn.get());
      editor->show();
   }
}

////////////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DirectorView::BuildChildren(dtDirector::Director* parent, QList<EntryData>& childList)
{
   if (!parent)
   {
      return;
   }

   const std::vector<dtCore::ObserverPtr<dtDirector::Director> >& children = parent->GetChildren();

   int count = (int)children.size();
   for (int index = 0; index < count; ++index)
   {
      dtDirector::Director* child = children[index].get();
      if (child && !child->IsImported())
      {
         std::string fileName = child->GetScriptName();
         std::string contextDir = osgDB::convertFileNameToNativeStyle(dtCore::Project::GetInstance().GetContext()+"/directors/");
         contextDir = osgDB::getRealPath(contextDir);
         if (!fileName.empty())
         {
            fileName = osgDB::getRealPath(fileName);
         }
         fileName = dtUtil::FileUtils::GetInstance().RelativePath(contextDir, fileName);

         EntryData data;
         data.name = fileName.c_str();
         data.type = "Director Script";
         data.itemData = QVariant(child->GetID().ToString().c_str());
         BuildChildren(child, data.children);
         childList.push_back(data);
      }
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DirectorView::Update()
{
   if (mOperateOn.valid())
   {
      mUI->baseGroupBox->show();
      mUI->baseNameText->setText(QString::fromStdString(mOperateOn->GetScriptName()));
      mUI->baseRefCountLabel->setText(QString::number(mOperateOn->referenceCount()));

      mUI->directorScriptGroupBox->show();

      dtCore::BaseActorObject* player = mOperateOn->GetPlayerActor();
      if (player)
      {
         mUI->directorScriptPlayerEdit->setText(player->GetName().c_str());
      }
      else
      {
         mUI->directorScriptPlayerEdit->setText("<None>");
      }

      dtCore::BaseActorObject* owner = mOperateOn->GetScriptOwnerActor();
      if (owner)
      {
         mUI->directorScriptOwnerEdit->setText(owner->GetName().c_str());
      }
      else
      {
         mUI->directorScriptOwnerEdit->setText("<None>");
      }

      std::vector<dtCore::RefPtr<dtCore::PropertyContainer> > actorList;
      actorList.push_back(mOperateOn.get());
      mUI->propertyEditor->HandlePropertyContainersSelected(actorList);
   }
   else
   {
      mUI->directorScriptGroupBox->hide();
   }
}

////////////////////////////////////////////////////////////////////////////////
