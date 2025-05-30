//-----------------------------------------------------------------------------
// AnimatedTexture.cs
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
    class AnimatedTexture
    {
	private:
        int framecount;
        TextureID myTexture;
        float TimePerFrame;
        int Frame;
        float TotalElapsed;
        bool Paused;

        public:
	float Rotation, Scale, Depth;
        D3DXFROMWINEVECTOR2 Origin;
        AnimatedTexture(D3DXFROMWINEVECTOR2 origin, float rotation, 
            float scale, float depth)
        {
            this->Origin = origin;
            this->Rotation = rotation;
            this->Scale = scale;
            this->Depth = depth;
        }
        void Load(int frameCount, int framesPerSec)
        {
            framecount = frameCount;
            SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	    myTexture=IRenderer::GetRendererInstance()->addImageLibTexture("/ViewportProjectionContent/explosion.dds", false, ss);
	
            TimePerFrame = (float)1 / framesPerSec;
            Frame = 0;
            TotalElapsed = 0;
            Paused = false;
        }

        // class AnimatedTexture
        void UpdateFrame(float elapsed)
        {
            if (Paused)
                return;
            TotalElapsed += elapsed;
            if (TotalElapsed > TimePerFrame)
            {
                Frame++;
                // Keep the Frame between 0 and the total frames, minus one.
                Frame = Frame % framecount;
                TotalElapsed -= TimePerFrame;
            }
        }

        // class AnimatedTexture
        void DrawFrame(D3DXFROMWINEVECTOR2 screenPos)
        {
            DrawFrame(Frame, screenPos);
        }
        void DrawFrame(int frame, D3DXFROMWINEVECTOR2 screenPos)
        {
            int FrameWidth = myTexture.Width / framecount;
            Rectangle sourcerect = new Rectangle(FrameWidth * frame, 0,
                FrameWidth, myTexture.Height);
            batch.Draw(myTexture, screenPos, sourcerect, Color.White,
                Rotation, Origin, Scale, SpriteEffects.None, Depth);
        }

        bool IsPaused
        {
            return Paused;
        }
        void Reset()
        {
            Frame = 0;
            TotalElapsed = 0f;
        }
        void Stop()
        {
            Pause();
            Reset();
        }
        void Play()
        {
            Paused = false;
        }
        void Pause()
        {
            Paused = true;
        }

    }

