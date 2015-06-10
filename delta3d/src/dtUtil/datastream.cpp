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
#include <prefix/dtutilprefix.h>
#include <climits>
#include <osg/Endian>
#include <dtUtil/exception.h>
#include <dtUtil/datastream.h>
#include <cstring>

namespace dtUtil
{
   static const char* LOGNAME = "DataStream";

   /////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(DataStream::SeekTypeEnum)
   const DataStream::SeekTypeEnum DataStream::SeekTypeEnum::SET("SET");
   const DataStream::SeekTypeEnum DataStream::SeekTypeEnum::CURRENT("CURRENT");
   const DataStream::SeekTypeEnum DataStream::SeekTypeEnum::END("END");

   /////////////////////////////////////////////////////////////////////////////
   DataStream::DataStream()
      : mBuffer(NULL)
      , mBufferSize(0)
      , mBufferCapacity(16)
      , mReadPos(0)
      , mWritePos(0)
      , mAutoFreeBuffer(true)
      , mForceLittleEndian(false)
   {
      mBuffer = new char[this->mBufferCapacity];
      mIsLittleEndian = osg::getCpuByteOrder() == osg::LittleEndian;
   }

   /////////////////////////////////////////////////////////////////////////////
   DataStream::DataStream(char* buffer, unsigned int bufferSize, bool autoFree)
      : mBuffer(buffer)
      , mBufferSize(bufferSize)
      , mBufferCapacity(bufferSize)
      , mReadPos(0)
      , mWritePos(0)
      , mAutoFreeBuffer(autoFree)
      , mForceLittleEndian(false)
   {
      if (bufferSize == 0)
      {
         throw DataStreamBufferInvalid("Buffer size cannot be zero.", __FILE__, __LINE__);
      }

      if (buffer == NULL)
      {
         throw DataStreamBufferInvalid("Source buffer is not valid.", __FILE__, __LINE__);
      }

      mIsLittleEndian = osg::getCpuByteOrder() == osg::LittleEndian;
   }

   /////////////////////////////////////////////////////////////////////////////
   DataStream::DataStream(const DataStream& rhs)
   {
      *this = rhs;
   }

   /////////////////////////////////////////////////////////////////////////////
   DataStream& DataStream::operator=(const DataStream& rhs)
   {
      if (this != &rhs)
      {
         if (rhs.mBufferSize == 0)
         {
            throw DataStreamBufferInvalid("Attempted to copy an invalid data stream.  BufferSize is zero.", __FILE__, __LINE__);
         }

         mBufferCapacity    = rhs.mBufferCapacity;
         mBufferSize        = rhs.mBufferSize;
         mBuffer            = new char[mBufferCapacity];
         mWritePos          = rhs.mWritePos;
         mReadPos           = rhs.mReadPos;
         mAutoFreeBuffer    = rhs.mAutoFreeBuffer;
         mForceLittleEndian = rhs.mForceLittleEndian;
         mIsLittleEndian    = rhs.mIsLittleEndian;

         if (mBufferSize > 0)
         {
            memcpy(&mBuffer[0], &rhs.mBuffer[0], mBufferSize);
         }
      }

      return *this;
   }

   /////////////////////////////////////////////////////////////////////////////
   DataStream::~DataStream()
   {
      if (mAutoFreeBuffer)
      {
         delete[] mBuffer;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(bool& c)
   {
      unsigned char val;
      Read(val);
      c = val != 0;
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(bool c)
   {
      unsigned char val = c ? 1 : 0;
      Write(val);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(unsigned char& c)
   {
      if (mReadPos + sizeof(unsigned char) > mBufferSize)
      {
         throw DataStreamBufferReadError("Buffer underflow detected.", __FILE__, __LINE__);
      }

      c = *((unsigned char *)(&mBuffer[mReadPos]));
      mReadPos += sizeof(unsigned char);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(unsigned char c)
   {
      if (mWritePos + sizeof(unsigned char) > mBufferCapacity)
      {
         ResizeBuffer();
      }

      *((unsigned char *)(&mBuffer[mWritePos])) = c;
      mWritePos += sizeof(unsigned char);
      if (mWritePos > mBufferSize)
      {
         mBufferSize = mWritePos;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::WriteBytes(unsigned char c, size_t count)
   {
      for (size_t i = 0; i < count; ++i)
      {
         Write(c);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(char& c)
   {
      if (mReadPos + sizeof(char) > mBufferSize)
      {
         throw DataStreamBufferReadError("Buffer underflow detected.", __FILE__, __LINE__);
      }

      c = *((char *)(&mBuffer[mReadPos]));
      mReadPos += sizeof(char);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(char c)
   {
      if (mWritePos + sizeof(char) > mBufferCapacity)
      {
         ResizeBuffer();
      }

      *((char *)(&mBuffer[mWritePos])) = c;
      mWritePos += sizeof(char);
      if (mWritePos > mBufferSize)
      {
         mBufferSize = mWritePos;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(short& s)
   {
      if (mReadPos + sizeof(short) > mBufferSize)
      {
         throw DataStreamBufferReadError("Buffer underflow detected.", __FILE__, __LINE__);
      }

      s = *((short *)(&mBuffer[mReadPos]));

      if (mForceLittleEndian ^ mIsLittleEndian)
      {
         osg::swapBytes((char*)&s,sizeof(s));
      }

      mReadPos += sizeof(short);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(short s)
   {
      if (mWritePos + sizeof(short) > mBufferCapacity)
      {
         ResizeBuffer();
      }

      if (mForceLittleEndian ^ mIsLittleEndian)
      {
         osg::swapBytes((char*)&s, sizeof(s));
      }

      *((short *)(&mBuffer[mWritePos])) = s;
      mWritePos += sizeof(short);
      if (mWritePos > mBufferSize)
      {
         mBufferSize = mWritePos;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(unsigned short& s)
   {
      if (mReadPos + sizeof(unsigned short) > mBufferSize)
      {
         throw DataStreamBufferReadError("Buffer underflow detected.", __FILE__, __LINE__);
      }

      s = *((unsigned short *)(&mBuffer[mReadPos]));

      if (mForceLittleEndian ^ mIsLittleEndian)
      {
         osg::swapBytes((char*)&s, sizeof(s));
      }

      mReadPos += sizeof(unsigned short);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(unsigned short s)
   {
      if (mWritePos + sizeof(unsigned short) > mBufferCapacity)
      {
         ResizeBuffer();
      }

      if (mForceLittleEndian ^ mIsLittleEndian)
      {
         osg::swapBytes((char*)&s, sizeof(s));
      }

      *((unsigned short *)(&mBuffer[mWritePos])) = s;

      mWritePos += sizeof(unsigned short);
      if (mWritePos > mBufferSize)
      {
         mBufferSize = mWritePos;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(int& i)
   {
      if (mReadPos + sizeof(int) > mBufferSize)
      {
         throw DataStreamBufferReadError("Buffer underflow detected.", __FILE__, __LINE__);
      }

      i = *((int *)(&mBuffer[mReadPos]));

      if (mForceLittleEndian ^ mIsLittleEndian)
      {
         osg::swapBytes((char*)&i, sizeof(i));
      }

      mReadPos += sizeof(int);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(int i)
   {
      if (mWritePos + sizeof(int) > mBufferCapacity)
      {
         ResizeBuffer();
      }

      if (mForceLittleEndian ^ mIsLittleEndian)
      {
         osg::swapBytes((char*)&i, sizeof(i));
      }

      *((int *)(&mBuffer[mWritePos])) = i;
      mWritePos += sizeof(int);
      if (mWritePos > mBufferSize)
      {
         mBufferSize = mWritePos;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(unsigned& i)
   {

      if (mReadPos + sizeof(unsigned) > mBufferSize)
      {
         throw DataStreamBufferReadError("Buffer underflow detected.", __FILE__, __LINE__);
      }

      i = *((unsigned*)(&mBuffer[mReadPos]));

      if (mForceLittleEndian ^ mIsLittleEndian)
      {
         osg::swapBytes((char*)&i, sizeof(i));
      }

      mReadPos += sizeof(unsigned);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(unsigned int i)
   {
      if (mWritePos + sizeof(unsigned int) > mBufferCapacity)
      {
         ResizeBuffer();
      }

      if (mForceLittleEndian ^ mIsLittleEndian)
         osg::swapBytes((char*)&i, sizeof(i));

      *((unsigned int *)(&mBuffer[mWritePos])) = i;
      mWritePos += sizeof(unsigned int);
      if (mWritePos > mBufferSize)
      {
         mBufferSize = mWritePos;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(long& i)
   {
      if (mReadPos + sizeof(long) > mBufferSize)
      {
         throw DataStreamBufferReadError("Buffer underflow detected.", __FILE__, __LINE__);
      }

      i = *((long *)(&mBuffer[mReadPos]));

      if (mForceLittleEndian ^ mIsLittleEndian)
      {
         osg::swapBytes((char*)&i, sizeof(i));
      }

      mReadPos += sizeof(long);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(long i)
   {
      if (mWritePos + sizeof(long) > mBufferCapacity)
      {
         ResizeBuffer();
      }

      if (mForceLittleEndian ^ mIsLittleEndian)
      {
         osg::swapBytes((char*)&i, sizeof(i));
      }

      *((long *)(&mBuffer[mWritePos])) = i;
      mWritePos += sizeof(long);
      if (mWritePos > mBufferSize)
      {
         mBufferSize = mWritePos;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(unsigned long& i)
   {

      if (mReadPos + sizeof(unsigned long) > mBufferSize)
      {
         throw DataStreamBufferReadError("Buffer underflow detected.", __FILE__, __LINE__);
      }

      i = *((unsigned long*)(&mBuffer[mReadPos]));

      if (mForceLittleEndian ^ mIsLittleEndian)
      {
         osg::swapBytes((char*)&i, sizeof(i));
      }

      mReadPos += sizeof(unsigned long);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(unsigned long i)
   {
      if (mWritePos + sizeof(unsigned long) > mBufferCapacity)
      {
         ResizeBuffer();
      }

      if (mForceLittleEndian ^ mIsLittleEndian)
      {
         osg::swapBytes((char*)&i, sizeof(i));
      }

      *((unsigned long *)(&mBuffer[mWritePos])) = i;
      mWritePos += sizeof(unsigned long);
      if (mWritePos > mBufferSize)
      {
         mBufferSize = mWritePos;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(float& f)
   {
      if (mReadPos + sizeof(float) > mBufferSize)
      {
         throw DataStreamBufferReadError("Buffer underflow detected.", __FILE__, __LINE__);
      }

      memcpy(&f, &mBuffer[mReadPos], sizeof(float));
      // The following line was unsafe - it actually corrupts your data on Windows!
      // Apparently, MS makes sure it's not an invalid float (ie NAN) when it casts.
      // If it is invalid, it can CHANGE a bit to make it valid.
      //f = *((float *)((char *)&mBuffer[mReadPos]));

      if (mForceLittleEndian ^ mIsLittleEndian)
      {
         osg::swapBytes((char*)&f, sizeof(float));
      }

      mReadPos += sizeof(float);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(float f)
   {
      if (mWritePos + sizeof(float) > mBufferCapacity)
      {
         ResizeBuffer();
      }

      memcpy(&mBuffer[mWritePos], &f, sizeof(float));
      if (mForceLittleEndian ^ mIsLittleEndian)
      {
         osg::swapBytes4(&mBuffer[mWritePos]);
      }

      // Memcpy is safer (see Read(float))
      //*((float *)(&mBuffer[mWritePos])) = f;
      mWritePos += sizeof(float);
      if (mWritePos > mBufferSize)
      {
         mBufferSize = mWritePos;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(double& d)
   {
      if (mReadPos + sizeof(double) > mBufferSize)
      {
         throw DataStreamBufferReadError("Buffer underflow detected.", __FILE__, __LINE__);
      }

      d = *((double *)(&mBuffer[mReadPos]));

      if (mForceLittleEndian ^ mIsLittleEndian)
      {
         osg::swapBytes((char*)&d, sizeof(d));
      }

      mReadPos += sizeof(double);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(double d)
   {
      if (mWritePos + sizeof(double) > mBufferCapacity)
      {
         ResizeBuffer();
      }

      if (mForceLittleEndian ^ mIsLittleEndian)
      {
         osg::swapBytes((char*)&d, sizeof(d));
      }

      *((double *)(&mBuffer[mWritePos])) = d;
      mWritePos += sizeof(double);
      if (mWritePos > mBufferSize)
      {
         mBufferSize = mWritePos;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(long long& d)
   {
      if (mReadPos + sizeof(long long) > mBufferSize)
      {
         throw DataStreamBufferReadError("Buffer underflow detected.", __FILE__, __LINE__);
      }

      d = *((long long *)(&mBuffer[mReadPos]));

      if (mForceLittleEndian ^ mIsLittleEndian)
      {
         osg::swapBytes((char*)&d, sizeof(d));
      }

      mReadPos += sizeof(long long);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(long long d)
   {
      if (mWritePos + sizeof(long long) > mBufferCapacity)
      {
         ResizeBuffer();
      }

      if (mForceLittleEndian ^ mIsLittleEndian)
      {
         osg::swapBytes((char*)&d, sizeof(d));
      }

      *((long long *)(&mBuffer[mWritePos])) = d;
      mWritePos += sizeof(long long);
      if (mWritePos > mBufferSize)
      {
         mBufferSize = mWritePos;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(unsigned long long& d)
   {
      if (mReadPos + sizeof(long long) > mBufferSize)
      {
         throw DataStreamBufferReadError("Buffer underflow detected.", __FILE__, __LINE__);
      }

      d = *((unsigned long long *)(&mBuffer[mReadPos]));

      if (mForceLittleEndian ^ mIsLittleEndian)
      {
         osg::swapBytes((char*)&d, sizeof(d));
      }

      mReadPos += sizeof(long long);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(unsigned long long d)
   {
      if (mWritePos + sizeof(long long) > mBufferCapacity)
      {
         ResizeBuffer();
      }

      if (mForceLittleEndian ^ mIsLittleEndian)
      {
         osg::swapBytes((char*)&d, sizeof(d));
      }

      *((unsigned long long *)(&mBuffer[mWritePos])) = d;
      mWritePos += sizeof(long long);
      if (mWritePos > mBufferSize)
      {
         mBufferSize = mWritePos;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(std::string& str)
   {
      if (mReadPos + sizeof(unsigned char) > mBufferSize)
      {
         throw DataStreamBufferReadError("Buffer underflow detected.", __FILE__, __LINE__);
      }

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
         unsigned char c2;
         Read(c2);
         str.append(1, (char)c2);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(const std::string& str)
   {
      // it will truncate any strings longer than a short can handle

      if (str.length() > SHRT_MAX)
      {
         LOGN_WARNING(LOGNAME,
            "Attempting to write string a string longer than the max size for messages, truncating.");
      }

      short strSize = (short)str.length();

      // for short and long strings: write one byte for a short string and write the
      // the negative length for the long string so that when the string is read back in, it
      // the first bit of the size can be checked to see if one should read one or two bytes.
      // this could technically fail if the string is longer than a signed int can hold, but one would not
      // be wise to send a string that long.
      if (strSize < 128)
      {
         Write((unsigned char)strSize);
      }
      else
      {
         Write((short)-strSize);
      }

      while (mWritePos + strSize > mBufferCapacity)
      {
         ResizeBuffer();
      }

      memcpy(mBuffer+mWritePos, str.c_str(), strSize);

      mWritePos += strSize;
      if (mWritePos > mBufferSize)
      {
         mBufferSize = mWritePos;
      }

   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(const osg::Vec2f& vec)
   {
      Write(vec[0]);
      Write(vec[1]);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(osg::Vec2f& vec)
   {
      Read(vec[0]);
      Read(vec[1]);
   }
   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(const osg::Vec2d &vec)
   {
      Write(vec[0]);
      Write(vec[1]);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(osg::Vec2d& vec)
   {
      Read(vec[0]);
      Read(vec[1]);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(const osg::Vec3f& vec)
   {
      Write(vec[0]);
      Write(vec[1]);
      Write(vec[2]);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(osg::Vec3f& vec)
   {
      Read(vec[0]);
      Read(vec[1]);
      Read(vec[2]);
   }
   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(const osg::Vec3d& vec)
   {
      Write(vec[0]);
      Write(vec[1]);
      Write(vec[2]);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(osg::Vec3d& vec)
   {
      Read(vec[0]);
      Read(vec[1]);
      Read(vec[2]);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(const osg::Vec4f& vec)
   {
      Write(vec[0]);
      Write(vec[1]);
      Write(vec[2]);
      Write(vec[3]);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(osg::Vec4f& vec)
   {
      Read(vec[0]);
      Read(vec[1]);
      Read(vec[2]);
      Read(vec[3]);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Write(const osg::Vec4d& vec)
   {
      Write(vec[0]);
      Write(vec[1]);
      Write(vec[2]);
      Write(vec[3]);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Read(osg::Vec4d& vec)
   {
      Read(vec[0]);
      Read(vec[1]);
      Read(vec[2]);
      Read(vec[3]);
   }

   /////////////////////////////////////////////////////////////////////////////
   unsigned int DataStream::WriteBinary(const char* pBuffer, const unsigned int size)
   {
      if (mBufferCapacity - mWritePos < size)
      {
         IncreaseBufferSize(size - (mBufferCapacity - mWritePos));
      }
      memcpy(&mBuffer[mWritePos], pBuffer, size);
      mWritePos += size;
      mBufferSize = mWritePos;
      return size;
   }

   /////////////////////////////////////////////////////////////////////////////
   unsigned int DataStream::ReadBinary(char* pBuffer, const unsigned int size)
   {
      unsigned int readSize = 0;
      if ( (mBufferSize - mReadPos) < size)
      {
         readSize = mBufferSize - mReadPos;
      }
      else
      {
         readSize = size;
      }
      memcpy(pBuffer, &mBuffer[mReadPos], readSize);
      mReadPos += readSize;
      return readSize;
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Seekp(unsigned int offset, const SeekTypeEnum& type)
   {
      //Position the write marker..
      if (type == SeekTypeEnum::SET)
      {
         if (offset > mBufferSize)
         {
            throw DataStreamBufferInvalidPos("Write position cannot be greater than the current data size.", __FILE__, __LINE__);
         }

         mWritePos = offset;
      }
      else if (type == SeekTypeEnum::CURRENT)
      {
         if (mWritePos + offset > mBufferSize)
         {
            throw DataStreamBufferInvalidPos("Write position cannot be greater than the current data size.", __FILE__, __LINE__);
         }

         mWritePos += offset;
      }
      else if (type == SeekTypeEnum::END)
      {
         if (offset > mBufferSize)
         {
            throw DataStreamBufferInvalidPos("Specified offset is greater than the current data size.", __FILE__, __LINE__);
         }
         mWritePos = mBufferSize - offset;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void DataStream::Seekg(unsigned int offset, const SeekTypeEnum& type)
   {
      //Position the read marker..
      if (type == SeekTypeEnum::SET)
      {
         if (offset > mBufferSize)
         {
            throw DataStreamBufferInvalidPos("Read position cannot be greater than the current data size.", __FILE__, __LINE__);
         }
         mReadPos = offset;
      }
      else if (type == SeekTypeEnum::CURRENT)
      {
         if (mReadPos + offset > mBufferSize)
         {
            throw DataStreamBufferInvalidPos("Read position cannot be greater than the current data size.", __FILE__, __LINE__);
         }
         mReadPos += offset;
      }
      else if (type == SeekTypeEnum::END)
      {
         if (offset > mBufferSize)
         {
            throw DataStreamBufferInvalidPos("Specified offset is greater than the current data size.", __FILE__, __LINE__);
         }
         mReadPos = mBufferSize - offset;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   unsigned int DataStream::ResizeBuffer(unsigned int size)
   {
      unsigned int newSize = 0;
      if (size == 0)
      {
         newSize = mBufferCapacity * 2;
      }
      else
      {
         newSize = size;
      }

      char* newBuffer = new char[newSize];

      // copy old buffercontents
      if (newSize < mBufferCapacity)
      {
         memcpy(&newBuffer[0], &mBuffer[0], newSize);
      }
      else
      {
         memcpy(&newBuffer[0], &mBuffer[0], mBufferCapacity);
      }

      if (mAutoFreeBuffer)
      {
         delete [] mBuffer;
      }

      mBuffer = newBuffer;
      mBufferCapacity = newSize;
      return mBufferCapacity;
   }

   /////////////////////////////////////////////////////////////////////////////
   unsigned int DataStream::IncreaseBufferSize(const unsigned int size)
   {
      return ResizeBuffer(mBufferCapacity + size);
   }

   /////////////////////////////////////////////////////////////////////////////
   unsigned int DataStream::GetRemainingReadSize() const
   {
      return mBufferSize - mReadPos;
   }

   /////////////////////////////////////////////////////////////////////////////
   unsigned int DataStream::ClearBuffer()
   {
      memset(mBuffer, 0, mBufferCapacity);
      Rewind();
      mBufferSize = 0;
      return mBufferCapacity;
   }

   /////////////////////////////////////////////////////////////////////////////
   unsigned int DataStream::AppendDataStream(const DataStream& dataStream)
   {
      IncreaseBufferSize(dataStream.GetBufferSize());
      return WriteBinary(dataStream.GetBuffer(), dataStream.GetBufferSize());
   }

   /////////////////////////////////////////////////////////////////////////////
   DataStreamBufferInvalid::DataStreamBufferInvalid(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   DataStreamBufferReadError::DataStreamBufferReadError(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   DataStreamBufferWriteError::DataStreamBufferWriteError(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   DataStreamBufferInvalidPos::DataStreamBufferInvalidPos(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
   }
} // namespace dtUtil
