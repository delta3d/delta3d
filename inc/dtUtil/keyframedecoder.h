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

/** \file dtUtil/keyframedecoder.h
  * Utility methods for using strings, often for XML purposes.
  * \author John K. Grant
  */

#ifndef DTUTIL_KEYFRAME_DECODER_INC
#define DTUTIL_KEYFRAME_DECODER_INC

#include <dtUtil/log.h>
#include <dtUtil/xercesutils.h>
#include <dtCore/refptr.h>

#include <xercesc/dom/DOMTreeWalker.hpp>

namespace dtUtil
{
   /** \brief A class that fills key frame data with DOM parsing.
     * Developed for the dtCore::Recorder class, it assumes a DOM
     * tree's structure, and uses existing recordable instances
     * to "decode", or deserialize, the XML hierarchy into a data package,
     * known here as a FrameDataType.
     *
     * Requires the following layout in XML:
     * Top
     *   FrameStamp,  Attr="Time"
     *      Source
     *
     * "Time" is required to an attribute name, and its value is assumed to be a double precision floating point number.
     * A 'Source' tag is given to a RecordableType instance and deserialized into usable data.
     *
     */
   template<typename RecordableT,typename FrameDataT>
   class KeyFrameDecoder
   {
   public:
      typedef RecordableT                                    RecordableType;         /// The type of object of interest.  RecordableTypes know how to create, serialize, and deserialize FrameDataTypes.
      typedef FrameDataT                                     FrameDataType;          /// The data to be saved from the object of interest.
      typedef std::vector< dtCore::RefPtr<FrameDataType> >   FrameDataPtrContainer;  /// A container to hold each source's frame data.
      typedef std::pair<double,FrameDataPtrContainer>        KeyFrame;               /// The time stamp applied to the entire container of frame data.
      typedef std::vector<KeyFrame>                          KeyFrameContainer;      /// The container of KeyFrame data.
      typedef std::vector< dtCore::RefPtr<RecordableType> >  RecordablePtrContainer; /// The container of sources of frame data.

      /** \brief The constructor.
        * @param sources The RecordableType instances that are needed for deserializing the XML information.
        * @param kfc The KeyFrameContainer provided will be appended during loading.  If only loaded data
        * is wished to be contained, then the container should be cleared before executing the Walk.
        */
      KeyFrameDecoder(const RecordablePtrContainer& sources, KeyFrameContainer& kfc): mSources(sources), mKFC(kfc), mSourcesSize(sources.size())
      {
      }

      ~KeyFrameDecoder()
      {
      }

      void Walk(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc)
      {
         if (mSources.empty())
         {
            LOG_INFO("No recordable instances are available for loading from XML.")
            return;
         }

         // --- build a vector of frame data --- //
         // tree walking assumes a schema
         // root
         //   framestamp
         //     source
         //       framedata
         //     source
         //       framedata
         //     source (etc)
         //       framedata
         // ------------------------------------ //
         XERCES_CPP_NAMESPACE_QUALIFIER DOMTreeWalker* framestampwalker = doc->createTreeWalker( doc->getDocumentElement(), XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeFilter::SHOW_ELEMENT, 0, true );

         float timeStamp(0.0);
         // start at the root
         // assume the first child is a frame stamp
         for (XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* framestamp=framestampwalker->firstChild();
            framestamp != 0;
            framestamp = framestampwalker->nextSibling())
         {
            std::string val(dtUtil::FindAttributeValueFor("TimeCode", framestamp->getAttributes()));
            timeStamp = dtUtil::ToFloat(val);

            FrameDataPtrContainer dataToBeFilled;
            DecodeFrameStamp(doc, framestamp, dataToBeFilled);
            mKFC.push_back(typename KeyFrameContainer::value_type( timeStamp, dataToBeFilled));
         }
      }

   protected:
      /** Walks the frame stamp tag to find each source's data and
        * asks the source to deserialize the XML tag into a data package.
        */
      void DecodeFrameStamp(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc,
                            XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* fs,
                            FrameDataPtrContainer& fdc)
      {
         XERCES_CPP_NAMESPACE_QUALIFIER DOMTreeWalker* sourcewalker = doc->createTreeWalker( fs, XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeFilter::SHOW_ELEMENT, 0, true );

         // move the walker to the first source
         unsigned int sourceIndex(0);
         for (XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* child=sourcewalker->firstChild();
             child != 0;
             child=sourcewalker->nextSibling())
         {
            XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* element = static_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*>( child );
            FrameDataType* data = DecodeSourceData( doc, element, sourceIndex );
            fdc.push_back( data );
         }
      }

      FrameDataType* DecodeSourceData(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc,
                                      XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* e, unsigned int index)
      {
         if ( index < mSourcesSize )
         {
            return mSources[index]->Deserialize( doc, e );
         }
         else
         {
            LOG_ERROR("No available recordable source to handle serialization.");
            return NULL;
         }
      }

   private:
      const RecordablePtrContainer& mSources;
      KeyFrameContainer& mKFC;
      unsigned int mSourcesSize;
   };

}

#endif // DTUTIL_KEYFRAME_DECODER_INC

