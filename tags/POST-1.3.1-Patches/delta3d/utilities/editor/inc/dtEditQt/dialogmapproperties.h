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
 * @author Matthew W. Campbell
*/
#ifndef DELTA_DIALOG_MAP_PROPERTIES
#define DELTA_DIALOG_MAP_PROPERTIES

#include <QtGui/QDialog>

class QLineEdit;
class QTextEdit;

namespace dtEditQt {

    /**
     * This class is a dialog box used to edit properties of the current map.
     */
    class DialogMapProperties : public QDialog 
    {
        Q_OBJECT
    public:
        DialogMapProperties(QWidget *parent);
        virtual ~DialogMapProperties() { }

        QLineEdit *getMapName() {
            return this->name;
        }

        QLineEdit *getMapDescription() {
            return this->description;
        }

        QLineEdit *getMapAuthor() {
            return this->author;
        }

        QLineEdit *getMapCopyright() {
            return this->copyright;
        }

        QTextEdit *getMapComments() {
            return this->comments;
        }

    private slots:
        void onOk();

    private:
        QLineEdit *name;
        QLineEdit *description;
        QLineEdit *author;
        QLineEdit *copyright;
        QTextEdit *comments;
    };

}

#endif
