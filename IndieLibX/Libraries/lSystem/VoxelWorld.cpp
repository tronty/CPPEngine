#include "precomp.h"
#include <GL\glew.h>
#include <GL\glut.h>
#include "VoxelWorld.h"
#include "utils.h"

VoxelWorld::VoxelWorld(int worldSize, int voxelSize) : m_worldSize(worldSize), m_voxelSize(voxelSize), m_pWorldHasBlock(new bool[worldSize*worldSize*worldSize]), m_fShowMode(0)
{
	m_brickTexture = createTexture("C:\\Development\\openSource\\lSystemOld\\lSystem\\brick.bmp");
	for (int y = 0; y < m_worldSize; y++)
	{
		for (int z = 0; z < m_worldSize; z++)
		{
			for (int x = 0; x < m_worldSize; x++)
			{
				m_pWorldHasBlock[y * m_worldSize * m_worldSize + z * m_worldSize + x] = false;
			}
		}
	}
}

void VoxelWorld::draw() const
{
	glPushMatrix();
	glColor3f(255,255,255);

	if (m_fShowMode == 1)
	{
		for (int y = 0; y < m_worldSize; y++)
		{
			for (int z = 0; z < m_worldSize; z++)
			{
				for (int x = 0; x < m_worldSize; x++)
				{
					glTranslatef(x*m_voxelSize + m_voxelSize/2, y*m_voxelSize + m_voxelSize/2, z*m_voxelSize + m_voxelSize/2);
					if (!m_pWorldHasBlock[y * m_worldSize * m_worldSize + z * m_worldSize + x])
					{
						glutWireCube(m_voxelSize);
					}
					else
					{
						glPolygonMode(GL_FRONT, GL_FILL);
						solidCube(m_voxelSize);
					}
					glTranslatef(-x*m_voxelSize -m_voxelSize/2, -y*m_voxelSize -m_voxelSize/2, -z*m_voxelSize -m_voxelSize/2);
				}
			}
		}
	}
	else if (m_fShowMode == 2)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_brickTexture);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		for (int y = 0; y < m_worldSize; y++)
		{
			for (int z = 0; z < m_worldSize; z++)
			{
				for (int x = 0; x < m_worldSize; x++)
				{
					glTranslatef(x*m_voxelSize + m_voxelSize/2, y*m_voxelSize + m_voxelSize/2, z*m_voxelSize + m_voxelSize/2);
					if (m_pWorldHasBlock[y * m_worldSize * m_worldSize + z * m_worldSize + x])
					{
						glPolygonMode(GL_FRONT, GL_FILL);
						solidCube(m_voxelSize);
					}
					glTranslatef(-x*m_voxelSize -m_voxelSize/2, -y*m_voxelSize -m_voxelSize/2, -z*m_voxelSize -m_voxelSize/2);
				}
			}
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
		glCullFace(0);
		glDisable(GL_CULL_FACE);
	}

	glPopMatrix();
}

void VoxelWorld::addWorldBlock(const Point3Df& position)
{
	std::tuple<int, int, int> grid = getGridFor(position);
	int x = std::get<0>(grid);
	int y = std::get<1>(grid);
	int z = std::get<2>(grid);
	m_pWorldHasBlock[y * m_worldSize * m_worldSize + z * m_worldSize + x] = true;
}

void VoxelWorld::removeWorldBlock(const Point3Df& position)
{
	std::tuple<int, int, int> grid = getGridFor(position);
	int x = std::get<0>(grid);
	int y = std::get<1>(grid);
	int z = std::get<2>(grid);
	m_pWorldHasBlock[y * m_worldSize * m_worldSize + z * m_worldSize + x] = false;
}

void VoxelWorld::addWall(const Point3Df& position)
{
	std::tuple<int, int, int> grid = getGridFor(position);
	int x = std::get<0>(grid);
	int y = std::get<1>(grid);
	int z = std::get<2>(grid);
	if (isValidGrid(grid))
	{
		for (int i = 0; i < m_worldSize; i++)
		{
			for (int j = 0; j < m_worldSize; j++)
			{
				m_pWorldHasBlock[i * m_worldSize * m_worldSize + j * m_worldSize + x] = true;
			}
		}
	}
}

void VoxelWorld::addPillar(const Point3Df& position)
{
	std::tuple<int, int, int> grid = getGridFor(position);
	int x = std::get<0>(grid);
	int y = std::get<1>(grid);
	int z = std::get<2>(grid);
	for (int i = 0; i < m_worldSize; i++)
	{
		m_pWorldHasBlock[i * m_worldSize * m_worldSize + z * m_worldSize + x] = true;
	}
}

bool VoxelWorld::isValidGrid(const std::tuple<int, int, int>& grid) const throw()
{
	int x = std::get<0>(grid);
	int y = std::get<1>(grid);
	int z = std::get<2>(grid);
	
	return (x >= 0 && x < m_worldSize && y >= 0 && y < m_worldSize && z >= 0 && z < m_worldSize);
}

bool VoxelWorld::isOccupied(const std::tuple<int, int, int>& grid) const throw()
{
	int x = std::get<0>(grid);
	int y = std::get<1>(grid);
	int z = std::get<2>(grid);

	return m_pWorldHasBlock[y * m_worldSize * m_worldSize + z * m_worldSize + x];
}

std::tuple<int, int, int> VoxelWorld::getGridFor(const Point3Df& position) const throw()
{
	int gridX = position.X() / m_voxelSize;
	int gridY = position.Y() / m_voxelSize;
	int gridZ = position.Z() / m_voxelSize;
	std::tuple<int, int, int> grid(gridX, gridY, gridZ);
	if (isValidGrid(grid))
	{
		return grid;
	}
	else
	{
		return std::tuple<int, int, int>(-1,-1,-1);
	}
}

std::tuple<int, int, int> VoxelWorld::getNextGridForRay(float xDir, float yDir, float zDir, const std::tuple<int, int, int>& currentGrid) const throw()
{
	std::tuple<int, int, int> nextGrid(currentGrid);
	if (xDir >= 0) 
	{
		std::get<0>(nextGrid) = std::get<0>(nextGrid) + 1;
	}
	else
	{
		std::get<0>(nextGrid) = std::get<0>(nextGrid);
	}

	if (yDir >= 0) 
	{
		std::get<1>(nextGrid) = std::get<1>(nextGrid) + 1;
	}
	else
	{
		std::get<1>(nextGrid) = std::get<1>(nextGrid);
	}

	if (zDir >= 0) 
	{
		std::get<2>(nextGrid) = std::get<2>(nextGrid) + 1;
	}
	else
	{
		std::get<2>(nextGrid) = std::get<2>(nextGrid);
	}
	return nextGrid;
}

Point3Df VoxelWorld::getNextGridCoordinates(int axis, const std::tuple<int, int, int>& currentGrid, const std::tuple<int, int, int>& nextGrid) const throw()
{
	int x = (axis == 0)? std::get<0>(nextGrid) : std::get<0>(currentGrid);
	int y = (axis == 1)? std::get<1>(nextGrid) : std::get<1>(currentGrid);
	int z = (axis == 2)? std::get<2>(nextGrid) : std::get<2>(currentGrid);
	return Point3Df(x *m_voxelSize, y * m_voxelSize, z * m_voxelSize);
}

std::tuple<int, int, int> VoxelWorld::getGridForRayIntersection(const Vector4& ray, const Point3Df& origin) const throw()
{
	std::tuple<int, int, int> currentGrid = getGridFor(origin);
	assert(isValidGrid(currentGrid));
	float alongX = ray.x; float alongY = ray.y; float alongZ = ray.z;
	
	int stepX = alongX >= 0? 1 : -1;
	int stepY = alongY >= 0? 1 : -1;
	int stepZ = alongZ >= 0? 1 : -1;

	std::tuple<int, int, int> potentialNextGrids = getNextGridForRay(alongX, alongY, alongZ, currentGrid);

	Point3Df nextXCoordinates = getNextGridCoordinates(0, currentGrid, potentialNextGrids);
	Point3Df nextYCoordinates = getNextGridCoordinates(1, currentGrid, potentialNextGrids);
	Point3Df nextZCoordinates = getNextGridCoordinates(2, currentGrid, potentialNextGrids);

	float tMaxX = FLT_MAX, tMaxY = FLT_MAX, tMaxZ = FLT_MAX;
	float tDeltaX = FLT_MAX, tDeltaY = FLT_MAX, tDeltaZ = FLT_MAX;
	if (alongX != 0)
	{
		tDeltaX = m_voxelSize / abs(alongX);
		tMaxX = (nextXCoordinates.X() - origin.X())/ alongX;
	}

	if (alongY != 0)
	{
		tDeltaY = m_voxelSize / abs(alongY);
		tMaxY = (nextYCoordinates.Y() - origin.Y())/ alongY;
	}

	if (alongZ != 0)
	{
		tDeltaZ = m_voxelSize / abs(alongZ);
		tMaxZ = (nextZCoordinates.Z() - origin.Z())/ alongZ;
	}

	do
	{
        if (tMaxX < tMaxY && tMaxX < tMaxZ)
        {
            std::get<0>(currentGrid) += stepX;
            tMaxX += tDeltaX;
        }
        else if (tMaxY < tMaxZ)
        {
            std::get<1>(currentGrid) += stepY;
            tMaxY += tDeltaY;
        }
        else
        {
            std::get<2>(currentGrid) += stepZ;
            tMaxZ += tDeltaZ;
        }
		if (!isValidGrid(currentGrid))
		{
			return std::tuple<int, int, int>(-1,-1,-1);
		}
		if (isOccupied(currentGrid))
		{
			break;
		}
	}while (true);
	return currentGrid;
}