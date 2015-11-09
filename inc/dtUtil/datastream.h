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
#ifndef DELTA_DATASTREAM
#define DELTA_DATASTREAM

#include <string>
#include <osg/Vec2>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Vec2f>
#include <osg/Vec3f>
#include <osg/Vec4f>
#include <osg/Vec2d>
#include <osg/Vec3d>
#include <osg/Vec4d>

#include <dtUtil/enumeration.h>
#include <dtUtil/export.h>
#include <dtUtil/exception.h>

namespace dtUtil
{
   class DT_UTIL_EXPORT DataStream
   {
   public:
      class DT_UTIL_EXPORT SeekTypeEnum : public dtUtil::Enumeration
      {
         DECLARE_ENUM(SeekTypeEnum);

      public:
         static const SeekTypeEnum SET;
         static const SeekTypeEnum CURRENT;
         static const SeekTypeEnum END;

      private:
         SeekTypeEnum(const std::string& name) : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
      };

      DataStream();

      /**
       * Constructs the datastream using an existing byte buffer.
       * @param buffer The existing valid buffer.
       * @param bufferSize The size in bytes of the buffer.
       * @param reverseByteSwapping If this flag is true, the buffer will treat
       *    its contents as little endian by default instead of big endian.  This is
       *    useful if working with file data where the file is always in little endian
       *    format.
       * @param autoFree If true, the buffer's memory is freed when the DataStream
       *    instance gets destructed.  If false, the caller is responsible for freeing
       *    the associated buffer memory.
       */
      DataStream(char* buffer, unsigned int bufferSize, bool autoFree=true);
      DataStream(const DataStream& rhs);
      DataStream& operator=(const DataStream& rhs);
      virtual ~DataStream();

      DataStream& operator>>(bool& value) { Read(value); return *this; }
      DataStream& operator<<(bool value) { Write(value); return *this; }

      DataStream& operator>>(unsigned char& value) { Read(value); return *this; }
      DataStream& operator<<(unsigned char value) { Write(value); return *this; }

      DataStream& operator>>(char& value) { Read(value); return *this; }
      DataStream& operator<<(char value) { Write(value); return *this; }

      DataStream& operator>>(short& value) { Read(value); return *this; }
      DataStream& operator<<(short value) { Write(value); return *this; }

      DataStream& operator>>(unsigned short& value) { Read(value); return *this; }
      DataStream& operator<<(unsigned short value) { Write(value); return *this; }

      DataStream& operator>>(int& value) { Read(value); return *this; }
      DataStream& operator<<(int value) { Write(value); return *this; }

      DataStream& operator>>(unsigned& value) { Read(value); return *this; }
      DataStream& operator<<(unsigned value) { Write(value); return *this; }

      DataStream& operator>>(long& value) { Read(value); return *this; }
      DataStream& operator<<(long value) { Write(value); return *this; }

      DataStream& operator>>(unsigned long& value) { Read(value); return *this; }
      DataStream& operator<<(unsigned long value) { Write(value); return *this; }

      DataStream& operator>>(float& value) { Read(value); return *this; }
      DataStream& operator<<(float value) { Write(value); return *this; }

      DataStream& operator>>(double& value) { Read(value); return *this; }
      DataStream& operator<<(double value) { Write(value); return *this; }

      DataStream& operator>>(long long& value) { Read(value); return *this; }
      DataStream& operator<<(long long value) { Write(value); return *this; }

      DataStream& operator>>(unsigned long long& value) { Read(value); return *this; }
      DataStream& operator<<(unsigned long long value) { Write(value); return *this; }

      DataStream& operator>>(std::string& value) { Read(value); return *this; }
      DataStream& operator<<(const std::string& value) { Write(value); return *this; }

      //DataStream& operator>>(dtCore::UniqueId& value) { Read(value); return *this; }
      //DataStream& operator<<(const dtCore::UniqueId& value) { Write(value); return *this; }

      DataStream& operator>>(osg::Vec2f& value) { Read(value); return *this; }
      DataStream& operator<<(const osg::Vec2f& value) { Write(value); return *this; }

      DataStream& operator>>(osg::Vec2d& value) { Read(value); return *this; }
      DataStream& operator<<(const osg::Vec2d& value) { Write(value); return *this; }

      DataStream& operator>>(osg::Vec3f& value) { Read(value); return *this; }
      DataStream& operator<<(const osg::Vec3f& value) { Write(value); return *this; }

      DataStream& operator>>(osg::Vec3d& value) { Read(value); return *this; }
      DataStream& operator<<(const osg::Vec3d& value) { Write(value); return *this; }

      DataStream& operator>>(osg::Vec4f& value) { Read(value); return *this; }
      DataStream& operator<<(const osg::Vec4f& value) { Write(value); return *this; }

      DataStream& operator>>(osg::Vec4d& value) { Read(value); return *this; }
      DataStream& operator<<(const osg::Vec4d& value) { Write(value); return *this; }

      void Read(bool& c);
      void Write(bool c);

      void Read(unsigned char& c);
      void Write(unsigned char c);

      void Read(char& c);
      void Write(char c);

      void Read(short& s);
      void Write(short s);

      void Read(unsigned short& s);
      void Write(unsigned short s);

      void Read(int& i);
      void Write(int i);

      void Read(unsigned& i);
      void Write(unsigned i);

      void Read(long& i);
      void Write(long i);

      void Read(unsigned long& i);
      void Write(unsigned long i);

      void Read(float& f);
      void Write(float f);

      void Read(double& d);
      void Write(double d);

      void Read(long long& d);
      void Write(long long d);

      void Read(unsigned long long& d);
      void Write(unsigned long long d);

      void Read(std::string& string);
      void Write(const std::string& string);

      void Read(osg::Vec2f& vector);
      void Write(const osg::Vec2f& vector);

      void Read(osg::Vec2d& vector);
      void Write(const osg::Vec2d& vector);

      void Read(osg::Vec3f& vector);
      void Write(const osg::Vec3f& vector);

      void Read(osg::Vec3d& vector);
      void Write(const osg::Vec3d& vector);

      void Read(osg::Vec4f& vector);
      void Write(const osg::Vec4f& vector);

      void Read(osg::Vec4d& vector);
      void Write(const osg::Vec4d& vector);

      unsigned int ReadBinary(char* pBuffer, const unsigned int isize);
      unsigned int WriteBinary(const char* pBuffer, const unsigned int isize);

      void WriteBytes(unsigned char c, size_t count);

      unsigned int GetBufferCapacity() const { return mBufferCapacity; }
      unsigned int GetBufferSize() const { return mBufferSize; }
      unsigned int GetReadPosition() const { return mReadPos; }
      unsigned int GetWritePosition() const { return mWritePos; }

      void Rewind() { mReadPos = mWritePos = 0; }

      void Seekp(unsigned int offset, const SeekTypeEnum &type);
      void Seekg(unsigned int offset, const SeekTypeEnum &type);

      const char* GetBuffer() const { return mBuffer; }

      /**
       * Gets the endian'ness of the current platform.
       * @return True if little endian, false if big endian.
       */
      bool IsLittleEndian() const { return mIsLittleEndian; }

      /**
       * Forces the stream to interpret its data contents as little endian.
       * @param force True if the stream should interpret its contents as little endian.
       * @note This is useful when working with binary files that are guaranteed to be little
       *    endian.  The particular file loading code can therefore, read the file contents
       *    into the data stream and on big endian machines, byte swapping will occur
       *    automatically.
       */
      void SetForceLittleEndian(bool force) { mForceLittleEndian = force; }

      unsigned int SetBufferSize(unsigned int size) { return ResizeBuffer(size); };
      unsigned int IncreaseBufferSize(const unsigned int size = 0);
      unsigned int GetRemainingReadSize() const;
      unsigned int ClearBuffer();
      unsigned int AppendDataStream(const DataStream& dataStream);

   private:
      unsigned int ResizeBuffer(unsigned int size = 0);

   private:
      char* mBuffer;
      unsigned int mBufferSize, mBufferCapacity;
      unsigned int mReadPos, mWritePos;
      bool mAutoFreeBuffer;
      bool mIsLittleEndian;
      bool mForceLittleEndian;
   };

   class DT_UTIL_EXPORT DataStreamBufferInvalid : public dtUtil::Exception
   {
   public:
      DataStreamBufferInvalid(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~DataStreamBufferInvalid() {};
   };

   class DT_UTIL_EXPORT DataStreamBufferReadError : public dtUtil::Exception
   {
   public:
      DataStreamBufferReadError(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~DataStreamBufferReadError() {};
   };

   class DT_UTIL_EXPORT DataStreamBufferWriteError : public dtUtil::Exception
   {
   public:
      DataStreamBufferWriteError(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~DataStreamBufferWriteError() {};
   };

   class DT_UTIL_EXPORT DataStreamBufferInvalidPos : public dtUtil::Exception
   {
   public:
      DataStreamBufferInvalidPos(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~DataStreamBufferInvalidPos() {};
   };
}


#endif //DELTA_DATASTREAM
