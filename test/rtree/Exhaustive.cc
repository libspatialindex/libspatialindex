/******************************************************************************
 * Project:  libspatialindex - A C++ library for spatial indexing
 * Author:   Marios Hadjieleftheriou, mhadji@gmail.com
 ******************************************************************************
 * Copyright (c) 2002, Marios Hadjieleftheriou
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
#include <cstring>
#include <cmath>

#define INSERT 1
#define DELETE 0
#define QUERY 2



class Region
{
public:
	double m_xmin, m_ymin, m_xmax, m_ymax;

	Region(double x1, double y1, double x2, double y2)
	{
		m_xmin = (x1 < x2) ? x1 : x2;
		m_ymin = (y1 < y2) ? y1 : y2;
		m_xmax = (x1 > x2) ? x1 : x2;
		m_ymax = (y1 > y2) ? y1 : y2;
	}

	bool intersects(Region& r)
	{
		if (
			m_xmin > r.m_xmax || m_xmax < r.m_xmin ||
			m_ymin > r.m_ymax || m_ymax < r.m_ymin)
			return false;

		return true;
	}

	double getMinDist(const Region& r)
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

	struct greater : public std::binary_function<NNEntry*, NNEntry*, bool>
	{
		bool operator()(const NNEntry* __x, const NNEntry* __y) const { return __x->m_dist > __y->m_dist; }
	};
};

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: " << argv[0] << " data_file query_type [intersection | 10NN | selfjoin]." << std::endl;
		return -1;
	}
	uint32_t queryType = 0;

	if (strcmp(argv[2], "intersection") == 0) queryType = 0;
	else if (strcmp(argv[2], "10NN") == 0) queryType = 1;
	else if (strcmp(argv[2], "selfjoin") == 0) queryType = 2;
	else
	{
		std::cerr << "Unknown query type." << std::endl;
		return -1;
	}

	std::ifstream fin(argv[1]);
	if (! fin)
	{
		std::cerr << "Cannot open data file" << argv[1] << "." << std::endl;
		return -1;
	}

	std::multimap<size_t, Region> data;
	size_t id;
	uint32_t op;
	double x1, x2, y1, y2;

	while (fin)
	{
		fin >> op >> id >> x1 >> y1 >> x2 >> y2;
		if (! fin.good()) continue;

		if (op == INSERT)
		{
			//insert
			data.insert(std::pair<size_t, Region>(id, Region(x1, y1, x2, y2)));
		}
		else if (op == DELETE)
		{
			data.erase(id);
		}
		else if (op == QUERY)
		{
			//query
			if (queryType == 0)
			{
				Region query = Region(x1, y1, x2, y2);
				for (std::multimap<size_t, Region>::iterator it = data.begin(); it != data.end(); it++)
				{
					if (query.intersects((*it).second)) std::cout << (*it).first << std::endl;
				}
			}
			else if (queryType == 1)
			{
				Region query = Region(x1, y1, x1, y1);

				std::priority_queue<NNEntry*, std::vector<NNEntry*>, NNEntry::greater > queue;

				for (std::multimap<size_t, Region>::iterator it = data.begin(); it != data.end(); it++)
				{
					queue.push(new NNEntry((*it).first, (*it).second.getMinDist(query)));
				}

				size_t count = 0;
				double knearest = 0.0;

				while (! queue.empty())
				{
					NNEntry* e = queue.top(); queue.pop();

					if (count >= 10 && e->m_dist > knearest) break;

					//std::cout << e->m_id << " " << e->m_dist << std::endl;
					std::cout << e->m_id << std::endl;
					count++;
					knearest = e->m_dist;
					delete e;
				}

				while (! queue.empty())
				{
					NNEntry* e = queue.top(); queue.pop();
					delete e;
				}
			}
			else
			{
				Region query = Region(x1, y1, x2, y2);

				for (std::multimap<size_t, Region>::iterator it1 = data.begin(); it1 != data.end(); it1++)
				{
					if (query.intersects((*it1).second))
					{
						for (std::multimap<size_t, Region>::iterator it2 = data.begin(); it2 != data.end(); it2++)
						{
							if (
								(*it1).first != (*it2).first &&
								query.intersects((*it2).second) &&
								(*it1).second.intersects((*it2).second))
							{
								std::cout << (*it1).first << " " << (*it2).first << std::endl;
							}
						}
					}
				}
			}
		}
	}

	return 0;
}
