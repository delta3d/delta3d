/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004-2005 MOVES Institute 
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
*/

#ifndef DELTA_MACROS
#define DELTA_MACROS

// macros.h: System-wide macro definitions.
//
//////////////////////////////////////////////////////////////////////


#include <string>
#include <vector>


//
// The "is-a" macro.  Checks whether the first parameter (a pointer) is an
// instance of the second parameter (a class).
//
// Example usage:
//
//    Base* scene = new Scene();
//
//    if( IS_A(scene, Scene*) )
//    {
//       cout << "Yes, it is." << endl;
//    }
//

#define IS_A(P, T) (dynamic_cast<T>(P)!=NULL)


//
// The management layer declaration macro.  Should be included in the
// declarations of all heavyweight dtCore classes, with the (unquoted) name of
// the class specified as its parameter.
//

#define DECLARE_MANAGEMENT_LAYER(T)                \
   private:                                        \
      static std::vector<T*> instances;            \
      static void RegisterInstance(T* instance);   \
      static void DeregisterInstance(T* instance); \
   public:                                         \
      static int GetInstanceCount();               \
      static T* GetInstance(int index);            \
      static T* GetInstance(std::string name);


//
// The management layer implementation macro.  Should be included in the
// implementations of all heavyweight dtCore classes, with the (unquoted)
// name of the class specified as its parameter.
//

#define IMPLEMENT_MANAGEMENT_LAYER(T)                          \
   std::vector<T*> T::instances;                               \
   void T::RegisterInstance(T* instance)                       \
   {                                                           \
      instances.push_back(instance);                           \
   }                                                           \
   void T::DeregisterInstance(T* instance)                     \
   {                                                           \
      for(std::vector<T*>::iterator it = instances.begin();    \
          it != instances.end();                               \
          it++)                                                \
      {                                                        \
         if((*it) == instance)                                 \
         {                                                     \
            instances.erase(it);                               \
            return;                                            \
         }                                                     \
      }                                                        \
   }                                                           \
   int T::GetInstanceCount() { return instances.size(); }      \
   T* T::GetInstance(int index) { return instances[index]; }   \
   T* T::GetInstance(std::string name)                         \
   {                                                           \
      for(std::vector<T*>::iterator it = instances.begin();    \
          it != instances.end();                               \
          it++)                                                \
      {                                                        \
         if((*it)->GetName() == name)                          \
         {                                                     \
            return *it;                                        \
         }                                                     \
      }                                                        \
      return NULL;                                             \
   }



#endif // DELTA_MACROS
