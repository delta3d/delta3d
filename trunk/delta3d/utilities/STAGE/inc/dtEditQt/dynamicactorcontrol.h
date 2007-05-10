/* -*-c++-*-
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
 * William E. Johnson II
 */
#ifndef DELTA_DYNAMIC_ACTOR_CONTROL
#define DELTA_DYNAMIC_ACTOR_CONTROL

#include <dtEditQt/dynamicabstractcontrol.h>
#include <dtEditQt/dynamicsubwidgets.h>

// Forward References
namespace dtDAL
{
   class ActorActorProperty;
}

namespace dtEditQt
{
   class PropertyEditorModel;
   class DynamicActorControl : public DynamicAbstractControl
   {
      Q_OBJECT

      public:

         /// Constructor
         DynamicActorControl();

         /// Destructor
         virtual ~DynamicActorControl();

         /**
          * @see DynamicAbstractControl#initializeData
          */
         void initializeData(DynamicAbstractControl *newParent,PropertyEditorModel *newModel, 
                             dtDAL::ActorProxy *newProxy, dtDAL::ActorProperty *newProperty);

         /**
          * @see DynamicAbstractControl#updateEditorFromModel
          */
         virtual void updateEditorFromModel(QWidget *widget);

         /**
          * @see DynamicAbstractControl#updateModelFromEditor
          */
         virtual bool updateModelFromEditor(QWidget *widget);

         /**
          * @see DynamicAbstractControl#createEditor
          */
         virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index);

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

         void actorPropertyChanged(ActorProxyRefPtr proxy, ActorPropertyRefPtr property);

         /**
          * Called when the user selects an item in the combo box
          */
         void itemSelected(int index);

         void onGotoClicked();

         /**
          * @see DynamicAbstractControl#handleSubEditDestroy
          */
         void handleSubEditDestroy(QWidget *widget, QAbstractItemDelegate::EndEditHint hint = QAbstractItemDelegate::NoHint)
         {
            if (widget == mTemporaryWrapper)
            {
               mTemporaryWrapper = NULL;
               mTemporaryEditControl = NULL;
               mTemporaryGotoButton = NULL;
            }
         }

      private:

         /**
          * Finds actors in the current map that match the provided class name
          * and fills a vector with their names
          * @param className The class name to search for
          * @return A vector with the name and proxy pointer mapped 
          */
         void GetActorProxies(std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > &toFill, const std::string &className);

         dtDAL::ActorActorProperty *myProperty;

         // This pointer is not really in our control.  It is constructed in the createEditor() 
         // method and destroyed whenever QT feels like it (mostly when the control looses focus). 
         // We work around this by trapping the destruction of this object, it should
         // call our handleSubEditDestroy() method so we know to not hold this anymore
         QWidget *mTemporaryWrapper;
         SubQComboBox *mTemporaryEditControl;
         SubQPushButton *mTemporaryGotoButton;
   };
}

#endif
