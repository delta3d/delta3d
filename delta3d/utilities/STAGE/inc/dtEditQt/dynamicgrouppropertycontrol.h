/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine 
 * Simulation, Training, and Game Editor (STAGE)
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation 
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
 * David Guthrie
 */
#ifndef DELTA_DYNAMICGROUPPROPERTYCONTROL
#define DELTA_DYNAMICGROUPPROPERTYCONTROL

#include <dtEditQt/dynamicabstractparentcontrol.h>
#include <vector>


namespace dtDAL 
{
   class GroupActorProperty;
}

namespace dtEditQt 
{
   class GroupUIPlugin;
   class PropertyEditorModel;
   
   /**
    * @class DynamicGroupPropertyControl
    * @brief This is the dynamic control for the Group data type - used in the property editor
    * The primary purpose of the group control is to provide a visual grouping of property types
    * so that they aren't all laid out together.  
    */
   class DynamicGroupPropertyControl : public DynamicAbstractParentControl
   {
         Q_OBJECT
      public:
         /**
          * Constructor
          */
         DynamicGroupPropertyControl();
         
         /**
          * Destructor
          */
         virtual ~DynamicGroupPropertyControl();
                  
         /**
          * Groups can have children.  This is how you add children to the group. Note that you can't 
          * remove a child once it's added.
          */
         void addChildControl(DynamicAbstractControl *child, PropertyEditorModel *model);

         // OVERRIDDEN METHODS FROM ABSTRACT BASE
         
         /**
          * @see DynamicAbstractControl#initializeData
          */
         virtual void initializeData(DynamicAbstractControl *newParent, PropertyEditorModel *model,
                                     dtDAL::ActorProxy *proxy, dtDAL::ActorProperty *property);
         
         /**
          * @see DynamicAbstractControl#addSelfToParentWidget
          */
         void addSelfToParentWidget(QWidget &parent, QGridLayout &layout, int row);
         
         /**
          * @see DynamicAbstractControl#createEditor
          */
         virtual QWidget *createEditor(QWidget *parent, 
                                       const QStyleOptionViewItem &option, 
                                       const QModelIndex &index);

         /**
          * @see DynamicAbstractControl#getDisplayName
          */
         virtual const QString getDisplayName();
         
         /**
         * @see DynamicAbstractControl#getDescription
         */
        virtual const QString getDescription();

         /**
         * @see DynamicAbstractControl#getValueAsString
         */
        virtual const QString getValueAsString();

         /**
         * @see DynamicAbstractControl#isEditable
         */
        virtual bool isEditable();

      public slots: 
            
         /**
          * @see DynamicAbstractControl#updateData
          */
         virtual bool updateData(QWidget *widget);
         
         ///Slot called when the edit button on the editor control is clicked.
         void EditClicked();

      protected:
            
      private:
         //helper method for getting the appropriate plugin for the property.
         GroupUIPlugin* GetPlugin();
         
         dtDAL::GroupActorProperty* mGroupProperty;
                 
   };
   
}

#endif
