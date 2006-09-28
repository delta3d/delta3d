/*
 * Delta3D Open Source Game and Simulation Engine
 * Simulation, Training, and Game Editor (STAGE)
 * Copyright (C) 2005, BMH Associates, Inc.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * William E. Johnson II 
 */
#include <prefix/dtstageprefix-src.h>
#include <dtEditQt/taskeditor.h>
#include <QtGui/QGridLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QPushButton>
#include <QtGui/QComboBox>
#include <QtGui/QScrollArea>
#include <QtGui/QListWidget>
#include <QtGui/QLabel>
#include <QtCore/QString>

namespace dtEditQt
{
   TaskEditor::TaskEditor(QWidget *parent) : QDialog(parent)
   {
      QGroupBox   *group = new QGroupBox(tr("Tasks"));
      QGridLayout *grid  = new QGridLayout(group);

      QVBoxLayout *rightSideLayout = new QVBoxLayout;
      QLabel      *child       = new QLabel(tr("Children")); 
      QPushButton *addExisting = new QPushButton(tr("Add Existing"));
      grid->addWidget(child, 0, 0);
      //grid->addWidget(addExisting, 0, 1);

      QScrollArea *scrollArea = new QScrollArea;
      mListWidget = new QListWidget(scrollArea);
      //mListWidget->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
      scrollArea->setWidget(mListWidget);
      scrollArea->setWidgetResizable(true);
      grid->addWidget(scrollArea, 1, 0);

      mComboBox = new QComboBox;
      rightSideLayout->addWidget(addExisting);
      rightSideLayout->addWidget(mComboBox);
      //grid->addWidget(mComboBox, 1, 1);
      grid->addLayout(rightSideLayout, 0, 1);

      QHBoxLayout *buttonLayout = new QHBoxLayout;
      QPushButton *mvUp = new QPushButton(tr("Move Up")), *mvDn = new QPushButton(tr("Move Down"));
      buttonLayout->addWidget(mvUp);
      buttonLayout->addStretch(1);
      buttonLayout->addWidget(mvDn);
      grid->addLayout(buttonLayout, 2, 0);

      QPushButton *remove = new QPushButton(tr("Remove Child"));
      grid->addWidget(remove, 3, 0);

      QHBoxLayout *okCancelLayout = new QHBoxLayout;
      QPushButton *ok = new QPushButton(tr("OK")), *cancel = new QPushButton(tr("Cancel"));
      okCancelLayout->addWidget(ok);
      okCancelLayout->addWidget(cancel);
      
      QVBoxLayout *mainLayout = new QVBoxLayout(this);
      mainLayout->addWidget(group);
      mainLayout->addLayout(okCancelLayout);

      setModal(true);
      setWindowTitle(tr("Task Editor"));
      //setMinimumSize(360, 375);

      connect(mComboBox, SIGNAL(activated(const QString&)), this, SLOT(RefreshComboBox(const QString&)));

      connect(mvUp,   SIGNAL(clicked()), this, SLOT(OnMoveUpClicked()));
      connect(mvDn,   SIGNAL(clicked()), this, SLOT(OnMoveDownClicked()));
      connect(remove, SIGNAL(clicked()), this, SLOT(OnRemoveChildClicked()));
      connect(ok,     SIGNAL(clicked()), this, SLOT(OnOkClicked()));
      connect(cancel, SIGNAL(clicked()), this, SLOT(close()));

      RefreshListWidget();
      RefreshComboBox(tr(""));
   }

   TaskEditor::~TaskEditor()
   {

   }

   void TaskEditor::RefreshListWidget()
   {

   }

   void TaskEditor::RefreshComboBox(const QString &itemName)
   {

   }

   void TaskEditor::OnMoveUpClicked()
   {

   }

   void TaskEditor::OnMoveDownClicked()
   {

   }

   void TaskEditor::OnRemoveChildClicked()
   {

   }

   void TaskEditor::OnOkClicked()
   {
      accept();
   }
}
