/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007 John K. Grant
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
 * John K. Grant, April 2007.
 */

#ifndef __DELTA_DT_DIS_PLUGIN_ENTITY_STATE_PDU_PLUGIN_SYMBOLS_H__
#define __DELTA_DT_DIS_PLUGIN_ENTITY_STATE_PDU_PLUGIN_SYMBOLS_H__

#include <dtDIS/plugins/default/dtdisdefaultpluginexport.h>

namespace dtDIS
{
   class IDISPlugin;

   extern "C" DT_DIS_DEFAULT_EXPORT IDISPlugin* CreateComponent();
   extern "C" DT_DIS_DEFAULT_EXPORT void DestroyComponent(IDISPlugin* plugin);
}

#endif  // __DELTA_DT_DIS_PLUGIN_ENTITY_STATE_PDU_PLUGIN_SYMBOLS_H__
