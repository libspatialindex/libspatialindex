/******************************************************************************
 * Project:  libspatialindex - A C++ library for spatial indexing
 * Author:   Marios Hadjieleftheriou, mhadji@gmail.com
 ******************************************************************************
 * Copyright (c) 2003, Marios Hadjieleftheriou
 *
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
******************************************************************************/

#include <spatialindex/tools/Tools.h>
#include <cmath>
#include <limits>

using namespace std;

#define DELETE 0
#define INSERT 1
#define QUERY 2

class Region
{
public:
	double m_xmin, m_ymin, m_xmax, m_ymax;
	bool m_bIsDead;

	Region() : m_xmin(numeric_limits<double>::max()), m_ymin(numeric_limits<double>::max()),
						 m_xmax(numeric_limits<double>::max()), m_ymax(numeric_limits<double>::max()) {}

	Region(double x1, double y1, double x2, double y2)
	{
		m_xmin = (x1 < x2) ? x1 : x2;
		m_ymin = (y1 < y2) ? y1 : y2;
		m_xmax = (x1 > x2) ? x1 : x2;
		m_ymax = (y1 > y2) ? y1 : y2;
		m_bIsDead = false;
	}
};

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		cerr << "Usage: " << argv[0] << " number_of_data." << endl;
		return -1;
	}

	size_t numberOfObjects = atoi(argv[1]);
	map<size_t, Region> data;
	Tools::Random rnd;

	for (size_t i = 0; i < numberOfObjects; i++)
	{
		double x = rnd.nextUniformDouble();
		double y = rnd.nextUniformDouble();
		double dx = rnd.nextUniformDouble(0.0001, 0.1);
		double dy = rnd.nextUniformDouble(0.0001, 0.1);
		Region r = Region(x, y, x + dx, y + dy);

		data.insert(pair<size_t, Region>(i, r));

		cout << "0 " << INSERT << " " << i << " " << r.m_xmin << " " << r.m_ymin << " "
			<< r.m_xmax << " " << r.m_ymax << endl;
	}

	size_t nextID = numberOfObjects;
	size_t A = static_cast<size_t>(std::floor(static_cast<double>(numberOfObjects) * 0.05));

	for (size_t T = 1; T <= 100; T++)
	{
		cerr << (101 - T) << endl;

		if (T == 50)
		{
			// delete all entries and reinsert
			for (map<size_t, Region>::iterator itMap = data.begin(); itMap != data.end(); itMap++)
			{
				if (! (*itMap).second.m_bIsDead)
				{
					(*itMap).second.m_bIsDead = true;
					cout << "50 " << DELETE << " " << (*itMap).first << " " << (*itMap).second.m_xmin << " " << (*itMap).second.m_ymin << " "
						<< (*itMap).second.m_xmax << " " << (*itMap).second.m_ymax << endl;
				}
			}

			for (size_t i = nextID; i < nextID + numberOfObjects; i++)
			{
				double x = rnd.nextUniformDouble();
				double y = rnd.nextUniformDouble();
				double dx = rnd.nextUniformDouble(0.0001, 0.1);
				double dy = rnd.nextUniformDouble(0.0001, 0.1);
				Region r = Region(x, y, x + dx, y + dy);

				data.insert(pair<size_t, Region>(i, r));

				cout << "50 " << INSERT << " " << i << " " << r.m_xmin << " " << r.m_ymin << " "
					<< r.m_xmax << " " << r.m_ymax << endl;
			}

			nextID += numberOfObjects;
			continue;
		}

		set<size_t> examined;

		for (size_t a = 0; a < A; a++)
		{
			// find an id that is not examined yet.
			size_t id = static_cast<size_t>(rnd.nextUniformLongLong(0, nextID));
			set<size_t>::iterator itSet = examined.find(id);
			while (itSet != examined.end() || data[id].m_bIsDead == true)
			{
				id = static_cast<size_t>(rnd.nextUniformLongLong(0, nextID));
				itSet = examined.find(id);
			}
			examined.insert(id);

			map<size_t, Region>::iterator itMap = data.find(id);
			assert(itMap != data.end() && (*itMap).second.m_bIsDead == false);

			cout << T << " " << DELETE << " " << id << " " << (*itMap).second.m_xmin << " " << (*itMap).second.m_ymin << " "
				<< (*itMap).second.m_xmax << " " << (*itMap).second.m_ymax << endl;
			(*itMap).second.m_bIsDead = true;

			double x = rnd.nextUniformDouble();
			double y = rnd.nextUniformDouble();
			double dx = rnd.nextUniformDouble(0.0001, 0.1);
			double dy = rnd.nextUniformDouble(0.0001, 0.1);
			Region r = Region(x, y, x + dx, y + dy);

			data.insert(pair<size_t, Region>(nextID, r));

			cout << T << " " << INSERT << " " << nextID << " " << r.m_xmin << " " << r.m_ymin << " "
				<< r.m_xmax << " " << r.m_ymax << endl;
			examined.insert(nextID);
			nextID++;
		}

		double stx = rnd.nextUniformDouble();
		double sty = rnd.nextUniformDouble();
		size_t qt = rnd.nextUniformLongLong(0, T);
		cout << T << " " << QUERY << " 9999999 " << stx << " " << sty << " " << (stx + 0.01) << " " << (sty + 0.01)  << " " << qt << " " << qt + 2<< endl;
		stx = rnd.nextUniformDouble();
		sty = rnd.nextUniformDouble();
		qt = rnd.nextUniformLongLong(0, T);
		cout << T << " " << QUERY << " 9999999 " << stx << " " << sty << " " << (stx + 0.01) << " " << (sty + 0.01)  << " " << qt << " " << qt + 2<< endl;
		stx = rnd.nextUniformDouble();
		sty = rnd.nextUniformDouble();
		qt = rnd.nextUniformLongLong(0, T);
		cout << T << " " << QUERY << " 9999999 " << stx << " " << sty << " " << (stx + 0.01) << " " << (sty + 0.01)  << " " << qt << " " << qt + 2<< endl;
	}

	// delete everything at the end (for testing the special case when the tree dies out completely)
	for (map<size_t, Region>::iterator it = data.begin(); it != data.end(); it++)
	{
		if (! it->second.m_bIsDead)
		{
			cout << 102 << " "  << DELETE << " " << it->first << " " << it->second.m_xmin << " " << it->second.m_ymin << " "
				<< it->second.m_xmax << " " << it->second.m_ymax << endl;
		}
	}

	return 0;
}
