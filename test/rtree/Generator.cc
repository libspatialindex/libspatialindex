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
#include <cmath>
#include <limits>

#include <set>

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
};

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: " << argv[0] << " number_of_data time_instants." << std::endl;
		return -1;
	}

	size_t simulationLength = atol(argv[2]);
	size_t numberOfObjects = atol(argv[1]);
	std::map<size_t, Region> data;
	Tools::Random rnd;

	for (size_t i = 0; i < numberOfObjects; i++)
	{
		double x = rnd.nextUniformDouble();
		double y = rnd.nextUniformDouble();
		double dx = rnd.nextUniformDouble(0.0001, 0.1);
		double dy = rnd.nextUniformDouble(0.0001, 0.1);
		Region r = Region(x, y, x + dx, y + dy);

		data.insert(std::pair<size_t, Region>(i, r));

		std::cout << INSERT << " " << i << " " << r.m_xmin << " " << r.m_ymin << " "
			<< r.m_xmax << " " << r.m_ymax << std::endl;
	}

	if (simulationLength == 0)
	{
		for (size_t i = 0; i < 1000; i++)
		{
			double stx = rnd.nextUniformDouble();
			double sty = rnd.nextUniformDouble();
			std::cout << QUERY << " 9999999 " << stx << " " << sty << " " << (stx + 0.01) << " " << (sty + 0.01) << std::endl;
		}
	}

	size_t A = static_cast<size_t>(std::floor(static_cast<double>(numberOfObjects) * 0.05));

	for (size_t T = 1; T <= simulationLength; T++)
	{
		std::cerr << (simulationLength + 1 - T) << std::endl;
		std::set<size_t> examined;

		for (size_t a = 0; a < A; a++)
		{
			// find an id that is not yet examined.
			size_t id = static_cast<size_t>(rnd.nextUniformLong(0, numberOfObjects));
			std::set<size_t>::iterator itSet = examined.find(id);

			while (itSet != examined.end())
			{
				id = static_cast<size_t>(rnd.nextUniformLong(0, numberOfObjects));
				itSet = examined.find(id);
			}
			examined.insert(id);

			std::map<size_t, Region>::iterator itMap = data.find(id);
			assert(itMap != data.end());

			std::cout << DELETE << " " << id << " " << (*itMap).second.m_xmin << " " << (*itMap).second.m_ymin << " "
				<< (*itMap).second.m_xmax << " " << (*itMap).second.m_ymax << std::endl;

			double x = rnd.nextUniformDouble();
			double dx = rnd.nextUniformDouble(0.0001, 0.1);
			(*itMap).second.m_xmin = x;
			(*itMap).second.m_xmax = x + dx;
			double y = rnd.nextUniformDouble();
			double dy = rnd.nextUniformDouble(0.0001, 0.1);
			(*itMap).second.m_ymin = y;
			(*itMap).second.m_ymax = y + dy;

			std::cout << INSERT << " " << id << " " << (*itMap).second.m_xmin << " " << (*itMap).second.m_ymin << " "
				<< (*itMap).second.m_xmax << " " << (*itMap).second.m_ymax << std::endl;
		}

		double stx = rnd.nextUniformDouble();
		double sty = rnd.nextUniformDouble();
		std::cout << QUERY << " 9999999 " << stx << " " << sty << " " << (stx + 0.01) << " " << (sty + 0.01) << std::endl;
	}

	return 0;
}
