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

#ifndef _EDITOR_ABOUT_BOX_H_
#define _EDITOR_ABOUT_BOX_H_

#include <QWidget>
#include <QDialog>

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
        virtual ~EditorAboutBox();
    };
}

#endif
