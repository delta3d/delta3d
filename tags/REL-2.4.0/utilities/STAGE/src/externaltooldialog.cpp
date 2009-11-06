#include <dtEditQt/externaltooldialog.h>
#include <dtEditQt/uiresources.h>
#include <dtEditQt/externaltoolargeditor.h>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <cassert>

using namespace dtEditQt;

dtEditQt::ExternalToolDialog::ExternalToolDialog(QList<ExternalTool*>& tools, QWidget* parent)
   : QDialog(parent)
   , mTools(&tools)
{
   mUI.setupUi(this);

   mUI.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
   mUI.buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
   mUI.extensionWidget->setVisible(false);

   SetupConnections();
   PopulateToolsUI();
}

dtEditQt::ExternalToolDialog::~ExternalToolDialog()
{
   DeleteToolWidgets();
}

void dtEditQt::ExternalToolDialog::SetupConnections()
{
   connect(mUI.addButton,        SIGNAL(clicked()),                  this, SLOT(OnNewTool()));
   connect(mUI.deleteButton,     SIGNAL(clicked()),                  this, SLOT(OnRemoveTool()));
   connect(mUI.toolList,         SIGNAL(itemSelectionChanged()),     this, SLOT(OnToolSelectionChanged()));
   connect(mUI.buttonBox->button(QDialogButtonBox::Apply),
                                SIGNAL(clicked()),                  this, SLOT(OnApplyChanges()));
   connect(mUI.titleEdit,        SIGNAL(textEdited(const QString&)), this, SLOT(OnStringChanged(const QString&)));
   connect(mUI.commandEdit,      SIGNAL(textEdited(const QString&)), this, SLOT(OnStringChanged(const QString&)));
   connect(mUI.commandButton,    SIGNAL(clicked()),                  this, SLOT(OnFindCommandFile()));
   connect(mUI.argsEdit,         SIGNAL(textEdited(const QString&)), this, SLOT(OnStringChanged(const QString&)));
   connect(mUI.argsButton,       SIGNAL(clicked()),                  this, SLOT(OnEditArgs()));
   connect(mUI.workingDirEdit,   SIGNAL(textEdited(const QString&)), this, SLOT(OnStringChanged(const QString&)));
   connect(mUI.workingDirButton, SIGNAL(clicked()),                  this, SLOT(OnFindWorkingDir()));
   connect(mUI.moveDownButton,   SIGNAL(clicked()),                  this, SLOT(OnMoveToolDown()));
   connect(mUI.moveUpButton,     SIGNAL(clicked()),                  this, SLOT(OnMoveToolUp()));
   connect(mUI.iconFileEdit,     SIGNAL(textEdited(const QString&)), this, SLOT(OnStringChanged(const QString&)));
   connect(mUI.iconDirButton,    SIGNAL(clicked()),                  this, SLOT(OnFindIconFile()));
   connect(mUI.resetButton,      SIGNAL(clicked()),                  this, SLOT(OnResetToDefaultTools()));
}


//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolDialog::OnNewTool()
{
   ExternalTool* tool = NULL;

   // find the first ExternalTool that has a non-visible QAction.  We'll edit that one.
   for (int toolIdx = 0; toolIdx < mTools->size(); ++toolIdx)
   {
      if (mTools->at(toolIdx)->GetAction()->isVisible() == false)
      {
         tool = mTools->at(toolIdx);
         break;
      }
   }

   if (tool == NULL)
   {
      // no ExternalTools are available for editing.  We must have reached
      // the max.
      QMessageBox::information(this, tr("Tools"),
                                     tr("Maximum number of external tools reached.  Try deleting tools not used."));
      return;
   }

   assert(tool);

   ResetTool(*tool);
   tool->GetAction()->setVisible(true);

   QListWidgetItem* item = new QListWidgetItem(tool->GetTitle(), mUI.toolList);
   mUI.toolList->setCurrentItem(item); //make it the currently selected item

   SetOkButtonEnabled(true);
   SetModifyButtonsEnabled(false);
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolDialog::OnRemoveTool()
{
   if (mUI.toolList->currentItem() == NULL)
   {
      // nothing selected
      return;
   }

   ExternalTool* tool = GetSelectedTool();
   if (tool == NULL)
   {
      return;
   }

   mUI.toolList->takeItem(mUI.toolList->currentRow()); // remove it's widget
   ResetTool(*tool);
   tool->GetAction()->setVisible(false); // turn off the QAction
   SetOkButtonEnabled(true);

   // now select something that still exists.
   if (mUI.toolList->count() > 0)
   {
      mUI.toolList->setCurrentItem(mUI.toolList->item(0));
   }
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolDialog::PopulateToolsUI()
{
   for (int toolIdx = 0; toolIdx < mTools->size(); ++toolIdx)
   {
      // if the QAction is visible, that means its already been created and ready to edit.
      // Otherwise, don't create an item for the ExternalTool
      if (mTools->at(toolIdx)->GetAction()->isVisible())
      {
         QListWidgetItem* item = new QListWidgetItem(mTools->at(toolIdx)->GetTitle(),
                                                     mUI.toolList);
         item->setIcon(QIcon(mTools->at(toolIdx)->GetIcon()));
         mUI.toolList->addItem(item);
      }
   }

   //now select something that still exists.
   if (mUI.toolList->count() > 0)
   {
      mUI.toolList->setCurrentItem(mUI.toolList->item(0));
   }
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolDialog::OnToolSelectionChanged()
{
   const ExternalTool* tool = GetSelectedTool();
   if (tool == NULL)
   {
      mUI.titleEdit->setText(QString());
      mUI.commandEdit->setText(QString());
      return;
   }

   mUI.titleEdit->setText(tool->GetTitle());
   mUI.commandEdit->setText(tool->GetCmd());
   mUI.argsEdit->setText(tool->GetArgs());
   mUI.workingDirEdit->setText(tool->GetWorkingDir());
   mUI.iconFileEdit->setText(tool->GetIcon());
}

//////////////////////////////////////////////////////////////////////////
ExternalTool* dtEditQt::ExternalToolDialog::GetSelectedTool() const
{
   const QListWidgetItem* currentItem = mUI.toolList->currentItem();
   if (currentItem == NULL)
   {
      // nothing selected
      return NULL;
   }

   ExternalTool* tool = NULL;
   for (int i = 0; i < mTools->size(); ++i)
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
   QListWidgetItem* currentItem = mUI.toolList->currentItem();
   if (currentItem == NULL)
   {
      // nothing selected
      return;
   }

   ExternalTool* tool = GetSelectedTool();
   if (tool == NULL)
   {
      return;
   }

   currentItem->setText(mUI.titleEdit->text());
   currentItem->setIcon(QIcon(mUI.iconFileEdit->text()));
   tool->SetTitle(mUI.titleEdit->text());
   tool->SetCmd(mUI.commandEdit->text());
   tool->SetArgs(mUI.argsEdit->text());
   tool->SetWorkingDir(mUI.workingDirEdit->text());
   tool->SetIcon(mUI.iconFileEdit->text());

   mUI.buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);

   SetModifyButtonsEnabled(true);
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolDialog::SetOkButtonEnabled(bool enabled)
{
   mUI.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(enabled);
   mUI.buttonBox->button(QDialogButtonBox::Apply)->setEnabled(enabled);
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
   if (mUI.buttonBox->button(QDialogButtonBox::Apply)->isEnabled())
   {
      OnApplyChanges();
   }

   QDialog::accept();
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolDialog::SetModifyButtonsEnabled(bool enabled)
{
   mUI.addButton->setEnabled(enabled);
   mUI.deleteButton->setEnabled(enabled);
   mUI.moveDownButton->setEnabled(enabled);
   mUI.moveUpButton->setEnabled(enabled);
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
      mUI.commandEdit->setText(filename);
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
      mUI.workingDirEdit->setText(workingDir);
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
      mUI.iconFileEdit->setText(iconFile);
      OnToolModified();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolDialog::OnMoveToolDown()
{
   QListWidgetItem* currentItem = mUI.toolList->currentItem();
   if (currentItem == NULL)
   {
      // nothing selected
      assert(currentItem);
      return;
   }

   const int currentRow = mUI.toolList->row(currentItem);
   if (currentRow == mUI.toolList->count()-1)
   {
      // already at end of list
      return;
   }

   currentItem = mUI.toolList->takeItem(currentRow);
   mUI.toolList->insertItem(currentRow+1, currentItem);

   mTools->swap(currentRow, currentRow+1);
   mUI.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);

   mUI.toolList->setCurrentItem(currentItem); //reselect the item
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolDialog::OnMoveToolUp()
{
   QListWidgetItem* currentItem = mUI.toolList->currentItem();
   if (currentItem == NULL)
   {
      // nothing selected
      assert(currentItem);
      return;
   }

   const int currentRow = mUI.toolList->row(currentItem);
   if (currentRow == 0)
   {
      // already at top of list
      return;
   }

   currentItem = mUI.toolList->takeItem(currentRow);
   mUI.toolList->insertItem(currentRow-1, currentItem);

   mTools->swap(currentRow, currentRow-1);
   mUI.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);

   mUI.toolList->setCurrentItem(currentItem); // reselect the item
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
   for (int toolIdx = 0; toolIdx < mTools->size(); ++toolIdx)
   {
      ExternalTool* tool = mTools->at(toolIdx);
      ResetTool(*tool);
      tool->GetAction()->setVisible(false);
   }

   DeleteToolWidgets(); //delete the widgets representing the external tools in the list

   { // Viewer
      ExternalTool* viewer = mTools->at(0);
      viewer->SetTitle(tr("Object Viewer"));
      viewer->SetCmd(FindDelta3DTool("ObjectViewer"));
      viewer->SetIcon(QString::fromStdString(UIResources::ICON_EDITOR_VIEWER));
      viewer->GetAction()->setVisible(true);
   }

   { // animation viewer
      ExternalTool* animViewer = mTools->at(1);
      animViewer->SetTitle(tr("Animation Viewer"));
      animViewer->SetCmd(FindDelta3DTool("AnimationViewer"));
      animViewer->SetIcon(QString::fromStdString(UIResources::ICON_EDITOR_SKELETAL_MESH));
      animViewer->GetAction()->setVisible(true);
   }

   { // particle system editor
      ExternalTool* psEditor = mTools->at(2);
      psEditor->SetTitle(tr("Particle Editor"));
      psEditor->SetCmd(FindDelta3DTool("ParticleEditor"));
      psEditor->SetIcon(QString::fromStdString(UIResources::ICON_EDITOR_PARTICLE_SYSTEM));
      psEditor->GetAction()->setVisible(true);
   }

   { // game start
      ExternalTool* gameStart = mTools->at(3);
      gameStart->SetTitle(tr("GameStart"));
      gameStart->SetCmd(FindDelta3DTool("GameStart"));
      gameStart->SetIcon(QString::fromStdString(UIResources::ICON_APPLICATION));
      gameStart->GetAction()->setVisible(true);
   }

   PopulateToolsUI(); // refresh the UI list
   mUI.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolDialog::DeleteToolWidgets()
{
   while (mUI.toolList->count() > 0)
   {
      QListWidgetItem* item = mUI.toolList->takeItem(0);
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

   // show arg editor dialog
   ExternalToolArgEditor diag(tool->GetArgParsers(), tool->GetArgs(), this);
   int retCode = diag.exec();

   if (retCode == QDialog::Accepted)
   {
      // get arg text from dialog
      const QString args = diag.GetArgs();

      // set the args on tool, argsEdit
      tool->SetArgs(args);
      mUI.argsEdit->setText(args);

      SetOkButtonEnabled(true);
   }
}
