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
