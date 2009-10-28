/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - dialogmapproperties (.h & .cpp) - Using 'The MIT License'
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
 * Matthew W. Campbell
 */

#ifndef DELTA_DIALOG_MAP_PROPERTIES
#define DELTA_DIALOG_MAP_PROPERTIES

#include <QtGui/QDialog>
#include <QtGui/QCheckBox>

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
        DialogMapProperties(QWidget* parent);
        virtual ~DialogMapProperties() { }

        QLineEdit* getMapName() { return mName; }

        QLineEdit* getMapDescription() { return mDescription; }

        QLineEdit* getMapAuthor() { return mAuthor; }

        QLineEdit* getMapCopyright() { return mCopyright; }

        QTextEdit* getMapComments() { return mComments; }

    private slots:
        void onOk();

    private:
        QLineEdit* mName;
        QLineEdit* mDescription;
        QLineEdit* mAuthor;
        QLineEdit* mCopyright;
        QTextEdit* mComments;
    };

} // namespace dtEditQt

#endif // DELTA_DIALOG_MAP_PROPERTIES
