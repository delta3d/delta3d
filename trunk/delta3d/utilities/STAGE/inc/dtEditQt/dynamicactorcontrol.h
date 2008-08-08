/* -*-c++-*-
* Delta3D Simulation Training And Game Editor (STAGE)
* STAGE - dynamicactorcontrol (.h & .cpp) - Using 'The MIT License'
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
