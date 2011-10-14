// Spatial Index Library
//
// Copyright (C) 2003 Navel Ltd.
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

#pragma once

namespace SpatialIndex
{
	class SIDX_DLL MovingPoint : public TimePoint, public IEvolvingShape
	{
	public:
		MovingPoint();
		MovingPoint(const double* pCoords, const double* pVCoords, const Tools::IInterval& ti, uint32_t dimension);
		MovingPoint(const double* pCoords, const double* pVCoords, double tStart, double tEnd, uint32_t dimension);
		MovingPoint(const Point& p, const Point& vp, const Tools::IInterval& ti);
		MovingPoint(const Point& p, const Point& vp, double tStart, double tEnd);
		MovingPoint(const MovingPoint& p);
		virtual ~MovingPoint();

		virtual MovingPoint& operator=(const MovingPoint& p);
		virtual bool operator==(const MovingPoint& p) const;

		virtual double getCoord(uint32_t index, double t) const;
		virtual double getProjectedCoord(uint32_t index, double t) const;
		virtual double getVCoord(uint32_t index) const;
		virtual void getPointAtTime(double t, Point& out) const;

		//
		// IObject interface
		//
		virtual MovingPoint* clone();

		//
		// ISerializable interface
		//
		virtual uint32_t getByteArraySize();
		virtual void loadFromByteArray(const byte* data);
		virtual void storeToByteArray(byte** data, uint32_t& len);

		//
		// IEvolvingShape interface
		//
		virtual void getVMBR(Region& out) const;
		virtual void getMBRAtTime(double t, Region& out) const;

		virtual void makeInfinite(uint32_t dimension);
		virtual void makeDimension(uint32_t dimension);

	private:
		void initialize(
			const double* pCoords, const double* pVCoords,
			double tStart, double tEnd, uint32_t dimension);

	public:
		double* m_pVCoords;

		friend SIDX_DLL std::ostream& operator<<(std::ostream& os, const MovingPoint& pt);
	}; // MovingPoint

	SIDX_DLL std::ostream& operator<<(std::ostream& os, const MovingPoint& pt);
}

