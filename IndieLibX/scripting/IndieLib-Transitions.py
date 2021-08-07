#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib

#enum TransitionType
RANDOM=0
ALPHA2=1
WAVE_ALPHA=2
WIPE_LEFT=3
WIPE_RIGHT=4
WIPE_UP=5
WIPE_DOWN=6
SCROLL_LEFT=7
SCROLL_RIGHT=8
SCROLL_UP=9
SCROLL_DOWN=10
SQUEEZE_LEFT=11
SQUEEZE_RIGHT=12
SQUEEZE_UP=13
SQUEEZE_DOWN=14
SHRINK=15
SHRINK_SPIN_CLOCKWISE=16
SHRINK_SPIN_COUNTERCLOCKWISE=17
SLIDE_LEFT=18
SLIDE_RIGHT=19
SLIDE_UP=20
SLIDE_DOWN=21
SLIDE_OUT_LEFT=22
SLIDE_OUT_RIGHT=23
SLIDE_OUT_UP=24
SLIDE_OUT_DOWN=25
EXPAND=26
EXPAND_SPIN_CLOCKWISE=27
EXPAND_SPIN_COUNTERCLOCKWISE=28
FADEOUT=29

class Transition:
	def __init__(self, pSource=IndieLib.IND_Factory.GetEntity2d(), pDest=IndieLib.IND_Factory.GetEntity2d(), type=0, width=0, height=0, duration=0.0):
		self.m_SourceEntity2d=pSource
		self.m_DestEntity2d=pDest
		self.OriginalSourceSurface=IndieLib.IND_Factory.GetSurface()
		self.OriginalDestSurface=IndieLib.IND_Factory.GetSurface()
		self.m_Type=RANDOM
		self.m_Timer=IndieLib.IND_Factory.GetTimer()
		self.m_Source_alpha=0
		self.m_Destination_alpha=0
		self.m_Source_x=0.0
		self.m_Source_y=0.0
		self.m_Source_x_scale=0.0
		self.m_Source_y_scale=0.0
		self.m_Source_rot=0.0
		self.m_Destination_x=0.0
		self.m_Destination_y=0.0
		self.m_Destination_x_scale=0.0
		self.m_Destination_y_scale=0.0
		self.m_Destination_rot=0.0
		self.m_fTime=0.0
		self.m_fDuration=duration
		self.m_nRows=0
		self.m_nCols=0
		self.m_fPercent=0.0
		self.m_ClonedSourceSurface=IndieLib.IND_Factory.GetSurface()
		self.m_ClonedDestSurface=IndieLib.IND_Factory.GetSurface()
		self.m_OriginalSourceSurface=IndieLib.IND_Factory.GetSurface()
		self.m_OriginalDestSurface=IndieLib.IND_Factory.GetSurface()
		self.m_NumBlocksX=0
		self.m_NumBlocksY=0
		self.m_WidthBlock=0
		self.m_HeightBlock=0

		self.fTime					= 0.0
		self.fDuration				= duration

		self.m_Width					= width
		self.m_Height				= height
		self.Source_alpha			= 0
		self.Destination_alpha		= 0
		self.Source_x				= 0.0
		self.Source_y				= 0.0
		self.Source_x_scale		= 1.0
		self.Source_y_scale		= 1.0
		self.Source_rot			= 0.0
		self.Destination_x			= 0.0
		self.Destination_y			= 0.0
		self.Destination_x_scale	= 1.0
		self.Destination_y_scale	= 1.0
		self.Destination_rot		= 0.0

		self.nRows					= 16
		self.nCols					= 16
		self.fPercent				= 0.0

		self.m_SourceEntity2d.SetTransparency(255)
		self.m_SourceEntity2d.SetPosition(0, 0, 0)
		self.m_SourceEntity2d.SetHotSpot(0,0)
		self.m_SourceEntity2d.SetRegion(0, 0, self.m_Width, self.m_Height)
		self.m_SourceEntity2d.SetScale(1, 1)
		self.m_SourceEntity2d.SetAngleXYZ(0, 0, 0)

		self.m_DestEntity2d.SetTransparency(255)
		self.m_DestEntity2d.SetPosition(0, 0, 0)
		self.m_DestEntity2d.SetHotSpot(0,0)
		self.m_DestEntity2d.SetRegion(0, 0, self.m_Width, self.m_Height)
		self.m_DestEntity2d.SetScale(1, 1)
		self.m_DestEntity2d.SetAngleXYZ(0, 0, 0)

		if type == WAVE_ALPHA:
	
			self.m_pIndieLib.SurfaceManager.Clone(self.m_ClonedSourceSurface, self.m_OriginalSourceSurface)
			self.m_ClonedSourceSurface.SetGrid(8, 8)
			self.m_SourceEntity2d.SetSurface(self.m_ClonedSourceSurface)

			self.m_pIndieLib.SurfaceManager.Clone(self.m_ClonedDestSurface, self.m_OriginalDestSurface)
			self.m_ClonedDestSurface.SetGrid(8, 8)
			self.m_DestEntity2d.SetSurface(self.m_ClonedDestSurface)
	
			self.m_NumBlocksX			= self.m_ClonedSourceSurface.GetBlocksX()
			self.m_NumBlocksY			= self.m_ClonedSourceSurface.GetBlocksY()
			self.m_WidthBlock			= self.m_ClonedSourceSurface.GetWidthBlock()
			self.m_HeightBlock			= self.m_ClonedSourceSurface.GetHeightBlock()
			self.m_Timer.Start()
		elif type == RANDOM:
	
			srand(time(0))
			idx=rand() % 27
			
			if idx==0:
				self.m_Type = ALPHA2
			elif idx==1:
				self.m_Type = WAVE_ALPHA
			elif idx==2:
				self.m_Type = WIPE_LEFT
			elif idx==3:
				self.m_Type = WIPE_RIGHT
			elif idx==4:
				self.m_Type = WIPE_UP
			elif idx==5:
				self.m_Type = WIPE_DOWN
			elif idx==6:
				self.m_Type = SCROLL_LEFT
			elif idx==7:
				self.m_Type = SCROLL_RIGHT
			elif idx==8:
				self.m_Type = SCROLL_UP
			elif idx==9:
				self.m_Type = SCROLL_DOWN
			elif idx==10:
				self.m_Type = SQUEEZE_LEFT
			elif idx==11:
				self.m_Type = SQUEEZE_RIGHT
			elif idx==12:
				self.m_Type = SQUEEZE_UP
			elif idx==13:
				self.m_Type = SQUEEZE_DOWN
			elif idx==14:
				self.m_Type = SHRINK
			elif idx==15:
				self.m_Type = SHRINK_SPIN_CLOCKWISE
			elif idx==16:
				self.m_Type = SHRINK_SPIN_COUNTERCLOCKWISE
			elif idx==17:
				self.m_Type = SLIDE_LEFT
			elif idx==18:
				self.m_Type = SLIDE_RIGHT
			elif idx==19:
				self.m_Type = SLIDE_UP
			elif idx==20:
				self.m_Type = SLIDE_DOWN
			elif idx==21:
				self.m_Type = SLIDE_OUT_LEFT
			elif idx==22:
				self.m_Type = SLIDE_OUT_RIGHT
			elif idx==23:
				self.m_Type = SLIDE_OUT_UP
			elif idx==24:
				self.m_Type = SLIDE_OUT_DOWN
			elif idx==25:
				self.m_Type = EXPAND
			elif idx==26:
				self.m_Type = EXPAND_SPIN_CLOCKWISE
			elif idx==27:
				self.m_Type = EXPAND_SPIN_COUNTERCLOCKWISE
			elif idx==28:
				self.m_Type = FADEOUT
		else:
			self.m_Type = type
	
	def IsComplete(self):
		return self.m_fTime > self.m_fDuration

	def Tick(self, fElapsedTime):
		time=0.0

		self.m_fTime += fElapsedTime
		self.m_fPercent = self.m_fTime / self.m_fDuration
		if self.m_fPercent > 1.0:
			self.m_fPercent = 1.0

		if self.m_Type==ALPHA2:
        		self.m_Source_alpha = 255 - (255 * self.m_fPercent)
			self.m_Destination_alpha = (255 * self.m_fPercent)
			self.m_SourceEntity2d.SetTransparency( self.m_Source_alpha)
			self.m_DestEntity2d.SetTransparency(self.m_Destination_alpha)
				
		elif self.m_Type==WAVE_ALPHA:
			self.m_Source_alpha = 255 - (255 * self.m_fPercent)
			self.m_Destination_alpha = (255 * self.m_fPercent)
			self.m_SourceEntity2d.SetTransparency( self.m_Source_alpha)
			self.m_DestEntity2d.SetTransparency(self.m_Destination_alpha)
 
			time = self.m_Timer.GetTicks() / 1000.0
		
			for i in range(1,self.m_NumBlocksX):
				for j in range(1,self.m_NumBlocksY):
					self.m_DestEntity2d.GetSurface().SetVertexPos (j,i,((j * self.m_HeightBlock + cosf (time * 10 + (i + j) / 2) * 5)),((i * self.m_WidthBlock  + sinf (time * 10 + (i + j) / 2) * 5)))

		elif self.m_Type==WIPE_LEFT:
			self.m_SourceEntity2d.SetRegion(0, 0, (self.m_Width - self.m_Width * self.m_fPercent), self.m_Height)
			self.m_DestEntity2d.SetRegion(0, 0, self.m_Width, self.m_Height)
			self.m_SourceEntity2d.SetPosition(0, 0, 1)
				
		elif self.m_Type==WIPE_RIGHT:
			self.m_SourceEntity2d.SetRegion(0, 0, self.m_Width, self.m_Height)
			self.m_DestEntity2d.SetRegion(0, 0, (self.m_Width * self.m_fPercent), self.m_Height)
			self.m_DestEntity2d.SetPosition(0, 0, 1)
				
		elif self.m_Type==WIPE_UP:
			self.m_SourceEntity2d.SetRegion(0, 0, self.m_Width, (self.m_Height * (1.0 - self.m_fPercent)))
			self.m_DestEntity2d.SetRegion(0, 0, self.m_Width, self.m_Height)
			self.m_SourceEntity2d.SetPosition(0, 0, 1)
				
		elif self.m_Type==WIPE_DOWN:
			self.m_DestEntity2d.SetRegion(0, 0, self.m_Width, (self.m_Height * self.m_fPercent))
			self.m_SourceEntity2d.SetRegion(0, 0, self.m_Width, self.m_Height)
			self.m_DestEntity2d.SetPosition(0, 0, 1)
				
		elif self.m_Type==SCROLL_LEFT:
			self.m_Source_x = -self.m_Width * self.m_fPercent
			self.m_Source_y = 0
			self.m_Destination_x = self.m_Width - self.m_Width * self.m_fPercent
			self.m_Destination_y = 0
			self.m_SourceEntity2d.SetPosition(self.m_Source_x, self.m_Source_y, 0)
			self.m_DestEntity2d.SetPosition(self.m_Destination_x, self.m_Destination_y, 0)
				
		elif self.m_Type==SCROLL_RIGHT:
       			self.m_Source_x = self.m_Width * self.m_fPercent
			self.m_Source_y = 0
			self.m_Destination_x = self.m_Width * self.m_fPercent - self.m_Width
			self.m_Destination_y = 0
			self.m_SourceEntity2d.SetPosition(self.m_Source_x, self.m_Source_y, 0)
			self.m_DestEntity2d.SetPosition(self.m_Destination_x, self.m_Destination_y, 0)
				
		elif self.m_Type==SCROLL_UP:
			self.m_Source_x = 0
			self.m_Source_y = - self.m_Height * self.m_fPercent
			self.m_Destination_x = 0
			self.m_Destination_y = self.m_Height - self.m_Height * self.m_fPercent
			self.m_SourceEntity2d.SetPosition(self.m_Source_x, self.m_Source_y, 0)
			self.m_DestEntity2d.SetPosition(self.m_Destination_x, self.m_Destination_y, 0)
				
		elif self.m_Type==SCROLL_DOWN:
			self.m_Source_x = 0
			self.m_Source_y = self.m_Height * self.m_fPercent
			self.m_Destination_x = 0
			self.m_Destination_y = self.m_Height * self.m_fPercent - self.m_Height
			self.m_SourceEntity2d.SetPosition(self.m_Source_x, self.m_Source_y, 0)
			self.m_DestEntity2d.SetPosition(self.m_Destination_x, self.m_Destination_y, 0)
				
		elif self.m_Type==SQUEEZE_LEFT:
			self.m_Source_x_scale = (1.0 - self.m_fPercent)
			self.m_Source_x = 0
			self.m_Destination_x_scale = self.m_fPercent
			self.m_Destination_x = self.m_Width - self.m_Destination_x_scale * self.m_Width
			self.m_SourceEntity2d.SetScale(self.m_Source_x_scale, self.m_Source_y_scale)
			self.m_SourceEntity2d.SetPosition(self.m_Source_x, self.m_Source_y, 0)
			self.m_DestEntity2d.SetScale(self.m_Destination_x_scale, self.m_Destination_y_scale)
			self.m_DestEntity2d.SetPosition(self.m_Destination_x, self.m_Destination_y, 0)
				
		elif self.m_Type==SQUEEZE_RIGHT:
			self.m_Source_x_scale = (1.0 - self.m_fPercent)
			self.m_Source_x = self.m_Width - self.m_Source_x_scale * self.m_Width
			self.m_Destination_x_scale = self.m_fPercent
			self.m_SourceEntity2d.SetScale(self.m_Source_x_scale, self.m_Source_y_scale)
			self.m_SourceEntity2d.SetPosition(self.m_Source_x, self.m_Source_y, 0)
			self.m_DestEntity2d.SetScale(self.m_Destination_x_scale, self.m_Destination_y_scale)
				
		elif self.m_Type==SQUEEZE_UP:
			self.m_Source_y_scale = (1.0000001 - self.m_fPercent)
			self.m_Destination_y_scale = self.m_fPercent
			self.m_Destination_y = self.m_Height - self.m_Height * self.m_fPercent
			self.m_SourceEntity2d.SetScale(self.m_Source_x_scale, self.m_Source_y_scale)
			self.m_SourceEntity2d.SetPosition(self.m_Source_x, self.m_Source_y, 0)
			self.m_DestEntity2d.SetScale(self.m_Destination_x_scale, self.m_Destination_y_scale)
			self.m_DestEntity2d.SetPosition(self.m_Destination_x, self.m_Destination_y, 0)
			
		elif self.m_Type==SQUEEZE_DOWN:
			self.m_Source_y_scale = (1.0000001 - self.m_fPercent)
			self.m_Source_y = self.m_Height - self.m_Source_y_scale * self.m_Height
			self.m_Destination_y_scale = self.m_fPercent
			self.m_SourceEntity2d.SetScale(self.m_Source_x_scale, self.m_Source_y_scale)
			self.m_SourceEntity2d.SetPosition(self.m_Source_x, self.m_Source_y, 0)
			self.m_DestEntity2d.SetScale(self.m_Destination_x_scale, self.m_Destination_y_scale)
			self.m_DestEntity2d.SetPosition(self.m_Destination_x, self.m_Destination_y, 0)
			
		elif self.m_Type==SHRINK:
			self.m_SourceEntity2d.SetHotSpot(0.5, 0.5)
			self.m_Source_x_scale = (1.0 - self.m_fPercent)
			self.m_Source_y_scale = (1.0 - self.m_fPercent)
			self.m_Source_x = self.m_Width / 2
			self.m_Source_y = self.m_Height / 2
			self.m_SourceEntity2d.SetPosition(self.m_Source_x, self.m_Source_y, 0)
			self.m_SourceEntity2d.SetScale(self.m_Source_x_scale, self.m_Source_y_scale)
			
		elif self.m_Type==SHRINK_SPIN_CLOCKWISE:
			self.m_Source_rot = self.m_fPercent * 360 * 2
			self.m_Source_x_scale = (1.0 - self.m_fPercent)
			self.m_Source_y_scale = (1.0 - self.m_fPercent)
			self.m_Source_x = self.m_Width / 2
			self.m_Source_y = self.m_Height / 2
			self.m_SourceEntity2d.SetHotSpot(0.5, 0.5)
			self.m_SourceEntity2d.SetPosition(self.m_Source_x, self.m_Source_y, 1)
			self.m_SourceEntity2d.SetScale(self.m_Source_x_scale, self.m_Source_y_scale)
			self.m_SourceEntity2d.SetAngleXYZ(0, 0, self.m_Source_rot)
			
		elif self.m_Type==SHRINK_SPIN_COUNTERCLOCKWISE:
			self.m_Source_rot = -self.m_fPercent * 360 * 2
			self.m_Source_x_scale = (1.0 - self.m_fPercent)
			self.m_Source_y_scale = (1.0 - self.m_fPercent)
			self.m_Source_x = self.m_Width / 2
			self.m_Source_y = self.m_Height / 2
			self.m_SourceEntity2d.SetHotSpot(0.5, 0.5)
			self.m_SourceEntity2d.SetPosition(self.m_Source_x, self.m_Source_y, 1)
			self.m_SourceEntity2d.SetScale(self.m_Source_x_scale, self.m_Source_y_scale)
			self.m_SourceEntity2d.SetAngleXYZ(0, 0, self.m_Source_rot)
			
		elif self.m_Type==SLIDE_LEFT:
			self.m_Destination_x = (self.m_Width - self.m_Width * self.m_fPercent)
			self.m_DestEntity2d.SetPosition(self.m_Destination_x, self.m_Destination_y, 1)
			
		elif self.m_Type==SLIDE_RIGHT:
			self.m_Source_x = self.m_Width * self.m_fPercent - self.m_Width
			self.m_DestEntity2d.SetPosition(self.m_Source_x, self.m_Source_y, 1)
			
		elif self.m_Type==SLIDE_UP:
			self.m_Destination_y = self.m_Height - (self.m_Height * self.m_fPercent)
			self.m_DestEntity2d.SetPosition(self.m_Destination_x, self.m_Destination_y, 1)
			
		elif self.m_Type==SLIDE_DOWN:
			self.m_Destination_y = self.m_Height * self.m_fPercent - self.m_Height
			self.m_DestEntity2d.SetPosition(self.m_Destination_x, self.m_Destination_y, 1)
			

		elif self.m_Type==SLIDE_OUT_LEFT:
			self.m_Source_x = - (self.m_Width * self.m_fPercent)
			self.m_SourceEntity2d.SetPosition(self.m_Source_x, self.m_Source_y, 1)
			
		elif self.m_Type==SLIDE_OUT_RIGHT:
			self.m_Source_x = self.m_Width * self.m_fPercent
			self.m_SourceEntity2d.SetPosition(self.m_Source_x, self.m_Source_y, 1)
			
		elif self.m_Type==SLIDE_OUT_UP:
			self.m_Destination_y =  - (self.m_Height * self.m_fPercent)
			self.m_SourceEntity2d.SetPosition(self.m_Destination_x, self.m_Destination_y, 1)
			
		elif self.m_Type==SLIDE_OUT_DOWN:
			self.m_Source_y = self.m_Height * self.m_fPercent
			self.m_SourceEntity2d.SetPosition(self.m_Source_x, self.m_Source_y, 1)
			
		elif self.m_Type==EXPAND:
			self.m_Destination_x_scale = self.m_fPercent
			self.m_Destination_y_scale = self.m_fPercent
			self.m_Destination_x = (self.m_Width / 2)
			self.m_Destination_y = (self.m_Height / 2)
			self.m_DestEntity2d.SetHotSpot(0.5, 0.5)
			self.m_DestEntity2d.SetPosition(self.m_Destination_x, self.m_Destination_y, 1)
			self.m_DestEntity2d.SetScale(self.m_Destination_x_scale, self.m_Destination_y_scale)
			
		elif self.m_Type==EXPAND_SPIN_CLOCKWISE:
			self.m_Destination_rot = self.m_fPercent * 360 * 2
			self.m_Destination_x_scale = self.m_fPercent
			self.m_Destination_y_scale = self.m_fPercent
			self.m_Destination_x = (self.m_Width / 2)
			self.m_Destination_y = (self.m_Height / 2)
			self.m_DestEntity2d.SetHotSpot(0.5, 0.5)
			self.m_DestEntity2d.SetPosition(self.m_Destination_x, self.m_Destination_y, 1)
			self.m_DestEntity2d.SetScale(self.m_Destination_x_scale, self.m_Destination_y_scale)
			self.m_DestEntity2d.SetAngleXYZ(0, 0, self.m_Destination_rot)
			
		elif self.m_Type==EXPAND_SPIN_COUNTERCLOCKWISE:
			self.m_Destination_rot = -self.m_fPercent * 360 * 2
			self.m_Destination_x_scale = self.m_fPercent
			self.m_Destination_y_scale = self.m_fPercent
			self.m_Destination_x = (self.m_Width / 2)
			self.m_Destination_y = (self.m_Height / 2)
			self.m_DestEntity2d.SetHotSpot(0.5, 0.5)
			self.m_DestEntity2d.SetPosition(self.m_Destination_x, self.m_Destination_y, 1)
			self.m_DestEntity2d.SetScale(self.m_Destination_x_scale, self.m_Destination_y_scale)
			self.m_DestEntity2d.SetAngleXYZ(0, 0, self.m_Destination_rot)
			
	def GetPercentComplete(self):
		return self.m_fPercent

class AppPy:
	def __init__(self):
		self.g_Counter		= 0
		self.g_TransitionType	= RANDOM
		self.g_Swap		= False
		self.g_szText=""
		self.g_szTitle=""
		self.g_pMainTransition	= 0
		self.g_pSourceEntity2d=IndieLib.IND_Factory.GetEntity2d()
		self.g_pDestEntity2d=IndieLib.IND_Factory.GetEntity2d()
		self.g_fDuration			= 1.0
		self.fElapsedTime = 0.0
		self.iiiAlpha = 255
		self.SurfaceSnake=IndieLib.IND_Factory.GetSurface()
		self.SurfacePool=IndieLib.IND_Factory.GetSurface()
		self.FontSmall=IndieLib.IND_Factory.GetFont()
		self.TextSmallWhite=IndieLib.IND_Factory.GetEntity2d()
		self.timer=IndieLib.IND_Factory.GetTimer()

	def Update(self, fElapsedTime):

		if IndieLib.CIndieLib.Instance().Input.OnKeyPress (IndieLib.STX_KEY_SPACE):
	
			if self.g_pMainTransition.IsComplete():
		
				if self.g_Counter > 27:
					self.g_Counter = 0

				idx=self.g_Counter
			
				if idx==0:
					self.g_TransitionType = ALPHA2
					self.g_szTitle =  "ALPHA2"
				
				elif idx==1:
					self.g_TransitionType = WAVE_ALPHA
					self.g_szTitle =  "WAVE_ALPHA"
				
				elif idx==2:
					self.g_TransitionType = WIPE_LEFT
					self.g_szTitle =  "WIPE_LEFT"
				
				elif idx==3:
					self.g_TransitionType = WIPE_RIGHT
 					self.g_szTitle =  "WIPE_RIGHT"
				
				elif idx==4:
					self.g_TransitionType = WIPE_UP
					self.g_szTitle =  "WIPE_UP"
				
				elif idx==5:
					self.g_TransitionType = WIPE_DOWN
					self.g_szTitle =  "WIPE_DOWN"
				
				elif idx==6:
					self.g_TransitionType = SCROLL_LEFT
					self.g_szTitle =  "SCROLL_LEFT"
				
				elif idx==7:
					self.g_TransitionType = SCROLL_RIGHT
					self.g_szTitle =  "SCROLL_RIGHT"
				
				elif idx==8:
					self.g_TransitionType = SCROLL_UP
					self.g_szTitle =  "SCROLL_UP"
				
				elif idx==9:
					self.g_TransitionType = SCROLL_DOWN
					self.g_szTitle =  "SCROLL_DOWN"
				
				elif idx==10:
					self.g_TransitionType = SQUEEZE_LEFT
					self.g_szTitle =  "SQUEEZE_LEFT"
				
				elif idx==11:
					self.g_TransitionType = SQUEEZE_RIGHT
					self.g_szTitle =  "SQUEEZE_RIGHT"
				
				elif idx==12:
					self.g_TransitionType = SQUEEZE_UP
					self.g_szTitle =  "SQUEEZE_UP"
				
				elif idx==13:
					self.g_TransitionType = SQUEEZE_DOWN
					self.g_szTitle =  "SQUEEZE_DOWN"
				
				elif idx==14:
					self.g_TransitionType = SHRINK
					self.g_szTitle =  "SHRINK"
				
				elif idx==15:
					self.g_TransitionType = SHRINK_SPIN_CLOCKWISE
					self.g_szTitle =  "SHRINK_SPIN_CLOCKWISE"
				
				elif idx==16:
					self.g_TransitionType = SHRINK_SPIN_COUNTERCLOCKWISE
					self.g_szTitle =  "SHRINK_SPIN_COUNTERCLOCKWISE"
				
				elif idx==17:
					self.g_TransitionType = SLIDE_LEFT
					self.g_szTitle =  "SLIDE_LEFT"
				
				elif idx==18:
					self.g_TransitionType = SLIDE_RIGHT
					self.g_szTitle =  "SLIDE_RIGHT"
				
				elif idx==19:
					self.g_TransitionType = SLIDE_UP
					self.g_szTitle =  "SLIDE_UP"
				
				elif idx==20:
					self.g_TransitionType = SLIDE_DOWN
					self.g_szTitle =  "SLIDE_DOWN"
				
				elif idx==21:
					self.g_TransitionType = SLIDE_OUT_LEFT
					self.g_szTitle =  "SLIDE_OUT_LEFT"
				
				elif idx==22:
					self.g_TransitionType = SLIDE_OUT_RIGHT
					self.g_szTitle =  "SLIDE_OUT_RIGHT"
				
				elif idx==23:
					self.g_TransitionType = SLIDE_OUT_UP
					self.g_szTitle =  "SLIDE_OUT_UP"
				
				elif idx==24:
					self.g_TransitionType = SLIDE_OUT_DOWN
					self.g_szTitle =  "SLIDE_OUT_DOWN"
				
				elif idx==25:
					self.g_TransitionType = EXPAND
					self.g_szTitle =  "EXPAND"
				
				elif idx==26:
					self.g_TransitionType = EXPAND_SPIN_CLOCKWISE
					self.g_szTitle =  "EXPAND_SPIN_CLOCKWISE"
				
				elif idx==27:
					self.g_TransitionType = EXPAND_SPIN_COUNTERCLOCKWISE
					self.g_szTitle =  "EXPAND_SPIN_COUNTERCLOCKWISE"

				if self.g_Swap:
					self.g_pMainTransition = Transition(self.g_pSourceEntity2d, self.g_pDestEntity2d, self.g_TransitionType, self.g_pSourceEntity2d.GetSurface().GetWidth(), self.g_pSourceEntity2d.GetSurface().GetHeight(), self.g_fDuration)
				else:
					self.g_pMainTransition = Transition(self.g_pDestEntity2d, self.g_pSourceEntity2d, self.g_TransitionType, self.g_pSourceEntity2d.GetSurface().GetWidth(), self.g_pSourceEntity2d.GetSurface().GetHeight(), self.g_fDuration)
				self.g_Swap = not self.g_Swap
				self.g_Counter=self.g_Counter+1

		if not self.g_pMainTransition.IsComplete():
			self.g_pMainTransition.Tick( self.fElapsedTime )

	def init(self, width, height):
		IndieLib.CIndieLib.Instance()
		IndieLib.CIndieLib.Instance().Init("IndieLib-Transitions")

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.SurfaceSnake, "/IndieLib-Transitions/snake_512.jpg", IndieLib.IND_ALPHA, IndieLib.IND_32):
			 return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.SurfacePool, "/IndieLib-Transitions/pool_512.jpg", IndieLib.IND_ALPHA, IndieLib.IND_32):
		 	return 0

		if not IndieLib.CIndieLib.Instance().FontManager.Add (self.FontSmall, "/IndieLib-Transitions/font_small.png", "/IndieLib-Transitions/font_small.xml", IndieLib.IND_ALPHA, IndieLib.IND_32):
			 return 0

		IndieLib.CIndieLib.Instance().Entity2dManager.Add		(1, self.TextSmallWhite)
		self.TextSmallWhite.SetFont			(self.FontSmall)
		self.TextSmallWhite.SetLineSpacing	(18)
		self.TextSmallWhite.SetCharSpacing	(-8)
		self.TextSmallWhite.SetPosition		(5, 5, 1)
		self.TextSmallWhite.SetAlign		(IndieLib.IND_LEFT)

		self.g_pSourceEntity2d =IndieLib.IND_Factory.GetEntity2d()
		IndieLib.CIndieLib.Instance().Entity2dManager.Add (0, self.g_pSourceEntity2d)
		self.g_pSourceEntity2d.SetSurface (self.SurfacePool)

		self.g_pDestEntity2d	=IndieLib.IND_Factory.GetEntity2d()
		IndieLib.CIndieLib.Instance().Entity2dManager.Add (0, self.g_pDestEntity2d)
		self.g_pDestEntity2d.SetSurface (self.SurfaceSnake)
		self.g_pDestEntity2d.SetPosition(0, 0, 0)

		self.g_szTitle = "ALPHA2"
	
		self.timer.Start()

		self.g_pMainTransition = Transition(self.g_pSourceEntity2d, self.g_pDestEntity2d, ALPHA2, self.g_pSourceEntity2d.GetSurface().GetWidth(), self.g_pSourceEntity2d.GetSurface().GetHeight(), 1)
		return 0

	def render(self):
		IndieLib.CIndieLib.Instance().Input.Update ()

		fElapsedTime = IndieLib.CIndieLib.Instance().Render.GetFrameTime() / 1000.0

		self.g_szText = self.g_szTitle
		self.g_szText += "\n"
		self.g_szText += "Hit SPACE for next transition\n"
		self.g_szText += "ESC to quit"
		self.TextSmallWhite.SetText	 (self.g_szText)

		IndieLib.CIndieLib.Instance().Render.BeginScene ()
		IndieLib.CIndieLib.Instance().Render.ClearViewPort (60, 60, 60)
		IndieLib.CIndieLib.Instance().Render.SetViewPort2d (0, 0, IndieLib.CIndieLib.Instance().Window.GetWidth(), IndieLib.CIndieLib.Instance().Window.GetHeight())

		self.Update(self.fElapsedTime)

		for i in range(0,2):
			IndieLib.CIndieLib.Instance().Entity2dManager.RenderEntities2d ( i )

		#???IndieLib.CIndieLib.Instance().Render.ShowFpsInWindowTitle()
		IndieLib.CIndieLib.Instance().Render.EndScene ()
		return 0

	def run(self):
		self.init(0,0)
		while not IndieLib.CIndieLib.Instance().Input.IsKeyUp(IndieLib.STX_KEY_ESCAPE):
			self.render()

if __name__=="__main__":
	app=AppPy()
	app.run()

