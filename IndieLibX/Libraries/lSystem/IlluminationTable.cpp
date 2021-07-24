#include "precomp.h"
#include "IlluminationTable.h"
#include "VoxelWorld.h"
#include "utils.h"

//TODO msati3: remove hardcoding? or this is too interally embedded?
IlluminationTableCell::IlluminationTableCell(int row, int col) : m_gridSize(3), m_planeDistance(12)
{
	m_location.set(row * m_gridSize, m_planeDistance, col * m_gridSize, 1);
	m_location.set(m_location.x - 2*m_gridSize, m_planeDistance, m_location.z - 2*m_gridSize, 1);
	m_illumination = 0;
}

Vector4 IlluminationTableCell::getRay(const Point3Df& pointFrom) const throw()
{
	int randRow = rand()%(int)m_gridSize;
	int randCol = rand()%(int)m_gridSize;
	Point3Df pointGrid(m_location.x + randRow, m_planeDistance, m_location.z + randCol);
	return Vector4(Point3Df(0,0,0), pointGrid);
}

IlluminationTable::IlluminationTable() : m_size(4)
{
	for (int i = 0; i < m_size; i++)
	{
		for (int j = 0; j < m_size; j++)
		{
			m_illuminationTable.push_back(IlluminationTableCell(i,j));
		}
	}
}

Vector4 IlluminationTable::getRay(int i, int j, const Point3Df& position) const throw()
{
	return m_illuminationTable[i*m_size + j].getRay(position);
}

void IlluminationTable::addLightEntry(int row, int col) throw()
{
	m_illuminationTable[row*m_size + col].increaseIllumination();
}

IlluminationResult IlluminationTable::findMaxIllumination(const Vector3& headingOriginal) const throw()
{
	int maxIllumination = -1;
	int maxI = -1, maxJ = -1;
	float minAngle = FLT_MAX;
	for (int i = 0; i < m_size; i++)
	{
		for (int j = 0; j < m_size; j++)
		{ 
			int illumination = m_illuminationTable[i*m_size + j].getIllumination();
			if ( illumination >= maxIllumination)
			{
				Vector4 vectorToIlluminationLocation = m_illuminationTable[i*m_size + j].getLocation();
				Vector3 vectorToIlluminationLocation3(vectorToIlluminationLocation.x, vectorToIlluminationLocation.y, vectorToIlluminationLocation.z);
				//float angle = vectorToIlluminationLocation3.angle(headingOriginal);
				float angle = vectorToIlluminationLocation3.angle(Vector3(0,1,0));
				if (abs(angle) < minAngle)
				{
					maxIllumination = illumination;
					maxI = i;
					maxJ = j;
					minAngle = abs(angle);
				}
			}
		}
	}
	if (maxI != -1)
	{
		return IlluminationResult(m_illuminationTable[maxI*m_size + maxJ].getLocation(), maxIllumination);
	}
	return IlluminationResult(Vector4(0,0,0,0), 0);
}

IlluminationTableConstructor::IlluminationTableConstructor(const VoxelWorld& world) : m_world(world)
{
}

bool IlluminationTableConstructor::rayOccluded(const Vector4& ray, const Point3Df& position) const throw()
{
	//Don't allow branches to go below the current position
	//if (ray.y < 0)
	//{
	//	return true;
	//}
	std::tuple<int, int, int> grid = m_world.getGridForRayIntersection(ray, position);
	return (std::get<0>(grid) != -1);
}

std::unique_ptr<IlluminationTable> IlluminationTableConstructor::constructTable(const Point3Df& position, const Matrix4& orientation) const throw()
{
	int numRays = 8;
	std::unique_ptr<IlluminationTable> pTable(new IlluminationTable());
	for (int i = 0; i < pTable->getSizeY(); i++)
	{
		for (int j = 0; j < pTable->getSizeX(); j++)
		{
			for (int k = 0; k < numRays; k++)
			{
				Vector4 ray = pTable->getRay(i, j, position);
				ray = orientation * ray;
				ray.normalize();
				if(!rayOccluded(ray, position))
				{
					pTable->addLightEntry(i, j);
				}
				//std::unique_ptr<DrawableVector> pRayDrawable(new DrawableVector(ray, position));
				//getRenderer().addDrawable(std::move(pRayDrawable));
			}
		}
	}
	return pTable;
}
