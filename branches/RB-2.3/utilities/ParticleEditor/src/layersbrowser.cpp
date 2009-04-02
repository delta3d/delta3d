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
   connect(mpLayerList, SIGNAL(currentRowChanged(int)), this, SLOT(SelectIndexOfLayersList(int)));
   connect(mpNewLayerButton, SIGNAL(clicked()), this, SLOT(NewLayerButtonPushed()));
   connect(mpTrashLayerButton, SIGNAL(clicked()), this, SLOT(TrashLayerButtonPushed()));
   connect(mpRenameLayerButton, SIGNAL(clicked()), this, SLOT(RenameLayerButtonPushed()));
}

///////////////////////////////////////////////////////////////////////////////
void LayersBrowser::ClearLayerList()
{
   mpLayerList->clear();
   ToggleLayers(false);
}

///////////////////////////////////////////////////////////////////////////////
void LayersBrowser::AddLayerToLayerList(const QString &layerName)
{
   mpLayerList->addItem(layerName);
}

///////////////////////////////////////////////////////////////////////////////
void LayersBrowser::SelectIndexOfLayersList(int newIndex)
{
   if(0 <= newIndex && newIndex < mpLayerList->count())
   {
      mpLayerList->setCurrentRow(newIndex);
      mpLayerList->setFocus();
      ToggleLayers(true);
   }
}

///////////////////////////////////////////////////////////////////////////////
void LayersBrowser::NewLayerButtonPushed()
{
   if(mpLayerList->count() == 1)
   {
      // Reenable tabs and buttons
      ToggleLayers(true);
   }
}

///////////////////////////////////////////////////////////////////////////////
void LayersBrowser::TrashLayerButtonPushed()
{
   if(mpLayerList->count() < 1)
   {
      // Disable tabs and buttons
      ToggleLayers(false);
   }
}

///////////////////////////////////////////////////////////////////////////////
void LayersBrowser::RenameLayerButtonPushed()
{
   QString text = mpLayerList->currentItem()->text();
   QString newText = QInputDialog::getText(NULL, "Item", 
      "Enter new item", QLineEdit::Normal, text);
   newText = newText.simplified();
   if(!newText.isEmpty())
   {
      mpLayerList->currentItem()->setText(newText);
      emit RenameLayer(newText);
   }
}

///////////////////////////////////////////////////////////////////////////////
void LayersBrowser::ToggleLayers(bool enabled)
{
   mpTrashLayerButton->setEnabled(enabled);
   mpHideLayerButton->setEnabled(enabled);
   mpRenameLayerButton->setEnabled(enabled);
   emit ToggleTabs(enabled);
}

///////////////////////////////////////////////////////////////////////////////
