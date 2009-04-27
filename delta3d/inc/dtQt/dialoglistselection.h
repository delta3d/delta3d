/* -*-c++-*-
 * Delta3D
 * Copyright 2009, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * David Guthrie
 * Matthew Campbell
 */
#ifndef DELTA_DIALOG_LIST_SELECTION
#define DELTA_DIALOG_LIST_SELECTION

#include <dtQt/export.h>
#include <QtGui/QDialog>

class QStringList;
class QListWidget;
class QListWidgetItem;
class QPushButton;

namespace dtQt
{

    /**
     * This is a generic dialog box for presenting a list of items for the
     * user to choose from.  The list is presented in a list box and
     * contains ok and cancel buttons.
     */
    class DT_QT_EXPORT DialogListSelection : public QDialog
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
        DialogListSelection(QWidget* parent, const QString& windowTitle,
            const QString& groupName = "");

        /**
         * Empty destructor.
         * @return
         */
        virtual ~DialogListSelection();

        /**
         * Sets the list of strings to be displayed in the list box.
         * @param list The selection list.
         * @note The dialog will correctly handle an empty list, therefore,
         *  an empty list is valid, however, the dialog will not allow the
         *  user to do anything other than cancel or close.
         */
        void SetListItems(const QStringList& list);

        /**
         * Gets the item currently selected in the dialog.  Call this
         * if the user presses the ok button to get the selected item.
         * @return The item selected from the list box.
         */
        const QString& GetSelectedItem() const;

    protected slots:
        void onSelectionChanged();
        void onItemClicked(QListWidgetItem* i);
        void onItemDoubleClicked(QListWidgetItem* i);
        void onCurrentRowChanged(int newRow);

    private:
        QString mCurrentItem;
        QListWidget* mListBox;
        QPushButton* mOKButton;
        QPushButton* mCancelButton;
    };

}

#endif
