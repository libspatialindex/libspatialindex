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
