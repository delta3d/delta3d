#include <dtActors/prefabactorproxy.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/mapxml.h>
#include <dtDAL/project.h>
#include <dtUtil/exception.h>

namespace dtActors
{
   /////////////////////////////////////////////////////////////////////////////
   // ACTOR CODE
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   PrefabActor::PrefabActor(const std::string& name)
      : BaseClass(name)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   PrefabActor::~PrefabActor()
   {
   }


   /////////////////////////////////////////////////////////////////////////////
   // PROXY CODE
   /////////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString PrefabActorProxy::CLASS_NAME("dtActors::PrefabActor");

   /////////////////////////////////////////////////////////////////////////////
   PrefabActorProxy::PrefabActorProxy()
      : BaseClass()
   {
      SetClassName(CLASS_NAME.Get());
   }

   /////////////////////////////////////////////////////////////////////////////
   PrefabActorProxy::~PrefabActorProxy()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void PrefabActorProxy::CreateActor()
   {
      SetActor(*new PrefabActor());
   }

   /////////////////////////////////////////////////////////////////////////////
   void PrefabActorProxy::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      PrefabActor* actor = NULL;
      GetActor(actor);

      AddProperty(new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::PREFAB,
         "PrefabResource", "Prefab",
         dtDAL::MakeFunctor(*this, &PrefabActorProxy::SetPrefab),
         "Defines the Prefab resource to use.", "Prefab"));

      // REMOVE USELESS PROPERTIES - These properties really should not show in
      // STAGE and ought to be completely removed from the object.
      // However, the overhead is part of sub-classing GameActor.
      RemoveProperty("Show Collision Geometry"); //"ODE Show Collision Geometry"
      RemoveProperty(TransformableActorProxy::PROPERTY_ENABLE_COLLISION);
      RemoveProperty(TransformableActorProxy::PROPERTY_COLLISION_TYPE);
      RemoveProperty(TransformableActorProxy::PROPERTY_COLLISION_RADIUS);
      RemoveProperty(TransformableActorProxy::PROPERTY_COLLISION_LENGTH);
      RemoveProperty(TransformableActorProxy::PROPERTY_COLLISION_BOX);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PrefabActorProxy::SetPrefab(const std::string& fileName)
   {
      PrefabActor* actor = NULL;
      GetActor(actor);

      if (!actor)
      {
         return;
      }

      // First remove the current actors from any previous Prefabs...
      for (int proxyIndex = 0; proxyIndex < (int)mProxies.size(); proxyIndex++)
      {
         dtDAL::ActorProxy* proxy = mProxies[proxyIndex].get();
         if (proxy)
         {
            dtCore::DeltaDrawable* proxyActor = NULL;
            proxy->GetActor(proxyActor);

            if (proxyActor)
            {
               actor->RemoveChild(proxyActor);
            }
         }
      }

      mProxies.clear();

      if (!fileName.empty())
      {
         dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
         fileUtils.PushDirectory(dtDAL::Project::GetInstance().GetContext());
         try
         {
            dtCore::RefPtr<dtDAL::MapParser> parser = new dtDAL::MapParser;
            parser->ParsePrefab(fileName, mProxies);

            for (int proxyIndex = 0; proxyIndex < (int)mProxies.size(); proxyIndex++)
            {
               dtDAL::ActorProxy* proxy = mProxies[proxyIndex].get();

               dtCore::DeltaDrawable* proxyActor = NULL;
               proxy->GetActor(proxyActor);

               if (proxyActor)
               {
                  actor->AddChild(proxyActor);
               }
            }
         }
         catch (const dtUtil::Exception& e)
         {
            LOG_ERROR(e.What());
         }
         fileUtils.PopDirectory();
      }
   }
}
