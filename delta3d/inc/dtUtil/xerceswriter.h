#ifndef _xml_serializer_xercesc_include_
#define _xml_serializer_xercesc_include_

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
     */
   class DT_EXPORT XercesWriter
   {
   public:
      XercesWriter();
      ~XercesWriter();

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

#endif  // _xml_serializer_xercesc_include_
