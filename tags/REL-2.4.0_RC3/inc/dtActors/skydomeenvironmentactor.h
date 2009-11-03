#ifndef skydomeenvironmentactor_h__
#define skydomeenvironmentactor_h__

#include <dtDAL/plugin_export.h>
#include <dtGame/environmentactor.h>
#include <dtCore/refptr.h>

namespace dtCore
{
   class Environment;
   class SkyDome;
}

namespace dtActors
{
   class DT_PLUGIN_EXPORT SkyDomeEnvironmentActor : public dtGame::IEnvGameActor
   {
   public:
      SkyDomeEnvironmentActor(dtGame::GameActorProxy& proxy);
      virtual ~SkyDomeEnvironmentActor();


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
      * Gets the number of children of this environment
      */
      virtual unsigned int GetNumEnvironmentChildren() const;


      void SetFogEnable(bool enabled);
      bool GetFogEnable() const;

      void SetVisibility( float distance );
      float GetVisbility() const;


      /**
      * Sets the date and time on this environment
      * @param year The year to set
      * @param month The month to set
      * @param day The day to set
      * @param hour The hour to set in military time
      * @param min The minute to set
      * @param sec The second to set
      */
      void SetTimeAndDate(const unsigned year, const unsigned month, const unsigned day,
                                  const unsigned hour, const unsigned min,   const unsigned sec);


      /**
      * Gets the time and date of the current environment
      * @param year The year to get
      * @param month The month to get
      * @param day The day to get
      * @param hour The hour to get
      * @param min The minute to get
      * @param sec The second to get
      */
      void GetTimeAndDate(unsigned &year, unsigned &month, unsigned &day, unsigned &hour, unsigned &min, unsigned &sec) const;


      void SetSkyDomeEnable( bool enabled );
      bool GetSkyDomeEnable() const;

   protected:

   private:
      dtCore::RefPtr<dtCore::Environment> mEnv;
      dtCore::RefPtr<dtCore::SkyDome> mSkyDome;
   };


   class DT_PLUGIN_EXPORT SkyDomeEnvironmentActorProxy : public dtGame::IEnvGameActorProxy
   {
   public:
      SkyDomeEnvironmentActorProxy();

     virtual void BuildPropertyMap();
     virtual void CreateActor();

   protected:
      virtual ~SkyDomeEnvironmentActorProxy();
   };
}

#endif // skydomeenvironmentactor_h__
