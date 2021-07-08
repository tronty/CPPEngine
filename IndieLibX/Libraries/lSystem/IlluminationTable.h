#ifndef _ILLUMINATIONTABLE_H_
#define _ILLUMINATIONTABLE_H_

class VoxelWorld;

class IlluminationResult
{
private:
	Vector4 m_illuminationDirection;
	int m_illumination;
public:
	IlluminationResult(const Vector4& direction, int illumination) : m_illuminationDirection(direction), m_illumination(illumination) {}
	Vector4 diretion() { return m_illuminationDirection; }
	float illumination() { return (float)m_illumination/8; }
};

class IlluminationTableCell
{
private:
	const float m_gridSize;
	const float m_planeDistance;
	Vector4 m_location;
	int m_illumination;

public:
	IlluminationTableCell(int row, int col);
	Vector4 getRay(const Point3Df& pointFrom) const throw();
	void increaseIllumination() throw() { m_illumination++; }
	int getIllumination() const throw() { return m_illumination; }
	Vector4 getLocation() const throw() { return m_location; }
};

class IlluminationTable
{
private:
	const int m_size;
	std::vector<IlluminationTableCell> m_illuminationTable;
public:
	IlluminationTable();
	int getSizeX() const throw(){ return m_size; }
	int getSizeY() const throw(){ return m_size; }
	void addLightEntry(int row, int col) throw();
	IlluminationResult findMaxIllumination(const Vector3& currentHeading) const throw();
	Vector4 getRay(int x, int y, const Point3Df& pointFrom) const throw();
};

class IlluminationTableConstructor
{
private:
	const VoxelWorld& m_world;
public:
	IlluminationTableConstructor(const VoxelWorld& world);
	std::unique_ptr<IlluminationTable> constructTable(const Point3Df& position, const Matrix4& orientation) const throw();
	bool rayOccluded(const Vector4& ray, const Point3Df& position) const throw();
};

#endif// _ILLUMINATIONTABLE_H_