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
