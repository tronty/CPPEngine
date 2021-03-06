
//-----------------------------------------------------------------------------
// Game1.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
/*
  Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h> 
#include "AnimatedTexture.h"
    class Game1
    {
#if 0
        GraphicsDeviceManager graphics;
        SpriteBatch spriteBatch;

        Model Ring;
        float RingRotation;
        D3DXFROMWINEVECTOR3 RingPosition;

        SampleArcBallCamera Camera1;
#endif
        D3DXFROMWINEMATRIX projectionMatrix;

        AnimatedTexture explosion;
        D3DXFROMWINEVECTOR2 explosionpos;

        Game1()
        {
#if 0
            graphics = new GraphicsDeviceManager(this);
            Content.RootDirectory = "Content";
            Camera1 = new SampleArcBallCamera(SampleArcBallCameraMode.Free);
#endif
//#if ZUNE
            // Frame rate is 30 fps by default for Zune.
//            TargetElapsedTime = TimeSpan.FromSeconds(1 / 30.0);
//#endif
        }


	void Initialize()
        {
            // TODO: Add your initialization logic here
#if 0
            Camera1.Target = new D3DXFROMWINEVECTOR3(0, 0, 0);
            Camera1.Distance = 25f;
            Camera1.OrbitRight(MathHelper.PiOver4);
            Camera1.OrbitUp(MathHelper.PiOver4);
#endif
            projectionMatrix = D3DXFROMWINEMATRIX.CreatePerspectiveFieldOfView(
                MathHelper.PiOver4, 4.0f / 3.0f, 1.0f, 10000f);
#if 0
            RingPosition = D3DXFROMWINEVECTOR3.Zero;
            RingRotation = 0.0f;
#endif
            explosion = new AnimatedTexture(new D3DXFROMWINEVECTOR2(75), 0, 1.0f, 1.0f);

            //base.Initialize();
        }

        /// <summary>
        /// LoadContent will be called once per game and is the place to load
        /// all of your content.
        /// </summary>
	void LoadContent()
        {
#if 0
            // Create a new SpriteBatch, which can be used to draw textures.
            spriteBatch = new SpriteBatch(GraphicsDevice);

            Ring = Content.Load<Model>("redtorus");
#endif
            explosion.Load(10, 3);
            explosion.Pause();
            // Enable DepthBufferEnable
            //GraphicsDevice.DepthStencilState = DepthStencilState.Default;
        }


        /// <summary>
        /// Allows the game to run logic such as updating the world,
        /// checking for collisions, gathering input, and playing audio.
        /// </summary>
        /// <param name="gameTime">Provides a snapshot of timing values.</param>
	void Update(GameTime gameTime)
        {
#if 0
            // Allows the game to exit
            if (GamePad.GetState(PlayerIndex.One).Buttons.Back ==
                ButtonState.Pressed)
                this.Exit();

            if (Keyboard.GetState().IsKeyDown(Keys.Escape))
                this.Exit();


            GamePadState PlayerOne = GamePad.GetState(PlayerIndex.One);

            // Move the camera using thumbsticks
            MoveCamera(PlayerOne);
#endif
            // Start or stop the animated sprite using buttons
            if (STX_Service::GetInputInstance()->OnKeyPress (KEY_A))
                explosion.Play();
            if (STX_Service::GetInputInstance()->OnKeyPress (KEY_B))
                explosion.Stop();

            // Update the animated sprite
            explosion.UpdateFrame((float)timeGetTime()*1000000.0f);
#if 0
            // Create a total bounding sphere for the mesh
            BoundingSphere totalbounds = new BoundingSphere();
            foreach (ModelMesh mesh in Ring.Meshes)
            {
                totalbounds = BoundingSphere.CreateMerged(totalbounds,
                    mesh.BoundingSphere);
            }

            // Project the center of the 3D object to the screen, and center the
            // sprite there
            D3DXFROMWINEVECTOR3 center = GraphicsDevice.Viewport.Project(totalbounds.Center,
                projectionMatrix, Camera1.ViewMatrix, D3DXFROMWINEMATRIX.Identity);
            explosionpos.X = center.X;
            explosionpos.Y = center.Y;

            // Create a bounding box from the bounding sphere, 
            // and find the corner that is farthest away from 
            // the center using Project
            BoundingBox extents = BoundingBox.CreateFromSphere(totalbounds);
            float maxdistance = 0;
            float distance;
            D3DXFROMWINEVECTOR3 screencorner;
            foreach (D3DXFROMWINEVECTOR3 corner in extents.GetCorners())
            {
                screencorner = GraphicsDevice.Viewport.Project(corner,
                projectionMatrix, Camera1.ViewMatrix, D3DXFROMWINEMATRIX.Identity);
                distance = D3DXFROMWINEVECTOR3.Distance(screencorner, center);
                if (distance > maxdistance)
                    maxdistance = distance;
            }

            // Scale the sprite using the two points (the sprite is 
            // 75 pixels square)
            explosion.Scale = maxdistance / 75;

            //base.Update(gameTime);
#endif
        }
#if 0
        private void MoveCamera(GamePadState Player)
        {
            Camera1.OrbitUp(Player.ThumbSticks.Right.Y / 4);
            Camera1.OrbitRight(Player.ThumbSticks.Right.X / 4);
            Camera1.Distance -= Player.ThumbSticks.Left.Y;
            D3DXFROMWINEVECTOR3 newTarget = Camera1.Target;
            newTarget.X += Player.ThumbSticks.Left.X;
            Camera1.Target = newTarget;
        }
#endif
        /// <summary>
        /// This is called when the game should draw itself.
        /// </summary>
        /// <param name="gameTime">Provides a snapshot of timing values.</param>
	void Draw(GameTime gameTime)
        {
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
#if 0
            //Draw the model, a model can have multiple meshes, so loop
            foreach (ModelMesh mesh in Ring.Meshes)
            {
                //This is where the mesh orientation is set, as well as 
                //our camera and projection
                foreach (BasicEffect effect in mesh.Effects)
                {
                    effect.EnableDefaultLighting();
                    effect.World = D3DXFROMWINEMATRIX.Identity *
                        D3DXFROMWINEMATRIX.CreateRotationY(RingRotation) *
                        D3DXFROMWINEMATRIX.CreateTranslation(RingPosition);
                    effect.View = Camera1.ViewMatrix;
                    effect.Projection = projectionMatrix;
                }
                //Draw the mesh, will use the effects set above.
                mesh.Draw();
            }
#endif

            // Draw the sprite over the 3D object
            //            spriteBatch.Begin(SpriteBlendMode.AlphaBlend,
            //                SpriteSortMode.Deferred, SaveStateMode.SaveState);
            //spriteBatch.Begin();
            explosion.DrawFrame(spriteBatch, explosionpos);
            //spriteBatch.End();

            //base.Draw(gameTime);
        }
    }

