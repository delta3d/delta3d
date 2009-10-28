#ifndef DTGUI_CEGUICONNECTIONMANAGER_H
#define DTGUI_CEGUICONNECTIONMANAGER_H

#include <string>
#include <map>

#include <dtGUI/ceguiconnectionsignatures.h>

#include <CEGUI/CEGUIWindow.h>
#include <CEGUI/CEGUIEvent.h>
#include <CEGUI/CEGUIInputEvent.h>
#include <CEGUI/CEGUIEventSet.h>
#include <CEGUI/CEGUIEventArgs.h>
#include <CEGUI/CEGUISubscriberSlot.h>
#include <dtUtil/log.h>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace CEGUI
{
   class EventArgs;
}
/// @endcond


namespace dtGUI
{

/**
 * @brief
 *     this class is used to keep track of connections (event-callbacks)
 *     for any widget/EventSet created for/by a gui-object.
 *
 **/
class CEGUIConnectionManager
{
public:
   /********************************************************************************
                  Connect/Disconnect for object-member-callbacks
    ********************************************************************************/

   /**
    * connects object-member from an event
    *
    * @param pEventSet
    *     CEGUIEventSet which includes the event
    * @param sEventName
    *     name of the event to be connected
    * @param pObjectMember
    *     (relative)addr of object member
    * @param pObject
    *     pointer to the instantiated object which owns the callback
    */
   template<class T>
   void Connect(CEGUI::EventSet*   pEventSet,
                const std::string& sEventName,
                bool (T::*         pObjectMember)(const CEGUI::EventArgs&),
                T*                 pObject)
   {
      //generate signature
      CEGUIMemberSignature<T>* pNewSignature = new CEGUIMemberSignature<T>(pEventSet, sEventName, pObjectMember, pObject);

      //ckeck existing connections and generate an error if signature is present:
      for (MemberToConnectionMap::iterator it = m_mapMemberToConnection.begin(); it!= m_mapMemberToConnection.end(); it++)
      {
         if ((*(*it).first) == (*pNewSignature))
         {
            delete pNewSignature;
            LOG_ERROR("Event \"" + sEventName + "\" is already Connected to this Callback");
            return;
         }
      }

      //subscribe
      CEGUI::Event::Connection connection = pEventSet->subscribeEvent(sEventName, CEGUI::Event::Subscriber(pObjectMember, pObject));
      //register in m_mapMemberToConnection(-map)
      m_mapMemberToConnection[pNewSignature] = connection;
      //subscribe to receive an event if the destructor of the EventSet(e.g. CEGUIWindow) is called:
      //CEGUI::Window *pWindow;
      //if (pWindow = dynamic_cast<CEGUI::Window *>(pEventSet))
      //    pWindow->subscribeEvent(CEGUI::Window::EventDestructionStarted, CEGUI::Event::Subscriber(&CEGUIConnectionManager::OnCEGUIWindowDestruction, this) );
      pEventSet->subscribeEvent(CEGUI::Window::EventDestructionStarted, CEGUI::Event::Subscriber(&CEGUIConnectionManager::OnCEGUIWindowDestruction, this) );
   }

   /**
    * @brief
    *     disconnects object-member from an event
    *
    * @param pEventSet
    *     CEGUIEventSet which includes the event
    * @param sEventName
    *     name of the CEGUIEvent
    * @param pObjectMember
    *     (relative)addr of object member
    * @param pObject
    *     pointer to the instantiated object which owns the callback
    */
   template<class T>
   void Disconnect(CEGUI::EventSet*  pEventSet,
                  const std::string& sEventName,
                  bool (T::*         pObjectMember)(const CEGUI::EventArgs&),
                  T*                 pObject)
   {
      //generate (temporary) signature
      CEGUIMemberSignature<T> signature(pEventSet, sEventName, pObjectMember, pObject);

      //delete all connections which mathes the signature:
      MemberToConnectionMap::iterator it = m_mapMemberToConnection.begin();
      while (it != m_mapMemberToConnection.end())
      {
         if ( signature.match(pEventSet, sEventName, pObjectMember, pObject) )
         {
            //if connected, disconnect
            if ((*it).second->connected())
            {
               (*it).second->disconnect();
            }
            //deleted signature allocated by the manager
            delete it->first;
            //erase map-entry
            m_mapMemberToConnection.erase(it);
            //traverse the map from the beginning
            it = m_mapMemberToConnection.begin();
         }
         else
         {
            it++;
         }
      }
   }

   /********************************************************************************
                  Connect/Disconnect for static-callbacks
    ********************************************************************************/

   /**
    * @brief
    *     connects static function to an event
    *
    * @param pEventSet
    *     CETUIEventSet which includes the event
    * @param sEventName
    *     name of the event to be connected
    * @param pFunction
    *     addr of static function
    */
   void Connect(CEGUI::EventSet* pEventSet, const std::string& sEventName, bool (* pFunction)(const CEGUI::EventArgs&));

   /**
    * @brief
    *     disconnects static functions from an event
    *
    * @param pEventSet
    *     CEGUIEventSet which includes the event
    * @param
    *     sEventName name of the CEGUIEvent
    * @param pFunction
    *     addr of static function
    */
   void Disconnect(CEGUI::EventSet* pEventSet, const std::string& sEventName, bool (* pFunction)(const CEGUI::EventArgs&));

protected:
   /********************************************************************************
                           construction/destruction
    ********************************************************************************/

   ///constructs the a connection-manager
   CEGUIConnectionManager() {}

   ///destructs/clean up the connection-manager
   virtual ~CEGUIConnectionManager();

private:
   //if a window which has connections is destructed we need to clear the connection-signatures
   bool OnCEGUIWindowDestruction(const CEGUI::EventArgs &args)
   {
      //remove all signatures for the destructed window:
      const CEGUI::WindowEventArgs* pWEA;

      //note: if RTTI was enabled for CEGUI, we should be doing a dynamic_cast
      pWEA = static_cast<const CEGUI::WindowEventArgs*>(&args);
      //pWEA = dynamic_cast<const CEGUI::WindowEventArgs *>(&args);

      if (pWEA != NULL)
      {
         Disconnect<CEGUIConnectionManager>(pWEA->window, "", 0, 0);
      }
      return true;
   }

   typedef std::map<CEGUIMemberSignatureBase*, CEGUI::Event::Connection> MemberToConnectionMap;
   MemberToConnectionMap m_mapMemberToConnection;
   typedef std::map<CEGUISignatureStatic*,     CEGUI::Event::Connection> StaticToConnectionMap;
   StaticToConnectionMap m_mapStaticToConnection;
};

} // namespace dtGUI

#endif // DTGUI_CEGUICONNECTIONMANAGER_H
