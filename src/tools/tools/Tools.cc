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

#include <Tools.h>
#include "ExternalSort.h"
#include "SHA1.h"

Tools::IndexOutOfBoundsException::IndexOutOfBoundsException(size_t i)
{
	std::ostringstream s;
	s << "Invalid index " << i;
	m_error = s.str();
}

std::string Tools::IndexOutOfBoundsException::what()
{
	return "IndexOutOfBoundsException: " + m_error;
}

Tools::IllegalArgumentException::IllegalArgumentException(std::string s) : m_error(s)
{
}

std::string Tools::IllegalArgumentException::what()
{
	return "IllegalArgumentException: " + m_error;
}

Tools::IllegalStateException::IllegalStateException(std::string s) : m_error(s)
{
}

std::string Tools::IllegalStateException::what()
{
	return "IllegalStateException: " + m_error + "\nPlease contact " + PACKAGE_BUGREPORT;
}

Tools::EndOfStreamException::EndOfStreamException(std::string s) : m_error(s)
{
}

std::string Tools::EndOfStreamException::what()
{
	return "EndOfStreamException: " + m_error;
}

Tools::ResourceLockedException::ResourceLockedException(std::string s) : m_error(s)
{
}

std::string Tools::ResourceLockedException::what()
{
	return "ResourceLockedException: " + m_error;
}

Tools::InvalidPageException::InvalidPageException(size_t id)
{
	std::ostringstream s;
	s << "Unknown page id " << id;
	m_error = s.str();
}

std::string Tools::InvalidPageException::what()
{
	return "InvalidPageException: " + m_error;
}

Tools::NotSupportedException::NotSupportedException(std::string s) : m_error(s)
{
}

std::string Tools::NotSupportedException::what()
{
	return "NotSupportedException: " + m_error;
}

Tools::ParseErrorException::ParseErrorException(std::string s) : m_error(s)
{
}

std::string Tools::ParseErrorException::what()
{
	return "ParseErrorException: " + m_error;
}

Tools::Variant::Variant() : m_varType(VT_EMPTY)
{
}

Tools::PropertySet::PropertySet(const byte* data)
{
	loadFromByteArray(data);
}

void Tools::PropertySet::loadFromByteArray(const byte* ptr)
{
	m_propertySet.clear();

	size_t numberOfProperties;
	memcpy(&numberOfProperties, ptr, sizeof(size_t));
	ptr += sizeof(size_t);

	Variant v;

	for (size_t cIndex = 0; cIndex < numberOfProperties; cIndex++)
	{
		std::string s(reinterpret_cast<const char*>(ptr));
		ptr += s.size() + 1;
		memcpy(&(v.m_varType), ptr, sizeof(VariantType));
		ptr += sizeof(VariantType);

		switch (v.m_varType)
		{
		case VT_SHORT:
			int16_t s;
			memcpy(&s, ptr, sizeof(int16_t));
			ptr += sizeof(int16_t);
			v.m_val.iVal = s;
			break;
		case VT_LONG:
			int32_t l;
			memcpy(&l, ptr, sizeof(int32_t));
			ptr += sizeof(int32_t);
			v.m_val.lVal = l;
			break;
		case VT_LONGLONG:
			int64_t ll;
			memcpy(&ll, ptr, sizeof(int64_t));
			ptr += sizeof(int64_t);
			v.m_val.llVal = ll;
			break;
		case VT_BYTE:
			byte b;
			memcpy(&b, ptr, sizeof(byte));
			ptr += sizeof(byte);
			v.m_val.bVal = b;
			break;
		case VT_FLOAT:
			float f;
			memcpy(&f, ptr, sizeof(float));
			ptr += sizeof(float);
			v.m_val.fltVal = f;
			break;
		case VT_DOUBLE:
			double d;
			memcpy(&d, ptr, sizeof(double));
			ptr += sizeof(double);
			v.m_val.dblVal = d;
			break;
		case VT_CHAR:
			char c;
			memcpy(&c, ptr, sizeof(char));
			ptr += sizeof(char);
			v.m_val.cVal = c;
			break;
		case VT_USHORT:
			uint16_t us;
			memcpy(&us, ptr, sizeof(uint16_t));
			ptr += sizeof(uint16_t);
			v.m_val.uiVal = us;
			break;
		case VT_ULONG:
			uint32_t ul;
			memcpy(&ul, ptr, sizeof(uint32_t));
			ptr += sizeof(uint32_t);
			v.m_val.ulVal = ul;
			break;
		case VT_ULONGLONG:
			uint64_t ull;
			memcpy(&ull, ptr, sizeof(uint64_t));
			ptr += sizeof(uint64_t);
			v.m_val.ullVal = ull;
			break;
		case VT_BOOL:
			byte bl;
			memcpy(&bl, ptr, sizeof(byte));
			ptr += sizeof(byte);
			v.m_val.blVal = static_cast<bool>(bl);
			break;
		default:
			throw IllegalStateException(
				"Tools::PropertySet::PropertySet: Deserialization problem."
			);
		}

		m_propertySet.insert(std::pair<std::string, Variant>(s, v));
	}
}

size_t Tools::PropertySet::getByteArraySize()
{
	size_t size = sizeof(size_t);
	std::map<std::string, Variant>::iterator it;

	for (it = m_propertySet.begin(); it != m_propertySet.end(); it++)
	{
		switch ((*it).second.m_varType)
		{
		case VT_SHORT:
			size += sizeof(int16_t);
			break;
		case VT_LONG:
			size += sizeof(int32_t);
			break;
		case VT_LONGLONG:
			size += sizeof(int64_t);
			break;
		case VT_BYTE:
			size += sizeof(byte);
			break;
		case VT_FLOAT:
			size += sizeof(float);
			break;
		case VT_DOUBLE:
			size += sizeof(double);
			break;
		case VT_CHAR:
			size += sizeof(char);
			break;
		case VT_USHORT:
			size += sizeof(uint16_t);
			break;
		case VT_ULONG:
			size += sizeof(uint32_t);
			break;
		case VT_ULONGLONG:
			size += sizeof(uint64_t);
			break;
		case VT_BOOL:
			size += sizeof(byte);
			break;
		default:
			throw NotSupportedException(
				"Tools::PropertySet::getSize: Unknown type."
			);
		}
		size += (*it).first.size() + 1 + sizeof(VariantType);
	}

	return size;
}

void Tools::PropertySet::storeToByteArray(byte** data, size_t& length)
{
	length = getByteArraySize();
	*data = new byte[length];
	byte* ptr = *data;

	size_t numberOfProperties = m_propertySet.size();
	memcpy(ptr, &numberOfProperties, sizeof(size_t));
	ptr += sizeof(size_t);

	std::map<std::string, Variant>::iterator it;

	for (it = m_propertySet.begin(); it != m_propertySet.end(); it++)
	{
		size_t strSize = (*it).first.size();
		memcpy(ptr, (*it).first.c_str(), strSize);
		ptr += strSize;
		*ptr = 0;
		ptr++;

		memcpy(ptr, &((*it).second.m_varType), sizeof(VariantType));
		ptr += sizeof(VariantType);

		switch ((*it).second.m_varType)
		{
		case VT_SHORT:
			memcpy(ptr, &((*it).second.m_val.iVal), sizeof(int16_t));
			ptr += sizeof(int16_t);
			break;
		case VT_LONG:
			memcpy(ptr, &((*it).second.m_val.lVal), sizeof(int32_t));
			ptr += sizeof(int32_t);
			break;
		case VT_LONGLONG:
			memcpy(ptr, &((*it).second.m_val.llVal), sizeof(int64_t));
			ptr += sizeof(int64_t);
			break;
		case VT_BYTE:
			memcpy(ptr, &((*it).second.m_val.bVal), sizeof(byte));
			ptr += sizeof(byte);
			break;
		case VT_FLOAT:
			memcpy(ptr, &((*it).second.m_val.fltVal), sizeof(float));
			ptr += sizeof(float);
			break;
		case VT_DOUBLE:
			memcpy(ptr, &((*it).second.m_val.dblVal), sizeof(double));
			ptr += sizeof(double);
			break;
		case VT_CHAR:
			memcpy(ptr, &((*it).second.m_val.cVal), sizeof(char));
			ptr += sizeof(char);
			break;
		case VT_USHORT:
			memcpy(ptr, &((*it).second.m_val.uiVal), sizeof(uint16_t));
			ptr += sizeof(uint16_t);
			break;
		case VT_ULONG:
			memcpy(ptr, &((*it).second.m_val.ulVal), sizeof(uint32_t));
			ptr += sizeof(uint32_t);
			break;
		case VT_ULONGLONG:
			memcpy(ptr, &((*it).second.m_val.ullVal), sizeof(uint64_t));
			ptr += sizeof(uint64_t);
			break;
		case VT_BOOL:
			byte bl;
			bl = (*it).second.m_val.blVal;
			memcpy(ptr, &bl, sizeof(byte));
			ptr += sizeof(byte);
			break;
		default:
			throw NotSupportedException(
				"Tools::PropertySet::getData: Cannot serialize a variant of this type."
			);
		}
	}

	assert(ptr == (*data) + length);
}

Tools::Variant Tools::PropertySet::getProperty(std::string property)
{
	std::map<std::string, Variant>::iterator it = m_propertySet.find(property);
	if (it != m_propertySet.end()) return (*it).second;
	else return Variant();
}

void Tools::PropertySet::setProperty(std::string property, Variant& v)
{
	m_propertySet.insert(std::pair<std::string, Variant>(property, v));
}

void Tools::PropertySet::removeProperty(std::string property)
{
	std::map<std::string, Variant>::iterator it = m_propertySet.find(property);
	if (it != m_propertySet.end()) m_propertySet.erase(it);
}

Tools::IObjectStream* Tools::externalSort(IObjectStream& source, size_t bufferSize)
{
	return new ExternalSort(source, bufferSize);

}

Tools::IObjectStream* Tools::externalSort(IObjectStream& source, IObjectComparator& comp, size_t bufferSize)
{
	return new ExternalSort(source, comp, bufferSize);
}

Tools::Interval::Interval() : m_type(IT_RIGHTOPEN), m_low(0.0), m_high(0.0)
{
}

Tools::Interval::Interval(IntervalType t, double l, double h) : m_type(t), m_low(l), m_high(h)
{
	assert(l < h);
}

Tools::Interval::Interval(double l, double h) : m_type(IT_RIGHTOPEN), m_low(l), m_high(h)
{
	assert(l < h);
}

Tools::Interval::Interval(const Interval& iv)
{
	m_low = iv.m_low;
	m_high = iv.m_high;
	m_type = iv.m_type;
}

Tools::IInterval& Tools::Interval::operator=(const IInterval& iv)
{
	if (this != &iv)
	{
		m_low = iv.getLowerBound();
		m_high = iv.getUpperBound();
		m_type = iv.getIntervalType();
	}

	return *this;
}

Tools::Interval& Tools::Interval::operator=(const Interval& iv)
{
	*this = *(static_cast<const IInterval*>(&iv));
	return *this;
}

bool Tools::Interval::operator==(const Interval& iv) const
{
	if (
		m_type == iv.m_type &&
		m_low >= iv.m_low - std::numeric_limits<double>::epsilon() &&
		m_low <= iv.m_low + std::numeric_limits<double>::epsilon() &&
		m_high >= iv.m_high - std::numeric_limits<double>::epsilon() &&
		m_high <= iv.m_high + std::numeric_limits<double>::epsilon())
		return true;

	return false;
}

bool Tools::Interval::operator!=(const Interval& iv) const
{
	return ! (*this == iv);
}

double Tools::Interval::getLowerBound() const
{
	return m_low;
}

double Tools::Interval::getUpperBound() const
{
	return m_high;
}

void Tools::Interval::setBounds(double l, double h)
{
	assert(l <= h);

	m_low = l;
	m_high = h;
}

bool Tools::Interval::intersectsInterval(const IInterval& i) const
{
	return intersectsInterval(i.getIntervalType(), i.getLowerBound(), i.getUpperBound());
}

bool Tools::Interval::intersectsInterval(IntervalType type, const double low, const double high) const
{
	if (m_high < m_low)
		throw IllegalStateException(
			"Tools::Interval::intersectsInterval: high boundary is smaller than low boundary."
		);

	if (m_low > high || m_high < low) return false;
	if ((m_low > low && m_low < high) || (m_high > low && m_high < high)) return true;

	switch (m_type)
	{
	case IT_CLOSED:
		if (m_low == high)
		{
			if (type == IT_CLOSED || type == IT_LEFTOPEN) return true;
			else return false;
		}
		else if (m_high == low)
		{
			if (type == IT_CLOSED || type == IT_RIGHTOPEN) return true;
			else return false;
		}
		break;
	case IT_OPEN:
		if (m_low == high || m_high == low) return false;
		break;
	case IT_RIGHTOPEN:
		if (m_low == high)
		{
			if (type == IT_CLOSED || type == IT_LEFTOPEN) return true;
			else return false;
		}
		else if (m_high == low)
		{
			return false;
		}
		break;
	case IT_LEFTOPEN:
		if (m_low == high)
		{
			return false;
		}
		else if (m_high == low)
		{
			if (type == IT_CLOSED || type == IT_RIGHTOPEN) return true;
			else return false;
		}
		break;
	}

	return true;
}

bool Tools::Interval::containsInterval(const IInterval& i) const
{
	if (m_high < m_low)
		throw IllegalStateException(
			"Tools::Interval::containsInterval: high boundary is smaller than low boundary."
		);

	double low = i.getLowerBound();
	double high = i.getUpperBound();
	IntervalType type = i.getIntervalType();

	if (m_low < low && m_high > high) return true;
	if (m_low > low || m_high < high) return false;

	switch (m_type)
	{
	case IT_CLOSED:
		break;
	case IT_OPEN:
		if ((m_low == low && m_high == high && type != IT_OPEN) ||
			(m_low == low && (type == IT_CLOSED || type == IT_RIGHTOPEN)) ||
			(m_high == high && ( type == IT_CLOSED || type == IT_LEFTOPEN)))
			return false;
		break;
	case IT_RIGHTOPEN:
		if (m_high == high && (type == IT_CLOSED || type == IT_LEFTOPEN))
			return false;
		break;
	case IT_LEFTOPEN:
		if (m_low == low && (type == IT_CLOSED || type == IT_RIGHTOPEN))
			return false;
		break;
	}

	return true;
}

Tools::IntervalType Tools::Interval::getIntervalType() const
{
	return m_type;
}

Tools::Random::Random()
 : m_type(Tools::RGT_MERSENNE), m_seed(time(0))
{
	initMersenne();
}

Tools::Random::Random(uint32_t seed)
 : m_type(Tools::RGT_MERSENNE), m_seed(seed)
{
	initMersenne();
}

Tools::Random::Random(uint32_t seed, RandomGeneratorType t)
 : m_type(t), m_buffer(0), m_seed(seed)
{
	switch (m_type)
	{
	default:
		throw Tools::NotSupportedException(
			"Tools::Random::Random: This PRG type is not supported yet."
		);
	case RGT_MERSENNE:
		initMersenne();
		break;
	case RGT_DRAND48:
		initDrand(0x330E);
		break;
	}
}

Tools::Random::Random(uint32_t seed, uint16_t xsubi0)
 : m_type(RGT_DRAND48), m_buffer(0), m_seed(seed)
{
	initDrand(xsubi0);
}

Tools::Random::~Random()
{
	switch (m_type)
	{
	case RGT_MERSENNE:
		delete[] reinterpret_cast<uint32_t*>(m_buffer);
		break;
	case RGT_DRAND48:
		delete[] reinterpret_cast<uint16_t*>(m_buffer);
		break;
	}
}

void Tools::Random::initMersenne()
{
	m_buffer = new uint32_t[Tools::Random::MERS_N + 1];

	uint32_t& mti = *(reinterpret_cast<uint32_t*>(m_buffer));
	uint32_t* mt = reinterpret_cast<uint32_t*>(m_buffer) + 1;

	mt[0] = m_seed;

	for (mti = 1; mti < MERS_N; mti++)
    {
		mt[mti] = (1812433253UL * (mt[mti - 1] ^ (mt[mti - 1] >> 30)) + mti);
	}

	assert(mti == *(reinterpret_cast<uint32_t*>(m_buffer)));

	m_architecture = Tools::System::getArchitecture();
}

void Tools::Random::initDrand(uint16_t xsubi0)
{
	uint16_t* pS = new uint16_t[3];
	pS[0] = static_cast<uint16_t>(xsubi0);
	uint32_t mask = 0xFFFF;
	pS[1] = static_cast<uint16_t>(m_seed & mask);
	mask = mask << 16;
	pS[2] = static_cast<uint16_t>((m_seed & mask) >> 16);
	m_buffer = pS;
	
#ifdef BUILD_OS_CYGWIN
	srand48(*(reinterpret_cast<uint32_t*>(pS)));
		// BUG: There is a bug in cygwin gcc 3.4.4. srand48 needs to be called
		// even if we are using the functions that take the seed as a parameter.
		// This does not affect random number generation, which still happens
		// using the seed provided as a parameter and not the one provided to srand48 :-S
#endif
}

int32_t Tools::Random::nextUniformLong()
{
	if (m_type == RGT_DRAND48)
	{
		uint16_t* xsubi = reinterpret_cast<uint16_t*>(m_buffer);
		return jrand48(xsubi);
	}
	else if (m_type == RGT_MERSENNE)
	{
		// generate 32 random bits
		uint32_t y;
		uint32_t& mti = *(reinterpret_cast<uint32_t*>(m_buffer));
		uint32_t* mt = reinterpret_cast<uint32_t*>(m_buffer) + 1;

		if (mti >= MERS_N)
		{
			// generate MERS_N words at one time
			const uint32_t LOWER_MASK = (1LU << MERS_R) - 1;
				// lower MERS_R bits
			const uint32_t UPPER_MASK = -1L << MERS_R;
				// upper (32 - MERS_R) bits
			static const uint32_t mag01[2] = {0, MERS_A};
    
			uint32_t kk;
			for (kk = 0; kk < MERS_N - MERS_M; kk++)
			{
				y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
				mt[kk] = mt[kk + MERS_M] ^ (y >> 1) ^ mag01[y & 1];
			}

			for (; kk < MERS_N - 1; kk++)
			{
				y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
				mt[kk] = mt[kk + (MERS_M - MERS_N)] ^ (y >> 1) ^ mag01[y & 1];
			}

			y = (mt[MERS_N - 1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
			mt[MERS_N - 1] = mt[MERS_M - 1] ^ (y >> 1) ^ mag01[y & 1];
			mti = 0;
		}

		y = mt[mti++];

		// Tempering (May be omitted):
		y ^=  y >> MERS_U;
		y ^= (y << MERS_S) & MERS_B;
		y ^= (y << MERS_T) & MERS_C;
		y ^=  y >> MERS_L;

		return y;
	}
	else
	{
		throw Tools::IllegalStateException(
			"Tools::Random::nextUniformLong: Should never reach here."
		);
	}
}

uint32_t Tools::Random::nextUniformUnsignedLong()
{
	return static_cast<uint32_t>(nextUniformLong());
}

int32_t Tools::Random::nextUniformLong(int32_t low, int32_t high)
{
	return low + static_cast<int32_t>((high - low) * nextUniformDouble());
}

uint32_t Tools::Random::nextUniformUnsignedLong(uint32_t low, uint32_t high)
{
	return low + static_cast<uint32_t>((high - low) * nextUniformDouble());
}

int64_t Tools::Random::nextUniformLongLong()
{
	return static_cast<int64_t>(nextUniformUnsignedLongLong());
}

uint64_t Tools::Random::nextUniformUnsignedLongLong()
{
	uint64_t lh = static_cast<uint64_t>(nextUniformLong());
	uint64_t ll = static_cast<uint64_t>(nextUniformLong());
	uint64_t ret = (lh << 32) | ll;
	return ret;
}

int64_t Tools::Random::nextUniformLongLong(int64_t low, int64_t high)
{
	return low + static_cast<int64_t>((high - low) * nextUniformDouble());
}

uint64_t Tools::Random::nextUniformUnsignedLongLong(uint64_t low, uint64_t high)
{
	return low + static_cast<uint64_t>((high - low) * nextUniformDouble());
}

int16_t Tools::Random::nextUniformShort()
{
	return static_cast<int16_t>(nextUniformUnsignedShort());
}

uint16_t Tools::Random::nextUniformUnsignedShort()
{
	return nextUniformUnsignedLong() >> 16;
		// retain the high order bits.
}

double Tools::Random::nextUniformDouble()
{
	if (m_type == RGT_DRAND48)
	{
		uint16_t* xsubi = reinterpret_cast<uint16_t*>(m_buffer);
		return erand48(xsubi);
	}
	else if (m_type == RGT_MERSENNE)
	{
		union {double f; uint32_t i[2];} convert;
		uint32_t r = nextUniformUnsignedLong();

		switch (m_architecture)
		{
		case ARCH_LITTLEENDIAN:
			convert.i[0] =  r << 20;
			convert.i[1] = (r >> 12) | 0x3FF00000;
			return convert.f - 1.0;
		case ARCH_BIGENDIAN:
			convert.i[1] =  r << 20;
			convert.i[0] = (r >> 12) | 0x3FF00000;
			return convert.f - 1.0;
		case ARCH_NONIEEE:
		default:
			;
		}

		// This somewhat slower method works for all architectures, including 
		// non-IEEE floating point representation:
		return static_cast<double>(r) * (1.0 / (static_cast<double>(static_cast<uint32_t>(-1L) + 1.0)));
	}
	else
	{
		throw Tools::IllegalStateException(
			"Tools::Random::nextUniformDouble: Should never reach here."
		);
	}
}

double Tools::Random::nextUniformDouble(double low, double high)
{
	return (high - low) * nextUniformDouble() + low;
}

// mean 0.0, standard deviation 1.0
double Tools::Random::nextNormalDouble()
{
	static bool haveNextNextGaussian = false;
	static double nextNextGaussian;

	if (haveNextNextGaussian)
	{
		haveNextNextGaussian = false;
		return nextNextGaussian;
	}
	else
	{
		double v1, v2, s;

		do
		{
			v1 = 2 * nextUniformDouble() - 1;   // between -1.0 and 1.0
			v2 = 2 * nextUniformDouble() - 1;   // between -1.0 and 1.0
			s = v1 * v1 + v2 * v2;
		}
		while (s >= 1 || s == 0);

		double multiplier = std::sqrt(-2.0 * std::log(s)/s);
		nextNextGaussian = v2 * multiplier;
		haveNextNextGaussian = true;
		return v1 * multiplier;
	}
}

double Tools::Random::nextNormalDouble(double m, double std)
{
 	return m + (std * nextNormalDouble());
}

int32_t Tools::Random::nextSkewedLong(int32_t low, int32_t high, Level p)
{
	return low + static_cast<int32_t>(static_cast<double>(high - low) * nextSkewedDouble(p));
}

double Tools::Random::nextSkewedDouble(double low, double high, Level p)
{
	return (high - low) * nextSkewedDouble(p) + low;
}

// WARNING: The inversion method is very slow.
// For discrete distributions use PRGZipf.
double Tools::Random::nextSkewedDouble(Level p)
{
	double HsubV, l;
	uint64_t V = 1000000;

	//HsubV = 0.0;
	//for(uint64_t i = 1; i <= V; i++) HsubV += 1.0 / std::pow(static_cast<double>(i), l);

	switch (p)
	{
	case LVL_VERYLOW:
		HsubV = 1998.54;
		l = 0.5;
		break;
	case LVL_LOW:
		HsubV = 14.3927;
		l = 1.0;
		break;
	case LVL_MEDIUM:
		HsubV = 2.61038;
		l = 1.5;
		break;
	case LVL_HIGH:
		HsubV = 1.64493;
		l = 2.0;
		break;
	case LVL_VERYHIGH:
		HsubV = 1.34149;
		l = 2.5;
		break;
	default:
		throw Tools::IllegalArgumentException(
			"Tools::Random::nextSkewedDouble: Unknown skedeness level."
		);
	}

	double r = nextUniformDouble() * HsubV;
	double sum = 1.0;
	uint64_t i = 1;
	while(sum < r)
	{
		i++;
		sum += 1.0 / std::pow(static_cast<double>(i), l);
	}

	// i follows Zipf distribution and lies between 1 and V
	return (static_cast<double>(i) - 1.0) / (static_cast<double>(V) - 1.0);
}

bool Tools::Random::flipCoin()
{
	if (m_type == RGT_DRAND48)
	{
		if (nextUniformDouble() < 0.5) return true;
		return false;
	}
	else if (m_type == RGT_MERSENNE)
	{
		// the probability of a long being even or odd is 50%
		if ((nextUniformLong() & 1) == 1) return true;
		return false;
	}
	else
	{
		throw Tools::NotSupportedException(
			"Tools::Random::flipCoin: This PRG type is not supported yet."
		);
	}
}

bool Tools::Random::bernulliTrial(double p)
{
	assert(p >= 0.0 && p <= 1.0);

	if (nextUniformDouble() < p) return true;
	return false;	
}

size_t Tools::Random::getSize() const
{
	// the seed is enough.
	return sizeof(uint32_t);
}

uint32_t Tools::Random::getSeed() const
{
	return m_seed;
}

Tools::PRGZipf::PRGZipf(int32_t min, int32_t max, double s, Tools::Random* pRandom)
 : m_min(min), m_max(max), m_s(s), m_pRandom(pRandom)
{
	if (max - min < 2)
		throw Tools::IllegalArgumentException(
			"PRGZipf: max - min must be larger than two."
		);

	initLookupTable();
}

Tools::PRGZipf::~PRGZipf()
{
	delete[] m_pLookupTable;
}

void Tools::PRGZipf::initLookupTable()
{
	uint32_t N = m_max - m_min;
	double Hns = 0.0;

	m_pLookupTable = new double[N];

	for (uint32_t k = 1; k < N; k++)
		Hns += 1.0 / std::pow(static_cast<double>(k), m_s);

	double sum = 0.0;
	m_pLookupTable[0] = sum;
	for (uint32_t k = 1; k < N; k++)
	{
		sum += 1.0 / std::pow(static_cast<double>(k), m_s);
		m_pLookupTable[k] = sum / Hns;
	}
}

int32_t Tools::PRGZipf::nextLong()
{
	uint32_t N = m_max - m_min;
	double dart = m_pRandom->nextUniformDouble();
	double* i = std::lower_bound(m_pLookupTable, m_pLookupTable + N, dart);

	assert(i >= m_pLookupTable && i <= m_pLookupTable + N - 1);

	int32_t ret = static_cast<int32_t>(i - m_pLookupTable) + m_min;

	assert(ret >= m_min && ret < m_max);
	return ret;
}

Tools::PRGFrechet::PRGFrechet(double a, double b, Tools::Random* pRandom)
 : m_alpha(a), m_beta(b), m_pRandom(pRandom)
{
	if (m_alpha < 0.0 || m_beta < 0.0)
		throw Tools::IllegalArgumentException(
			"Tools::PRGFrechet::PRGFrechet: Parameters alhpa, beta need to be positive."
		);
}

Tools::PRGFrechet::~PRGFrechet()
{
}

double Tools::PRGFrechet::nextDouble()
{
	double u = m_pRandom->nextUniformDouble();
	return m_beta * std::pow(- std::log(u), - 1.0 / m_alpha);
}

Tools::Hash::~Hash()
{
}

Tools::UniversalHash::UniversalHash(uint16_t k)
 : m_k(k)
{
	if (m_k < 2)
		throw Tools::IllegalArgumentException(
			"Tools::UniversalHash::UniversalHash: k should be larger than 1."
		);

	Tools::Random r;
	m_a = new uint64_t[m_k];
	for (uint16_t i = 0; i < m_k; i++)
		m_a[i] = r.nextUniformUnsignedLongLong(1ull, m_P);
}

Tools::UniversalHash::UniversalHash(Tools::Random& r, uint16_t k)
 : m_k(k)
{
	if (m_k < 2)
		throw Tools::IllegalArgumentException(
			"Tools::UniversalHash::UniversalHash: k should be larger than 1."
		);

	m_a = new uint64_t[m_k];

	for (uint16_t i = 0; i < m_k; i++)
		m_a[i] = r.nextUniformUnsignedLongLong(1ull, m_P);
}

Tools::UniversalHash::UniversalHash(uint64_t a, uint64_t b)
 : m_k(2)
{
	m_a = new uint64_t[m_k];
	m_a[0] = a;
	m_a[1] = b;
}

Tools::UniversalHash::UniversalHash(const UniversalHash& h)
{
	m_k = h.m_k;
	m_a = new uint64_t[m_k];
	memcpy(m_a, h.m_a, m_k * sizeof(uint64_t));
}

Tools::UniversalHash::UniversalHash(const byte* data)
{
	const byte* p = data;
	memcpy(&m_k, p, sizeof(uint16_t));
	p += sizeof(uint16_t);
	m_a = new uint64_t[m_k];
	memcpy(m_a, p, m_k * sizeof(uint64_t));
}

Tools::UniversalHash::~UniversalHash()
{
	delete[] m_a;
}

Tools::UniversalHash& Tools::UniversalHash::operator=(const UniversalHash& in)
{
	if (this != &in)
	{
		if (m_k != in.m_k)
		{
			m_k = in.m_k;
			delete[] m_a;
			m_a = new uint64_t[m_k];
		}

		memcpy(m_a, in.m_a, m_k * sizeof(uint64_t));
	}

	return *this;
}

bool Tools::UniversalHash::operator==(const UniversalHash& in) const
{
	for (uint16_t i = 0; i < m_k; i++)
		if (m_a[i] != in.m_a[i]) return false;

	return true;
}

Tools::UniversalHash::value_type Tools::UniversalHash::hash(
	UniversalHash::value_type x
) const
{
	uint64_t r = m_a[0];
	uint64_t xd = 1;

	for (uint16_t i = 1; i < m_k; i++)
	{
		xd = (xd * x) % m_P;
		r += (m_a[i] * xd) % m_P;
			// FIXME: multiplications here might overflow.
	}

	r = (r % m_P) & 0xFFFFFFFF;
		// this is the same as x % 2^32. The modulo operation with powers
		// of 2 (2^n) is a simple bitwise AND with 2^n - 1.

	return static_cast<value_type>(r);
}

void Tools::UniversalHash::hash(
	const std::string& s,
	byte** out, size_t& lout
)
{
	hash(reinterpret_cast<const byte*>(s.c_str()), s.size(), out, lout);
}

void Tools::UniversalHash::hash(
	const byte* in, size_t lin,
	byte** out, size_t& lout
)
{
	if (lin != sizeof(value_type))
		throw Tools::IllegalArgumentException(
			"Tools::UniversalHash::hash: UniversalHash can hash only "
			"values sizeof(value_type) bytes long."
		);

	value_type h = hash(*(reinterpret_cast<const value_type*>(in)));

	lout = sizeof(value_type);
	*out = new byte[lout];
	memcpy(*out, &h, lout);
}

size_t Tools::UniversalHash::getSize() const
{
	return m_k * sizeof(uint64_t) + sizeof(uint16_t);
}

void Tools::UniversalHash::getData(byte** buffer, size_t& length) const
{
	length = getSize();
	*buffer = new byte[length];
	byte* p = *buffer;

	memcpy(p, &m_k, sizeof(uint16_t));
	p += sizeof(uint16_t);
	memcpy(p, m_a, m_k * sizeof(uint64_t));
	p += m_k * sizeof(uint64_t);

	assert(p == (*buffer) + length);
}

Tools::SHA1Hash::~SHA1Hash()
{
}

std::string Tools::SHA1Hash::hash(const std::string& s)
{
	byte* out;
	size_t lout;
	hash(reinterpret_cast<const byte*>(s.c_str()), s.size(), &out, lout);
	std::string ret(reinterpret_cast<char*>(out), lout);
	delete[] out;
	return ret;
}

void Tools::SHA1Hash::hash(
	const std::string& s,
	byte** out,
	size_t& lout
)
{
	hash(reinterpret_cast<const byte*>(s.c_str()), s.size(), out, lout);
}

void Tools::SHA1Hash::hash(
	std::istream& s,
	byte** out,
	size_t& lout
)
{
	int err;
	SHA1Context sha;

	err = SHA1Reset(&sha);
	if (err)
		throw Tools::IllegalStateException(
			"Tools::SHA1Hash::hash: Error reseting SHA1 context."
		);

	byte* buffer = new byte[100000];

	while (s)
	{
		// readsome appears to be buggy.
		//std::streamsize l =
		//	s.readsome(reinterpret_cast<char*>(buffer), 100000);

		s.read(reinterpret_cast<char*>(buffer), 100000);

		err = SHA1Input(&sha, reinterpret_cast<uint8_t*>(buffer), s.gcount());
		if (err)
		{
			delete[] buffer;

			throw Tools::IllegalStateException(
				"Tools::SHA1Hash::hash: Error computing SHA1 context."
			);
		}
	}

	delete[] buffer;

	lout = Tools::SHA1Hash::HashLength;
	*out = new byte[lout];

	err = SHA1Result(&sha, reinterpret_cast<uint8_t*>(*out));
	if (err)
	{
		lout = 0;
		delete[] *out;
		throw Tools::IllegalStateException(
			"Tools::SHA1Hash::hash: Error computing result."
		);
	}
}

void Tools::SHA1Hash::hash(
	const byte* in,
	size_t lin,
	byte** out,
	size_t& lout
)
{
	int err;
	SHA1Context sha;

	err = SHA1Reset(&sha);
	if (err)
		throw Tools::IllegalStateException(
			"Tools::SHA1Hash::hash: Error reseting SHA1 context."
		);

	err = SHA1Input(&sha, reinterpret_cast<const uint8_t*>(in), lin);
	if (err)
		throw Tools::IllegalStateException(
			"Tools::SHA1Hash::hash: Error computing SHA1 context."
		);

	lout = Tools::SHA1Hash::HashLength;
	*out = new byte[lout];

	err = SHA1Result(&sha, reinterpret_cast<uint8_t*>(*out));
	if (err)
	{
		lout = 0;
		delete[] *out;
		throw Tools::IllegalStateException(
			"Tools::SHA1Hash::hash: Error computing result."
		);
	}
}

void Tools::compressRLE(
	size_t blockSize,
	byte* in, size_t lin,
	byte** out, size_t& lout)
{
	if (lin == 0) { *out = 0; lout = 0; return; }
	if (lin % blockSize != 0)
		throw Tools::IllegalArgumentException(
			"Tools::compressRLE: Buffer length "
			"should be divisible by block size."
		);

	byte *data = 0, *pdata = 0, *pin;
	byte cv[blockSize], pv[blockSize];
	byte rl;

	pin = in;
	memcpy(cv, pin, blockSize);
	pv[0] = ~cv[0]; // force next character to be different.
	assert(pv[0] != cv[0]);
	data = new byte[2 * lin];
	pdata = data;
	rl = 0;

	while (pin < in + lin)
	{
		memcpy(cv, pin, blockSize);
		pin += blockSize;
		memcpy(pdata, cv, blockSize);
		pdata += blockSize;

		assert(pin <= in + lin);

		if (memcmp(cv, pv, blockSize) == 0)
		{
			while (pin < in + lin)
			{
				memcpy(cv, pin, blockSize);
				pin += blockSize;

				assert(pin <= in + lin);

				if (memcmp(cv, pv, blockSize) == 0)
				{
					rl++;

					if (rl == std::numeric_limits<byte>::max())
					{
						memcpy(pdata, &rl, sizeof(byte));
						pdata += sizeof(byte);
						pv[0] = ~cv[0];
						assert(pv[0] != cv[0]);
						rl = 0;
						break;
					}
				}
				else
				{
					memcpy(pdata, &rl, sizeof(byte));
					pdata += sizeof(byte);
					memcpy(pdata, cv, blockSize);
					pdata += blockSize;
					memcpy(pv, cv, blockSize);
					rl = 0;
					break;
				}
			}
		}
		else memcpy(pv, cv, blockSize);

		if (rl > 0)
		{
			// end of buffer was reached.
			memcpy(pdata, &rl, sizeof(byte));
			pdata += sizeof(byte);
		}
	}

	lout = pdata - data;
	assert(lout <= 2 * lin);

	try
	{
		*out = new byte[lout];
	}
	catch(...)
	{
		delete[] data;
		throw;
	}

	memcpy(*out, data, lout);
	delete[] data;
}

void Tools::uncompressRLE(
	size_t blockSize,
	byte* in, size_t lin,
	byte** out, size_t& lout)
{
	if (lin == 0) { *out = 0; lout = 0;	return; }

	byte *data = 0, *pdata = 0, *pin;
	byte cv[blockSize], pv[blockSize];
	byte rl;
	size_t bufferLength = 2 * lin;

	pin = in;
	memcpy(cv, pin, blockSize);
	pv[0] = ~cv[0]; // force next character to be different.
	assert(pv[0] != cv[0]);
	data = new byte[bufferLength];
	pdata = data;

	while (pin < in + lin)
	{
		memcpy(cv, pin, blockSize);
		pin += blockSize;

		assert(pin <= in + lin);

		if (
			bufferLength - static_cast<size_t>(pdata - data) <=
			blockSize
		)
		{
			byte* tmp;
			try
			{
				tmp = new byte[2 * bufferLength];
			}
			catch(...)
			{
				delete[] data;
				throw;
			}

			memcpy(tmp, data, bufferLength);
			pdata = tmp + (pdata - data);
			byte* tmp2 = data;
			data = tmp;
			delete[] tmp2;
			bufferLength *= 2;
		}

		memcpy(pdata, cv, blockSize);
		pdata += blockSize;

		if (memcmp(cv, pv, blockSize) == 0 && pin < in + lin)
		{
			memcpy(&rl, pin, sizeof(byte));
			pin += sizeof(byte);
			assert(pin <= in + lin);
			if (
				bufferLength - static_cast<size_t>(pdata - data) <=
				rl * blockSize
			)
			{
				size_t l = std::max(bufferLength, rl * blockSize);

				byte* tmp;
				try
				{
					tmp = new byte[2 * l];
				}
				catch(...)
				{
					delete[] data;
					throw;
				}

				memcpy(tmp, data, bufferLength);
				pdata = tmp + (pdata - data);
				byte* tmp2 = data;
				data = tmp;
				delete[] tmp2;
				bufferLength = 2 * l;
			}

			while (rl > 0)
			{
				memcpy(pdata, cv, blockSize);
				pdata += blockSize;
				rl--;
			}
			memcpy(cv, pin, blockSize);
			pv[0] = ~cv[0];
			assert(pv[0] != cv[0]);
		}
		else memcpy(pv, cv, blockSize);
	}

	lout = pdata - data;

	try
	{
		*out = new byte[lout];
	}
	catch(...)
	{
		delete[] data;
		throw;
	}

	memcpy(*out, data, lout);
	delete[] data;
}

#if HAVE_GETTIMEOFDAY
Tools::ResourceUsage::ResourceUsage()
{
	reset();
}

void Tools::ResourceUsage::start()
{
	struct timezone dummy;

	if (getrusage(RUSAGE_SELF, &m_tmpRU) != 0)
		throw IllegalStateException(
			"Tools::ResourceUsage::start: getrusage failed."
		);

	if (gettimeofday(&m_tmpTV, &dummy) != 0)
		throw IllegalStateException(
			"Tools::ResourceUsage::start: gettimeofday failed."
		);

	// maximum resident set size
	m_peakMemory = std::max(m_peakMemory, static_cast<size_t>(m_tmpRU.ru_maxrss));

	// total memory
	m_totalMemory = std::max(
		m_totalMemory,
		static_cast<size_t>(
			m_tmpRU.ru_ixrss +
			m_tmpRU.ru_idrss +
			m_tmpRU.ru_isrss +
			m_tmpRU.ru_maxrss
		)
	);
}

void Tools::ResourceUsage::stop()
{
	struct timezone dummy;
  	struct timeval dif;
	struct rusage ru;
  	struct timeval tv;

	if (getrusage(RUSAGE_SELF, &ru) != 0)
		throw IllegalStateException(
			"Tools::ResourceUsage::stop: getrusage failed."
		);

	if (gettimeofday(&tv, &dummy) != 0)
		throw IllegalStateException(
			"Tools::ResourceUsage::stop: gettimeofday failed."
		);

	// total_time
	subtractTimeval(dif, tv, m_tmpTV);
	addTimeval(m_totalTime, dif);

	// system_time
	subtractTimeval(dif, ru.ru_stime, m_tmpRU.ru_stime);
	addTimeval(m_systemTime, dif);

	// user_time
	subtractTimeval(dif, ru.ru_utime, m_tmpRU.ru_utime);
  	addTimeval(m_userTime, dif);

	// readIO, writeIOs
	m_readIO += ru.ru_inblock - m_tmpRU.ru_inblock;
	m_writeIO += ru.ru_oublock - m_tmpRU.ru_oublock;

	// maximum resident set size
	m_peakMemory = std::max(m_peakMemory, static_cast<size_t>(ru.ru_maxrss));

	// total memory
	m_totalMemory = std::max(
		m_totalMemory,
		static_cast<size_t>(
			ru.ru_ixrss +
			ru.ru_idrss +
			ru.ru_isrss +
			ru.ru_maxrss
		)
	);

	// page faults
	m_pageFaults += ru.ru_majflt - m_tmpRU.ru_majflt;
}

void Tools::ResourceUsage::reset()
{
	m_pageFaults = 0;
	m_readIO = 0;
	m_writeIO = 0;
	m_peakMemory = 0;
	m_totalMemory = 0;
	m_totalTime.tv_sec = 0;
	m_totalTime.tv_usec = 0;
	m_userTime.tv_sec = 0;
	m_userTime.tv_usec = 0;
	m_systemTime.tv_sec = 0;
	m_systemTime.tv_usec = 0;
}

double Tools::ResourceUsage::combineTime(const struct timeval& t)
{
	return
		static_cast<double>(t.tv_sec) +
		static_cast<double>(t.tv_usec) / 1000000.0;
}

void Tools::ResourceUsage::addTimeval(struct timeval& result, const struct timeval& a)
{
	result.tv_sec += a.tv_sec;
	result.tv_usec += a.tv_usec;

	if (result.tv_usec > 1000000)
	{
		int32_t div = result.tv_usec / 1000000;
		result.tv_sec += div;
		result.tv_usec -= div * 1000000;
	}
}

void Tools::ResourceUsage::subtractTimeval(struct timeval& result, const struct timeval& a, const struct timeval& b)
{
	result.tv_sec = a.tv_sec - b.tv_sec;
	result.tv_usec = a.tv_usec - b.tv_usec;

	if (result.tv_usec < 0)
	{
		result.tv_sec -= 1;
    	result.tv_usec += 1000000;
    }
}

double Tools::ResourceUsage::getTotalTime()
{
	return combineTime(m_totalTime);
}

double Tools::ResourceUsage::getUserTime()
{
	return combineTime(m_userTime);
}

double Tools::ResourceUsage::getSystemTime()
{
	return combineTime(m_systemTime);
}

size_t Tools::ResourceUsage::getPageFaults()
{
	return m_pageFaults;
}

size_t Tools::ResourceUsage::getReadIO()
{
	return m_readIO;
}

size_t Tools::ResourceUsage::getWriteIO()
{
	return m_writeIO;
}

size_t Tools::ResourceUsage::getPeakResidentMemoryUsage()
{
	return m_peakMemory;
}

size_t Tools::ResourceUsage::getTotalMemoryUsage()
{
	return m_totalMemory;
}
#endif

#if BUILD_CPU_I686
Tools::CycleCounter::CycleCounter() : m_totalCycles(0), m_bRunning(false)
{
}

double Tools::CycleCounter::getCPUMHz()
{
	uint64_t v1 = rdtsc();
	sleep(10);
	uint64_t v2 = rdtsc();
	return ((v2 - v1) / 1e7);
}

double Tools::CycleCounter::getCyclesPerSecond()
{
	uint64_t v1 = rdtsc();
	sleep(10);
	uint64_t v2 = rdtsc();
	return ((v2 - v1) / 10);
}

inline uint64_t Tools::CycleCounter::rdtsc()
{
	uint64_t ret;
	__asm__ __volatile__("rdtsc": "=A" (ret):);
	return ret;
}

void Tools::CycleCounter::start()
{
	if (! m_bRunning)
	{
		m_tmpCycles = rdtsc();
		m_bRunning = true;
	}
}

void Tools::CycleCounter::stop()
{
	if (m_bRunning)
	{
		uint64_t t = rdtsc();
		m_totalCycles += t - m_tmpCycles;
		m_bRunning = false;
	}
}

void Tools::CycleCounter::reset()
{
	m_totalCycles = 0;
	m_bRunning = false;
}

double Tools::CycleCounter::getTotalCycles()
{
	return m_totalCycles;
}
#endif

#if HAVE_PTHREAD_H
Tools::SharedLock::SharedLock(pthread_rwlock_t* pLock)
	: m_pLock(pLock)
{
	pthread_rwlock_rdlock(m_pLock);
}

Tools::SharedLock::~SharedLock()
{
	pthread_rwlock_unlock(m_pLock);
}

Tools::ExclusiveLock::ExclusiveLock(pthread_rwlock_t* pLock)
	: m_pLock(pLock)
{
	pthread_rwlock_wrlock(m_pLock);
}

Tools::ExclusiveLock::~ExclusiveLock()
{
	pthread_rwlock_unlock(m_pLock);
}
#endif

Tools::StringTokenizer::StringTokenizer(
	const std::string& str,
	const std::string& delimiters,
	bool bTrim
) : m_index(0)
{
	std::string::size_type lastPos = 0, pos;

	if (bTrim)
	{
		// Skip delimiters at beginning.
		lastPos = str.find_first_not_of(delimiters, 0);
	}

	// Find first delimiter.
	pos = str.find_first_of(delimiters, lastPos);

	while (std::string::npos != pos || std::string::npos != lastPos)
	{
		// Found a token, add it to the vector.
		m_token.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delimiters. 
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next delimiter.
		pos = str.find_first_of(delimiters, lastPos);
	}
}

std::string Tools::StringTokenizer::getNextToken()
{
	return m_token.at(m_index++);
}

bool Tools::StringTokenizer::hasMoreTokens()
{
	return (m_index < m_token.size());
}

void Tools::StringTokenizer::reset()
{
	m_index = 0;
}

std::string Tools::trimLeft(const std::string& source, const std::string& t)
{
	std::string str = source;
	return str.erase(0, source.find_first_not_of(t));
}

std::string Tools::trimRight(const std::string& source, const std::string& t)
{
	std::string str = source;
	return str.erase(str.find_last_not_of(t) + 1);
}

std::string Tools::trim(const std::string& source, const std::string& t)
{
	std::string str = source;
	return trimLeft(trimRight(str, t), t);
}

char Tools::toLower(char c)
{
	return std::tolower(c);
}

char Tools::toUpper(char c)
{
	return std::toupper(c);
}

std::string Tools::toUpperCase(const std::string& s)
{
	std::string t = s;
	transform(t.begin(), t.end(), t.begin(), Tools::toUpper);
	return t;
}

std::string Tools::toLowerCase(const std::string& s)
{
	std::string t = s;
	transform(t.begin(), t.end(), t.begin(), Tools::toLower);
	return t;
}

uint64_t Tools::choose(uint32_t a, uint32_t k)
{
	uint64_t cnm = 1, n = a, m = k;
	uint64_t i, f;

	if (m * 2 > n) m = n - m;
	for (i = 1 ; i <= m; n--, i++)
	{
		if ((f = n) % i == 0) f /= i;
		else cnm /= i;
		cnm *= f;
	}
	return cnm;
}

Tools::Architecture Tools::System::getArchitecture()
{
	union {double f; uint32_t i[2];} convert;
	convert.f = 1.0;

	// Note: Old versions of the Gnu g++ compiler may make an error here,
	// compile with the option  -fenum-int-equiv  to fix the problem
	if (convert.i[1] == 0x3FF00000) return ARCH_LITTLEENDIAN;
	else if (convert.i[0] == 0x3FF00000) return ARCH_BIGENDIAN;
	else return ARCH_NONIEEE;
}

std::ostream& Tools::operator<<(std::ostream& os, const Tools::PropertySet& p)
{
	std::map<std::string, Variant>::const_iterator it;

	for (it = p.m_propertySet.begin(); it != p.m_propertySet.end(); it++)
	{
		if (it != p.m_propertySet.begin()) os << ", ";

		switch ((*it).second.m_varType)
		{
		case VT_LONG:
			os << (*it).first << ": " << (*it).second.m_val.lVal;
			break;
		case VT_LONGLONG:
			os << (*it).first << ": " << (*it).second.m_val.llVal;
			break;
		case VT_BYTE:
			os << (*it).first << ": " << (*it).second.m_val.bVal;
			break;
		case VT_SHORT:
			os << (*it).first << ": " << (*it).second.m_val.iVal;
			break;
		case VT_FLOAT:
			os << (*it).first << ": " << (*it).second.m_val.fltVal;
			break;
		case VT_DOUBLE:
			os << (*it).first << ": " << (*it).second.m_val.dblVal;
			break;
		case VT_CHAR:
			os << (*it).first << ": " << (*it).second.m_val.cVal;
			break;
		case VT_USHORT:
			os << (*it).first << ": " << (*it).second.m_val.uiVal;
			break;
		case VT_ULONG:
			os << (*it).first << ": " << (*it).second.m_val.ulVal;
			break;
		case VT_ULONGLONG:
			os << (*it).first << ": " << (*it).second.m_val.ullVal;
			break;
		case VT_BOOL:
			os << (*it).first << ": " << (*it).second.m_val.blVal;
			break;
		case VT_PCHAR:
			os << (*it).first << ": " << (*it).second.m_val.pcVal;
			break;
		case VT_PVOID:
			os << (*it).first << ": ?";
			break;
		case VT_EMPTY:
			os << (*it).first << ": empty";
			break;
		default:
			os << (*it).first << ": unknown";
		}
	}

	return os;
}

std::ostream& Tools::operator<<(std::ostream& os, const Tools::Interval& iv)
{
	os << iv.m_type << " " << iv.m_low << " " << iv.m_high;
	return os;
}

std::ostream& Tools::operator<<(std::ostream& os, const Tools::UniversalHash& h)
{
	os << h.m_k;

	for (uint16_t i = 0; i < h.m_k; i++)
		os << " " << h.m_a[i];

	return os;
}

