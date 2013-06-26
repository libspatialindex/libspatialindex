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

#include <assert.h>
#include <iostream>
#include <fstream>
#include <map>
#include <queue>
#include <cmath>
#include <cstring>
#include <limits>
#include <cmath>

using namespace std;

#define DELETE 0
#define INSERT 1
#define QUERY 2


#include <spatialindex/tools/Tools.h>

class TimeRegion
{
public:
	double m_xmin, m_ymin, m_xmax, m_ymax;
	double m_startTime, m_endTime;

	TimeRegion() {}

	TimeRegion(double x1, double y1, double x2, double y2, double t1, double t2)
	{
		m_xmin = (x1 < x2) ? x1 : x2;
		m_ymin = (y1 < y2) ? y1 : y2;
		m_xmax = (x1 > x2) ? x1 : x2;
		m_ymax = (y1 > y2) ? y1 : y2;
		m_startTime = t1;
		m_endTime = (t2 <= 0) ? std::numeric_limits<double>::max() : t2;
	}

	bool intersects(TimeRegion& r)
	{
		if (m_xmin > r.m_xmax || m_xmax < r.m_xmin ||
				m_ymin > r.m_ymax || m_ymax < r.m_ymin) return false;

		return true;
	}

	bool intersectsInTime(TimeRegion& r)
	{
		//if (m_startTime != r.m_startTime && (m_endTime <= r.m_startTime || m_startTime >= r.m_endTime)) return false;
		if (m_endTime <= r.m_startTime || m_startTime >= r.m_endTime) return false;
		return intersects(r);
	}

	double getMinDist(const TimeRegion& r)
	{
		double ret = 0.0;

		if (r.m_xmax < m_xmin)
			ret += std::pow(m_xmin - r.m_xmax, 2.0);
		else if (r.m_xmin > m_xmax)
			ret += std::pow(r.m_xmin - m_xmax, 2.0);

		if (r.m_ymax < m_ymin)
			ret += std::pow(m_ymin - r.m_ymax, 2.0);
		else if (r.m_ymin > m_ymax)
			ret += std::pow(r.m_ymin - m_ymax, 2.0);

		return ret;
	}
};

class NNEntry
{
public:
	size_t m_id;
	double m_dist;

	NNEntry(size_t id, double dist) : m_id(id), m_dist(dist) {}

	struct greater : public binary_function<NNEntry*, NNEntry*, bool>
	{
		bool operator()(const NNEntry* __x, const NNEntry* __y) const { return __x->m_dist > __y->m_dist; }
	};
};

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		cerr << "Usage: " << argv[0] << " data_file query_type [intersection | 10NN]." << endl;
		return -1;
	}

	uint32_t queryType = 0;

	if (strcmp(argv[2], "intersection") == 0) queryType = 0;
	else if (strcmp(argv[2], "10NN") == 0) queryType = 1;
	else
	{
		cerr << "Unknown query type." << endl;
		return -1;
	}

	ifstream fin(argv[1]);
	if (! fin)
	{
		cerr << "Cannot open data file" << argv[1] << "." << endl;
		return -1;
	}

	multimap<size_t, TimeRegion> data;
	size_t id;
	uint32_t op;
	double x1, x2, y1, y2, t;

	while (fin)
	{
		fin >> t >> op >> id >> x1 >> y1 >> x2 >> y2;
		if (! fin.good()) continue;

		if (op == INSERT)
		{
			//insert
			data.insert(pair<size_t, TimeRegion>(id, TimeRegion(x1, y1, x2, y2, t, std::numeric_limits<double>::max())));
		}
		else if (op == DELETE)
		{
			//delete
			// find the live instance of id.
			multimap<size_t, TimeRegion>::iterator it = data.find(id);
			assert(it != data.end());
			while (it->first == id && it->second.m_endTime < std::numeric_limits<double>::max()) it++;
			assert(it->first == id);
			(*it).second.m_endTime = t;
		}
		else if (op == QUERY)
		{
			size_t qt1, qt2;
			fin >> qt1 >> qt2;
			if (! fin.good()) continue;

			//query
			if (queryType == 0)
			{
				TimeRegion query = TimeRegion(x1, y1, x2, y2, qt1, qt2);
				for (multimap<size_t, TimeRegion>::iterator it = data.begin(); it != data.end(); it++)
				{
					if (query.intersectsInTime((*it).second)) cout << (*it).first << endl;
				}
			}
			else
			{
				/*
				TimeRegion query = TimeRegion(x1, y1, x1, y1, qt1, qt2);

				priority_queue<NNEntry*, vector<NNEntry*>, NNEntry::greater > queue;

				for (multimap<size_t, Region>::iterator it = data.begin(); it != data.end(); it++)
				{
					queue.push(new NNEntry((*it).first, (*it).second.getMinDist(query)));
				}

				size_t count = 0;
				double knearest = 0.0;

				while (! queue.empty())
				{
					NNEntry* e = queue.top(); queue.pop();

					if (count >= 10 && e->m_dist > knearest) break;

					//cout << e->m_id << " " << e->m_dist << endl;
					cout << e->m_id << endl;
					count++;
					knearest = e->m_dist;
					delete e;
				}

				while (! queue.empty())
				{
					NNEntry* e = queue.top(); queue.pop();
					delete e;
				}
			*/
			}
		}
	}

	return 0;
}
