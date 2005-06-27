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

#ifndef _PROJECT_CONTEXT_DIALOG_H_
#define _PROJECT_CONTEXT_DIALOG_H_

#include <QDialog>

class QLineEdit;
class QPushButton;
class QCloseEvent;

namespace dtEditQt
{
    class ProjectContextDialog : public QDialog
    {
        Q_OBJECT

    public:

        /// Constructor
        ProjectContextDialog(QWidget *parent = NULL);

        /// Destructor
        virtual ~ProjectContextDialog();

        /**
         * Returns the path to the new project specified by the user.
         * @return The directory path.
         */
        QString getProjectPath() const;

    public slots:
        void spawnFileBrowser();

    private:
        QPushButton *applyButton;
        QLineEdit   *pathEdit;
    };

}
#endif
