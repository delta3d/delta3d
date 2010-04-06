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
 * Curtiss Murphy
 */
#ifndef DELTA_PROPERTYEDITORMODEL
#define DELTA_PROPERTYEDITORMODEL

#include <dtQt/export.h>
#include <dtQt/dynamicabstractcontrol.h>
#include <dtQt/dynamicabstractparentcontrol.h>
#include <dtQt/dynamicgroupcontrol.h>
#include <QtCore/QAbstractItemModel>

namespace dtQt
{
   /**
    * This class is the model used to represent the properties for a selected object.
    * This clas is fairly complex. It follows the model/view architecture for QT.
    * The base idea for this class was taken from qpropertyeditor_model.h from the QT
    * Beta2 Designer application.
    *
    * Goto: QT\tools\designer\src\components\propertyeditor\qpropertyeditor_model.h
    * to see the original.  The original class is distributable according to the
    * standard QT license (GPL).
    */
   class DT_QT_EXPORT PropertyEditorModel : public QAbstractItemModel
   {
      Q_OBJECT

   public:
      /**
       * Constructor
       */
      PropertyEditorModel(QObject* parent = 0);

      /**
       * Destructor
       */
      ~PropertyEditorModel();

      /**
       * Remove the children of the root group object.  This is necessary because the
       * model needs to tell the underlying QAbstractItemModel to update itself.
       * If we don't remove items safely, then the base class will eventually
       * try to access an object that doesn't exist.
       */
      //void clearRootChildren();

      DynamicGroupControl* GetRootControl() const;

      void SetRootControl(DynamicGroupControl* newRoot);

      /**
       * This class returns the QModelIndex for the property.  The index comes
       * from getting the parent of the property and then finding this properties
       * row in the parent.
       */
      QModelIndex IndexOf(DynamicAbstractControl* property, int column = 0) const;

      /**
       * Convenience method - This returns the DynamicAbstractControl from the
       * QModelIndex.  It's kinda wonky, but QT allows you to contruct a QModelIndex
       * with a void * pointer but doesn't allow you to access the data as a void *
       * later.  Instead, we have to static cast it out from the QVariant in data().
       */
      DynamicAbstractControl* GetAbstractControlFromIndex(const QModelIndex& index) const;

      virtual Qt::ItemFlags flags(const QModelIndex& index) const;

   public:
      //
      // QAbstractItemModel interface
      //
      virtual QModelIndex index(int row, int column, const QModelIndex& parent) const;
      virtual QModelIndex parent(const QModelIndex& index) const;

      virtual void setDescription(DynamicAbstractControl* control);

      virtual int rowCount(const QModelIndex& parent) const;
      virtual int columnCount(const QModelIndex& parent) const;
      virtual bool hasChildren(const QModelIndex& parent) const
      {
         return rowCount(parent) > 0;
      }

      virtual QVariant data(const QModelIndex& index, int role) const;
      virtual bool setData(const QModelIndex& index, const QVariant& value, int role)
      {
         return true;
      }

      virtual bool isEditable(const QModelIndex& index) const;
      //virtual QModelIndex buddy(const QModelIndex &index) const;

      virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

      virtual bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex());
      virtual bool removeRows(int position, int rows, const QModelIndex& parent);

   protected:
      //inline IProperty *childAt(IProperty *parent, int pos) const
      // {
      //    if (parent && parent->kind() == IProperty::Property_Group)
      //        return static_cast<IPropertyGroup*>(parent)->propertyAt(pos);
      //    return 0;
      //}

   private:
      //void refreshHelper(DynamicAbstractControl *property);
      DynamicGroupControl* rootControl;//IProperty *m_initialInput;

      // the parent control class needs to be a friend so that it can call a stupid beginremoverow method.
      friend class DynamicAbstractParentControl;
      friend class PropertyEditor;
      friend class DynamicGroupControl;
   };

} // namespace dtEditQt

#endif // DELTA_PROPERTYEDITORMODEL
