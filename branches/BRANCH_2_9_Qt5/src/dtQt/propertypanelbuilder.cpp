/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
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
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtQt/propertypanelbuilder.h>
#include <dtUtil/log.h>



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   PropertyPanelBuilder::PropertyPanelBuilder()
   {}

   PropertyPanelBuilder::~PropertyPanelBuilder()
   {}

   int PropertyPanelBuilder::AddPropertiesToPanel(
      dtCore::PropertyContainer& propertyContainer, dtQt::PropertyPanel& outPanel)
   {
      int count = 0;

      typedef dtCore::PropertyContainer::PropertyVector PropList;
      PropList props;
      propertyContainer.GetPropertyList(props);

      dtCore::DataType* curType = NULL;
      dtCore::ActorProperty* curProp = NULL;
      PropList::iterator curIter = props.begin();
      PropList::iterator endIter = props.end();

      for ( ; curIter != endIter; ++curIter)
      {
         curProp = *curIter;
         curType = &curProp->GetDataType();

         if (curType == NULL)
         {
            LOG_WARNING("No matching dtCore::DataType for property \"" +
               curProp->GetName() + "\". Cannot map to appropriate UI control.");
         }
         else
         {
            dtUtil::RefString propName = curProp->GetName();

            dtQt::BasePropertyControl* control = outPanel.AddProperty(curType->GetName(), propName);

            if (control == NULL)
            {
               LOG_ERROR("Cannot create UI control for property \""
                  + propName + "\" of type [" + curType->GetName() + "]");
            }
            else
            {
               control->AddLinkedProperty(*curProp);
               control->Init();

               ++count;
            }
         }
      }

      return count;
   }

}
