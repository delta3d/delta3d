/*
* Delta3D Open Source Game and Simulation Engine Level Editor
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

#ifndef _EDITOR_ABOUT_BOX_H_
#define _EDITOR_ABOUT_BOX_H_

#include <QDialog>

class QTextEdit;

namespace dtEditQt
{
    /**
     * @class EditorAboutBox
     * @brief This class generates the Editor's about box.
     */
    class EditorAboutBox : public QDialog
    {
        Q_OBJECT

    public:
        /**
         * Constructor
         */
        EditorAboutBox(QWidget *parent = 0);

        /**
         * Destructor
         */
        virtual ~EditorAboutBox() { }

    private:
        /**
         * Creates a tab displaying copyright information about the level editor.
         * @return Text edit object.
         */
        QTextEdit *createEditorInfoTab();

        /**
         * Creates a tab displaying the GPL license.
         * @return Text edit object.
         */
        QTextEdit *createLicenseTab();
    };
}

#endif
