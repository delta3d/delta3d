#include <dtGUI/ceguiconnectionmanager.h>
using namespace dtGUI;



/********************************************************************************
                      construction/destruction                                  
 ********************************************************************************/
CEGUIConnectionManager::~CEGUIConnectionManager()
{
    for(StaticToConnectionMap::iterator it=m_mapStaticToConnection.begin(); it!=m_mapStaticToConnection.end(); it++)
        delete (*it).first;
    m_mapStaticToConnection.clear();
    for(MemberToConnectionMap::iterator it=m_mapMemberToConnection.begin(); it!=m_mapMemberToConnection.end(); it++)
        delete (*it).first;
    m_mapMemberToConnection.clear();
}



/********************************************************************************
                 Connect/Disconnect for static callbacks                        
 ********************************************************************************/
void CEGUIConnectionManager::Connect(CEGUI::EventSet *pEventSet, const std::string& sEventName, bool (* pFunction)(const CEGUI::EventArgs&))
{
    //generate signature
    CEGUISignatureStatic *pNewSignature = new CEGUISignatureStatic(pEventSet, sEventName, pFunction);

    //generate an error if already connected:
    for(StaticToConnectionMap::iterator it = m_mapStaticToConnection.begin(); 
        it!= m_mapStaticToConnection.end(); it++)
    {
        if( (*pNewSignature) == (*(*it).first) )
        {
            delete pNewSignature;
            LOG_ERROR("Event \"" + sEventName + "\" is already Connected to this Callback");
            return;
        }
    }

    //subscribe
    CEGUI::Event::Connection connection = pEventSet->subscribeEvent(sEventName, CEGUI::Event::Subscriber(pFunction));
    //register to map
    m_mapStaticToConnection[pNewSignature] = connection;
    //subscribe to receive an event if the destructor of the EventSet(e.g. CEGUIWindow) is called:
    CEGUI::Window *pWindow = dynamic_cast<CEGUI::Window *>(pEventSet);
    if(pWindow)
        pWindow->subscribeEvent(CEGUI::Window::EventDestructionStarted, CEGUI::Event::Subscriber(&CEGUIConnectionManager::OnCEGUIWindowDestruction, this) );

}

void CEGUIConnectionManager::Disconnect(CEGUI::EventSet *pEventSet, const std::string& sEventName, bool (* pFunction)(const CEGUI::EventArgs&))
{
    //generate (temporary) signature
    CEGUISignatureStatic signature(pEventSet, sEventName, pFunction);

    //delete all connections whose signatures match the above defined signature:
    StaticToConnectionMap::iterator it = m_mapStaticToConnection.begin();
    while(it != m_mapStaticToConnection.end())
    {
        if( signature.match(pEventSet, sEventName, pFunction) )
        {
            if( (*it).second->connected() )
                (*it).second->disconnect();
            delete it->first;
            m_mapStaticToConnection.erase(it);
            it = m_mapStaticToConnection.begin();
        }
        else 
            it++;
    }
}

