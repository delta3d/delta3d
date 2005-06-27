/* 
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2005, BMH Associates, Inc. 
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
* @author William E. Johnson II
*/

#include "dtEditQt/editoraboutbox.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QImage>

namespace dtEditQt
{
    EditorAboutBox::EditorAboutBox(QWidget *parent) : QDialog(parent)
    {
        QVBoxLayout *vLay = new QVBoxLayout(this);
        QHBoxLayout *hLay = new QHBoxLayout;
        QLabel      *text = new QLabel("Delta3D Level Editor\n\nCopyright 2005.", this);
        QPushButton *ok   = new QPushButton("OK", this);

        setWindowTitle("About Delta3D Level Editor");

        //vLay->addWidget(new QIcon(tr("delta_logo.jpg")));
        vLay->addWidget(text, Qt::AlignHCenter);
        
        hLay->addStretch(1);
        hLay->addWidget(ok);
        hLay->addStretch(1);

        vLay->addStretch(1);
        vLay->addLayout(hLay);

        connect(ok, SIGNAL(clicked()), this, SLOT(accept()));
        setModal(true);
        resize(275, 250);
    }

    EditorAboutBox::~EditorAboutBox()
    {

    }
}
