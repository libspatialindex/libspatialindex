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

#include <cstring>
#include <cmath>
#include <limits>

#include "RandomGenerator.h"

int main(int argc, char** argv)
{
	size_t ds = 1000, sl = 100, mui = 20, id = UNIFORM;
	double a = 0.01;

	for (int i = 1; i < argc; i++)
	{
		if (! strcmp(argv[i], "-ds"))
		{
			i++;
			if (i >= argc)
			{
				cerr << "Missing dataset size." << endl;
				return -1;
			}
			ds = atoi(argv[i]);
		}
		else if (! strcmp(argv[i], "-sl"))
		{
			i++;
			if (i >= argc)
			{
				cerr << "Missing simulation length." << endl;
				return -1;
			}
			sl = atoi(argv[i]);
		}
		else if (! strcmp(argv[i], "-a"))
		{
			i++;
			if (i >= argc)
			{
				cerr << "Missing agility." << endl;
				return -1;
			}
			a = atof(argv[i]);
		}
		else if (! strcmp(argv[i], "-mui"))
		{
			i++;
			if (i >= argc)
			{
				cerr << "Missing update rate." << endl;
				return -1;
			}
			mui = atoi(argv[i]);
		}
		else
		{
			cerr << "Usage: " << endl
					 << "  -ds dataset size" << endl
					 << "  -sl simulation length" << endl
					 << "  -a agility" << endl
					 << "  -sd speed distribution" << endl
					 << "  -id initial distribution" << endl
					 << "  -mui maximum update interval" << endl;
			return -1;
		}
	}

	RandomGenerator g = RandomGenerator(ds, sl, mui, a);
	g.m_initialDistribution = id;
	g.m_maxX = 1.0; g.m_maxY = 1.0;
	g.m_minQueryExtent = 0.05; g.m_maxQueryExtent = 0.1;
	g.m_minSpeed = 0.0025; // 15 miles/hour = 0.25 miles/minute
	g.m_maxSpeed = 0.0166; // 100 miles/hour = 1.66 miles/minute
	g.m_speedMean = 0.005; // 30 miles/hour = 0.5 miles/minute
	g.m_speedStandardDeviation = 0.0033; // 20 miles/hour = 0.33 miles/minute

	g.generate();
}
