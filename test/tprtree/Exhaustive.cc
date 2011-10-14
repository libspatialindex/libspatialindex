// Spatial Index Library
//
// Copyright (C) 2002 Navel Ltd.
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

#include <assert.h>
#include <iostream>
#include <fstream>
#include <map>
#include <queue>
#include <cmath>
#include <stdint.h>

using namespace std;

#define INSERT 1
#define DELETE 0
#define QUERY 2

#if defined _WIN32 || defined _WIN64 || defined WIN32 || defined WIN64
#if _MSC_VER <= 1500
  typedef __int8 int8_t;
  typedef __int16 int16_t;
  typedef __int32 int32_t;
  typedef __int64 int64_t;
  typedef unsigned __int8 uint8_t;
  typedef unsigned __int16 uint16_t;
  typedef unsigned __int32 uint32_t;
  typedef unsigned __int64 uint64_t;
#endif

// Nuke this annoying warning.  See http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
#pragma warning( disable: 4251 )

#else
  #include <stdint.h>
#endif

class Rectangle
{
public:
	Rectangle(double xlow, double xhigh, double ylow, double yhigh)
		: m_xlow(xlow), m_xhigh(xhigh), m_ylow(ylow), m_yhigh(yhigh) {}

	size_t computeCode(double x, double y)
	{
		size_t c = 0;
		if(y > m_yhigh) c |= TOP;
		else if(y < m_ylow) c |= BOTTOM;
		if( x > m_xhigh) c |= RIGHT;
		else if(x < m_xlow) c |= LEFT;
		return c;
	}

	bool intersectsPoint(double x, double y)
	{
		if (m_xlow <= x && x <= m_xhigh &&
			m_ylow <= y && y <= m_yhigh) return true;
		return false;
	}

	bool intersectsSegment(double x0, double x1, double y0, double y1)
	{
		size_t C0, C1, C;
		double x,y;

		C0 = computeCode(x0, y0);
		C1 = computeCode(x1, y1);

		for(;;)
		{
			/*
			 * trivial accept: both ends inside rectangle
			 */
			if((C0 | C1) == 0)
			{
				return true;
			}

			/*
			 * trivial reject: both ends on the external side
			 * of the rectanlge
			 */
			if((C0 & C1) != 0)
			{
				return false;
			}

			/*
			 * normal case: clip end outside rectangle
			 */
			C = C0 ? C0 : C1;
			if(C & TOP)
			{
				x = x0 + (x1 - x0) * (m_yhigh - y0) / (y1 - y0);
				y = m_yhigh;
			}
			else if(C & BOTTOM)
			{
				x = x0 + (x1 - x0) * (m_ylow  - y0) / (y1 - y0);
				y = m_ylow;
			}
			else if(C & RIGHT)
			{
				x = m_xhigh;
				y = y0 + (y1 - y0) * (m_xhigh - x0) / (x1 - x0);
			}
			else
			{
				x = m_xlow;
				y = y0 + (y1 - y0) * (m_xlow - x0) / (x1 - x0);
			}

			/*
			 * set new end point and iterate
			 */
			if(C == C0)
			{
				x0 = x; y0 = y;
				C0 = computeCode(x0, y0);
			}
			else
			{
				x1 =x; y1 = y;
				C1 = computeCode(x1, y1);
			}
		}
	}

	static const size_t TOP = 0x1;
	static const size_t BOTTOM = 0x2;
	static const size_t RIGHT = 0x4;
	static const size_t LEFT = 0x8;

	double m_xlow, m_xhigh, m_ylow, m_yhigh;
};

class MovingPoint
{
public:
	MovingPoint(double ax, double vx, double ay, double vy, double rt)
		: m_ax(ax), m_vx(vx), m_ay(ay), m_vy(vy), m_rt(rt) {}

	double getX(double t) { return m_ax + m_vx * (t - m_rt); }
	double getY(double t) { return m_ay + m_vy * (t - m_rt); }

	double m_ax, m_vx, m_ay, m_vy;
	double m_rt;
};

class TimeRectangle
{
public:
	TimeRectangle(double xlow, double xhigh, double ylow, double yhigh, double tlow, double thigh)
		: m_xlow(xlow), m_xhigh(xhigh), m_ylow(ylow), m_yhigh(yhigh), m_tlow(tlow), m_thigh(thigh) {}

	bool intersects(MovingPoint& mp)
	{
		double x0 = mp.getX(m_tlow);
		double x1 = mp.getX(m_thigh);
		double y0 = mp.getY(m_tlow);
		double y1 = mp.getY(m_thigh);
		//double t0 = m_tlow;
		//double t1 = m_thigh;

		Rectangle rxy(m_xlow, m_xhigh, m_ylow, m_yhigh);
		return rxy.intersectsSegment(x0, x1, y0, y1);

/*
		// not needed to check all planes since it is
		// guaranteed that on the time dimension
		// the line segment and the query cube have
		// exactly the same length (thus, if they intersect
		// they should intersect on the X-Y projection for sure).

		Rectangle rxy(m_xlow, m_xhigh, m_ylow, m_yhigh);
		if (rxy.intersectsSegment(x0, x1, y0, y1))
		{
			Rectangle rxt(m_xlow, m_xhigh, m_tlow, m_thigh);
			if (rxt.intersectsSegment(x0, x1, t0, t1))
			{
				Rectangle ryt(m_ylow, m_yhigh, m_tlow, m_thigh);
				if (ryt.intersectsSegment(y0, y1, t0, t1)) return true;
			}
		}
*/

		return false;
	}

	bool intersectsStupid(MovingPoint& mp)
	{
		size_t t0 = static_cast<size_t>(std::floor(m_tlow));
		size_t t1 = static_cast<size_t>(std::floor(m_thigh));

		Rectangle rxy(m_xlow, m_xhigh, m_ylow, m_yhigh);

		for (size_t T = t0; T <= t1; T++)
		{
			if (rxy.intersectsPoint(mp.getX(T), mp.getY(T))) return true;
		}
		return false;
	}

	double m_xlow, m_xhigh, m_ylow, m_yhigh;
	double m_tlow, m_thigh;
};

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		cerr << "Usage: " << argv[0] << " data_file." << endl;
		return -1;
	}

	ifstream fin(argv[1]);
	if (! fin)
	{
		cerr << "Cannot open data file" << argv[1] << "." << endl;
		return -1;
	}

	map<size_t, MovingPoint> data;
	size_t id, op;
	double ax, vx, ay, vy, ct, rt, unused;

	while (fin)
	{
		fin >> id >> op >> ct >> rt >> unused >> ax >> vx >> unused >> ay >> vy;
		if (! fin.good()) continue;

		if (op == INSERT)
		{
			data.insert(pair<size_t, MovingPoint>(id, MovingPoint(ax, vx, ay, vy, ct)));
		}
		else if (op == DELETE)
		{
			data.erase(id);
		}
		else if (op == QUERY)
		{
			TimeRectangle query = TimeRectangle(ax, vx, ay, vy, ct, rt);
			std::map<size_t, MovingPoint>::iterator it;
			for (it = data.begin(); it != data.end(); it++)
			{
				//assert(query.intersects((*it).second) == query.intersectsStupid((*it).second));
				if (query.intersects((*it).second) == false && query.intersectsStupid((*it).second) == true)
				{
					cerr << "Something is wrong: " << ct << " " << (*it).first << endl;
					return -1;
				}
				if (query.intersects((*it).second)) cout << (*it).first << endl;
			}
		}
	}

	return 0;
}
