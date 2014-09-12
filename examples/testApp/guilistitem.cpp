/* -*-c++-*-
 * Damage Control Simulation (DCSim)
 * Copyright (C) 2007-2011, Alion Science and Technology
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
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "guilistitem.h"



namespace dtExample
{
   //////////////////////////////////////////////////////////////////////////
   // CUSTOM WINDOW FACTORIES
   //////////////////////////////////////////////////////////////////////////
   CEGUI_DEFINE_WINDOW_FACTORY(GuiListItem)



   //////////////////////////////////////////////////////////////////////////
   // CUSTOM WIDGET FUNCTIONS
   //////////////////////////////////////////////////////////////////////////
   void GuiListItem::BindFactory()
   {
      // Register special window factories.
      CEGUI::WindowFactoryManager& wfm = CEGUI::WindowFactoryManager::getSingleton();
      wfm.addFactory( &CEGUI_WINDOW_FACTORY(GuiListItem) );
   }

   //////////////////////////////////////////////////////////////////////////
   GuiListItem* GuiListItem::Create(const std::string& itemName)
   {
      CEGUI::WindowManager *wm = CEGUI::WindowManager::getSingletonPtr();
      CEGUI::Window* item = wm->createWindow(GuiListItem::WIDGET_TYPE.Get(), itemName);

      return static_cast<GuiListItem*>(item);
   }



   //////////////////////////////////////////////////////////////////////////
   // LIST ITEM CODE
   //////////////////////////////////////////////////////////////////////////
   const CEGUI::String GuiListItem::WidgetTypeName("TestApp/GuiListItem");
   const dtUtil::RefString GuiListItem::WIDGET_TYPE(GuiListItem::WidgetTypeName.c_str());
   const dtUtil::RefString GuiListItem::LISTBOX_TYPE("WindowsLook/ItemListbox");

   //////////////////////////////////////////////////////////////////////////
   GuiListItem::GuiListItem( CEGUI::String type, CEGUI::String name )
      : BaseClass(type,name)
   {
      rename( name );
   }

   //////////////////////////////////////////////////////////////////////////
   GuiListItem::~GuiListItem()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   CEGUI::Size GuiListItem::getItemPixelSize(void) const
   {
      return CEGUI::Size( 10.0f, 10.0f );
   }
}
