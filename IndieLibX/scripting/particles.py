#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib

# Particle emitter types
EMITTER_CLOUD       =0
EMITTER_TREE        =1
EMITTER_PEOPLE      =2

# Particle types and # of particle types
PARTICLE_SMOKE      =0
PARTICLE_TREE1      =1
PARTICLE_TREE2      =2
PARTICLE_TREE3      =3
PARTICLE_PEOPLE1    =4
PARTICLE_PEOPLE2    =5
NUM_PARTICLE_TYPES  =6

# Vertex structure for vertex-shader particles
typedef struct 
  IndieLib.D3DXFROMWINEVECTOR3 vecPos     # Coordinates of particle
  IndieLib.D3DXFROMWINEVECTOR2 vecOffset  # Corner vertex offset coordinates
  #IndieLib.D3DXFROMWINEVECTOR4       Diffuse    # Diffuse color of particle
  float       u, v       # Texture coordinates
 sShaderVertex

# Particle container class
class cParticle:
	def __init__(self):
		
		self.m_Type=0         # Type of particle

		self.m_vecPos=IndieLib.D3DXFROMWINEVECTOR3()       # Position of particle
		self.m_vecVelocity=IndieLib.D3DXFROMWINEVECTOR3()  # Velocity of particle

		self.m_Life=0         # Life of particle in ms
		self.m_Size=0         # Size of particle

    		#IndieLib.D3DXFROMWINEVECTOR4       m_Color        # Diffuse color of particle

		self.m_Prev = 0          # Prev particle in linked list
		self.m_Next = 0          # Next particle in linked list


# Particle emitter class
class cParticleEmitter:
	def __init__(self):
		self.Ptr=0
		self.Ptr0=0

		self.m_EmitterType=0   # Type of emitter

		self.m_VB=-1            # Particles' vertex buffer
		self.m_IB=-1            # Particles' index buffer

                self.m_vecPosition=IndieLib.D3DXFROMWINEVECTOR3()   # Position of emitter
                self.m_NumParticles=0  # Max # particles in buffer
    		cParticle   m_Particles     # Particle list

                self.m_RefCount=0      # Class reference count

                self.m_pShader=-1    # Vertex shader
                self.m_pDecl=-1 # Vertex decl
                self.m_pTextures=-1      # Textures
                self.alphaState=-1

  		if(alphaState==-1)
  			alphaState=IndieLib.IRenderer.GetRendererInstance().addAlphaState(1,1.0)


	def Create(vecPosition, EmitterType, NumParticlesPerBuffer)
		# Save emitter's position
		m_vecPosition = (*vecPosition)

		# Save type of emitter
		m_EmitterType = EmitterType

		# Save # of particles in buffer
		m_NumParticles = NumParticlesPerBuffer

		# Load textures if none already loaded
		if(m_pTextures == 0) 

			m_pTextures = new TextureID[NUM_PARTICLE_TYPES]

			m_pTextures[0]=IndieLib.IRenderer.GetRendererInstance().addImageLibTexture("/AdvAniCh12/Particle_Smoke.png", False, IndieLib.IRenderer.GetRendererInstance().Getlinear())

			m_pTextures[1]=IndieLib.IRenderer.GetRendererInstance().addImageLibTexture("/dx8sdk/tree01S.tga", False, IndieLib.IRenderer.GetRendererInstance().Getlinear())

			m_pTextures[2]=IndieLib.IRenderer.GetRendererInstance().addImageLibTexture("/dx8sdk/tree02S.tga", False, IndieLib.IRenderer.GetRendererInstance().Getlinear())

			m_pTextures[3]=IndieLib.IRenderer.GetRendererInstance().addImageLibTexture("/dx8sdk/tree35S.tga", False, IndieLib.IRenderer.GetRendererInstance().Getlinear())

			m_pTextures[4]=IndieLib.IRenderer.GetRendererInstance().addImageLibTexture("/AdvAniCh12/Particle_People1.png", False, IndieLib.IRenderer.GetRendererInstance().Getlinear())

			m_pTextures[5]=IndieLib.IRenderer.GetRendererInstance().addImageLibTexture("/AdvAniCh12/Particle_People2.png", False, IndieLib.IRenderer.GetRendererInstance().Getlinear())
  

#ifdef __IB__
			Ptri=unsigned short[6*m_NumParticles]
  for(unsigned short i=0i<m_NumParticlesi++) 
    Ptri[i*6+0] = i * 4 + 0
    Ptri[i*6+1] = i * 4 + 1
    Ptri[i*6+2] = i * 4 + 2
    Ptri[i*6+3] = i * 4 + 3
    Ptri[i*6+4] = i * 4 + 2
    Ptri[i*6+5] = i * 4 + 1
  
  # Create the index buffer and fill with face data
			m_IB=IndieLib.IRenderer.addIndexBuffer(m_NumParticles * 6 , sizeof(short),STATIC,Ptri)
			delete[] Ptri
#endif
			Ptr=0
			Ptr0=0

  # Load the vertex shader if not already done
			if(-1==m_pShader) 

			g_szParticleShader = '''
	[Vertex shader]
	row_major float4x4 WorldViewProjection	: WORLDVIEWPROJECTION;
	float4 right;
	float4 up;
	struct VS_INPUT
	
		float3 Position : POSITION;
		float2 vecOffset : TEXCOORD0;
		#float4 Diffuse : TEXCOORD1;
		float2 uv : TEXCOORD1;
	
	struct VS_OUTPUT
	
		float4 Position : POSITION;
	#	float4 Diffuse : TEXCOORD0;
		float2 uv : TEXCOORD0;
	
	VS_OUTPUT main(VS_INPUT IN)
	
		VS_OUTPUT Out = (VS_OUTPUT)0;
		#if (IN.Diffuse.rgb == float3(0.0,0.0,0.0)) discard;
		float3 objPos =  IN.vecOffset.y*up.xyz+ IN.vecOffset.x*right.xyz+ IN.Position;
		Out.Position = mul( WorldViewProjection,float4( objPos.x, objPos.y ,objPos.z, 1.0));
		Out.uv=IN.uv;
		return Out;
	

[Fragment shader]
	struct VS_OUTPUT
	
		float4 Position : POSITION;
	#	float4 Diffuse : TEXCOORD0;
		float2 uv : TEXCOORD0;
		

		sampler Sampler;
	float4 main(VS_OUTPUT IN) : COLOR
	
		float4 OUT = tex2D(Sampler,IN.uv);
		#if (OUT.rgb == float3(0.0,0.0,0.0)) discard;
		return OUT;
	
	'''

	m_pShader=IndieLib.IRenderer.GetRendererInstance().addHLSLShader(g_szParticleShader,"main","main")
	a = IndieLib.new_intArray(8)
	IndieLib.intArray_setitem(a, 0, 0)
	IndieLib.intArray_setitem(a, 1, IndieLib.TYPE_VERTEX)
	IndieLib.intArray_setitem(a, 2, IndieLib.FORMAT_FLOAT)
	IndieLib.intArray_setitem(a, 3, 3)
	IndieLib.intArray_setitem(a, 4, 0)
	IndieLib.intArray_setitem(a, 5, IndieLib.TYPE_TEXCOORD)
	IndieLib.intArray_setitem(a, 6, IndieLib.FORMAT_FLOAT)
	IndieLib.intArray_setitem(a, 7, 2)
	IndieLib.intArray_setitem(a, 8, 0)
	IndieLib.intArray_setitem(a, 9, IndieLib.TYPE_TEXCOORD)
	IndieLib.intArray_setitem(a, 10, IndieLib.FORMAT_FLOAT)
	IndieLib.intArray_setitem(a, 11, 2)
	self.vf = IndieLib.IRenderer.GetRendererInstance().addVertexFormatV(a, 3, m_pShader)
	IndieLib.delete_intArray(a)


  	# Increase class reference count
  	m_RefCount++

	def Free():
 		# Decrease reference count and free IndieLib_resources if needed
  		if(m_RefCount)
    			m_RefCount--
  		if(!m_RefCount) 

    # Release textures
    if(m_pTextures) 
      for(DWORD i=0i<NUM_PARTICLE_TYPESi++)
        #ReleaseCOM(m_pTextures[i])
      delete [] m_pTextures
      m_pTextures = 0
    

    # Release shader data
    #ReleaseCOM(m_pShader)
    #ReleaseCOM(m_pDecl)
  

  # Clear object pointers
  #ReleaseCOM(m_VB)
  #ReleaseCOM(m_IB)

  # Clear particle data
  m_NumParticles = 0
  delete m_Particles
  m_Particles = 0

    delete[] Ptr0

	def Add(Type, vecPos, Size, Color, Life, vecVelocity):
  # Allocate a particle object and add to head of list
  cParticle *Particle = new cParticle()
  Particle.m_Prev = 0
  if((Particle.m_Next = m_Particles))
    m_Particles.m_Prev = Particle
  m_Particles = Particle

  # Set particle data
  Particle.m_Type   = Type
  Particle.m_vecPos = (*vecPos)
  Particle.m_Size   = Size
#  Particle.m_Color  = Color
  Particle.m_Life   = Life
  Particle.m_vecVelocity = (*vecVelocity)

	def ClearAll():
  # Clear particle data
  delete m_Particles
  m_Particles = 0

	def Begin():
		IndieLib.IRenderer.setShader(m_pShader)
		IndieLib.IRenderer.setVertexFormat(m_pDecl)

		IndieLib.D3DXFROMWINEMATRIX matView=IndieLib.IRenderer.Getvv()
		IndieLib.D3DXFROMWINEMATRIX matProj=IndieLib.IRenderer.Getpp()
#if 1
  IndieLib.D3DXFROMWINEMATRIX matVP = (matView) * (matProj)
  IndieLib.D3DXFROMWINEMatrixTranspose(matVP, matVP)
#else
  IndieLib.D3DXFROMWINEMATRIX matVP = (matProj)*(matView)
#endif
  IndieLib.IRenderer.setShaderConstant4x4("WorldViewProjection", matVP)

  # Get right and up vectors from view transformation and store as constants
  IndieLib.D3DXFROMWINEVECTOR4 vecRight, vecUp
  IndieLib.D3DXFROMWINEVECTOR4 vr(matView.m[0][0], matView.m[1][0], matView.m[2][0],0.0)
  IndieLib.D3DXFROMWINEVECTOR4 vu(matView.m[0][1], matView.m[1][1], matView.m[2][1], 0.0)

  IndieLib.D3DXFROMWINEVec4Normalize(vecRight, vr)
  IndieLib.D3DXFROMWINEVec4Normalize(vecUp,    vu)
  IndieLib.IRenderer.setShaderConstant4("right", vecRight)
  IndieLib.IRenderer.setShaderConstant4("up", vecUp)
  
	def Render(bA):
  DWORD LastTexture = -1
  
  int bM=0

  # MUST CALL BEGIN BEFORE CALLING RENDER!!!
  

  # Go through each type of particle to draw
  # Chance to optimize - speed this up
  for(DWORD i=0i<NUM_PARTICLE_TYPESi++) 

    # Start at first particle
    cParticle *Particle = m_Particles

    # Set a count for flushing buffer
    DWORD Num = 0
  

    # Lock the vertex buffer for use
	if(Ptr==0)
	
		Ptr=sShaderVertex[m_NumParticles * 4]
		Ptr0=Ptr
	
	else
		Ptr=Ptr0
    #m_VB.Lock(0, 0, (void**)Ptr, D3DLOCK_DISCARD)
  

    # Loop for all particles
    while((Particle != 0)and(((Ptr-Ptr0)/4)<m_NumParticles)) 
  

      # Do types match?
      if(Particle.m_Type == i) 
  

	  #iiii=(Ptr-Ptr0)/4
        # Set the type's texture (if not done already)
        #if(i != LastTexture) 
        #  LastTexture = i
        #

        # Copy over particle data
        float HalfSize = Particle.m_Size * 0.5f

#ifdef __IB__
        Ptr[0].vecPos    = Particle.m_vecPos
        Ptr[0].vecOffset = IndieLib.D3DXFROMWINEVECTOR2(-HalfSize, HalfSize)
#        Ptr[0].Diffuse   = Particle.m_Color
        Ptr[0].u         = 0.0
        Ptr[0].v         = 0.0
        Ptr[1].vecPos    = Particle.m_vecPos
        Ptr[1].vecOffset = IndieLib.D3DXFROMWINEVECTOR2(HalfSize, HalfSize)
  #      Ptr[1].Diffuse   = Particle.m_Color
        Ptr[1].u         = 1.0
        Ptr[1].v         = 0.0
        Ptr[2].vecPos    = Particle.m_vecPos
        Ptr[2].vecOffset = IndieLib.D3DXFROMWINEVECTOR2(-HalfSize, -HalfSize)
    #    Ptr[2].Diffuse   = Particle.m_Color
        Ptr[2].u         = 0.0
        Ptr[2].v         = 1.0
        Ptr[3].vecPos    = Particle.m_vecPos
        Ptr[3].vecOffset = IndieLib.D3DXFROMWINEVECTOR2(HalfSize, -HalfSize)
      #  Ptr[3].Diffuse   = Particle.m_Color
        Ptr[3].u         = 1.0
        Ptr[3].v         = 1.0
		#LOG_PRINT("i=%d",(Ptr-Ptr0)/4)
        Ptr+=4
#else
        Ptr[0].vecPos    = Particle.m_vecPos
        Ptr[0].vecOffset = IndieLib.D3DXFROMWINEVECTOR2(HalfSize, -HalfSize)
#        Ptr[0].Diffuse   = Particle.m_Color
        Ptr[0].u         = 1.0
        Ptr[0].v         = 1.0

        Ptr[1].vecPos    = Particle.m_vecPos
        Ptr[1].vecOffset = IndieLib.D3DXFROMWINEVECTOR2(HalfSize, HalfSize)
  #      Ptr[1].Diffuse   = Particle.m_Color
        Ptr[1].u         = 1.0
        Ptr[1].v         = 0.0

        Ptr[2].vecPos    = Particle.m_vecPos
        Ptr[2].vecOffset = IndieLib.D3DXFROMWINEVECTOR2(-HalfSize, -HalfSize)
    #    Ptr[2].Diffuse   = Particle.m_Color
        Ptr[2].u         = 0.0
        Ptr[2].v         = 1.0

        Ptr[3].vecPos    = Particle.m_vecPos
        Ptr[3].vecOffset = IndieLib.D3DXFROMWINEVECTOR2(-HalfSize, HalfSize)
      #  Ptr[3].Diffuse   = Particle.m_Color
        Ptr[3].u         = 0.0
        Ptr[3].v         = 0.0
		#LOG_PRINT("i=%d",(Ptr-Ptr0)/4)
        Ptr+=4
#endif
		

        # Increase vertex count and flush buffer if full
        Num++
        if(Num >= m_NumParticles) 
  

          # Unlock buffer and render polygons
          #m_VB.Unlock()

  m_VB=IndieLib.IRenderer.addVertexBuffer(m_NumParticles * 4 * sizeof(sShaderVertex),STATIC,Ptr0,m_VB)
    

	Begin()
	  

	IndieLib.IRenderer.setTexture("Sampler", m_pTextures[i])
  # Set the stream sources
  IndieLib.IRenderer.setVertexBuffer(0,m_VB)
  #ifdef __IB__
  IndieLib.IRenderer.setIndexBuffer(m_IB)
#endif
	IndieLib.IRenderer.GetRendererInstance().setRasterizerState(IndieLib.IRenderer.GetRendererInstance().GetnoDepthTest())
	#if(bA) 
    	#IndieLib.IRenderer.GetRendererInstance().setBlendState(IndieLib.IRenderer.GetRendererInstance().GetblendSrcAlpha())
	
    IndieLib.IRenderer.GetRendererInstance().changeAlphaState(alphaState)

#ifdef __IB__
          IndieLib.IRenderer.DrawIndexedPrimitive(IndieLib.PRIM_TRIANGLES, 0, 0, Num*4, 0, Num*2)
#else
          IndieLib.IRenderer.DrawPrimitive(IndieLib.PRIM_TRIANGLES, 0, Num*2)
#endif
          # Lock vertex buffer again
          #m_VB.Lock(0, 0, (void**)Ptr, D3DLOCK_DISCARD)

          # Clear vertex count
          Num=0
  

		
  

	  
  

      # Go to next particle
      Particle = Particle.m_Next
  

	
  

    # Unlock vertex buffer
    #m_VB.Unlock()
  # Create the vertex buffer
  m_VB=IndieLib.IRenderer.addVertexBuffer(m_NumParticles * 4 * sizeof(sShaderVertex),STATIC,Ptr0,m_VB)

  

    # Render any polygons left
    if(Num)
    
  

		Begin()
  

  IndieLib.IRenderer.setTexture("Sampler", m_pTextures[i])
  # Set the stream sources
  IndieLib.IRenderer.setVertexBuffer(0,m_VB)
 #ifdef __IB__
  IndieLib.IRenderer.setIndexBuffer(m_IB)
#endif
	IndieLib.IRenderer.GetRendererInstance().setRasterizerState(IndieLib.IRenderer.GetRendererInstance().GetnoDepthTest())

#ifdef __IB__
      IndieLib.IRenderer.DrawIndexedPrimitive(IndieLib.PRIM_TRIANGLES, 0, 0, Num*4, 0, Num*2)
#else
      IndieLib.IRenderer.DrawPrimitive(IndieLib.PRIM_TRIANGLES, 0, Num*2)
#endif

	def Process(Elapsed):
		cParticle *Particle = m_Particles

		while(Particle != 0) 

			cParticle *NextParticle = Particle.m_Next

			BOOL RemoveFromList = False

			if(Particle.m_Life) 
      				if(Particle.m_Life <= Elapsed)
        				RemoveFromList = True
      				else
        				Particle.m_Life -= Elapsed
    			

    			float Scalar = Elapsed / 1000.0

    			Particle.m_vecPos += (Particle.m_vecVelocity * Scalar)

    			if(RemoveFromList == True) 

    			if(Particle.m_Prev)
        Particle.m_Prev.m_Next = Particle.m_Next
    			else
        m_Particles = Particle.m_Next

    			if(Particle.m_Next)
        Particle.m_Next.m_Prev = Particle.m_Prev

    			Particle.m_Prev = 0
    			Particle.m_Next = 0
    			delete Particle
    			
			Particle = NextParticle

	def HandleSmoke(IndieLib.D3DXFROMWINEVECTOR3 *vecPos, DWORD Elapsed):
		static DWORD Timer = 0

		Timer += Elapsed
		if(Timer > 66) 
    Timer = 0

		rot = ((rand() % 628) / 100.0)
	static IndieLib.D3DXFROMWINEVECTOR3 v1=IndieLib.D3DXFROMWINEVECTOR3(vecPos.x, 5.0, vecPos.z)
	static IndieLib.D3DXFROMWINEVECTOR3 v2=IndieLib.D3DXFROMWINEVECTOR3(cos(rot)*20.0, 0.0, sin(rot)*20.0)
    Add(PARTICLE_SMOKE, v1,
        10.0, IndieLib.D3DXFROMWINEVECTOR4(255.0, 34.0, 34.0, 34.0), 1000,
        v2)
  


	def HandleDucking(vecPos):
		Particle = m_Particles
		while(Particle) 

			IndieLib.D3DXFROMWINEVECTOR3 vecDiff = Particle.m_vecPos - (*vecPos)
			Dist = vecDiff.x*vecDiff.x + vecDiff.z*vecDiff.z

			if(Dist < (40.0 * 40.0))
      				Particle.m_Type = PARTICLE_PEOPLE2
			else
      				Particle.m_Type = PARTICLE_PEOPLE1

			Particle = Particle.m_Next

class AppPy:
	def __init__(self):
		
		self.g_vecChopper = IndieLib.D3DXFROMWINEVECTOR3(0.0, 50.0, 0.0)
		self.g_szClass = "VSParticlesClass"
		self.g_szCaption = "Vertex Shader Particles Demo by Jim Adams"

		self.g_matProj=IndieLib.D3DXFROMWINEMATRIX()

		self.g_ChopperEmitter
		self.g_TreeEmitter
		self.g_PeopleEmitter
		self.g_ChopperMesh  
		self.g_RotorMesh    
		self.g_ShadowMesh   
		self.g_BackdropMesh 
		self.g_rotChopper 

	def init( width,  height):
		IndieLib.IRenderer.GetRendererInstance().GetTransform(TS_PROJECTION, g_matProj)

		g_ChopperEmitter=cParticleEmitter()
		g_TreeEmitter=cParticleEmitter()
		g_PeopleEmitter=cParticleEmitter()
		g_ChopperMesh=AssimpView::MeshRenderer("/AdvAniCh12/chopper.X")
		g_RotorMesh=AssimpView::MeshRenderer("/AdvAniCh12/Rotor.X")
		g_ShadowMesh=AssimpView::MeshRenderer("/AdvAniCh12/Shadow.X")

		g_BackdropMesh=AssimpView::MeshRenderer("/AdvAniCh12/Backdrop.x")

		IndieLib.D3DXFROMWINEVECTOR3 O(0.0, 0.0, 0.0)
		g_ChopperEmitter.Create(IRenderer::GetRendererInstance(), O, EMITTER_CLOUD)
		g_TreeEmitter.Create( IRenderer::GetRendererInstance(),   O, EMITTER_TREE)
		g_PeopleEmitter.Create(IRenderer::GetRendererInstance(),  O, EMITTER_PEOPLE)

		DWORD Type = PARTICLE_TREE1
		for(DWORD i=0i<50i++) 
			float rot  = (rand() % 628) / 100.0
    float dist = (rand() % 400)
    IndieLib.D3DXFROMWINEVECTOR3 o(cos(rot)*dist,25.0,sin(rot)*dist)
    IndieLib.D3DXFROMWINEVECTOR3 t(0.0, 0.0, 0.0)

    g_TreeEmitter.Add(Type++, o, 50.0, IndieLib.D3DXFROMWINEVECTOR4(1.0, 1.0, 1.0, 1.0), 0, t)
    if(Type > PARTICLE_TREE3)
      Type = PARTICLE_TREE1

    rot  = (rand() % 628) / 100.0
    dist = (rand() % 400)
    IndieLib.D3DXFROMWINEVECTOR3 v1(cos(rot)*dist,7.5,sin(rot)*dist)
    IndieLib.D3DXFROMWINEVECTOR3 v2(0.0, 0.0, 0.0)

    g_PeopleEmitter.Add(PARTICLE_PEOPLE1, v1, 15.0, IndieLib.D3DXFROMWINEVECTOR4(1.0, 1.0, 1.0, 1.0), 0, v2)

  

		STX_Service::GetAudioInstance().Init()
		STX_Service::GetAudioInstance().Load("/AdvAniCh12/chopper.wav", "Chopper")

	def render():
		STX_Service::GetAudioInstance().Play("Chopper")
	
		static DWORD LastTime = timeGetTime()/1000.0
		DWORD Time = timeGetTime()/1000.0
		DWORD Elapsed
		IndieLib.D3DXFROMWINEMATRIX matWorld
		static SmokeTimer = 0

		Elapsed = Time - LastTime
		LastTime = Time

		g_rotChopper = Time / 5000.0
		g_vecChopper.x += cos(g_rotChopper) * (Elapsed / 30.0)
		g_vecChopper.z -= sin(g_rotChopper) * (Elapsed / 30.0)

		g_ChopperEmitter.HandleSmoke(g_vecChopper, Elapsed)

		g_PeopleEmitter.HandleDucking(g_vecChopper)

		g_ChopperEmitter.Process(Elapsed)

		IndieLib.D3DXFROMWINEMATRIX matProj
		IndieLib.D3DXFROMWINEMatrixPerspectiveFovLH( matProj, IndieLib.D3DXFROMWINEToRadian( 45.0 ),IndieLib.IRenderer.GetRendererInstance().GetAspect(), 0.001, 20000.0
								)
		IndieLib.IRenderer.GetRendererInstance().SetTransform( TS_PROJECTION, matProj )

		IndieLib.D3DXFROMWINEMATRIX matView
		IndieLib.D3DXFROMWINEVECTOR3 v1(g_vecChopper.x, 80.0, g_vecChopper.z - 150.0)
		IndieLib.D3DXFROMWINEVECTOR3 v2(g_vecChopper.x, 0.0, g_vecChopper.z)
		IndieLib.D3DXFROMWINEVECTOR3 v3(0.0, 1.0, 0.0)

		IndieLib.D3DXFROMWINEMatrixLookAtLH(matView, v1, v2, v3)
		IndieLib.IRenderer.GetRendererInstance().SetTransform(TS_VIEW, matView)

		IndieLib.IRenderer.GetRendererInstance().Clear(True,True,IndieLib.D3DXFROMWINEVECTOR4(0.35, 0.53, 0.7, 1.0))#IndieLib.D3DXFROMWINEVECTOR4(0,0,64,255))
		IndieLib.IRenderer.GetRendererInstance().BeginScene()

		IndieLib.D3DXFROMWINEMatrixIdentity(matWorld)
		g_BackdropMesh.render(matWorld)#0.0,0.0,0.0,1.0,1.0,1.0,0.0,0.0,0.0)

		IndieLib.D3DXFROMWINEMatrixTranslation(matWorld, g_vecChopper.x, 0.5, g_vecChopper.z)
		g_ShadowMesh.render(matWorld)#g_vecChopper.x, 0.5, g_vecChopper.z,1.0,1.0,1.0,0.0,0.0,0.0)

		IndieLib.D3DXFROMWINEMatrixRotationYawPitchRoll(matWorld, g_rotChopper, 0.0, -0.0872222f)
		matWorld.m[3][0] = g_vecChopper.x  # Set position manually in matrix
		matWorld.m[3][1] = g_vecChopper.y
		matWorld.m[3][2] = g_vecChopper.z
		g_ChopperMesh.render(matWorld)

		x='''
    IndieLib.D3DXFROMWINEMATRIX matRotY, matRotZ, matRotYY
    IndieLib.D3DXFROMWINEMatrixRotationY(matRotY, Time / 10.0)  # Turn rotor
    IndieLib.D3DXFROMWINEMatrixRotationZ(matRotZ, -0.174444)           # Angle forward
    IndieLib.D3DXFROMWINEMatrixRotationY(matRotYY, g_rotChopper)        # Orient to chopper
    IndieLib.D3DXFROMWINEMatrixTranslation(matWorld, g_vecChopper.x, g_vecChopper.y, g_vecChopper.z)
    IndieLib.IRenderer.GetRendererInstance().SetTransform(TS_WORLD, (matRotY * matRotZ * matRotYY * matWorld))

		'''
		g_RotorMesh.render(g_vecChopper.x, g_vecChopper.y, g_vecChopper.z,1.0,1.0,1.0,0.0, Time / 10.0,-0.174444)
		IndieLib.IRenderer.GetRendererInstance().SetRenderState(D3DRS_ALPHATESTENABLE, True)
		IndieLib.IRenderer.GetRendererInstance().SetRenderState(D3DRS_ALPHAREF, 0x08)
		IndieLib.IRenderer.GetRendererInstance().SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL)

		g_TreeEmitter.Render(1)
	
		g_PeopleEmitter.Render(1)
	
		g_ChopperEmitter.Render(1)

		STX_Service::GetAudioInstance().Update()

		IndieLib.IRenderer.GetRendererInstance().EndScene()

		IndieLib.IRenderer.GetRendererInstance().Present()

if __name__=="__main__":
	app=AppPy()
	IndieLib.IRenderer.GetRendererInstance("ParticleVS")		
	while ((not IndieLib.STX_Service.GetInputInstance().OnKeyPress (IndieLib.STX_KEY_ESCAPE)) and (not IndieLib.STX_Service.GetInputInstance().Quit())):
		IndieLib.STX_Service.GetInputInstance().Update()	
		app.render()

