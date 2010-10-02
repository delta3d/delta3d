/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * William E. Johnson II
 */
#include <prefix/stageprefix.h>
#include "dtEditQt/editoraboutbox.h"
#include "dtEditQt/uiresources.h"
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QPixmap>
#include <QtGui/QTextEdit>
#include <QtGui/QTabWidget>
#include <QtCore/QTextStream>
#include <QtCore/QFile>
#include <QtGui/QIcon>

namespace dtEditQt
{

   //////////////////////////////////////////////////////////////////////////////
   EditorAboutBox::EditorAboutBox(QWidget* parent)
      : QDialog(parent)
   {
      setWindowTitle("About STAGE");

      QPixmap      splashImage = QPixmap(UIResources::SPLASH_SCREEN.c_str());
      QLabel*      splashLabel = new QLabel(this);
      QVBoxLayout* vLay        = new QVBoxLayout(this);
      QHBoxLayout* hLay        = new QHBoxLayout();
      QPushButton* ok          = new QPushButton("OK",this);
      QTabWidget*  tabs        = new QTabWidget(this);

      splashImage = splashImage.scaled(450,300,Qt::KeepAspectRatio,Qt::SmoothTransformation);
      splashLabel->setPixmap(splashImage);
      splashLabel->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

      tabs->addTab(createEditorInfoTab(),"About");
      tabs->addTab(createLicenseTab(),"License Agreement");

      hLay->addStretch(1);
      hLay->addWidget(ok);
      hLay->addStretch(1);
      vLay->addWidget(splashLabel);
      vLay->addWidget(tabs);
      vLay->addLayout(hLay);

      connect(ok,SIGNAL(clicked()), this, SLOT(accept()));
      setModal(true);
   }

   //////////////////////////////////////////////////////////////////////////////
   QTextEdit* EditorAboutBox::createEditorInfoTab()
   {
      QTextEdit* textEdit = new QTextEdit();

      textEdit->setReadOnly(true);
      textEdit->setHtml("<h2><p align=\"center\"><u>STAGE: Delta3D's Simulation, Training, and Game Editor</u></p></h2>"
         "<h4><p align=\"center\">Copyright 2004-2005<br>"
         "MOVES Institute, Naval Postgraduate School<br>"
         "700 Dyer Road<br>"
         "Monterey, CA 93943-5001</p></h4>"
         "<h3><p align=\"center\">For more details and licensing"
         " information please visit: <a href=\"http://www.delta3d.org\">"
         "http://www.delta3d.org.</a></p></h3>");
      textEdit->setTextInteractionFlags(Qt::TextBrowserInteraction);

      return textEdit;
   }

   //////////////////////////////////////////////////////////////////////////////
   QTextEdit* EditorAboutBox::createLicenseTab()
   {
      QTextEdit* textEdit = new QTextEdit();
      QFile file(UIResources::LICENSE_FILE.c_str());

      if (!file.open(QFile::ReadOnly | QFile::Text))
      {
         textEdit->setPlainText("Could not locate GPL license file.");
      }
      else
      {
         QTextStream in(&file);
         textEdit->setPlainText(in.readAll());
      }

      textEdit->setAlignment(Qt::AlignLeft);
      textEdit->setReadOnly(true);
      return textEdit;
   }

} // namespace dtEditQt
