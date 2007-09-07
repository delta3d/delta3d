#ifndef environmentActor_h__
#define environmentActor_h__

#include <dtDAL/plugin_export.h>
#include <dtGame/environmentactor.h>
#include <dtCore/refptr.h>

namespace dtCore
{
   class Environment;
}

namespace dtActors
{
   class DT_PLUGIN_EXPORT EnvActor : public dtGame::IEnvGameActor
   {
   public:
      EnvActor( dtGame::GameActorProxy &proxy );
   	virtual ~EnvActor();

      void SetFogEnable(bool enabled);
      bool GetFogEnable() const;

      /**
      * Adds an actor to the internal hierarchy of the environment
      * @param dd The DeltaDrawable to add as a child
      */
      virtual void AddActor(dtCore::DeltaDrawable &dd);

      /**
      * Removes a DeltaDrawable from the internal hierarchy
      * @param dd The DeltaDrawable to remove
      */
      virtual void RemoveActor(dtCore::DeltaDrawable &dd);

      /**
      * Removes all actors associated with this environment
      */
      virtual void RemoveAllActors();

      /**
      * Called to see if this environment has the specified proxy
      * @param proxy The proxy to look for
      * @return True if it contains it, false if not
      */
      virtual bool ContainsActor(dtCore::DeltaDrawable &dd) const;

      /**
      * Gets all the actors associated with this environment
      * @param vec The vector to fill
      */
      virtual void GetAllActors(std::vector<dtCore::DeltaDrawable*> &vec);

      /**
      * Sets the date and time on this environment
      * @param year The year to set
      * @param month The month to set
      * @param day The day to set
      * @param hour The hour to set in military time
      * @param min The minute to set
      * @param sec The second to set
      */
      virtual void SetTimeAndDate(const int year, const int month, const int day,
                                  const int hour, const int min,   const int sec);


      /**
      * Gets the time and date of the current environment
      * @param year The year to get
      * @param month The month to get
      * @param day The day to get
      * @param hour The hour to get
      * @param min The minute to get
      * @param sec The second to get
      */
      virtual void GetTimeAndDate(int &year, int &month, int &day, int &hour, int &min, int &sec) const;

      /**
      * Gets the number of children of this environment
      */
      virtual unsigned int GetNumEnvironmentChildren() const;

   protected:
   	
   private:
      dtCore::RefPtr<dtCore::Environment> mEnv;
   };


   class DT_PLUGIN_EXPORT EnvActorProxy : public dtGame::IEnvGameActorProxy
   {
   public:
      EnvActorProxy();

     virtual void BuildPropertyMap();
     virtual void CreateActor();
     virtual void BuildInvokables();
     virtual void OnEnteredWorld() {};

   protected:
      virtual ~EnvActorProxy();
   };
}

#endif // environmentActor_h__
