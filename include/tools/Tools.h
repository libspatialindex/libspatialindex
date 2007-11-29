// Spatial Index Library
//
// Copyright (C) 2004  Navel Ltd.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Email:
//    mhadji@gmail.com

#ifndef __tools_h
#define __tools_h

#include <stdint.h>
#include <assert.h>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <string>
#include <sstream>
#include <fstream>
#include <queue>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <list>
#include <algorithm>
#include <cmath>
#include <limits>
#include <climits>

#if HAVE_PTHREAD_H
#include <pthread.h>
#endif

#include "SmartPointer.h"
#include "PointerPool.h"
#include "PoolPointer.h"

#define interface class
typedef uint8_t byte;

namespace Tools
{
	enum IntervalType
	{
		IT_RIGHTOPEN = 0x0,
		IT_LEFTOPEN,
		IT_OPEN,
		IT_CLOSED
	};

	enum VariantType
	{
		VT_LONG = 0x0,
		VT_BYTE,
		VT_SHORT,
		VT_FLOAT,
		VT_DOUBLE,
		VT_CHAR,
		VT_USHORT,
		VT_ULONG,
		VT_INT,
		VT_UINT,
		VT_BOOL,
		VT_PCHAR,
		VT_PVOID,
		VT_EMPTY,
		VT_LONGLONG,
		VT_ULONGLONG
	};

	//
	// Exceptions
	//
	class Exception
	{
	public:
		virtual std::string what() = 0;
		virtual ~Exception() {}
	};

	class IndexOutOfBoundsException : public Exception
	{
	public:
		IndexOutOfBoundsException(size_t i);
		virtual ~IndexOutOfBoundsException() {}
		virtual std::string what();

	private:
		std::string m_error;
	}; // IndexOutOfBoundsException

	class IllegalArgumentException : public Exception
	{
	public:
		IllegalArgumentException(std::string s);
		virtual ~IllegalArgumentException() {}
		virtual std::string what();

	private:
		std::string m_error;
	}; // IllegalArgumentException

	class IllegalStateException : public Exception
	{
	public:
		IllegalStateException(std::string s);
		virtual ~IllegalStateException() {}
		virtual std::string what();

	private:
		std::string m_error;
	}; // IllegalStateException

	class EndOfStreamException : public Exception
	{
	public:
		EndOfStreamException(std::string s);
		virtual ~EndOfStreamException() {}
		virtual std::string what();

	private:
		std::string m_error;
	}; // EndOfStreamException

	class ResourceLockedException : public Exception
	{
	public:
		ResourceLockedException(std::string s);
		virtual ~ResourceLockedException() {}
		virtual std::string what();

	private:
		std::string m_error;
	}; // ResourceLockedException

	class InvalidPageException : public Exception
	{
	public:
		InvalidPageException(size_t id);
		virtual ~InvalidPageException() {}
		virtual std::string what();

	private:
		std::string m_error;
	}; // InvalidPageException

	class NotSupportedException : public Exception
	{
	public:
		NotSupportedException(std::string s);
		virtual ~NotSupportedException() {}
		virtual std::string what();

	private:
		std::string m_error;
	}; // NotSupportedException

	//
	// Interfaces
	//
	interface IInterval
	{
	public:
		virtual ~IInterval() {}

		virtual IInterval& operator=(const IInterval&) = 0;
		virtual double getLowerBound() const = 0;
		virtual double getUpperBound() const = 0;
		virtual void setBounds(double, double) = 0;
		virtual bool intersectsInterval(const IInterval&) const = 0;
		virtual bool intersectsInterval(IntervalType type, double start, double end) const = 0;
		virtual bool containsInterval(const IInterval&) const = 0;
		virtual IntervalType getIntervalType() const = 0;
	}; // IInterval

	interface IObject
	{
	public:
		virtual ~IObject() {}

		virtual IObject* clone() = 0;
			// return a new object that is an exact copy of this one.
			// IMPORTANT: do not return the this pointer!
	}; // IObject

	interface ISerializable //: public virtual IObject
	{
	public:
		virtual ~ISerializable() {}

		virtual size_t getByteArraySize() = 0;
			// returns the size of the required byte array.
		virtual void loadFromByteArray(const byte* data) = 0;
			// load this object using the byte array.
		virtual void storeToByteArray(byte** data, size_t& length) = 0;
			// store this object in the byte array.
	};

	interface IComparable //: public virtual IObject
	{
	public:
		virtual ~IComparable() {}

		virtual bool operator<(const IComparable& o) const = 0;
		virtual bool operator>(const IComparable& o) const = 0;
		virtual bool operator==(const IComparable& o) const = 0;
	}; //IComparable

	interface IObjectComparator
	{
	public:
		virtual ~IObjectComparator() {}

		virtual int compare(IObject* o1, IObject* o2) = 0;
	}; // IObjectComparator

	interface IObjectStream
	{
	public:
		virtual ~IObjectStream() {}

		virtual IObject* getNext() = 0;
			// returns a pointer to the next entry in the
			// stream or 0 at the end of the stream.

		virtual bool hasNext() = 0;
			// returns true if there are more items in the stream.

		virtual size_t size() = 0;
			// returns the total number of entries available in the stream.

		virtual void rewind() = 0;
			// sets the stream pointer to the first entry, if possible.
	}; // IObjectStream

	//
	// Classes & Functions
	//

	class Variant
	{
	public:
		Variant();

		VariantType m_varType;

		union
		{
			int16_t iVal;              // VT_SHORT
			int32_t lVal;              // VT_LONG
			int64_t llVal;             // VT_LONGLONG
			byte bVal;                 // VT_BYTE
			float fltVal;              // VT_FLOAT
			double dblVal;             // VT_DOUBLE
			char cVal;                 // VT_CHAR
			uint16_t uiVal;            // VT_USHORT
			uint32_t ulVal;            // VT_ULONG
			uint64_t ullVal;           // VT_ULONGLONG
			bool blVal;                // VT_BOOL
			char* pcVal;               // VT_PCHAR
			void* pvVal;               // VT_PVOID
		} m_val;
	}; // Variant

	class PropertySet : public ISerializable
	{
	public:
		PropertySet() {}
		PropertySet(const byte* data);
		virtual ~PropertySet() {}

		Variant getProperty(std::string property);
		void setProperty(std::string property, Variant& v);
		void removeProperty(std::string property);

		virtual size_t getByteArraySize();
		virtual void loadFromByteArray(const byte* data);
		virtual void storeToByteArray(byte** data, size_t& length);

	private:
		std::map<std::string, Variant> m_propertySet;

		friend std::ostream& Tools::operator<<(
			std::ostream& os,
			const Tools::PropertySet& p
		);
	}; // PropertySet

	std::ostream& operator<<(std::ostream& os, const Tools::PropertySet& p);

	// does not support degenerate intervals.
	class Interval : public IInterval
	{
	public:
		Interval();
		Interval(IntervalType, double, double);
		Interval(double, double);
		Interval(const Interval&);
		virtual IInterval& operator=(const IInterval&);
		virtual Interval& operator=(const Interval&);
		virtual bool operator==(const Interval&) const;
		virtual bool operator!=(const Interval&) const;
		virtual double getLowerBound() const;
		virtual double getUpperBound() const;
		virtual void setBounds(double, double);
		virtual bool intersectsInterval(const IInterval&) const;
		virtual bool intersectsInterval(IntervalType type, double start, double end) const;
		virtual bool containsInterval(const IInterval&) const;
		virtual IntervalType getIntervalType() const;
		virtual ~Interval() {}

		IntervalType m_type;
		double m_low;
		double m_high;
	}; // Interval

	std::ostream& operator<<(std::ostream& os, const Tools::Interval& iv);

	class Random
	{
	public:
		Random();
		Random(uint32_t seed, uint16_t xsubi0);
		virtual ~Random();

		int32_t nextUniformLong();
			// returns a uniformly distributed long.
		uint32_t nextUniformUnsignedLong();
			// returns a uniformly distributed unsigned long.
		int32_t nextUniformLong(int32_t low, int32_t high);
			// returns a uniformly distributed long in the range [low, high).
		uint32_t nextUniformUnsignedLong(uint32_t low, uint32_t high);
			// returns a uniformly distributed unsigned long in the range [low, high).
		int64_t nextUniformLongLong();
			// returns a uniformly distributed long long.
		uint64_t nextUniformUnsignedLongLong();
			// returns a uniformly distributed unsigned long long.
		int64_t nextUniformLongLong(int64_t low, int64_t high);
			// returns a uniformly distributed unsigned long long in the range [low, high).
		uint64_t nextUniformUnsignedLongLong(uint64_t low, uint64_t high);
			// returns a uniformly distributed unsigned long long in the range [low, high).
		int16_t nextUniformShort();
			// returns a uniformly distributed short.
		uint16_t nextUniformUnsignedShort();
			// returns a uniformly distributed unsigned short.
		double nextUniformDouble();
			// returns a uniformly distributed double in the range [0, 1).
		double nextUniformDouble(double low, double high);
			// returns a uniformly distributed double in the range [low, high).
		bool flipCoin();

	private:
		void initDrand(uint32_t seed, uint16_t xsubi0);

		uint16_t* m_pBuffer;
	}; // Random

	class SharedLock
	{
	public:
#if HAVE_PTHREAD_H
		SharedLock(pthread_rwlock_t* pLock);
		~SharedLock();

	private:
		pthread_rwlock_t* m_pLock;
#endif
	}; // SharedLock

	class ExclusiveLock
	{
	public:
#if HAVE_PTHREAD_H
		ExclusiveLock(pthread_rwlock_t* pLock);
		~ExclusiveLock();

	private:
		pthread_rwlock_t* m_pLock;
#endif
	}; // ExclusiveLock

	IObjectStream* externalSort(IObjectStream& source, size_t bufferSize);
	IObjectStream* externalSort(IObjectStream& source, IObjectComparator& pComp, size_t bufferSize);
}

#include "TemporaryFile.h"

#endif /* __tools_h */
