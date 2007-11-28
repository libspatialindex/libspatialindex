// Tools Library
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
#include <climits>

#if TIME_WITH_SYS_TIME
	#include <sys/time.h>
	#include <time.h>
#else
	#if HAVE_SYS_TIME_H
		#include <sys/time.h>
	#else
		#include <time.h>
	#endif
#endif

#include <sys/resource.h>
#include <unistd.h>

#if HAVE_PTHREAD_H
#include <pthread.h>
#endif

#include "SmartPointer.h"
#include "PointerPool.h"
#include "PoolPointer.h"

typedef uint8_t byte;

#define interface class

namespace Tools
{
	enum IntervalType
	{
		IT_RIGHTOPEN = 0x0,
		IT_LEFTOPEN,
		IT_OPEN,
		IT_CLOSED
	};

	enum Level
	{
		LVL_VERYLOW = 0x0,
		LVL_LOW,
		LVL_MEDIUM,
		LVL_HIGH,
		LVL_VERYHIGH
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

	enum Architecture
	{
		ARCH_LITTLEENDIAN = 0x0,
		ARCH_BIGENDIAN,
		ARCH_NONIEEE
	};

	enum RandomGeneratorType
	{
		RGT_DRAND48 = 0x0,
		RGT_MERSENNE
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

	class ParseErrorException : public Exception
	{
	public:
		ParseErrorException(std::string s);
		virtual ~ParseErrorException() {}
		virtual std::string what();

	private:
		std::string m_error;
	}; // ParseErrorException

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

	namespace Geometry
	{
		class Region;
		class Point;

		enum Quadrant
		{
			Q_UPPERRIGHT = 0x0,
			Q_LOWERRIGHT,
			Q_UPPERLEFT,
			Q_LOWERLEFT
		};

		// since all base classes are interfaces (there is no state involved) all
		// inheritance can be virtual for efficiency.
		interface IShape : public virtual ISerializable
		{
		public:
			virtual bool intersectsShape(const IShape& in) const = 0;
			virtual bool containsShape(const IShape& in) const = 0;
			virtual bool touchesShape(const IShape& in) const = 0;
			virtual void getCenter(Point& out) const = 0;
			virtual size_t getDimension() const = 0;
			virtual void getMBR(Region& out) const = 0;
			virtual double getArea() const = 0;
			virtual double getMinimumDistance(const IShape& in) const = 0;
			virtual ~IShape() {}
		}; // IShape

		// since all base classes are interfaces (there is no state involved) all
		// inheritance can be virtual for efficiency.
		interface ITimeShape : public virtual IShape, public virtual IInterval
		{
		public:
			virtual bool intersectsShapeInTime(const ITimeShape& in) const = 0;
			virtual bool intersectsShapeInTime(const IInterval& ivI, const ITimeShape& in) const = 0;
			virtual bool containsShapeInTime(const ITimeShape& in) const = 0;
			virtual bool containsShapeInTime(const IInterval& ivI, const ITimeShape& in) const = 0;
			virtual bool touchesShapeInTime(const ITimeShape& in) const = 0;
			virtual bool touchesShapeInTime(const IInterval& ivI, const ITimeShape& in) const = 0;
			virtual double getAreaInTime() const = 0;
			virtual double getAreaInTime(const IInterval& ivI) const = 0;
			virtual double getIntersectingAreaInTime(const ITimeShape& r) const = 0;
			virtual double getIntersectingAreaInTime(const IInterval& ivI, const ITimeShape& r) const = 0;
			virtual ~ITimeShape() {}
		}; // ITimeShape

		// since all base classes are interfaces (there is no state involved) all
		// inheritance can be virtual for efficiency.
		interface IEvolvingShape : public virtual IShape
		{
		public:
			virtual void getVMBR(Region& out) const = 0;
			virtual void getMBRAtTime(double t, Region& out) const = 0;
			virtual ~IEvolvingShape() {}
		}; // IEvolvingShape
	}

	IObjectStream* externalSort(IObjectStream& source, size_t bufferSize);
	IObjectStream* externalSort(IObjectStream& source, IObjectComparator& pComp, size_t bufferSize);

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

	// Code for the Mersenne generator has been kindly contributed by the MassDAL Code Bank:
	// http://www.cs.rutgers.edu/~muthu/massdal-code-index.html
	class Random
	{
	public:
		Random();
		Random(uint32_t seed);
		Random(uint32_t seed, RandomGeneratorType t);
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
	
		// these use the inversion method, thus they are extremely slow. Use with caution.
		double nextNormalDouble();
			// returns doubles using a normal distribution with mean 0.0 and std 1.0 (unbounded).
		double nextNormalDouble(double mean, double std);
			// returns doubles using a normal distribution with mean mean and std std (unbounded).

		// these use the inversion method, thus they are extremely slow. Use with caution.
		int32_t nextSkewedLong(int32_t low, int32_t high, Level);
			// returns longs using a Zipf distribution in the range [low, high).
		double nextSkewedDouble(double low, double high, Level);
			// returns doubles using a Zipf distribution in the range [low, high).
		double nextSkewedDouble(Level);
			// returns doubles using a Zipf distribution in the range [0.0, 1.0).

		bool flipCoin();
			// A Bernoulli trial with probability p = 50%.
		bool bernulliTrial(double p);
			// A Bernoulli trial with probability of success p.

		size_t getSize() const;
			// Returns the total size of the random number generator (seed size, etc.).

		uint32_t getSeed() const;

	private:
		void initMersenne();
		void initDrand(uint16_t xsubi0);

		enum
		{
			MERS_N = 624,
			MERS_M = 397,
			MERS_R = 31,
			MERS_U = 11,
			MERS_S = 7,
			MERS_T = 15,
			MERS_L = 18,
			MERS_A = 0x9908B0DF,
			MERS_B = 0x9D2C5680,
			MERS_C = 0xEFC60000
		};

		RandomGeneratorType m_type;
		void* m_buffer;
		Architecture m_architecture;
		uint32_t m_seed;
	}; // Random

	class PRGZipf
	{
	public:
		PRGZipf(int32_t min, int32_t max, double s, Tools::Random* pRandom);
		virtual ~PRGZipf();

		int32_t nextLong();

	private:
		void initLookupTable();

		int32_t m_min;
		int32_t m_max;
		double m_s;
		Tools::Random* m_pRandom;
		double* m_pLookupTable;
	}; // PRGZipf

	class PRGFrechet
	{
	public:
		PRGFrechet(double a, double b, Tools::Random* pRandom);
		virtual ~PRGFrechet();

		double nextDouble();

	private:
		double m_alpha;
		double m_beta;
		Tools::Random* m_pRandom;
	}; // PRGFrechet

	class Hash
	{
	public:
		virtual ~Hash();

		virtual void hash(
			const std::string& s,
			byte** out, size_t& lout
		) = 0;
		virtual void hash(
			const byte* in, size_t lin,
			byte** out, size_t& lout
		) = 0;
	}; // Hash

	// Implements the hash functions described in:
	// Wegman and Carter
	// New classes and applications of hash functions, FOCS 1979
	class UniversalHash : public Hash
	{
	public:
		typedef uint32_t value_type;

		UniversalHash(uint16_t k = 2);
		UniversalHash(Tools::Random& r, uint16_t k = 2);
		UniversalHash(uint64_t a, uint64_t b);
		UniversalHash(const UniversalHash& h);
		UniversalHash(const byte* data);
		virtual ~UniversalHash();

		virtual UniversalHash& operator=(const UniversalHash& in);
		virtual bool operator==(const UniversalHash& in) const;

		virtual value_type hash(value_type x) const;
		virtual size_t getSize() const;
		virtual void getData(byte** buffer, size_t& length) const;

		virtual void hash(
			const std::string& s,
			byte** out, size_t& lout
		);
		virtual void hash(
			const byte* in, size_t lin,
			byte** out, size_t& lout
		);

		static const uint64_t m_P = 0x1FFFFFFFFFFFFFFFull; // 2^61 - 1

	private:
		uint64_t* m_a;
		uint16_t m_k;

		friend std::ostream& Tools::operator<<(
			std::ostream& os,
			const Tools::UniversalHash& h
		);
	}; // UniversalHash

	std::ostream& operator<<(std::ostream& os, const Tools::UniversalHash& h);

	class SHA1Hash : public Hash
	{
	public:
		virtual ~SHA1Hash();

		virtual void hash(
			const std::string& s,
			byte** out, size_t& lout
		);
		virtual void hash(
			const byte* in, size_t lin,
			byte** out, size_t& lout
		);
		virtual void hash(
			std::istream& s,
			byte** out, size_t& lout
		);
		virtual std::string hash(const std::string& s);

	public:
		static const uint16_t HashLength = 20;
	}; // SHA1Hash

#if HAVE_GETTIMEOFDAY
	class ResourceUsage
	{
	public:
		ResourceUsage();

		void start();
		void stop();
		void reset();

		double getTotalTime();
		double getUserTime();
		double getSystemTime();
		size_t getPageFaults();
		size_t getReadIO();
		size_t getWriteIO();
		size_t getPeakResidentMemoryUsage();
		size_t getTotalMemoryUsage();

	private:
		double combineTime(const struct timeval&);
		void addTimeval(struct timeval&, const struct timeval&);
		void subtractTimeval(struct timeval&, const struct timeval&, const struct timeval&);

  		struct rusage m_tmpRU;
  		struct timeval m_tmpTV;
  		struct timeval m_totalTime;
  		struct timeval m_userTime;
  		struct timeval m_systemTime;
  		size_t m_pageFaults;
  		size_t m_readIO;
  		size_t m_writeIO;
  		size_t m_peakMemory;
  		size_t m_totalMemory;
  	}; // ResourceUsage
#endif

#if BUILD_CPU_I686
	class CycleCounter
	{
	public:
		CycleCounter();
		void start();
		void stop();
		void reset();
		double getTotalCycles();
		double getCPUMHz();
		double getCyclesPerSecond();

	private:
		uint64_t m_tmpCycles;
		uint64_t m_totalCycles;
		bool m_bRunning;

		uint64_t rdtsc();
	}; // CycleCounter
#endif

	class System
	{
	public:
		static Architecture getArchitecture();
	}; // System

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

	class StringTokenizer
	{
	public:
		StringTokenizer(
			const std::string& s,
			const std::string& delimiters = " \t",
			bool bTrim = true
		);
		bool hasMoreTokens();
		std::string getNextToken();
		void reset();

	private:
		size_t m_index;
		std::vector<std::string> m_token;
	}; // StringTokenizer

	std::string trimLeft(const std::string& source, const std::string& t = " \t");
	std::string trimRight(const std::string& source, const std::string& t = " \t");
	std::string trim(const std::string& source, const std::string& t = " \t");
	char toLower(char c);
	char toUpper(char c);
	std::string toUpperCase(const std::string& s);
	std::string toLowerCase(const std::string& s);

	uint64_t choose(uint32_t n, uint32_t k);

	void compressRLE(
		size_t blockSize,
		byte* in, size_t lin,
		byte** out, size_t& lout);
	void uncompressRLE(
		size_t blockSize,
		byte* in, size_t lin,
		byte** out, size_t& lout);
}

#include "Point.h"
#include "Region.h"
#include "LineSegment.h"
#include "TemporaryFile.h"

#endif /* __tools_h */

