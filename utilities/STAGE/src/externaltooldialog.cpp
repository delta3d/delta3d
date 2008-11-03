#include <dtEditQt/externaltooldialog.h>
#include <dtEditQt/uiresources.h>
#include <dtEditQt/externaltoolargeditor.h>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <cassert>

using namespace dtEditQt;

dtEditQt::ExternalToolDialog::ExternalToolDialog(QList<ExternalTool*> &tools, QWidget* parent)
: QDialog(parent)
, mTools(&tools)
{
   ui.setupUi(this);
   
   ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
   ui.buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
   ui.extensionWidget->setVisible(false);

   SetupConnections();
   PopulateToolsUI();
}

dtEditQt::ExternalToolDialog::~ExternalToolDialog()
{
   DeleteToolWidgets();

}

void dtEditQt::ExternalToolDialog::SetupConnections()
{
   connect(ui.addButton, SIGNAL(clicked()), this, SLOT(OnNewTool()));
   connect(ui.deleteButton, SIGNAL(clicked()), this, SLOT(OnRemoveTool()));
   connect(ui.toolList, SIGNAL(itemSelectionChanged()), this, SLOT(OnToolSelectionChanged()));
   connect(ui.buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(OnApplyChanges()));
   connect(ui.titleEdit, SIGNAL(textEdited(const QString&)), this, SLOT(OnStringChanged(const QString&)));
   connect(ui.commandEdit, SIGNAL(textEdited(const QString&)), this, SLOT(OnStringChanged(const QString&)));
   connect(ui.commandButton, SIGNAL(clicked()), this, SLOT(OnFindCommandFile()));
   connect(ui.argsEdit, SIGNAL(textEdited(const QString&)), this, SLOT(OnStringChanged(const QString&)));
   connect(ui.argsButton, SIGNAL(clicked()), this, SLOT(OnEditArgs()));
   connect(ui.workingDirEdit, SIGNAL(textEdited(const QString&)), this, SLOT(OnStringChanged(const QString&)));
   connect(ui.workingDirButton, SIGNAL(clicked()), this, SLOT(OnFindWorkingDir()));
   connect(ui.moveDownButton, SIGNAL(clicked()), this, SLOT(OnMoveToolDown()));
   connect(ui.moveUpButton, SIGNAL(clicked()), this, SLOT(OnMoveToolUp()));
   connect(ui.iconFileEdit, SIGNAL(textEdited(const QString&)), this, SLOT(OnStringChanged(const QString&)));
   connect(ui.iconDirButton, SIGNAL(clicked()), this, SLOT(OnFindIconFile()));
   connect(ui.resetButton, SIGNAL(clicked()), this, SLOT(OnResetToDefaultTools()));
}


//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolDialog::OnNewTool()
{
   ExternalTool *tool = NULL;

   //find the first ExternalTool that has a non-visible QAction.  We'll edit that one.
   for (int toolIdx=0; toolIdx<mTools->size(); toolIdx++)
   {
      if (mTools->at(toolIdx)->GetAction()->isVisible() == false)
      {
         tool = mTools->at(toolIdx);
         break;
      }
   }

   if (tool == NULL)
   {
      //no ExternalTools are available for editing.  We must have reached
      //the max.
      QMessageBox::information(this, tr("Tools"), 
                              tr("Maximum number of external tools reached.  Try deleting tools not used."));
      return;
   }

   assert(tool);   

   ResetTool(*tool);
   tool->GetAction()->setVisible(true);

   QListWidgetItem *item = new QListWidgetItem(tool->GetTitle(), ui.toolList);
   ui.toolList->setCurrentItem(item); //make it the currently selected item
   
   SetOkButtonEnabled(true);
   SetModifyButtonsEnabled(false);
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolDialog::OnRemoveTool()
{
   if (ui.toolList->currentItem() == NULL)
   {
      //nothing selected
      return;
   }

   ExternalTool* tool = GetSelectedTool();
   if (tool == NULL)
   {
      return;
   }

   ui.toolList->takeItem(ui.toolList->currentRow()); //remove it's widget
   ResetTool(*tool);
   tool->GetAction()->setVisible(false); //turn off the QAction
   SetOkButtonEnabled(true);
   
   //now select something that still exists.
   if (ui.toolList->count() > 0)
   {
      ui.toolList->setCurrentItem(ui.toolList->item(0));
   }
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolDialog::PopulateToolsUI()
{
   for (int toolIdx=0; toolIdx<mTools->size(); toolIdx++)
   {
      //if the QAction is visible, that means its already been created and ready to edit.
      //Otherwise, don't create an item for the ExternalTool
      if (mTools->at(toolIdx)->GetAction()->isVisible())
      {
         QListWidgetItem *item = new QListWidgetItem(mTools->at(toolIdx)->GetTitle(),
                                                     ui.toolList);
         item->setIcon(QIcon(mTools->at(toolIdx)->GetIcon()));
         ui.toolList->addItem(item);
      }
   }

   //now select something that still exists.
   if (ui.toolList->count() > 0)
   {
      ui.toolList->setCurrentItem(ui.toolList->item(0));
   }
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolDialog::OnToolSelectionChanged()
{
   const ExternalTool* tool = GetSelectedTool();
   if (tool == NULL)
   { 
      ui.titleEdit->setText(QString());
      ui.commandEdit->setText(QString());
      return;
   }

   ui.titleEdit->setText(tool->GetTitle());
   ui.commandEdit->setText(tool->GetCmd());
   ui.argsEdit->setText(tool->GetArgs());
   ui.workingDirEdit->setText(tool->GetWorkingDir());
   ui.iconFileEdit->setText(tool->GetIcon());
}

//////////////////////////////////////////////////////////////////////////
ExternalTool* dtEditQt::ExternalToolDialog::GetSelectedTool() const
{
   const QListWidgetItem *currentItem = ui.toolList->currentItem();
   if (currentItem == NULL)
   {
      //nothing selected
      return NULL;
   }

   ExternalTool* tool = NULL;
   for (int i=0; i<mTools->size(); ++i)
   {
      if (mTools->at(i)->GetTitle() == currentItem->text())
      {
         tool = mTools->at(i);
         break;
      }
   }

   return tool;
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolDialog::OnApplyChanges()
{
   QListWidgetItem *currentItem = ui.toolList->currentItem();
   if (currentItem == NULL)
   {
      //nothing selected
      return;
   }

   ExternalTool* tool = GetSelectedTool();
   if (tool == NULL)
   {
      return;
   }

   currentItem->setText(ui.titleEdit->text());
   currentItem->setIcon(QIcon(ui.iconFileEdit->text()));
   tool->SetTitle(ui.titleEdit->text());
   tool->SetCmd(ui.commandEdit->text());
   tool->SetArgs(ui.argsEdit->text());
   tool->SetWorkingDir(ui.workingDirEdit->text());
   tool->SetIcon(ui.iconFileEdit->text());

   ui.buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
   
   SetModifyButtonsEnabled(true);
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolDialog::SetOkButtonEnabled(bool enabled)
{
   ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(enabled);
   ui.buttonBox->button(QDialogButtonBox::Apply)->setEnabled(enabled);
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolDialog::OnToolModified()
{
   SetOkButtonEnabled(true);
   SetModifyButtonsEnabled(false);
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolDialog::OnStringChanged(const QString& text)
{
   OnToolModified();
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolDialog::accept()
{
   if (ui.buttonBox->button(QDialogButtonBox::Apply)->isEnabled())
   {
      OnApplyChanges();
   }
   
   QDialog::accept();
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolDialog::SetModifyButtonsEnabled(bool enabled)
{
   ui.addButton->setEnabled(enabled);
   ui.deleteButton->setEnabled(enabled);
   ui.moveDownButton->setEnabled(enabled);
   ui.moveUpButton->setEnabled(enabled);
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolDialog::OnFindCommandFile()
{
   ExternalTool* tool = GetSelectedTool();

   if (tool == NULL)
   {
      return;
   }

   //pop open a file dialog and query for a filename
   const QString filename = QFileDialog::getOpenFileName(this,
                     tr("Get File"),
                     QFileInfo(tool->GetCmd()).path());

   if (!filename.isEmpty())
   {
      tool->SetCmd(filename);
      ui.commandEdit->setText(filename);
      OnToolModified();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolDialog::OnFindWorkingDir()
{
   ExternalTool* tool = GetSelectedTool();

   if (tool == NULL)
   {
      return;
   }

   //pop open a file dialog and query for a workingDir
   const QString workingDir = QFileDialog::getExistingDirectory(this,
                              tr("Get Directory"),
                              tool->GetWorkingDir());

   if (!workingDir.isEmpty())
   {
      tool->SetWorkingDir(workingDir);
      ui.workingDirEdit->setText(workingDir);
      OnToolModified();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolDialog::OnFindIconFile()
{
   ExternalTool* tool = GetSelectedTool();

   if (tool == NULL)
   {
      return;
   }

   //pop open a file dialog and query for an icon file
   const QString iconFile = QFileDialog::getOpenFileName(this,
                                 tr("Get Icon file"),
                                 QFileInfo(tool->GetIcon()).path());

   if (!iconFile.isEmpty())
   {
      tool->SetIcon(iconFile);
      ui.iconFileEdit->setText(iconFile);
      OnToolModified();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolDialog::OnMoveToolDown()
{
   QListWidgetItem *currentItem = ui.toolList->currentItem();
   if (currentItem == NULL)
   {
      //nothing selected
      assert(currentItem);
      return;
   }

   const int currentRow = ui.toolList->row(currentItem);
   if (currentRow == ui.toolList->count()-1)
   { 
      //already at end of list
      return;
   }

   currentItem = ui.toolList->takeItem(currentRow);
   ui.toolList->insertItem(currentRow+1, currentItem);

   mTools->swap(currentRow, currentRow+1);
   ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);

   ui.toolList->setCurrentItem(currentItem); //reselect the item
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolDialog::OnMoveToolUp()
{
   QListWidgetItem *currentItem = ui.toolList->currentItem();
   if (currentItem == NULL)
   {
      //nothing selected
      assert(currentItem);
      return;
   }

   const int currentRow = ui.toolList->row(currentItem);
   if (currentRow == 0)
   { 
      //already at top of list
      return;
   }

   currentItem = ui.toolList->takeItem(currentRow);
   ui.toolList->insertItem(currentRow-1, currentItem);

   mTools->swap(currentRow, currentRow-1);
   ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);

   ui.toolList->setCurrentItem(currentItem); //reselect the item
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolDialog::ResetTool(ExternalTool& tool) const
{
   tool.SetTitle("Unnamed");
   tool.SetCmd("");
   tool.SetArgs("");
   tool.SetWorkingDir("");
   tool.SetIcon("");
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolDialog::OnResetToDefaultTools()
{
   QMessageBox::StandardButton b = QMessageBox::question(this, tr("Reset External Tools?"),
      tr("Delete existing external tool configurations and add default tools?"),
      QMessageBox::Yes|QMessageBox::No);

   if (b == QMessageBox::Yes)
   {
      ClearAndAddDefaultDelta3DTools();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolDialog::ClearAndAddDefaultDelta3DTools()
{
   for (int toolIdx=0; toolIdx<mTools->size(); ++toolIdx)
   {
      ExternalTool *tool = mTools->at(toolIdx); 
      ResetTool(*tool);
      tool->GetAction()->setVisible(false);
   }

   DeleteToolWidgets(); //delete the widgets representing the external tools in the list

   {//Viewer
      ExternalTool* viewer = mTools->at(0);
      viewer->SetTitle(tr("Viewer"));
      viewer->SetCmd(FindDelta3DTool("Viewer"));
      viewer->SetIcon(QString::fromStdString(UIResources::ICON_EDITOR_VIEWER));
      viewer->GetAction()->setVisible(true);
   }

   {//animation viewer
      ExternalTool* animViewer = mTools->at(1);
      animViewer->SetTitle(tr("Animation Viewer"));
      animViewer->SetCmd(FindDelta3DTool("AnimationViewer"));
      animViewer->SetIcon(QString::fromStdString(UIResources::ICON_EDITOR_SKELETAL_MESH));
      animViewer->GetAction()->setVisible(true);
   }

   {//particle system editor
      ExternalTool* psEditor = mTools->at(2);
      psEditor->SetTitle(tr("Particle Editor"));
      psEditor->SetCmd(FindDelta3DTool("psEditor"));
      psEditor->SetIcon(QString::fromStdString(UIResources::ICON_EDITOR_PARTICLE_SYSTEM));
      psEditor->GetAction()->setVisible(true);
   }

   {//game start
      ExternalTool* gameStart = mTools->at(3);
      gameStart->SetTitle(tr("GameStart"));
      gameStart->SetCmd(FindDelta3DTool("GameStart"));
      gameStart->SetIcon(QString::fromStdString(UIResources::ICON_APPLICATION));
      gameStart->GetAction()->setVisible(true);
   }

   PopulateToolsUI(); //refresh the UI list
   ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolDialog::DeleteToolWidgets()
{
   while (ui.toolList->count() > 0)
   {
      QListWidgetItem* item = ui.toolList->takeItem(0);
      delete item;
   }
}

//////////////////////////////////////////////////////////////////////////
QString dtEditQt::ExternalToolDialog::FindDelta3DTool(const QString& baseName) const
{
#ifdef Q_WS_WIN
   QString ext(".exe");
#else
   QString ext("");
#endif

   QString fullPath(QCoreApplication::applicationDirPath() + "/" + baseName + ext);

   QFileInfo tool(fullPath);
   if (tool.exists() && tool.isExecutable())
   {
      return tool.absoluteFilePath();
   }
   else
   {
      return baseName;
   }
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolDialog::OnEditArgs()
{
   ExternalTool* tool = GetSelectedTool();
   if (tool == NULL)
   {
      return;
   }

   //show arg editor dialog
   ExternalToolArgEditor diag(tool->GetArgParsers(), tool->GetArgs(), this);
   int retCode = diag.exec();

   if (retCode == QDialog::Accepted)
   {
      //get arg text from dialog
      const QString args = diag.GetArgs();

      //set the args on tool, argsEdit
      tool->SetArgs(args);
      ui.argsEdit->setText(args);

      SetOkButtonEnabled(true);
   }   
}
