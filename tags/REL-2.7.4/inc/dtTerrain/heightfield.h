/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2005, BMH Associates, Inc.
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
* Matthew W. Campbell
*/
#ifndef DELTA_HEIGHTFIELD
#define DELTA_HEIGHTFIELD

#include <vector>
#include <osg/Referenced>
#include <osg/Image>
#include <dtUtil/enumeration.h>
#include <dtUtil/exception.h>

namespace dtTerrain
{
   class HeightFieldOutOfBoundsException : public dtUtil::Exception
   {
   public:
   	HeightFieldOutOfBoundsException(const std::string& message, const std::string& filename, unsigned int linenum);
   	virtual ~HeightFieldOutOfBoundsException() {};
   };
   
   class HeightFieldInvalidException : public dtUtil::Exception
   {
   public:
   	HeightFieldInvalidException(const std::string& message, const std::string& filename, unsigned int linenum);
   	virtual ~HeightFieldInvalidException() {};
   };
   
   class HeightFieldInvalidImageException : public dtUtil::Exception
   {
   public:
   	HeightFieldInvalidImageException(const std::string& message, const std::string& filename, unsigned int linenum);
   	virtual ~HeightFieldInvalidImageException() {};
   };
   
   
   /**
    * This class wraps an array of elevation posts.  The elevation data is 16-bit short values,
    * therefore, the elevation can be a maximum of SHRT_MAX (32767) and a minimum of 
    * SHRT_MIN (-32768) with zero equaling "flat" or at sea-level.  This range should satisfy
    * the needs of most applications.  For example, the highest peak in the world is 
    * located on Mount Everest which sits at 8850 meters or 29,035 feet.
    */ 
   class HeightField : public osg::Referenced
   {
      public:
      
         /**
          * Constructs the heightfield.  Note, the data is invalid at this point
          * until is gets allocated.
          * @see Allocate
          */
         HeightField();
         
         /**
          * Constructs the heightfield while at the same time allocated memory to 
          * store data of the desired number of rows and columns.
          * @param numCols Number of columns stored in the heightfield.
          * @param numRows Number of rows stored in the heightfield.
          * @see Allocate
          */
         HeightField(unsigned int numCols, unsigned int numRows);
         
         /**
          * Allocates enough memory to store data of the desired number of rows
          * and columns.  Any exising data is destroyed before allocating room
          * for the new data.
          * @param numCols Number of columns stored in the heightfield.
          * @param numRows Number of rows stored in the heightfield.
          * @throws HeightFieldException::OUT_OF_BOUNDS if either the number of
          *    rows or columns is equal to zero.
          */ 
         void Allocate(unsigned int numCols, unsigned int numRows);
         
         /**
          * Gets the height stored at the given row and column.
          * @param c Column in the heightfield.
          * @param r Row in the heightfield.
          * @throws HeightFieldException::OUT_OF_BOUNDS if c or r is greater than
          *    the number of rows or columns in the heightfield.
          */
         short GetHeight(unsigned int c, unsigned int r) const;
         
         /**
          * Gets a bi-linearly interpolated height value from the specified height field.
          */
         float GetInterpolatedHeight(float x, float y) const;
         
         /**
          * Sets the height stored at the given row and column.
          * @param c Column in the heightfield.
          * @param r Row in the heightfield.
          * @throws HeightFieldException::OUT_OF_BOUNDS if c or r is greater than
          *    the number of rows or columns in the heightfield.
          */
         void SetHeight(unsigned int c, unsigned int r, short newHeight);
         
         /**
          * Gets a pointer directly to the array of height values.
          * @return The height values array.  Note, this cannot be modified or destroyed.
          */
         const short *GetHeightFieldData() const 
         { 
            if (mData.capacity() == 0)
               return NULL;
            else
               return &mData[0]; 
         }
         
         /**
          * Gets the number of columns in this heightfield.
          * @return The current number of columns or zero if the data is invalid.
          */
         unsigned int GetNumColumns() const { return mNumColumns; }
         
         /**
          * Gets the number of rows in this heightfield.
          * @return The current number of rows or zero if the data is invalid.
          */
         unsigned int GetNumRows() const { return mNumRows; }
         
         /**
          * Converts this heightfield into a valid Image.
          * @return A 16-bit (565) image.
          */
         osg::Image *ConvertToImage() const;
         
         /**
          * Converts the specified image into a valid heightfield.
          * @param image The image to convert.  
          * @note The image should be a 24-bit image representing a 16-bit
          *    heightfield.  Internally,this is converted into 16-bit shorts 
          *    which are inserted into this heightfield.
          */
         void ConvertFromImage(const osg::Image &image);
         
         /**
          * Copies a chunk of raw data values into this heightfield.
          * Space for the data is automatically allocated so there is no
          * need to call Allocate() on the heightfield first.
          * @param numColumns Number of columns in the data.
          * @param numRows Number of rows in the data.
          * @param heightData Pointer to the actual data values.  If this 
          *    is NULL, no action is taken.
          */
         void ConvertFromRaw(unsigned int numColumns, unsigned int numRows,
            short *heightData);
            
         void SetXInterval(float interval) { mXInterval = interval; }
         void SetYInterval(float interval) { mYInterval = interval; }
         
         float GetXInterval() const { return mXInterval; }
         float GetYInterval() const { return mYInterval; }
         
      protected:
      
         ///Destroys the memory in use by the heightfield.
         virtual ~HeightField();
         
      private:
         unsigned int mNumColumns;
         unsigned int mNumRows;
         std::vector<short> mData;  
         float mXInterval,mYInterval;  
   };
   
}

#endif 
