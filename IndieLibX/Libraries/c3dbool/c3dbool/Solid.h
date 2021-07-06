/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#ifndef __Solid_H__
#define __Solid_H__
/**
 * Class representing a 3D solid.
 *  
 * @author Danilo Balby Silva Castanheira (danbalby@yahoo.com)
 */
class Solid : public Object //extends MeshRenderer3 
{
	public:
	/** array of indices for the vertices from the 'vertices' attribute */
	std::vector<__WORD__> indices;
	/** array of points defining the solid's vertices */
	std::vector<Point3d> vertices;
	/** array of color defining the vertices colors */
	std::vector<Color3f> colors;
	
	//--------------------------------CONSTRUCTORS----------------------------------//
	
	/** Constructs an empty solid. */			
	public: 
	Solid(const Solid& rhs)
	{
		indices=rhs.indices;
		vertices=rhs.vertices;
		colors=rhs.colors;
	}
	Solid() : Object()
	{
		setInitialFeatures();
	}
	
	/**
	 * Construct a solid based on data arrays. An exception may occur in the case of 
	 * abnormal arrays (indices making references to inexistent vertices, there are less
	 * colors than vertices...)
	 * 
	 * @param vertices array of points defining the solid vertices
	 * @param indices array of indices for a array of vertices
	 * @param colors array of colors defining the vertices colors 
	 */
	Solid(std::vector<Point3d> vertices, std::vector<__WORD__> indices, std::vector<Color3f> colors) : Object()
	{
		Solid();
		setData(vertices, indices, colors);		
	}
	
	/**
	 * Constructs a solid based on a coordinates file. It contains vertices and indices, 
	 * and its format is like this:
	 * 
	 * <br><br>4
	 * <br>0 -5.00000000000000E-0001 -5.00000000000000E-0001 -5.00000000000000E-0001
	 * <br>1  5.00000000000000E-0001 -5.00000000000000E-0001 -5.00000000000000E-0001
	 * <br>2 -5.00000000000000E-0001  5.00000000000000E-0001 -5.00000000000000E-0001
	 * <br>3  5.00000000000000E-0001  5.00000000000000E-0001 -5.00000000000000E-0001
	 * 
	 * <br><br>2
	 * <br>0 0 2 3
	 * <br>1 3 1 0 
	 * 
	 * @param solidFile file containing the solid coordinates
	 * @param color solid color
	 */
	Solid(FILE solidFile, Color3f color) : Object()
	{
		Solid();
		loadCoordinateFile(solidFile, color);
	}
	
	Solid(MeshRenderer2& shape3D);
	void GetMesh(MeshRenderer2& shape3D);

	/** Sets the initial features common to all constructors */
	protected:
	void setInitialFeatures()
	{
		vertices.clear();
		colors.clear();
		indices.clear();
		#if 0 // ???
		setCapability(MeshRenderer3.ALLOW_GEOMETRY_WRITE);
		setCapability(MeshRenderer3.ALLOW_APPEARANCE_WRITE);
		setCapability(MeshRenderer3.ALLOW_APPEARANCE_READ);
		#endif
	}
		
	//---------------------------------------GETS-----------------------------------//
	
	/**
	 * Gets the solid vertices
	 * 
	 * @return solid vertices
	 */	
	public:
	std::vector<Point3d> getVertices()
	{
		std::vector<Point3d> newVertices(vertices.size());
		for(int i=0;i<newVertices.size();i++)
		{
			newVertices[i] = vertices[i];
		}
		return newVertices;
	}
	
	/** Gets the solid indices for its vertices
	 * 
	 * @return solid indices for its vertices
	 */
	std::vector<__WORD__> getIndices()
	{
		std::vector<__WORD__> newIndices(indices.size());
		stx_memcpy(&newIndices[0],&indices[0],indices.size()*sizeof(__WORD__));
		return newIndices;
	}
	
	/** Gets the vertices colors
	 * 
	 * @return vertices colors
	 */
	std::vector<Color3f> getColors()
	{
		std::vector<Color3f> newColors(colors.size());
		for(int i=0;i<newColors.size();i++)
		{
			newColors[i] = colors[i];
		}
		return newColors;
	}
	
	/**
	 * Gets if the solid is empty (without any vertex)
	 * 
	 * @return true if the solid is empty, false otherwise
	 */
	bool isEmpty()
	{
		if(indices.size()==0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}	

	//---------------------------------------SETS-----------------------------------//
	
	/**
	 * Sets the solid data. Each vertex may have a different color. An exception may 
	 * occur in the case of abnormal arrays (e.g., indices making references to  
	 * inexistent vertices, there are less colors than vertices...)
	 * 
	 * @param vertices array of points defining the solid vertices
	 * @param indices array of indices for a array of vertices
	 * @param colors array of colors defining the vertices colors 
	 */
	void setData(std::vector<Point3d> avertices, std::vector<__WORD__> aindices, std::vector<Color3f> acolors)
	{
		vertices.resize(avertices.size());
		colors.resize(acolors.size());
		indices.resize(aindices.size());
		if(aindices.size()!=0)
		{
			for(int i=0;i<vertices.size();i++)
			{
				vertices[i] = avertices[i];
				colors[i] = acolors[i];
			}
			stx_memcpy(&indices[0],&aindices[0],indices.size()*sizeof(__WORD__));
		
			defineGeometry();
		}
	}
	
	/**
	 * Sets the solid data. Defines the same color to all the vertices. An exception may 
	 * may occur in the case of abnormal arrays (e.g., indices making references to  
	 * inexistent vertices...)
	 * 
	 * @param vertices array of points defining the solid vertices
	 * @param indices array of indices for a array of vertices
	 * @param color the color of the vertices (the solid color) 
	 */
	void setData(std::vector<Point3d> avertices, std::vector<__WORD__> aindices, Color3f acolor)
	{
		std::vector<Color3f> colors(avertices.size());
		//Arrays.fill(colors, acolor);
		for(unsigned int i=0;i<avertices.size();i++)
			colors[i]=acolor;
			
		setData(avertices, aindices, colors);
	}
	
	//-------------------------GEOMETRICAL_TRANSFORMATIONS-------------------------//
	
	/**
	 * Applies a translation into a solid
	 * 
	 * @param dx translation on the x axis
	 * @param dy translation on the y axis
	 */
	void translate(__DOUBLE__ dx, __DOUBLE__ dy)
	{
		if(dx>TOL||dy>TOL)
		{
			for(int i=0;i<vertices.size();i++)
			{
				vertices[i].x += dx;
				vertices[i].y += dy;
			}
			
			defineGeometry();
		}
	}
	
	/**
	 * Applies a rotation into a solid
	 * 
	 * @param dx rotation on the x axis
	 * @param dy rotation on the y axis
	 */
	void rotate(__DOUBLE__ dx, __DOUBLE__ dy)
	{
		__DOUBLE__ cosX = cosf(dx);
		__DOUBLE__ cosY = cosf(dy);
		__DOUBLE__ sinX = sinf(dx);
		__DOUBLE__ sinY = sinf(dy);
					
		if(dx>TOL||dy>TOL)
		{
			//get mean
			Point3d mean = getMean();
			
			__DOUBLE__ newX, newY, newZ;
			for(int i=0;i<vertices.size();i++)
			{
				vertices[i].x -= mean.x; 
				vertices[i].y -= mean.y; 
				vertices[i].z -= mean.z; 
				
				//x rotation
				if(dx>TOL)
				{
					newY = vertices[i].y*cosX - vertices[i].z*sinX;
					newZ = vertices[i].y*sinX + vertices[i].z*cosX;
					vertices[i].y = newY;
					vertices[i].z = newZ;
				}
				
				//y rotation
				if(dy>TOL)
				{
					newX = vertices[i].x*cosY + vertices[i].z*sinY;
					newZ = -vertices[i].x*sinY + vertices[i].z*cosY;
					vertices[i].x = newX;
					vertices[i].z = newZ;
				}
										
				vertices[i].x += mean.x; 
				vertices[i].y += mean.y; 
				vertices[i].z += mean.z;
			}
		}
		
		defineGeometry();
	}
	
	/**
	 * Applies a zoom into a solid
	 * 
	 * @param dz translation on the z axis
	 */
	void zoom(__DOUBLE__ dz)
	{
		if(dz>TOL)
		{
			for(int i=0;i<vertices.size();i++)
			{
				vertices[i].z += dz;
			}
	
			defineGeometry();
		}
	}
	
	/**
	 * Applies a scale changing into the solid
	 * 
	 * @param dx scale changing for the x axis 
	 * @param dy scale changing for the y axis
	 * @param dz scale changing for the z axis
	 */
	void scale(__DOUBLE__ dx, __DOUBLE__ dy, __DOUBLE__ dz)
	{
		for(int i=0;i<vertices.size();i++)
		{
			vertices[i].x*=dx;
			vertices[i].y*=dy;
			vertices[i].z*=dz;
		}
		
		defineGeometry();
	}
	
	//-----------------------------------PRIVATES--------------------------------//
			
	/** Creates a geometry based on the indexes and vertices set for the solid */
	void defineGeometry()
	{
		#if 0
		GeometryInfo gi = GeometryInfo(GeometryInfo.TRIANGLE_ARRAY);
		gi.setCoordinateIndices(indices);
		gi.setCoordinates(vertices);
		NormalGenerator ng = NormalGenerator();
		ng.generateNormals(gi);
		
		gi.setColors(colors);
		gi.setColorIndices(indices);
		gi.recomputeIndices();
		
		setGeometry(gi.getIndexedGeometryArray());
		#else
			////LOG_FNLN;
			////LOG_PRINT("IMPLEMENT:\n");
		#endif
	}
	
	/**
	 * Loads a coordinates file, setting vertices and indices 
	 * 
	 * @param solidFile file used to create the solid
	 * @param color solid color
	 */
	void loadCoordinateFile(FILE solidFile, Color3f color)
	{
		#if 0
		//try
		{
			BufferedReader reader = BufferedReader(new FileReader(solidFile));
			
			std::string line = reader.readLine();
			int numVertices = Integer.parseInt(line);
			vertices = Point3d[numVertices];
									
			StringTokenizer tokens;
			std::string token;
						
			for(int i=0;i<numVertices;i++)
			{
				line = reader.readLine();
				tokens = StringTokenizer(line);
				tokens.nextToken();
				vertices[i]= *new Point3d(Double.parseDouble(tokens.nextToken()), Double.parseDouble(tokens.nextToken()), Double.parseDouble(tokens.nextToken()));
			}
			
			reader.readLine();
			
			line = reader.readLine();
			int numTriangles = Integer.parseInt(line);
			indices = int[numTriangles*3];
						
			for(int i=0,j=0;i<numTriangles*3;i=i+3,j++)
			{
				line = reader.readLine();
				tokens = StringTokenizer(line);
				tokens.nextToken();
				indices[i] = Integer.parseInt(tokens.nextToken());
				indices[i+1] = Integer.parseInt(tokens.nextToken());
				indices[i+2] = Integer.parseInt(tokens.nextToken());
			}
			
			colors = Color3f[vertices.length];
			Arrays.fill(colors, color);
			
			defineGeometry();
		}
		/*	
		catch(IOException e)
		{
			////LOG_PRINT("invalid file!");
			e.printStackTrace();
		}*/
		#endif
	}
	
	/**
	 * Gets the solid mean
	 * 
	 * @return point representing the mean
	 */
	Point3d getMean()
	{
		Point3d mean = Point3d();
		for(int i=0;i<vertices.size();i++)
		{
			mean.x += vertices[i].x;
			mean.y += vertices[i].y;
			mean.z += vertices[i].z;
		}
		mean.x /= vertices.size();
		mean.y /= vertices.size();
		mean.z /= vertices.size();
		
		return mean;
	}
};
#endif

