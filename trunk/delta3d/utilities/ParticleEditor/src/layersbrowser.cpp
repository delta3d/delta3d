#include <layersbrowser.h>
#include <QtGui/QInputDialog>

///////////////////////////////////////////////////////////////////////////////
LayersBrowser::LayersBrowser()
{
}

///////////////////////////////////////////////////////////////////////////////
LayersBrowser::~LayersBrowser()
{
}

///////////////////////////////////////////////////////////////////////////////
void LayersBrowser::SetupUI()
{
   mpLayerList->addItem("Layer 0");
   mpLayerList->setCurrentRow(0);
   connect(mpNewLayerButton, SIGNAL(clicked()), this, SLOT(NewLayerButtonPushed()));
   connect(mpTrashLayerButton, SIGNAL(clicked()), this, SLOT(TrashLayerButtonPushed()));
   connect(mpRenameLayerButton, SIGNAL(clicked()), this, SLOT(RenameLayerButtonPushed()));
}

///////////////////////////////////////////////////////////////////////////////
void LayersBrowser::NewLayerButtonPushed()
{
   int rowCount = mpLayerList->count();
   QString newLayerLabel;
   newLayerLabel.sprintf("Layer %d", rowCount);
   mpLayerList->addItem(newLayerLabel);
   mpLayerList->setCurrentRow(rowCount);

   if(rowCount == 0)
   {
      // Reenable tabs and buttons
      mpTrashLayerButton->setEnabled(true);
      mpHideLayerButton->setEnabled(true);
      mpRenameLayerButton->setEnabled(true);
   }
}

///////////////////////////////////////////////////////////////////////////////
void LayersBrowser::TrashLayerButtonPushed()
{
   mpLayerList->takeItem(mpLayerList->currentRow());
   if(mpLayerList->count() < 1)
   {
      // Disable tabs and buttons
      mpTrashLayerButton->setEnabled(false);
      mpHideLayerButton->setEnabled(false);
      mpRenameLayerButton->setEnabled(false);
   }
}

///////////////////////////////////////////////////////////////////////////////
void LayersBrowser::RenameLayerButtonPushed()
{
   QString text = mpLayerList->currentItem()->text();
   QString newText = QInputDialog::getText(this, "Item", 
      "Enter new item", QLineEdit::Normal, text);
   newText = newText.simplified();
   if(!newText.isEmpty())
   {
      mpLayerList->currentItem()->setText(newText);
   }
}

///////////////////////////////////////////////////////////////////////////////
void LayersBrowser::ResetParticleButtonPushed()
{
}

///////////////////////////////////////////////////////////////////////////////
