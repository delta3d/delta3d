/* -*-c++-*-
* Delta3D Simulation Training And Game Editor (STAGE)
* STAGE - dialoglistselection (.h & .cpp) - Using 'The MIT License'
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
#ifndef DELTA_DIALOG_LIST_SELECTION
#define DELTA_DIALOG_LIST_SELECTION

#include <QtGui/QDialog>

class QStringList;
class QListWidget;
class QListWidgetItem;
class QPushButton;

namespace dtEditQt
{

    /**
     * This is a generic dialog box for presenting a list of items for the
     * user to choose from.  The list is presented in a list box and
     * contains ok and cancel buttons.
     */
    class DialogListSelection : public QDialog 
    {
        Q_OBJECT
    public:
        /**
         * Constructs a new selection dialog box.
         * @param parent Parent widget to center this dialog over.
         * @param windowTitle Text to display in the dialog's title bar.
         * @param groupName An optional string. If specified, the list box
         *  will have a groupbox around it with this string as its title.
         */
        DialogListSelection(QWidget *parent, const QString &windowTitle,
            const QString &groupName = "");
                            
        /**
         * Empty destructor.
         * @return 
         */
        virtual ~DialogListSelection() { }

        /**
         * Sets the list of strings to be displayed in the list box.
         * @param list The selection list.
         * @note The dialog will correctly handle an empty list, therefore,
         *  an empty list is valid, however, the dialog will not allow the
         *  user to do anything other than cancel or close.
         */
        void setListItems(const QStringList &list);

        /**
         * Gets the item currently selected in the dialog.  Call this
         * if the user presses the ok button to get the selected item.
         * @return The item selected from the list box.
         */
        const QString &getSelectedItem() const { return currentItem; }

    protected slots:
        void onSelectionChanged();
        void onItemClicked(QListWidgetItem *i);
        void onItemDoubleClicked(QListWidgetItem *i);
        void onCurrentRowChanged(int newRow);

    private:
        QString currentItem;
        QListWidget *listBox;
        QPushButton *okButton;
        QPushButton *cancelButton;
    };

}

#endif
