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
* @author William E. Johnson II
*/

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
    EditorAboutBox::EditorAboutBox(QWidget *parent) : QDialog(parent)
    {
        setWindowTitle("About STAGE");

        QPixmap splashImage = QPixmap(UIResources::SPLASH_SCREEN.c_str());
        QLabel *splashLabel = new QLabel(this);
        QVBoxLayout *vLay = new QVBoxLayout(this);
        QHBoxLayout *hLay = new QHBoxLayout();
        QPushButton *ok = new QPushButton("OK",this);
        QTabWidget *tabs = new QTabWidget(this);

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

        connect(ok,SIGNAL(clicked()),this,SLOT(accept()));
        setModal(true);
    }

    //////////////////////////////////////////////////////////////////////////////
    QTextEdit *EditorAboutBox::createEditorInfoTab()
    {
        QTextEdit *textEdit = new QTextEdit();

        textEdit->setReadOnly(true);
        textEdit->setHtml("<h2><p align=\"center\"><u>STAGE: Delta3D's Simulation, Training, and Game Editor</u></p></h2>"
                "<h4><p align=\"center\">Copyright 2004-2005<br>"
                "MOVES Institute, Naval Postgraduate School<br>"
                "700 Dyer Road<br>"
                "Monterey, CA 93943-5001</p></h4>"
                "<h3><p align=\"center\">For more details and licensing"
                " information please visit: <a href=\"http://www.delta3d.org\">"
                "http://www.delta3d.org.</a></p></h3>");

        return textEdit;
    }

    //////////////////////////////////////////////////////////////////////////////
    QTextEdit *EditorAboutBox::createLicenseTab()
    {
        QTextEdit *textEdit = new QTextEdit();
        QFile file(UIResources::GPL_LICENSE_FILE.c_str());

        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            textEdit->setPlainText("Could not locate GPL license file.");
        }
        else {
            QTextStream in(&file);
            textEdit->setPlainText(in.readAll());
        }

        textEdit->setAlignment(Qt::AlignLeft);
        textEdit->setReadOnly(true);
        return textEdit;
    }
}
