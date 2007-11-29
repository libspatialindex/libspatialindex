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

#include "RandomGenerator.h"

RandomGenerator::MyMovingObject* RandomGenerator::createObject(int id, int st, double xmin, double xmax, double ymin, double ymax)
{
	double x, y;
	x = m_random.nextUniformDouble(xmin, xmax);
	y = m_random.nextUniformDouble(ymin, ymax);

	return createObject(id, st, x, y);
}

RandomGenerator::MyMovingObject* RandomGenerator::createObject(int id, int st, double x, double y)
{
	MyMovingObject* o = new MyMovingObject();

	o->m_id = id;
	o->m_sx = x;
	o->m_sy = y;
	o->m_st = st;
	o->m_kt = -1;

	double v = generateSpeed();
	if (m_random.flipCoin()) v *= -1.0;

	double angle = m_random.nextUniformDouble(-M_PI_2 , M_PI_2);
	if (m_random.flipCoin()) angle *= -1.0;

	o->m_vx = cos(angle) * v;
	o->m_vy = sin(angle) * v;

	cout << o->m_id << " " << INSERT << " " << st << " " << o->m_st << " 1 " << o->m_sx << " " << o->m_vx
		 << " 1 " << o->m_sy << " " << o->m_vy << endl;

	map<int, MyMovingObject*>::iterator itDataset = m_dataset.find(o->m_id);
	if (itDataset != m_dataset.end()) m_dataset.erase(itDataset);
	m_dataset.insert(pair<int, MyMovingObject*>(o->m_id, o));

	int t1 = o->m_st + m_maximumUpdateInterval;
	int t2;

	for (t2 = st; t2 <= m_simulationLength; t2++)
	{
		if (o->getX(t2) > m_maxX || o->getY(t2) > m_maxY || o->getX(t2) < 0.0 || o->getY(t2) < 0.0) break;
	}

	o->m_outOfBounds = (t2 < t1) ? true : false;
	int t = min(t1, t2);

	if (t == st) t++;
	if (t < m_simulationLength)
	{
		m_updateArray[t].insert(o->m_id);
		o->m_kt = t;
	}

	return o;
}

double RandomGenerator::generateSpeed()
{
	return m_random.nextUniformDouble(m_minSpeed, m_maxSpeed);
}

void RandomGenerator::generate()
{
	for (map<int, MyMovingObject*>::iterator itDataset = m_dataset.begin(); itDataset != m_dataset.end(); itDataset++)
	{
		delete (*itDataset).second;
	}
	m_dataset.clear();

	for (int cIndex = 0; cIndex < m_simulationLength; cIndex++)
	{
		m_updateArray[cIndex].clear();
	}

	int updatesPerTimeInstant = (int) ceil(((double) m_datasetSize) * m_agility);

	for (int cObject = 0; cObject < m_datasetSize; cObject++)
	{
		createObject(cObject, 0, 0.0, m_maxX, 0.0, m_maxY);

		if (cObject % 10000 == 0) cerr << cObject << endl;
	}

	for (int Tnow = 1; Tnow < m_simulationLength; Tnow++)
	{
		cerr << "Time: " << Tnow;

		int cTotalUpdates = 0;
		int cNeedToUpdate = updatesPerTimeInstant;
		set<int> updated;
		set<int>::iterator itUpdateArray = m_updateArray[Tnow].begin();

		while (cNeedToUpdate > 0 || itUpdateArray != m_updateArray[Tnow].end())
		{
			int id;
			bool bKilled = false;
			if (itUpdateArray != m_updateArray[Tnow].end())
			{
				bKilled = true;
				id = *itUpdateArray;
				itUpdateArray++;
			}
			else
			{
				id = m_random.nextUniformLong(0, m_datasetSize);
				set<int>::iterator itUpdated = updated.find(id);

				while (itUpdated != updated.end())
				{
					id = m_random.nextUniformLong(0, m_datasetSize);
					itUpdated = updated.find(id);
				}
			}
			updated.insert(id);
			cNeedToUpdate--;
			cTotalUpdates++;

			map<int,MyMovingObject*>::iterator itDataset = m_dataset.find(id);
			assert(itDataset != m_dataset.end());
			MyMovingObject* o = (*itDataset).second;
			m_dataset.erase(itDataset);
			if (o->m_kt >= 0) m_updateArray[o->m_kt].erase(o->m_id);

			cout << o->m_id << " " << DELETE << " " << Tnow << " " << o->m_st << " 1 " << o->m_sx << " " << o->m_vx
					 << " 1 " << o->m_sy << " " << o->m_vy << endl;

			if (bKilled && o->m_outOfBounds)
			{
				createObject(o->m_id, Tnow, 0.0, m_maxX, 0.0, m_maxY);
			}
			else
			{
				createObject(o->m_id, Tnow, o->getX(Tnow), o->getY(Tnow));
			}

			delete o;
		}

		for (int cQuery = 0; cQuery < m_queriesPerTimeInstant; cQuery++)
		{
			double x = m_random.nextUniformDouble(0.0, m_maxX);
			double y = m_random.nextUniformDouble(0.0, m_maxY);
			double dx = m_random.nextUniformDouble(m_minQueryExtent, m_maxQueryExtent);
			double dy = m_random.nextUniformDouble(m_minQueryExtent, m_maxQueryExtent);
			int dt = m_random.nextUniformLong(m_minQueryInterval, m_maxQueryInterval);
			int t = m_random.nextUniformLong(Tnow, Tnow + m_horizon - dt);

			cout << "9999999 " << QUERY << " " << t << " " << t + dt << " 1 " << x - dx << " " << x + dx << " 1 " << y - dy << " " << y + dy << endl;
		}

		cerr << ", Updates: " << cTotalUpdates << endl;
	}
}
