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

#include <spatialindex/tools/Tools.h>
#include <cstring>
#include <cmath>

#define INSERT 1
#define DELETE 0
#define QUERY 2

#if defined _WIN32 || defined _WIN64 || defined WIN32 || defined WIN64
	typedef __int8 int8_t;
	typedef __int16 int16_t;
	typedef __int32 int32_t;
	typedef __int64 int64_t;
	typedef unsigned __int8 uint8_t;
	typedef unsigned __int16 uint16_t;
	typedef unsigned __int32 uint32_t;
	typedef unsigned __int64 uint64_t;

	// Nuke this annoying warning.  See http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
	#pragma warning( disable: 4251 )
#else
	#include <stdint.h>
#endif

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
