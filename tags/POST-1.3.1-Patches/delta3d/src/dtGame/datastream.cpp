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
 * @author Matthew W. Campbell and David Guthrie
 */
#include <limits.h>
#include <osg/Endian>
#include "dtGame/datastream.h"

namespace dtGame
{
   static const char* LOGNAME = "DataStream";

   ///////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(DataStream::SeekTypeEnum);
   const DataStream::SeekTypeEnum DataStream::SeekTypeEnum::SET("SET");
   const DataStream::SeekTypeEnum DataStream::SeekTypeEnum::CURRENT("CURRENT");
   const DataStream::SeekTypeEnum DataStream::SeekTypeEnum::END("END");

   ///////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(DataStreamException);
   DataStreamException DataStreamException::BUFFER_INVALID("BUFFER_INVALID");
   DataStreamException DataStreamException::BUFFER_WRITE_ERROR("BUFFER_WRITE_ERROR");
   DataStreamException DataStreamException::BUFFER_READ_ERROR("BUFFER_READ_ERROR");
   DataStreamException DataStreamException::BUFFER_INVALID_POS("BUFFER_INVALID_POS");

   ///////////////////////////////////////////////////////////////////////////////
   DataStream::DataStream(): mBufferSize(0), mBufferCapacity(16), mReadPos(0), mWritePos(0)
   {
      mBuffer = new char[this->mBufferCapacity];
      mIsLittleEndian = osg::getCpuByteOrder() == osg::LittleEndian;
      mAutoFreeBuffer = true;
      mForceLittleEndian = false;
   }

   ///////////////////////////////////////////////////////////////////////////////
   DataStream::DataStream(char *buffer, unsigned int bufferSize, bool autoFree)
   {
      if (bufferSize == 0)
         EXCEPT(DataStreamException::BUFFER_INVALID,"Buffer size cannot be zero.");

      if (buffer == NULL)
         EXCEPT(DataStreamException::BUFFER_INVALID,"Source buffer is not valid.");

      mBufferSize = bufferSize;
      mBufferCapacity = bufferSize;
      mReadPos = mWritePos = 0;
      mBuffer = buffer;
      mAutoFreeBuffer = autoFree;
      mIsLittleEndian = osg::getCpuByteOrder() == osg::LittleEndian;
      mForceLittleEndian = false;
   }

   ///////////////////////////////////////////////////////////////////////////////
   DataStream::DataStream(const DataStream &rhs)
   {
      *this = rhs;
   }

   ///////////////////////////////////////////////////////////////////////////////
   DataStream &DataStream::operator=(const DataStream &rhs)
   {
      if (this != &rhs)
      {
         if (rhs.mBufferSize == 0)
            EXCEPT(DataStreamException::BUFFER_INVALID,
               "Attempted to copy an invalid data stream.  BufferSize is zero.");

         mBufferCapacity = rhs.mBufferCapacity;
         mBufferSize = rhs.mBufferSize;
         mBuffer = new char[mBufferCapacity];
         mWritePos = rhs.mWritePos;
         mReadPos = rhs.mReadPos;
         mAutoFreeBuffer = rhs.mAutoFreeBuffer;
         mForceLittleEndian = rhs.mForceLittleEndian;
         mIsLittleEndian = rhs.mIsLittleEndian;

         if (mBufferSize > 0)
            memcpy(&rhs.mBuffer[0],&mBuffer[0],mBufferSize);
      }

      return *this;
   }

   ///////////////////////////////////////////////////////////////////////////////
   DataStream::~DataStream()
   {
      if (mAutoFreeBuffer)
         delete [] mBuffer;
   }


   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(bool& c)
   {
      unsigned char val;
      Read(val);
      c = val != 0;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(bool c)
   {
      unsigned char val = c ? 1 : 0;
      Write(val);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(unsigned char& c)
   {
      if (mReadPos + sizeof(unsigned char) > mBufferSize)
          EXCEPT(DataStreamException::BUFFER_READ_ERROR,
            "Buffer underflow detected.");

      c = *((unsigned char *)(&mBuffer[mReadPos]));
      mReadPos += sizeof(unsigned char);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(unsigned char c)
   {
      if (mWritePos + sizeof(unsigned char) > mBufferCapacity)
         ResizeBuffer();

      *((unsigned char *)(&mBuffer[mWritePos])) = c;
      mWritePos += sizeof(unsigned char);
      if (mWritePos > mBufferSize)
         mBufferSize = mWritePos;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(char& c)
   {
      if (mReadPos + sizeof(char) > mBufferSize)
          EXCEPT(DataStreamException::BUFFER_READ_ERROR,
            "Buffer underflow detected.");

      c = *((char *)(&mBuffer[mReadPos]));
      mReadPos += sizeof(char);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(char c)
   {
      if (mWritePos + sizeof(char) > mBufferCapacity)
         ResizeBuffer();

      *((char *)(&mBuffer[mWritePos])) = c;
      mWritePos += sizeof(char);
      if (mWritePos > mBufferSize)
         mBufferSize = mWritePos;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(short& s)
   {
      if (mReadPos + sizeof(short) > mBufferSize)
         EXCEPT(DataStreamException::BUFFER_READ_ERROR,
            "Buffer underflow detected.");

      s = *((short *)(&mBuffer[mReadPos]));

      if (mForceLittleEndian ^ mIsLittleEndian)
         osg::swapBytes((char *)&s,sizeof(s));

      mReadPos += sizeof(short);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(short s)
   {
      if (mWritePos + sizeof(short) > mBufferCapacity)
         ResizeBuffer();

      if (mForceLittleEndian ^ mIsLittleEndian)
         osg::swapBytes((char*)&s, sizeof(s));

      *((short *)(&mBuffer[mWritePos])) = s;
      mWritePos += sizeof(short);
      if (mWritePos > mBufferSize)
         mBufferSize = mWritePos;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(unsigned short& s)
   {
      if (mReadPos + sizeof(unsigned short) > mBufferSize)
         EXCEPT(DataStreamException::BUFFER_READ_ERROR,
            "Buffer underflow detected.");

      s = *((unsigned short *)(&mBuffer[mReadPos]));

      if (mForceLittleEndian ^ mIsLittleEndian)
         osg::swapBytes((char*)&s, sizeof(s));

      mReadPos += sizeof(unsigned short);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(unsigned short s)
   {
      if (mWritePos + sizeof(unsigned short) > mBufferCapacity)
         ResizeBuffer();

      if (mForceLittleEndian ^ mIsLittleEndian)
         osg::swapBytes((char*)&s, sizeof(s));

      *((unsigned short *)(&mBuffer[mWritePos])) = s;

      mWritePos += sizeof(unsigned short);
      if (mWritePos > mBufferSize)
         mBufferSize = mWritePos;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(int& i)
   {
      if (mReadPos + sizeof(int) > mBufferSize)
         EXCEPT(DataStreamException::BUFFER_READ_ERROR,
            "Buffer underflow detected.");

      i = *((int *)(&mBuffer[mReadPos]));

      if (mForceLittleEndian ^ mIsLittleEndian)
         osg::swapBytes((char*)&i, sizeof(i));

      mReadPos += sizeof(int);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(int i)
   {
      if (mWritePos + sizeof(int) > mBufferCapacity)
         ResizeBuffer();

      if (mForceLittleEndian ^ mIsLittleEndian)
         osg::swapBytes((char*)&i, sizeof(i));

      *((int *)(&mBuffer[mWritePos])) = i;
      mWritePos += sizeof(int);
      if (mWritePos > mBufferSize)
         mBufferSize = mWritePos;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(unsigned& i)
   {

      if (mReadPos + sizeof(unsigned) > mBufferSize)
         EXCEPT(DataStreamException::BUFFER_READ_ERROR,
            "Buffer underflow detected.");

      i = *((unsigned*)(&mBuffer[mReadPos]));

      if (mForceLittleEndian ^ mIsLittleEndian)
         osg::swapBytes((char*)&i, sizeof(i));

      mReadPos += sizeof(unsigned);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(unsigned int i)
   {
      if (mWritePos + sizeof(unsigned int) > mBufferCapacity)
         ResizeBuffer();

      if (mForceLittleEndian ^ mIsLittleEndian)
         osg::swapBytes((char*)&i, sizeof(i));

      *((unsigned int *)(&mBuffer[mWritePos])) = i;
      mWritePos += sizeof(unsigned int);
      if (mWritePos > mBufferSize)
         mBufferSize = mWritePos;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(long& i)
   {
      if (mReadPos + sizeof(long) > mBufferSize)
         EXCEPT(DataStreamException::BUFFER_READ_ERROR,
            "Buffer underflow detected.");

      i = *((long *)(&mBuffer[mReadPos]));

      if (mForceLittleEndian ^ mIsLittleEndian)
         osg::swapBytes((char*)&i, sizeof(i));

      mReadPos += sizeof(long);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(long i)
   {
      if (mWritePos + sizeof(long) > mBufferCapacity)
         ResizeBuffer();

      if (mForceLittleEndian ^ mIsLittleEndian)
         osg::swapBytes((char*)&i, sizeof(i));

      *((long *)(&mBuffer[mWritePos])) = i;
      mWritePos += sizeof(long);
      if (mWritePos > mBufferSize)
         mBufferSize = mWritePos;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(unsigned long& i)
   {

      if (mReadPos + sizeof(unsigned long) > mBufferSize)
         EXCEPT(DataStreamException::BUFFER_READ_ERROR,
            "Buffer underflow detected.");

      i = *((unsigned long*)(&mBuffer[mReadPos]));

      if (mForceLittleEndian ^ mIsLittleEndian)
         osg::swapBytes((char*)&i, sizeof(i));

      mReadPos += sizeof(unsigned long);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(unsigned long i)
   {
      if (mWritePos + sizeof(unsigned long) > mBufferCapacity)
         ResizeBuffer();

      if (mForceLittleEndian ^ mIsLittleEndian)
         osg::swapBytes((char*)&i, sizeof(i));

      *((unsigned long *)(&mBuffer[mWritePos])) = i;
      mWritePos += sizeof(unsigned long);
      if (mWritePos > mBufferSize)
         mBufferSize = mWritePos;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(float& f)
   {
      if (mReadPos + sizeof(float) > mBufferSize)
         EXCEPT(DataStreamException::BUFFER_READ_ERROR,
            "Buffer underflow detected.");

      f = *((float *)(&mBuffer[mReadPos]));

      if (mForceLittleEndian ^ mIsLittleEndian)
         osg::swapBytes((char*)&f, sizeof(f));

      mReadPos += sizeof(float);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(float f)
   {
      if (mWritePos + sizeof(float) > mBufferCapacity)
         ResizeBuffer();

      if (mForceLittleEndian ^ mIsLittleEndian)
         osg::swapBytes((char*)&f, sizeof(f));

      *((float *)(&mBuffer[mWritePos])) = f;
      mWritePos += sizeof(float);
      if (mWritePos > mBufferSize)
         mBufferSize = mWritePos;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(double& d)
   {
      if (mReadPos + sizeof(double) > mBufferSize)
         EXCEPT(DataStreamException::BUFFER_READ_ERROR,
            "Buffer underflow detected.");

      d = *((double *)(&mBuffer[mReadPos]));

      if (mForceLittleEndian ^ mIsLittleEndian)
         osg::swapBytes((char*)&d, sizeof(d));

      mReadPos += sizeof(double);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(double d)
   {
      if (mWritePos + sizeof(double) > mBufferCapacity)
         ResizeBuffer();

      if (mForceLittleEndian ^ mIsLittleEndian)
         osg::swapBytes((char*)&d, sizeof(d));

      *((double *)(&mBuffer[mWritePos])) = d;
      mWritePos += sizeof(double);
      if (mWritePos > mBufferSize)
         mBufferSize = mWritePos;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(std::string& str)
   {
      if (mReadPos + sizeof(unsigned char) > mBufferSize)
          EXCEPT(DataStreamException::BUFFER_READ_ERROR,
            "Buffer underflow detected.");

      signed char c = *((signed char *)(&mBuffer[mReadPos]));

      unsigned strSize;
      if (c < 0)
      {
         short sStrSize;
         Read(sStrSize);
         strSize = (unsigned)(-sStrSize);
      }
      else
      {
         unsigned char cStrSize;
         Read(cStrSize);
         strSize = (unsigned)cStrSize;
      }

      str.clear();
      str.reserve(strSize);
      for (unsigned i = strSize; i > 0; --i)
      {
         unsigned char c;
         Read(c);
         str.append(1, (char)c);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(const std::string &str)
   {
      //it will truncate any strings longer than a short can handle

      if (str.length() > SHRT_MAX)
         LOGN_WARNING(LOGNAME, "Attempting to write string a string longer than the max size for messages, truncating.");

      short strSize = (short)str.length();

      //for short and long strings: write one byte for a short string and write the
      //the negative length for the long string so that when the string is read back in, it
      //the first bit of the size can be checked to see if one should read one or two bytes.
      //this could technically fail if the string is longer than a signed int can hold, but one would not
      //be wise to send a string that long.
      if (strSize < 128)
         Write((unsigned char)strSize);
      else
         Write((short)-strSize);

      while (mWritePos + strSize > mBufferCapacity)
         ResizeBuffer();

      memcpy(mBuffer+mWritePos, str.c_str(), strSize);

      mWritePos += strSize;
      if (mWritePos > mBufferSize)
         mBufferSize = mWritePos;

   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(dtCore::UniqueId& id)
   {
      std::string value;
      Read(value);
      id = value;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(const dtCore::UniqueId& id)
   {
      Write(id.ToString());
   }


   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(const osg::Vec2f& vec)
   {
      Write(vec[0]);
      Write(vec[1]);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(osg::Vec2f& vec)
   {
      Read(vec[0]);
      Read(vec[1]);
   }
   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(const osg::Vec2d &vec)
   {
      Write(vec[0]);
      Write(vec[1]);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(osg::Vec2d& vec)
   {
      Read(vec[0]);
      Read(vec[1]);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(const osg::Vec3f& vec)
   {
      Write(vec[0]);
      Write(vec[1]);
      Write(vec[2]);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(osg::Vec3f& vec)
   {
      Read(vec[0]);
      Read(vec[1]);
      Read(vec[2]);
   }
   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(const osg::Vec3d& vec)
   {
      Write(vec[0]);
      Write(vec[1]);
      Write(vec[2]);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(osg::Vec3d& vec)
   {
      Read(vec[0]);
      Read(vec[1]);
      Read(vec[2]);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(const osg::Vec4f& vec)
   {
      Write(vec[0]);
      Write(vec[1]);
      Write(vec[2]);
      Write(vec[3]);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(osg::Vec4f& vec)
   {
      Read(vec[0]);
      Read(vec[1]);
      Read(vec[2]);
      Read(vec[3]);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(const osg::Vec4d& vec)
   {
      Write(vec[0]);
      Write(vec[1]);
      Write(vec[2]);
      Write(vec[3]);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(osg::Vec4d& vec)
   {
      Read(vec[0]);
      Read(vec[1]);
      Read(vec[2]);
      Read(vec[3]);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Seekp(unsigned int offset, const SeekTypeEnum &type)
   {
      //Position the write marker..
      if (type == SeekTypeEnum::SET)
      {
         if (offset > mBufferSize)
            EXCEPT(DataStreamException::BUFFER_INVALID_POS,
               "Write position cannot be greater than the current data size.");

         mWritePos = offset;
      }
      else if (type == SeekTypeEnum::CURRENT)
      {
         if (mWritePos + offset > mBufferSize)
            EXCEPT(DataStreamException::BUFFER_INVALID_POS,
               "Write position cannot be greater than the current data size.");

         mWritePos += offset;
      }
      else if (type == SeekTypeEnum::END)
      {
         if (offset > mBufferSize)
            EXCEPT(DataStreamException::BUFFER_INVALID_POS,
               "Specified offset is greater than the current data size.");
         mWritePos = mBufferSize - offset;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::Seekg(unsigned int offset, const SeekTypeEnum &type)
   {
      //Position the read marker..
      if (type == SeekTypeEnum::SET)
      {
         if (offset > mBufferSize)
            EXCEPT(DataStreamException::BUFFER_INVALID_POS,
               "Read position cannot be greater than the current data size.");
         mReadPos = offset;
      }
      else if (type == SeekTypeEnum::CURRENT)
      {
         if (mReadPos + offset > mBufferSize)
            EXCEPT(DataStreamException::BUFFER_INVALID_POS,
               "Read position cannot be greater than the current data size.");
         mReadPos += offset;
      }
      else if (type == SeekTypeEnum::END)
      {
         if (offset > mBufferSize)
            EXCEPT(DataStreamException::BUFFER_INVALID_POS,
               "Specified offset is greater than the current data size.");
         mReadPos = mBufferSize - offset;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DataStream::ResizeBuffer()
   {
      unsigned int newSize = mBufferCapacity * 2;
      char *newBuffer = new char[newSize];
      memcpy(&newBuffer[0],&mBuffer[0],mBufferSize);
      delete [] mBuffer;
      mBuffer = newBuffer;
      mBufferCapacity = newSize;
   }

}
