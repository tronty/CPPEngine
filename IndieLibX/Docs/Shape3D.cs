using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace PerlinCombined
{
    /// <summary>
    /// Defines a custom vertex format structure that contains position and normal data.
    /// </summary>
    struct VertexPositionNormal
    {
        /// <summary>
        /// The vertex position.
        /// </summary>
        public Vector3 Position;

        /// <summary>
        /// The vertex normal.
        /// </summary>
        public Vector3 Normal;

        /// <summary>
        /// Initializes a new instance of the VertexPositionNormal class.
        /// </summary>
        /// <param name="position">The position of the vertex.</param>
        /// <param name="normal">The vertex normal.</param>
        public VertexPositionNormal(Vector3 position, Vector3 normal)
        {
            Position = position;
            Normal = normal;
        }

        /// <summary>
        /// An array of two vertex elements describing the position and normal of this vertex .
        /// </summary>
        public static VertexElement[] VertexElements =
         {
             new VertexElement(0, 0, VertexElementFormat.Vector3, VertexElementMethod.Default, VertexElementUsage.Position, 0),
             new VertexElement(0, sizeof(float)*3, VertexElementFormat.Vector3, VertexElementMethod.Default, VertexElementUsage.Normal, 0),
         };

        /// <summary>
        /// Gets the size of the VertexPositionNormal class.
        /// </summary>
        public static int SizeInBytes = sizeof(float) * 3 * 2;
    }

    /// <summary>
    /// Defines a simple 3D shape, such as a box, sphere, or cylinder.
    /// </summary>
    public class Shape3D
    {
        #region Properties
        /// <summary>
        /// The number of vertices making up the shape.
        /// </summary>
        public int VertexCount { get; private set; }

        /// <summary>
        /// The number of triangle making up the shape.
        /// </summary>
        public int TriangleCount { get; private set; }

        /// <summary>
        /// The buffer of vertices making up the shape.
        /// </summary>
        public VertexBuffer VertexBuffer { get; private set; }

        /// <summary>
        /// The buffer of indices making up the shape, if it has one.
        /// </summary>
        public IndexBuffer IndexBuffer { get; private set; }

        /// <summary>
        /// The declaration of the type of vertices making up the shape.
        /// </summary>
        public VertexDeclaration VertexDeclaration { get; private set; }

        /// <summary>
        /// The byte size of each vertex making up the shape.
        /// </summary>
        public int VertexSizeInBytes { get; private set; }
        #endregion

        /// <summary>
        /// Initializes a new instance of a <see cref="Shape3D"/>.
        /// Private so that a shape can only be instantiated from the Create methods.
        /// </summary>
        private Shape3D()
        {
            VertexCount = 0;
            TriangleCount = 0;
            VertexSizeInBytes = 0;
            VertexBuffer = null;
            IndexBuffer = null;
            VertexDeclaration = null;
        }

        /// <summary>
        /// Draws the shape.
        /// </summary>
        /// <param name="device">The device to use to draw the shape.</param>
        public void Draw(GraphicsDevice device)
        {
            device.VertexDeclaration = VertexDeclaration;
            device.Vertices[0].SetSource(VertexBuffer, 0, VertexSizeInBytes);
            device.Indices = IndexBuffer;

            if (IndexBuffer != null)
                device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 0, VertexCount, 0, TriangleCount);
            else
                device.DrawPrimitives(PrimitiveType.TriangleList, 0, TriangleCount);
        }

        #region Static Methods
        /// <summary>
        /// Creates a <see cref="VertexBuffer"/> filled with <see cref="VertexPositionNormalTexture"/> vertices forming a box.
        /// </summary>
        /// <remarks>Always draw using a non-indexed TriangleList of 12 primitives.</remarks>
        /// <param name="device">The <see cref="GraphicsDevice"/> that is associated with the created box.</param>
        /// <param name="width">Width of the box, along the x-axis.</param>
        /// <param name="height">Height of the box, along the y-axis.</param>
        /// <param name="depth">Depth of the box, along the z-axis.</param>
        public static Shape3D CreateTexturedBox(GraphicsDevice device, float width, float height, float depth)
        {
            Shape3D shape = new Shape3D();

            // Because the box is centered at the origin, need to divide by two to find the + and - offsets
            width = width / 2.0f;
            height = height / 2.0f;
            depth = depth / 2.0f;

            VertexPositionNormalTexture[] cubeVertices = new VertexPositionNormalTexture[36];

            Vector3 topLeftFront = new Vector3(-width, height, depth);
            Vector3 bottomLeftFront = new Vector3(-width, -height, depth);
            Vector3 topRightFront = new Vector3(width, height, depth);
            Vector3 bottomRightFront = new Vector3(width, -height, depth);
            Vector3 topLeftBack = new Vector3(-width, height, -depth);
            Vector3 topRightBack = new Vector3(width, height, -depth);
            Vector3 bottomLeftBack = new Vector3(-width, -height, -depth);
            Vector3 bottomRightBack = new Vector3(width, -height, -depth);

            Vector2 textureTopLeft = new Vector2(0.0f, 0.0f);
            Vector2 textureTopRight = new Vector2(1.0f, 0.0f);
            Vector2 textureBottomLeft = new Vector2(0.0f, 1.0f);
            Vector2 textureBottomRight = new Vector2(1.0f, 1.0f);

            Vector3 frontNormal = new Vector3(0.0f, 0.0f, 1.0f);
            Vector3 backNormal = new Vector3(0.0f, 0.0f, -1.0f);
            Vector3 topNormal = new Vector3(0.0f, 1.0f, 0.0f);
            Vector3 bottomNormal = new Vector3(0.0f, -1.0f, 0.0f);
            Vector3 leftNormal = new Vector3(-1.0f, 0.0f, 0.0f);
            Vector3 rightNormal = new Vector3(1.0f, 0.0f, 0.0f);

            // Front face.
            cubeVertices[1] = new VertexPositionNormalTexture(topLeftFront, frontNormal, textureTopLeft);
            cubeVertices[0] = new VertexPositionNormalTexture(bottomLeftFront, frontNormal, textureBottomLeft);
            cubeVertices[2] = new VertexPositionNormalTexture(topRightFront, frontNormal, textureTopRight);
            cubeVertices[4] = new VertexPositionNormalTexture(bottomLeftFront, frontNormal, textureBottomLeft);
            cubeVertices[3] = new VertexPositionNormalTexture(bottomRightFront, frontNormal, textureBottomRight);
            cubeVertices[5] = new VertexPositionNormalTexture(topRightFront, frontNormal, textureTopRight);

            // Back face.
            cubeVertices[7] = new VertexPositionNormalTexture(topLeftBack, backNormal, textureTopRight);
            cubeVertices[6] = new VertexPositionNormalTexture(topRightBack, backNormal, textureTopLeft);
            cubeVertices[8] = new VertexPositionNormalTexture(bottomLeftBack, backNormal, textureBottomRight);
            cubeVertices[10] = new VertexPositionNormalTexture(bottomLeftBack, backNormal, textureBottomRight);
            cubeVertices[9] = new VertexPositionNormalTexture(topRightBack, backNormal, textureTopLeft);
            cubeVertices[11] = new VertexPositionNormalTexture(bottomRightBack, backNormal, textureBottomLeft);

            // Top face.
            cubeVertices[13] = new VertexPositionNormalTexture(topLeftFront, topNormal, textureBottomLeft);
            cubeVertices[12] = new VertexPositionNormalTexture(topRightBack, topNormal, textureTopRight);
            cubeVertices[14] = new VertexPositionNormalTexture(topLeftBack, topNormal, textureTopLeft);
            cubeVertices[16] = new VertexPositionNormalTexture(topLeftFront, topNormal, textureBottomLeft);
            cubeVertices[15] = new VertexPositionNormalTexture(topRightFront, topNormal, textureBottomRight);
            cubeVertices[17] = new VertexPositionNormalTexture(topRightBack, topNormal, textureTopRight);

            // Bottom face. 
            cubeVertices[19] = new VertexPositionNormalTexture(bottomLeftFront, bottomNormal, textureTopLeft);
            cubeVertices[18] = new VertexPositionNormalTexture(bottomLeftBack, bottomNormal, textureBottomLeft);
            cubeVertices[20] = new VertexPositionNormalTexture(bottomRightBack, bottomNormal, textureBottomRight);
            cubeVertices[22] = new VertexPositionNormalTexture(bottomLeftFront, bottomNormal, textureTopLeft);
            cubeVertices[21] = new VertexPositionNormalTexture(bottomRightBack, bottomNormal, textureBottomRight);
            cubeVertices[23] = new VertexPositionNormalTexture(bottomRightFront, bottomNormal, textureTopRight);

            // Left face.
            cubeVertices[25] = new VertexPositionNormalTexture(topLeftFront, leftNormal, textureTopRight);
            cubeVertices[24] = new VertexPositionNormalTexture(bottomLeftBack, leftNormal, textureBottomLeft);
            cubeVertices[26] = new VertexPositionNormalTexture(bottomLeftFront, leftNormal, textureBottomRight);
            cubeVertices[28] = new VertexPositionNormalTexture(topLeftBack, leftNormal, textureTopLeft);
            cubeVertices[27] = new VertexPositionNormalTexture(bottomLeftBack, leftNormal, textureBottomLeft);
            cubeVertices[29] = new VertexPositionNormalTexture(topLeftFront, leftNormal, textureTopRight);

            // Right face. 
            cubeVertices[31] = new VertexPositionNormalTexture(topRightFront, rightNormal, textureTopLeft);
            cubeVertices[30] = new VertexPositionNormalTexture(bottomRightFront, rightNormal, textureBottomLeft);
            cubeVertices[32] = new VertexPositionNormalTexture(bottomRightBack, rightNormal, textureBottomRight);
            cubeVertices[34] = new VertexPositionNormalTexture(topRightBack, rightNormal, textureTopRight);
            cubeVertices[33] = new VertexPositionNormalTexture(topRightFront, rightNormal, textureTopLeft);
            cubeVertices[35] = new VertexPositionNormalTexture(bottomRightBack, rightNormal, textureBottomRight);

            // Create the actual vertex buffer
            shape.VertexBuffer = new VertexBuffer(device, VertexPositionNormalTexture.SizeInBytes * cubeVertices.Length, BufferUsage.None);
            shape.VertexBuffer.SetData<VertexPositionNormalTexture>(cubeVertices);

            shape.VertexCount = 36;
            shape.TriangleCount = 12;
            shape.VertexDeclaration = new VertexDeclaration(device, VertexPositionNormalTexture.VertexElements);
            shape.VertexSizeInBytes = VertexPositionNormalTexture.SizeInBytes;

            return shape;
        }

        /// <summary>
        /// Creates a <see cref="VertexBuffer"/> filled with <see cref="VertexPositionNormal"/> vertices forming a box.
        /// </summary>
        /// <remarks>Always draw using a non-indexed TriangleList of 12 primitives.</remarks>
        /// <param name="device">The <see cref="GraphicsDevice"/> that is associated with the created box.</param>
        /// <param name="width">Width of the box, along the x-axis.</param>
        /// <param name="height">Height of the box, along the y-axis.</param>
        /// <param name="depth">Depth of the box, along the z-axis.</param>
        public static Shape3D CreateBox(GraphicsDevice device, float width, float height, float depth)
        {
            Shape3D shape = new Shape3D();

            // Because the box is centered at the origin, need to divide by two to find the + and - offsets
            width = width / 2.0f;
            height = height / 2.0f;
            depth = depth / 2.0f;

            VertexPositionNormal[] cubeVertices = new VertexPositionNormal[36];

            Vector3 topLeftFront = new Vector3(-width, height, depth);
            Vector3 bottomLeftFront = new Vector3(-width, -height, depth);
            Vector3 topRightFront = new Vector3(width, height, depth);
            Vector3 bottomRightFront = new Vector3(width, -height, depth);
            Vector3 topLeftBack = new Vector3(-width, height, -depth);
            Vector3 topRightBack = new Vector3(width, height, -depth);
            Vector3 bottomLeftBack = new Vector3(-width, -height, -depth);
            Vector3 bottomRightBack = new Vector3(width, -height, -depth);

            Vector3 frontNormal = new Vector3(0.0f, 0.0f, 1.0f);
            Vector3 backNormal = new Vector3(0.0f, 0.0f, -1.0f);
            Vector3 topNormal = new Vector3(0.0f, 1.0f, 0.0f);
            Vector3 bottomNormal = new Vector3(0.0f, -1.0f, 0.0f);
            Vector3 leftNormal = new Vector3(-1.0f, 0.0f, 0.0f);
            Vector3 rightNormal = new Vector3(1.0f, 0.0f, 0.0f);

            // Front face.
            cubeVertices[1] = new VertexPositionNormal(topLeftFront, frontNormal);
            cubeVertices[0] = new VertexPositionNormal(bottomLeftFront, frontNormal);
            cubeVertices[2] = new VertexPositionNormal(topRightFront, frontNormal);
            cubeVertices[4] = new VertexPositionNormal(bottomLeftFront, frontNormal);
            cubeVertices[3] = new VertexPositionNormal(bottomRightFront, frontNormal);
            cubeVertices[5] = new VertexPositionNormal(topRightFront, frontNormal);

            // Back face.
            cubeVertices[7] = new VertexPositionNormal(topLeftBack, backNormal);
            cubeVertices[6] = new VertexPositionNormal(topRightBack, backNormal);
            cubeVertices[8] = new VertexPositionNormal(bottomLeftBack, backNormal);
            cubeVertices[10] = new VertexPositionNormal(bottomLeftBack, backNormal);
            cubeVertices[9] = new VertexPositionNormal(topRightBack, backNormal);
            cubeVertices[11] = new VertexPositionNormal(bottomRightBack, backNormal);

            // Top face.
            cubeVertices[13] = new VertexPositionNormal(topLeftFront, topNormal);
            cubeVertices[12] = new VertexPositionNormal(topRightBack, topNormal);
            cubeVertices[14] = new VertexPositionNormal(topLeftBack, topNormal);
            cubeVertices[16] = new VertexPositionNormal(topLeftFront, topNormal);
            cubeVertices[15] = new VertexPositionNormal(topRightFront, topNormal);
            cubeVertices[17] = new VertexPositionNormal(topRightBack, topNormal);

            // Bottom face. 
            cubeVertices[19] = new VertexPositionNormal(bottomLeftFront, bottomNormal);
            cubeVertices[18] = new VertexPositionNormal(bottomLeftBack, bottomNormal);
            cubeVertices[20] = new VertexPositionNormal(bottomRightBack, bottomNormal);
            cubeVertices[22] = new VertexPositionNormal(bottomLeftFront, bottomNormal);
            cubeVertices[21] = new VertexPositionNormal(bottomRightBack, bottomNormal);
            cubeVertices[23] = new VertexPositionNormal(bottomRightFront, bottomNormal);

            // Left face.
            cubeVertices[25] = new VertexPositionNormal(topLeftFront, leftNormal);
            cubeVertices[24] = new VertexPositionNormal(bottomLeftBack, leftNormal);
            cubeVertices[26] = new VertexPositionNormal(bottomLeftFront, leftNormal);
            cubeVertices[28] = new VertexPositionNormal(topLeftBack, leftNormal);
            cubeVertices[27] = new VertexPositionNormal(bottomLeftBack, leftNormal);
            cubeVertices[29] = new VertexPositionNormal(topLeftFront, leftNormal);

            // Right face. 
            cubeVertices[31] = new VertexPositionNormal(topRightFront, rightNormal);
            cubeVertices[30] = new VertexPositionNormal(bottomRightFront, rightNormal);
            cubeVertices[32] = new VertexPositionNormal(bottomRightBack, rightNormal);
            cubeVertices[34] = new VertexPositionNormal(topRightBack, rightNormal);
            cubeVertices[33] = new VertexPositionNormal(topRightFront, rightNormal);
            cubeVertices[35] = new VertexPositionNormal(bottomRightBack, rightNormal);

            // Create the actual vertex buffer
            shape.VertexBuffer = new VertexBuffer(device, VertexPositionNormal.SizeInBytes * cubeVertices.Length, BufferUsage.None);
            shape.VertexBuffer.SetData<VertexPositionNormal>(cubeVertices);

            shape.VertexCount = 36;
            shape.TriangleCount = 12;
            shape.VertexDeclaration = new VertexDeclaration(device, VertexPositionNormal.VertexElements);
            shape.VertexSizeInBytes = VertexPositionNormal.SizeInBytes;

            return shape;
        }

        /// <summary>
        /// Creates a <see cref="VertexBuffer"/> filled with <see cref="VertexPositionNormal"/> vertices forming a sphere.
        /// </summary>
        /// <remarks>
        /// Draw with an INDEXED TriangleList.  The values to use are as follows:
        /// Vertex Count    = slices * (stacks - 1) + 2
        /// Primitive Count = slices * (stacks - 1) * 2
        /// </remarks>
        /// <param name="device">The <see cref="GraphicsDevice"/> that is associated with the created sphere.</param>
        /// <param name="radius">Radius of the sphere. This value should be greater than or equal to 0.0f.</param>
        /// <param name="slices">Number of slices about the Y axis.</param>
        /// <param name="stacks">Number of stacks along the Y axis. Should be 2 or greater. (stack of 1 is just a cylinder)</param>
        public static Shape3D CreateSphere(GraphicsDevice device, float radius, int slices, int stacks)
        {
            Shape3D shape = new Shape3D();
            float sliceStep = MathHelper.TwoPi / slices;
            float stackStep = MathHelper.Pi / stacks;
            int vertexCount = slices * (stacks - 1) + 2;
            int triangleCount = slices * (stacks - 1) * 2;
            int indexCount = triangleCount * 3;

            VertexPositionNormal[] sphereVertices = new VertexPositionNormal[vertexCount];

            int currentVertex = 0;
            sphereVertices[currentVertex++] = new VertexPositionNormal(new Vector3(0, -radius, 0), Vector3.Down);
            float stackAngle = MathHelper.Pi - stackStep;
            for (int i = 0; i < stacks - 1; i++)
            {
                float sliceAngle = 0;
                for (int j = 0; j < slices; j++)
                {
                    //NOTE: y and z were switched from normal spherical coordinates because the sphere is "oriented" along the Y axis as opposed to the Z axis
                    float x = (float)(radius * Math.Sin(stackAngle) * Math.Cos(sliceAngle));
                    float y = (float)(radius * Math.Cos(stackAngle));
                    float z = (float)(radius * Math.Sin(stackAngle) * Math.Sin(sliceAngle));

                    Vector3 position = new Vector3(x, y, z);
                    sphereVertices[currentVertex++] = new VertexPositionNormal(position, Vector3.Normalize(position));

                    sliceAngle += sliceStep;
                }
                stackAngle -= stackStep;
            }
            sphereVertices[currentVertex++] = new VertexPositionNormal(new Vector3(0, radius, 0), Vector3.Up);

            // Create the actual vertex buffer
            shape.VertexBuffer = new VertexBuffer(device, VertexPositionNormal.SizeInBytes * sphereVertices.Length, BufferUsage.None);
            shape.VertexBuffer.SetData<VertexPositionNormal>(sphereVertices);

            shape.IndexBuffer = CreateIndexBuffer(device, vertexCount, indexCount, slices);

            shape.VertexCount = vertexCount;
            shape.TriangleCount = triangleCount;
            shape.VertexDeclaration = new VertexDeclaration(device, VertexPositionNormal.VertexElements);
            shape.VertexSizeInBytes = VertexPositionNormal.SizeInBytes;

            return shape;
        }

        /// <summary>
        /// Creates a <see cref="VertexBuffer"/> filled with <see cref="VertexPositionNormal"/> vertices forming a cylinder.
        /// </summary>
        /// <remarks>
        /// Draw with an INDEXED TriangleList.  The values to use are as follows:
        /// Vertex Count    = slices * (stacks + 1) + 2
        /// Primitive Count = slices * (stacks + 1) * 2
        /// </remarks>
        /// <param name="device">The <see cref="GraphicsDevice"/> that is associated with the created cylinder.</param>
        /// <param name="bottomRadius">Radius at the negative Y end. Value should be greater than or equal to 0.0f.</param>
        /// <param name="topRadius">Radius at the positive Y end. Value should be greater than or equal to 0.0f.</param>
        /// <param name="length">Length of the cylinder along the Y-axis.</param>
        /// <param name="slices">Number of slices about the Y axis.</param>
        /// <param name="stacks">Number of stacks along the Y axis.</param>
        public static Shape3D CreateCylinder(GraphicsDevice device, float bottomRadius, float topRadius, float length, int slices, int stacks)
        {
            // if both the top and bottom have a radius of zero, just return null, because invalid
            if (bottomRadius <= 0 && topRadius <= 0)
            {
                return null;
            }

            Shape3D shape = new Shape3D();
            float sliceStep = MathHelper.TwoPi / slices;
            float heightStep = length / stacks;
            float radiusStep = (topRadius - bottomRadius) / stacks;
            float currentHeight = -length / 2;
            int vertexCount = (stacks + 1) * slices + 2;   //cone = stacks * slices + 1
            int triangleCount = (stacks + 1) * slices * 2; //cone = stacks * slices * 2 + slices
            int indexCount = triangleCount * 3;
            float currentRadius = bottomRadius;

            VertexPositionNormal[] vertices = new VertexPositionNormal[vertexCount];

            // Start at the bottom of the cylinder            
            int currentVertex = 0;
            vertices[currentVertex++] = new VertexPositionNormal(new Vector3(0, currentHeight, 0), Vector3.Down);
            for (int i = 0; i <= stacks; i++)
            {
                float sliceAngle = 0;
                for (int j = 0; j < slices; j++)
                {
                    float x = currentRadius * (float)Math.Cos(sliceAngle);
                    float y = currentHeight;
                    float z = currentRadius * (float)Math.Sin(sliceAngle);

                    Vector3 position = new Vector3(x, y, z);
                    vertices[currentVertex++] = new VertexPositionNormal(position, Vector3.Normalize(position));

                    sliceAngle += sliceStep;
                }
                currentHeight += heightStep;
                currentRadius += radiusStep;
            }
            vertices[currentVertex++] = new VertexPositionNormal(new Vector3(0, length / 2, 0), Vector3.Up);

            // Create the actual vertex buffer object
            shape.VertexBuffer = new VertexBuffer(device, VertexPositionNormal.SizeInBytes * vertices.Length, BufferUsage.None);
            shape.VertexBuffer.SetData<VertexPositionNormal>(vertices);

            shape.IndexBuffer = CreateIndexBuffer(device, vertexCount, indexCount, slices);

            shape.VertexCount = vertexCount;
            shape.TriangleCount = triangleCount;
            shape.VertexDeclaration = new VertexDeclaration(device, VertexPositionNormal.VertexElements);
            shape.VertexSizeInBytes = VertexPositionNormal.SizeInBytes;

            return shape;
        }

        /// <summary>
        /// Creates an <see cref="IndexBuffer"/> for spherical shapes like Spheres, Cylinders, and Cones.
        /// </summary>
        /// <param name="device">The <see cref="GraphicsDevice"/> that is associated with the created cylinder.</param>
        /// <param name="vertexCount">The total number of vertices making up the shape.</param>
        /// <param name="indexCount">The total number of indices making up the shape.</param>
        /// <param name="slices">The number of slices about the Y axis.</param>
        /// <returns>The index buffer containing the index data for the shape.</returns>
        private static IndexBuffer CreateIndexBuffer(GraphicsDevice device, int vertexCount, int indexCount, int slices)
        {
            uint[] indices = new uint[indexCount];
            int currentIndex = 0;

            // Bottom circle/cone of shape
            for (uint i = 1; i <= slices; i++)
            {
                indices[currentIndex++] = 0;
                indices[currentIndex++] = i;
                if (i - 1 == 0)
                    indices[currentIndex++] = i + (uint)slices - 1;
                else
                    indices[currentIndex++] = i - 1;
            }

            // Middle sides of shape
            for (uint i = 1; i < vertexCount - (uint)slices - 1; i++)
            {
                indices[currentIndex++] = i;
                indices[currentIndex++] = i + (uint)slices;
                if ((i - 1) % slices == 0)
                    indices[currentIndex++] = i + (uint)slices + (uint)slices - 1;
                else
                    indices[currentIndex++] = i + (uint)slices - 1;

                if ((i - 1) % slices == 0)
                    indices[currentIndex++] = i + (uint)slices - 1;
                else
                    indices[currentIndex++] = i - 1;
                indices[currentIndex++] = i;
                if ((i - 1) % slices == 0)
                    indices[currentIndex++] = i + (uint)slices + (uint)slices - 1;
                else
                    indices[currentIndex++] = i + (uint)slices - 1;
            }

            // Top circle/cone of shape
            for (uint i = (uint)vertexCount - (uint)slices - 1; i < vertexCount - 1; i++)
            {
                indices[currentIndex++] = (uint)vertexCount - 1;
                if ((i - 1) % slices == 0)
                    indices[currentIndex++] = i + (uint)slices - 1;
                else
                    indices[currentIndex++] = i - 1;
                indices[currentIndex++] = i;
            }

            // Create the actual index buffer
            IndexBuffer indexBuffer = new IndexBuffer(device, typeof(uint), indexCount, BufferUsage.None);
            indexBuffer.SetData<uint>(indices);

            return indexBuffer;
        }
        #endregion
    }
}
