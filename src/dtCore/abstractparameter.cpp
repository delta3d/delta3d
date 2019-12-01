/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2015, Caper Holdings, LLC
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
 * David Guthrie
 */
#ifdef USE_TBB_MEMORY_POOL
#define TBB_PREVIEW_MEMORY_POOL 1
#include <tbb/memory_pool.h>
#include <memory>
#else
#include <cstdlib>
#endif
#include <dtCore/abstractparameter.h>

namespace dtCore
{
#ifdef USE_TBB_MEMORY_POOL
   tbb::memory_pool<std::allocator<char> > PARAM_POOL;
   void* AbstractParameter::operator new(size_t num_bytes)
   {
      void* result = PARAM_POOL.malloc(num_bytes);
      if (result == nullptr)
         throw std::bad_alloc();
      return result;
   }

   void AbstractParameter::operator delete(void* ptr)
   {
      PARAM_POOL.free(ptr);
   }
#else
   void* AbstractParameter::operator new(size_t num_bytes)
   {
      void* result = malloc(num_bytes);
      if (result == nullptr)
         throw std::bad_alloc();
      return result;
   }

   void AbstractParameter::operator delete(void* ptr)
   {
      free(ptr);
   }

#endif

}
