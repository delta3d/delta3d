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
#include <prefix/dtqtprefix.h>
#include <dtQt/dynamiclabelcontrol.h>

#include <dtCore/actorproxy.h>
#include <dtCore/datatype.h>

#include <dtQt/dynamicsubwidgets.h>

#include <QtCore/QString>

#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QWidget>

namespace dtQt
{

   ///////////////////////////////////////////////////////////////////////////////
   DynamicLabelControl::DynamicLabelControl()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicLabelControl::~DynamicLabelControl()
   {
   }


   /////////////////////////////////////////////////////////////////////////////////
   void DynamicLabelControl::InitializeData(DynamicAbstractControl* newParent,
      PropertyEditorModel* newModel, dtCore::PropertyContainer* newPC, dtCore::ActorProperty* newProperty)
   {
      //myProperty = dynamic_cast<dtCore::StringActorProperty *>(newProperty);
      DynamicAbstractControl::InitializeData(newParent, newModel, newPC, newProperty);

      // It's ok to do a NULL property.  It just means that someone is going to call the set
      // display values soon.
      if (newProperty != NULL)
      {
         mValueAsString = QString(tr("Unknown")); //newProperty->getValue().c_str()));

         std::string tooltip = newProperty->GetDescription() + "  [Type: " +
            newProperty->GetDataType().GetName() + "]";
         mDescription = QString(tr(tooltip.c_str()));

         mName = QString(tr(newProperty->GetLabel().c_str()));
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicLabelControl::getDisplayName()
   {
      QString name = DynamicAbstractControl::getDisplayName();
      if (!name.isEmpty())
      {
         return name;
      }
      return mName;
      //return QString(tr(myProperty->getLabel().c_str()));
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicLabelControl::getDescription()
   {
      return mDescription;
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicLabelControl::getValueAsString()
   {
      DynamicAbstractControl::getValueAsString();
      return mValueAsString;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicLabelControl::setDisplayValues(QString newName, QString newDescription, QString newValueAsString)
   {
      mName          = newName;
      mDescription   = newDescription;
      mValueAsString = newValueAsString;
   }

   /////////////////////////////////////////////////////////////////////////////////
   // SLOTS
   /////////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicLabelControl::updateData(QWidget* widget)
   {
      // Labels don't have editors, nor do they have any data to update.
      return false;
   }

} // namespace dtQt
