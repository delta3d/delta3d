/* -*-c++-*-
 * Delta3D
 * Copyright 2009, Alion Science and Technology
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * David Guthrie
 * Matt Campbell
 */
#include <prefix/dtqtprefix.h>
#include <dtQt/dialoglistselection.h>

#include <QtCore/QStringList>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QGroupBox>
#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>

namespace dtQt
{

   ///////////////////////////////////////////////////////////////////////////////
   DialogListSelection::DialogListSelection(QWidget* parent,
      const QString& windowTitle, const QString& groupBoxName)
      : QDialog(parent)
   {
      QVBoxLayout* mainLayout = new QVBoxLayout(this);

      setWindowTitle(windowTitle);

      // Create the group box and list view.
      if (!groupBoxName.isEmpty())
      {
         QGroupBox*   groupBox = new QGroupBox(groupBoxName,this);
         QVBoxLayout* layout = new QVBoxLayout(groupBox);
         mListBox = new QListWidget(groupBox);
         layout->addWidget(mListBox);
         mainLayout->addWidget(groupBox);
      }
      else
      {
         mListBox = new QListWidget(this);
         mainLayout->addWidget(mListBox);
      }

      connect(mListBox, SIGNAL(itemClicked(QListWidgetItem*)),
         this, SLOT(onItemClicked(QListWidgetItem*)));

      connect(mListBox, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
         this, SLOT(onItemDoubleClicked(QListWidgetItem*)));

      connect(mListBox, SIGNAL(currentRowChanged(int)),
         this, SLOT(onCurrentRowChanged(int)));

      //Create the ok and cancel buttons...
      mOKButton     = new QPushButton(tr("OK"),     this);
      mCancelButton = new QPushButton(tr("Cancel"), this);
      QHBoxLayout* buttonLayout = new QHBoxLayout;

      mCancelButton->setDefault(true);
      mOKButton->setEnabled(false);
      buttonLayout->addStretch(1);
      buttonLayout->addWidget(mOKButton);
      buttonLayout->addWidget(mCancelButton);
      buttonLayout->addStretch(1);

      connect(mOKButton,     SIGNAL(clicked()), this, SLOT(accept()));
      connect(mCancelButton, SIGNAL(clicked()), this, SLOT(reject()));

      mainLayout->addLayout(buttonLayout);
   }

   ///////////////////////////////////////////////////////////////////////////////
   DialogListSelection::~DialogListSelection() { }

   ///////////////////////////////////////////////////////////////////////////////
   void DialogListSelection::SetListItems(const QStringList& list)
   {
      if (!list.empty())
      {
         mListBox->addItems(list);
         mListBox->setCurrentRow(0);
         mOKButton->setEnabled(true);
         mOKButton->setDefault(true);
      }
      else
      {
         mOKButton->setEnabled(false);
         mCancelButton->setDefault(true);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DialogListSelection::onSelectionChanged()
   {
      QListWidgetItem* item = mListBox->currentItem();
      if (item != NULL)
      {
         mCurrentItem = item->text();
      }
      else
      {
         mCurrentItem = tr("");
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   const QString& DialogListSelection::GetSelectedItem() const
   {
      return mCurrentItem;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DialogListSelection::onItemClicked(QListWidgetItem* i)
   {
      mCurrentItem = i->text();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DialogListSelection::onItemDoubleClicked(QListWidgetItem* i)
   {
      mCurrentItem = i->text();
      this->done(QDialog::Accepted);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DialogListSelection::onCurrentRowChanged(int i)
   {
      QListWidgetItem* item = mListBox->item(i);

      mCurrentItem = item != NULL ? item->text() : tr("");
   }

} // namespace dtEditQt
