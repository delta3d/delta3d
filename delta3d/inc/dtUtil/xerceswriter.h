#ifndef DELTA_XERCESWRITER_INC
#define DELTA_XERCESWRITER_INC

#include <osg/Referenced>   // for base class

#include <string>
#include <vector>

#include <xercesc/util/XercesDefs.hpp>
#include <dtCore/export.h>

XERCES_CPP_NAMESPACE_BEGIN
   class DOMElement;
   class DOMDocument;
   class DOMImplementation;
XERCES_CPP_NAMESPACE_END

namespace dtUtil
{
   /** A class that manages one XML document.
     * \warning The PlatformUtils::Initialize() needs to be called before instantiating this class.
     */
   class DT_EXPORT XercesWriter : public osg::Referenced
   {
   public:
      XercesWriter();

   protected:
      ~XercesWriter();

   public:
      void CreateDocument(const std::string& rootname);

      XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* GetDocument() { return _document; }
      const XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* GetDocument() const { return _document; }

      void WriteFile(const std::string& file);

      static XMLCh* ConvertToTranscode(const char* str);
      static void ReleaseTranscode(XMLCh* str);

   private:
      XercesWriter(const XercesWriter&); /// not implemented because copying these members would be bad

      XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementation* _implementation;
      XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* _document;

      // string constants
      XMLCh* _CORE, *_root_name;
   };
};

#endif  // DELTA_XERCESWRITER_INC
