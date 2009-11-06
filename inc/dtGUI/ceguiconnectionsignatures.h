#ifndef DTHUD_CEGUICONNECTIONSIGNATURES_H
#define DTHUD_CEGUICONNECTIONSIGNATURES_H

#include <string>
/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace CEGUI
{
   class EventSet;
   class EventArgs;
}
/// @endcond

namespace dtGUI
{

/**
 * @brief
 *     Common interface for a member-callback-connection-signatures
 */
class CEGUIMemberSignatureBase
{
public:

   ///constructs a signature for an object-member-function
   CEGUIMemberSignatureBase(CEGUI::EventSet* pEventSet, const std::string &sEventName)
      : m_pEventSet(pEventSet)
      , m_sEventName(sEventName)
   {

   }

   ///compares member-signatures
   virtual bool operator==(const CEGUIMemberSignatureBase&) const = 0;

   ///returns the event set  which is part of the signature
   const CEGUI::EventSet* GetEventSet() const { return m_pEventSet; }

   ///returns the event name  which is part of the signature
   const std::string& GetEventName() const { return m_sEventName; }

protected:
   friend class CEGUIConnectionManager;
   virtual ~CEGUIMemberSignatureBase() {}

   CEGUI::EventSet* m_pEventSet; //event-set where the callback is connected to
   std::string      m_sEventName; //name of the event where the callback is listening to
};

/**
 * @brief
 *     objects instantiated from this class describes an object-member-"signature"
 */
template<class T>
class CEGUIMemberSignature : public CEGUIMemberSignatureBase
{
public:
   //typedef event-callback function-pointer:
   typedef bool (T::* CALLBACK_FPTR)(const CEGUI::EventArgs &);

   ///construcs a connection-signature-object
   CEGUIMemberSignature(CEGUI::EventSet*    pEventSet,
                        const std::string&  sEventName,
                        const CALLBACK_FPTR pMemberFunction,
                        const void*         pObject)
      : CEGUIMemberSignatureBase(pEventSet, sEventName)
      , m_pFunction(pMemberFunction)
      , m_pObject(pObject)
   {
   }

   //compares two signature-objects
   virtual bool operator==(const CEGUIMemberSignatureBase& x) const
   {
      const CEGUIMemberSignature<T>* _x = dynamic_cast<const CEGUIMemberSignature<T>*>(&x);
      if (_x)
      {
         //euqal if all provided data is euqal..
         return (m_pEventSet == _x->GetEventSet()) &&
                (m_sEventName == _x->GetEventName()) &&
                (m_pFunction == _x->GetFunctionPtr()) &&
                (m_pObject   == _x->GetObjectPtr());
      }
      else //not pointing to the same class-type => cannot be euqal!
      {
         return false;
      }
   }

   ///check if one signature "matches" another (empty strings and NULL-Pointer's are Wildcards, means: "match any value")
   bool match(CEGUI::EventSet*     pEventSet,
              const std::string&   sEventName,
              const CALLBACK_FPTR  pMemberFunction,
              const void*          pObject) const
   {
      return (m_pEventSet  == pEventSet       || (pEventSet       == 0))  &&
             (m_sEventName == sEventName      || (sEventName      == "")) &&
             (m_pFunction  == pMemberFunction || (pMemberFunction == 0))  &&
             (m_pObject    == pObject         || (pObject         == 0));
   }

   ///returns the pointer to the object-member
   const CALLBACK_FPTR GetFunctionPtr() const { return m_pFunction; }

   ///returns the pointer to the object
   const void* GetObjectPtr() const { return m_pObject; }

private:

   const CALLBACK_FPTR m_pFunction; //c++ - function pointer
   const void*         m_pObject;   //c++ - object which owns the function defined by m_pFunction
};

/**
 * @brief
 *     objects instantiated from this class describes a static-method's signature
 */
class CEGUISignatureStatic
{
public:
   //typedef event-callback function-pointer:
   typedef bool (* CALLBACK_FPTR)(const CEGUI::EventArgs&);

   //constructs a connection-signature-object
   CEGUISignatureStatic(CEGUI::EventSet*    pEventSet,
                        const std::string&  sEventName,
                        const CALLBACK_FPTR pFunction)
      : m_pEventSet(pEventSet)
      , m_sEventName(sEventName)
      , m_pFunction(pFunction)
   {
   }

   //compares two signare-objects
   virtual bool operator==(const CEGUISignatureStatic& x) const
   {
      const CEGUISignatureStatic* _x = dynamic_cast<const CEGUISignatureStatic*>(&x);
      if (_x)
      {
         //euqal if all provided data is euqal..
         return (m_pEventSet == _x->GetEventSet()) &&
                (m_sEventName == _x->GetEventName()) &&
                (m_pFunction == _x->GetFunctionPtr());
      }
      else //not pointing to the same class-type => canot be euqal!
      {
         return false;
      }
   }

   ///check if one signature "matches" another (empty strings and NULL-Pointer's are Wildcards, means: "match any value")
   bool match(CEGUI::EventSet *pEventSet, const std::string& sEventName, const CALLBACK_FPTR pFunction) const
   {
      return (m_pEventSet == pEventSet   || pEventSet == 0) &&
             (m_sEventName == sEventName || sEventName == "") &&
             (m_pFunction == pFunction   || pFunction == 0);
   }

   //returns a pointer to the callback of this signature
   const CALLBACK_FPTR GetFunctionPtr() const { return m_pFunction; }

   ///returns the event set which is part of the signature
   const CEGUI::EventSet* GetEventSet() const { return m_pEventSet; }

   ///returns the event set which is part of the signature
   const std::string& GetEventName() const { return m_sEventName; }

private:
   CEGUI::EventSet     *m_pEventSet; //event-set where the callback is connected to
   std::string          m_sEventName; //name of the event where the callback is listening to
   const CALLBACK_FPTR  m_pFunction; //c++ - function pointer
};


} //namespace dtHUD

#endif // DTHUD_..
