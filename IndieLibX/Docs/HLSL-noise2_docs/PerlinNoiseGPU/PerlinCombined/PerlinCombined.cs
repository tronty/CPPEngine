using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Audio;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.GamerServices;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using Microsoft.Xna.Framework.Media;
using Microsoft.Xna.Framework.Net;
using Microsoft.Xna.Framework.Storage;
using Microsoft.Xna.Framework.Graphics.PackedVector; //for NormalizedByte4

namespace PerlinCombined
{
    /// <summary>
    /// This is the main type for your game
    /// </summary>
    public class PerlinCombined : Microsoft.Xna.Framework.Game
    {
        GraphicsDeviceManager graphics;
        SpriteBatch spriteBatch;
        KeyboardState prevKbState = Keyboard.GetState();

        Effect perlinCombined;
        MeshRenderer2 sphere;
        Texture2D permTexture;
        Texture2D permTexture2d;
        Texture2D permGradTexture;
        Texture2D gradTexture4d;

        Matrix world;
        Matrix view;
        Matrix projection;

        float rotationY;
        float rotationX;

        // Procedural texture generation gradient color scales
        Texture2D sandcolorscale;
        Texture2D grasscolorscale;
        Texture2D rockcolorscale;
        Texture2D snowcolorscale;

        bool enableLighting = true;

        #region Arrays used for texture generation
        // gradients for 3d noise
        static float[,] g3 =  
        {
            {1,1,0},
            {-1,1,0},
            {1,-1,0},
            {-1,-1,0},
            {1,0,1},
            {-1,0,1},
            {1,0,-1},
            {-1,0,-1}, 
            {0,1,1},
            {0,-1,1},
            {0,1,-1},
            {0,-1,-1},
            {1,1,0},
            {0,-1,1},
            {-1,1,0},
            {0,-1,-1}
        };

        // gradients for 4D noise
        static float[,] g4 = 
        {
	        {0, -1, -1, -1},
	        {0, -1, -1, 1},
	        {0, -1, 1, -1},
	        {0, -1, 1, 1},
	        {0, 1, -1, -1},
	        {0, 1, -1, 1},
	        {0, 1, 1, -1},
	        {0, 1, 1, 1},
	        {-1, -1, 0, -1},
	        {-1, 1, 0, -1},
	        {1, -1, 0, -1},
	        {1, 1, 0, -1},
	        {-1, -1, 0, 1},
	        {-1, 1, 0, 1},
	        {1, -1, 0, 1},
	        {1, 1, 0, 1},
        	
	        {-1, 0, -1, -1},
	        {1, 0, -1, -1},
	        {-1, 0, -1, 1},
	        {1, 0, -1, 1},
	        {-1, 0, 1, -1},
	        {1, 0, 1, -1},
	        {-1, 0, 1, 1},
	        {1, 0, 1, 1},
	        {0, -1, -1, 0},
	        {0, -1, -1, 0},
	        {0, -1, 1, 0},
	        {0, -1, 1, 0},
	        {0, 1, -1, 0},
	        {0, 1, -1, 0},
	        {0, 1, 1, 0},
	        {0, 1, 1, 0}
        };

        static int[] perm = { 151, 160, 137, 91, 90, 15, 131, 13, 201, 95,
			    96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37,
			    240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62,
			    94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56,
			    87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139,
			    48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133,
			    230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25,
			    63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200,
			    196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3,
			    64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255,
			    82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
			    223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153,
			    101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79,
			    113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228, 251, 34, 242,
			    193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249,
			    14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204,
			    176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222,
			    114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180 };
        #endregion

        #region Texture generation methods
        private void GeneratePermTexture()
        {
            permTexture = new Texture2D(GraphicsDevice, 256, 1, 1, TextureUsage.None, SurfaceFormat.Luminance8);
            byte[] data = new byte[256 * 1];
            for (int x = 0; x < 256; x++)
            {
                for (int y = 0; y < 1; y++)
                {
                    data[x + (y * 256)] = (byte)(perm[x]);
                }
            }
            permTexture.SetData<byte>(data);
        }

        int perm2d(int i)
        {
            return perm[i % 256];
        }

        private void GeneratePermTexture2d()
        {
            permTexture2d = new Texture2D(GraphicsDevice, 256, 256, 1, TextureUsage.None, SurfaceFormat.Color);
            Color[] data = new Color[256 * 256];
            for (int x = 0; x < 256; x++)
            {
                for (int y = 0; y < 256; y++)
                {
                    int A = perm2d(x) + y;
                    int AA = perm2d(A);
                    int AB = perm2d(A + 1);
                    int B = perm2d(x + 1) + y;
                    int BA = perm2d(B);
                    int BB = perm2d(B + 1);
                    data[x + (y * 256)] = new Color((byte)(AA), (byte)(AB),
                                                    (byte)(BA), (byte)(BB));
                }
            }
            permTexture2d.SetData<Color>(data);
        }

        private void GeneratePermGradTexture()
        {
            permGradTexture = new Texture2D(GraphicsDevice, 256, 1, 1, TextureUsage.None, SurfaceFormat.NormalizedByte4);
            NormalizedByte4[] data = new NormalizedByte4[256 * 1];
            for (int x = 0; x < 256; x++)
            {
                for (int y = 0; y < 1; y++)
                {
                    data[x + (y * 256)] = new NormalizedByte4(g3[perm[x] % 16, 0], g3[perm[x] % 16, 1], g3[perm[x] % 16, 2], 1);
                }
            }
            permGradTexture.SetData<NormalizedByte4>(data);
        }

        private void GenerateGradTexture4d()
        {
            gradTexture4d = new Texture2D(GraphicsDevice, 32, 1, 1, TextureUsage.None, SurfaceFormat.NormalizedByte4);
            NormalizedByte4[] data = new NormalizedByte4[32 * 1];
            for (int x = 0; x < 32; x++)
            {
                for (int y = 0; y < 1; y++)
                {
                    data[x + (y * 32)] = new NormalizedByte4(g4[x, 0], g4[x, 1], g4[x, 2], g4[x, 3]);
                }
            }
            gradTexture4d.SetData<NormalizedByte4>(data);
        }

        /// <summary>
        /// Generates all of the needed textures on the CPU
        /// </summary>
        private void GenerateTextures()
        {
            GeneratePermTexture();
            GeneratePermTexture2d();
            GeneratePermGradTexture();
            GenerateGradTexture4d();
        }
        #endregion

        public PerlinCombined()
        {
            graphics = new GraphicsDeviceManager(this);
            Content.RootDirectory = "Content";
        }

        /// <summary>
        /// Allows the game to perform any initialization it needs to before starting to run.
        /// This is where it can query for any required services and load any non-graphic
        /// related content.  Calling base.Initialize will enumerate through any components
        /// and initialize them as well.
        /// </summary>
        protected override void Initialize()
        {
            base.Initialize();
        }

        /// <summary>
        /// LoadContent will be called once per game and is the place to load
        /// all of your content.
        /// </summary>
        protected override void LoadContent()
        {
            // Create a new SpriteBatch, which can be used to draw textures.
            spriteBatch = new SpriteBatch(GraphicsDevice);

            // create the WVP matrices
            world = Matrix.Identity;
            view = Matrix.CreateLookAt(new Vector3(0, 0, 20), Vector3.Zero, Vector3.Up);
            projection = Matrix.CreatePerspectiveFieldOfView(MathHelper.PiOver2, (float)GraphicsDevice.Viewport.Width / (float)GraphicsDevice.Viewport.Height, 1, 100);

            // create the sphere mesh
            sphere = MeshRenderer2.CreateSphere(GraphicsDevice, 10, 32, 32);
            perlinCombined = Content.Load<Effect>("PerlinCombined");
            GenerateTextures();

            // load the color scales
            sandcolorscale = Content.Load<Texture2D>("sandcolorscale");
            grasscolorscale = Content.Load<Texture2D>("grasscolorscale");
            rockcolorscale = Content.Load<Texture2D>("rockscolorscale");
            snowcolorscale = Content.Load<Texture2D>("snowcolorscale");
        }

        /// <summary>
        /// UnloadContent will be called once per game and is the place to unload
        /// all content.
        /// </summary>
        protected override void UnloadContent()
        {
        }

        /// <summary>
        /// Allows the game to run logic such as updating the world,
        /// checking for collisions, gathering input, and playing audio.
        /// </summary>
        /// <param name="gameTime">Provides a snapshot of timing values.</param>
        protected override void Update(GameTime gameTime)
        {
            // Allows the app to exit
            if (GamePad.GetState(PlayerIndex.One).Buttons.Back == ButtonState.Pressed)
                this.Exit();

            KeyboardState kbState = Keyboard.GetState();

            // Allows the app to exit
            if (kbState.IsKeyDown(Keys.Escape))
                this.Exit();

            // Use the keyboard to change the rotation of the mesh
            if (kbState.IsKeyDown(Keys.Left))
                rotationY += 0.01f;
            if (kbState.IsKeyDown(Keys.Right))
                rotationY -= 0.01f;

            if (kbState.IsKeyDown(Keys.Up))
                rotationX += 0.01f;
            if (kbState.IsKeyDown(Keys.Down))
                rotationX -= 0.01f;

            // update the World matix of the mesh with the rotatations
            world = Matrix.CreateRotationX(rotationX) * Matrix.CreateRotationY(rotationY);

            // allow toggling of wireframe rendering
            if (kbState.IsKeyDown(Keys.F1))
            {
                GraphicsDevice.RenderState.FillMode = FillMode.WireFrame;
            }
            if (kbState.IsKeyDown(Keys.F2))
            {
                GraphicsDevice.RenderState.FillMode = FillMode.Solid;
            }

            // allow toggling of the lighting
            if (kbState.IsKeyDown(Keys.F3) && prevKbState.IsKeyUp(Keys.F3))
            {
                enableLighting = !enableLighting;
                perlinCombined.Parameters["enableLighting"].SetValue(enableLighting);
            }

            prevKbState = kbState;

            base.Update(gameTime);
        }

        /// <summary>
        /// This is called when the game should draw itself.
        /// </summary>
        /// <param name="gameTime">Provides a snapshot of timing values.</param>
        protected override void Draw(GameTime gameTime)
        {
            GraphicsDevice.Clear(Color.CornflowerBlue);

            // Set the WVP matrices in the shader
            perlinCombined.Parameters["World"].SetValue(world);
            perlinCombined.Parameters["View"].SetValue(view);
            perlinCombined.Parameters["Projection"].SetValue(projection);

            // Set the perlin noise permutation and gradient textures
            perlinCombined.Parameters["permTexture"].SetValue(permTexture);
            perlinCombined.Parameters["permTexture2d"].SetValue(permTexture2d);
            perlinCombined.Parameters["permGradTexture"].SetValue(permGradTexture);
            perlinCombined.Parameters["gradTexture4d"].SetValue(gradTexture4d);

            // Set the color scale gradients in the shader
            perlinCombined.Parameters["sandscale"].SetValue(sandcolorscale);
            perlinCombined.Parameters["grassscale"].SetValue(grasscolorscale);
            perlinCombined.Parameters["rockscale"].SetValue(rockcolorscale);
            perlinCombined.Parameters["snowscale"].SetValue(snowcolorscale);

            // Use a combination of Perlin Noise in the Vertex Shader and Pixel Shader
            perlinCombined.CurrentTechnique = perlinCombined.Techniques["Combined"];

            perlinCombined.Begin();
            foreach (EffectPass pass in perlinCombined.CurrentTechnique.Passes)
            {
                pass.Begin();
                // draw the sphere using the combined Perlin Noise shader
                sphere.Draw(GraphicsDevice);
                pass.End();
            }
            perlinCombined.End();

            base.Draw(gameTime);
        }
    }
}
