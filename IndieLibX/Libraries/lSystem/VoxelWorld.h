#ifndef _VOXELWORLD_H_
#define _VOXELWORLD_H_

#include "drawable.h"
#include <GL\glew.h>

class VoxelWorld : public IDrawable
{
private:
	int m_worldSize;
	int m_voxelSize;
	int m_fShowMode;
	std::unique_ptr<bool[]> m_pWorldHasBlock;
	GLuint m_brickTexture;

	std::tuple<int, int, int> getGridFor(const Point3Df& point) const throw();
	Point3Df getNextGridCoordinates(int axis, const std::tuple<int, int, int>& currentGrid, const std::tuple<int, int, int>& nextGrid) const throw();
	bool isOccupied(const std::tuple<int, int, int>& grid) const throw();
	bool isValidGrid(const std::tuple<int, int, int>& grid) const throw();

public:
	explicit VoxelWorld(int worldSize, int voxelSize);
	void addWorldBlock(const Point3Df& pos);
	void removeWorldBlock(const Point3Df& pos);
	void addWall(const Point3Df& pos);
	void addPillar(const Point3Df& position);
	void toggleShowGrid() throw() { m_fShowMode = (m_fShowMode+1)%3; }
	void draw() const override;
	std::tuple<int,int,int> getGridSizes() { return std::tuple<int, int, int>(m_voxelSize, m_voxelSize, m_voxelSize); }
	std::tuple<int, int, int> getGridForRayIntersection(const Vector4& ray, const Point3Df& origin) const throw();
	std::tuple<int, int, int> getNextGridForRay(float xDir, float yDir, float zDir, const std::tuple<int, int, int>& currentGrid) const throw();
};

#endif//_VOXELWORLD_H_