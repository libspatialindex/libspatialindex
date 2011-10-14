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

#ifndef _random_generator_h
#define _random_generator_h

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <math.h>

#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <stack>

#include <spatialindex/SpatialIndex.h>
#include <spatialindex/tools/Tools.h>

using namespace std;

#define DELETE 0
#define INSERT 1
#define QUERY 2

#define UNIFORM 1

class RandomGenerator
{
public:
	RandomGenerator(int ds, int sl, int mui, double a)
		: m_datasetSize(ds),
		m_simulationLength(sl),
		m_initialDistribution(UNIFORM),
		m_maximumUpdateInterval(mui),
		m_queriesPerTimeInstant(5),
		m_minQueryExtent(5),
		m_maxQueryExtent(10),
		m_horizon(20),
		m_maxQueryInterval(10),
		m_minQueryInterval(2),
		m_agility(a),
		m_minSpeed(0.25), // 15 miles/hour = 0.25 miles/minute
		m_maxSpeed(1.66), // 100 miles/hour = 1.66 miles/minute
		m_speedMean(0.5), // 30 miles/hour = 0.5 miles/minute
		m_speedStandardDeviation(0.33), // 20 miles/hour = 0.33 miles/minute
		m_maxX(100.0),
		m_maxY(100.0),
		m_updateArray(0)
	{
		m_updateArray = new set<int>[m_simulationLength];
	}

	virtual ~RandomGenerator()
	{
		for (map<int, MyMovingObject*>::iterator it = m_dataset.begin(); it != m_dataset.end(); it++)
		{
			delete (*it).second;
		}
		delete[] m_updateArray;
	}

	class MyMovingObject
	{
	public:
		double getX(int t)
		{
			return m_sx + m_vx * (t - m_st);
		}

		double getY(int t)
		{
			return m_sy + m_vy * (t - m_st);
		}

	public:
		int m_id;
  		int m_st;
  		int m_kt;
		double m_sx, m_sy;
		double m_vx, m_vy;
		bool m_outOfBounds;
	};

	virtual void generate();
	MyMovingObject* createObject(int id, int st, double xmin, double xmax, double ymin, double ymax);
	MyMovingObject* createObject(int id, int st, double x, double y);
	double generateSpeed();

public:
	int m_datasetSize;
	int m_simulationLength;
	int m_initialDistribution;
	int m_maximumUpdateInterval;
	int m_queriesPerTimeInstant;
	double m_minQueryExtent;
	double m_maxQueryExtent;
	int m_horizon;
	int m_maxQueryInterval;
	int m_minQueryInterval;
	double m_agility;
	double m_minSpeed;
	double m_maxSpeed;
	double m_speedMean;
	double m_speedStandardDeviation;
	double m_maxX;
	double m_maxY;

	map<int, MyMovingObject*> m_dataset;
	set<int>* m_updateArray;
	Tools::Random m_random;
};

#endif
