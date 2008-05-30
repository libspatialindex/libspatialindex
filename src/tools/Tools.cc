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

#include "../../include/tools/Tools.h"
#include "../../include/tools/rand48.h"

#include "ExternalSort.h"

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
{
	m_pBuffer = 0;
	initDrand(time(0), 0xD31A);
}

Tools::Random::Random(uint32_t seed, uint16_t xsubi0)
{
	m_pBuffer = 0;
	initDrand(seed, xsubi0);
}

Tools::Random::~Random()
{
	delete[] m_pBuffer;
}

void Tools::Random::initDrand(uint32_t seed, uint16_t xsubi0)
{
	m_pBuffer = new uint16_t[3];
	m_pBuffer[0] = static_cast<uint16_t>(xsubi0);
	uint32_t mask = 0xFFFF;
	m_pBuffer[1] = static_cast<uint16_t>(seed & mask);
	mask = mask << 16;
	m_pBuffer[2] = static_cast<uint16_t>((seed & mask) >> 16);

#ifdef BUILD_OS_CYGWIN
	srand48(*(reinterpret_cast<uint32_t*>(m_pBuffer)));
		// BUG: There is a bug in cygwin gcc 3.4.4. srand48 needs to be called
		// even if we are using the functions that take the seed as a parameter.
		// This does not affect random number generation, which still happens
		// using the seed provided as a parameter and not the one provided to srand48 :-S
#endif
}

int32_t Tools::Random::nextUniformLong()
{
	return jrand48(m_pBuffer);
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
	uint16_t* xsubi = reinterpret_cast<uint16_t*>(m_pBuffer);
	return erand48(xsubi);
}

double Tools::Random::nextUniformDouble(double low, double high)
{
	return (high - low) * nextUniformDouble() + low;
}

bool Tools::Random::flipCoin()
{
	if (nextUniformDouble() < 0.5) return true;
	return false;
}

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
