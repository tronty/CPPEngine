#
#  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
#  Licensed under The GNU Lesser General Public License, version 2.1:
#      http://opensource.org/licenses/LGPL-2.1
#

import string

fn__=[
'/Lightning/chain_target.X',
'/Lightning/scene.X',
'/AdvAniCh12/chopper.X',
'/AdvAniCh12/Shadow.X',
'/AdvAniCh12/Rotor.X',
'/skeleton/skeleton.x',
'/ProceduralBuilding2.x',
'/dx9sdk/sphere.x',
'/dx9sdk/cube.x',
'/dx9sdk/sphere2.x',
'/dx9sdk/teapot.x',
'/dx9sdk/tiger.x',
'/dx9sdk/tiny.x',
'/Smoke/Gargoyle/gargoyle.x',
'/20-Explosions/Ch20p2_ExplosionCluster/Ch20p2_object.x',
'/20-Explosions/Ch20p3_ComplexExplosion/Ch20p3_object.x',
'/20-Explosions/Ch20p1_SimpleExplosion/Ch20p1_object.x',
'/Billboard/seafloor.x',
'/Billboard/skybox2.x',
'/DXJune2010/Dwarf/Dwarf.x',
'/DXJune2010/Dwarf/DwarfWithEffectInstance.x',
'/DXJune2010/trees/skybox2.x',
'/15_Ocean_Water/skybox.x',
'/Orcs/Bonfire_wOrcs.x',
'/trex_dx/dino_videogame.x',
'/trex_dx/dino_videogame2.x',
'/GLEssentials/Assets/demon.x',
'/AdvAniCh12/backdrop.x',
'/assimp--1.0.412-sdk/dwarf.x',
'/assimp--1.0.412-sdk/test.x',
]

fnani=[
'/dx9sdk/tiny.x',
'/assimp--1.0.412-sdk/dwarf.x',
'/dx9sdk/sphere.x',
'/dx9sdk/sphere2.x',
]

fnok=[
'/ProceduralBuilding2.x',
'/skeleton/skeleton.x',
'/Sparkles/cat.x',
'/GLEssentials/Assets/demon.x',
'/assimp--1.0.412-sdk/test.x',
'/dx9sdk/teapot.x',
'/dx9sdk/cube.x',
'/dx9sdk/tiger.x',
'/DXJune2010/Dwarf/Dwarf.x',
'/DXJune2010/Dwarf/DwarfWithEffectInstance.x',
]
fnnotok=[
'/AdvAniCh12/Shadow.X',
'/AdvAniCh12/chopper.X',
'/AdvAniCh12/Rotor.X',
'/Lightning/chain_target.X',
'/Lightning/scene.X',
'/DXJune2010/trees/skybox2.x',
'/Smoke/Gargoyle/gargoyle.x',
'/Billboard/seafloor.x',
'/trex_dx/dino_videogame.x',
'/trex_dx/dino_videogame2.x',
]
fn_=[
'/20-Explosions/Ch20p1_SimpleExplosion/Ch20p1_object.x',
'/20-Explosions/Ch20p3_ComplexExplosion/Ch20p3_object.x',
'/20-Explosions/Ch20p2_ExplosionCluster/Ch20p2_object.x',
'/AdvAniCh12/backdrop.x',
'/Billboard/skybox2.x',
'/15_Ocean_Water/skybox.x',
'/Orcs/Bonfire_wOrcs.x',
]

#fn=['/dx9sdk/sphere.x']
#fn=['/skeleton/skeleton.x']
fn=['/dx9sdk/tiny.x']
#fn=fnani
#fn=fnok
fn=['/ProceduralBuilding2.x']
fn=fnok+fnnotok+fn_
fn=fn__

def run():
	symbols= ['AnimationSet','Animation','AnimationKey','Frame', 'FrameTransformMatrix',	'Mesh', 'MeshNormals', 'MeshTextureCoords', 'MeshMaterialList', 'Material', 'TextureFilename']
	symbols_=['AnimationSet','Animation','AnimationKey',					'Mesh', 'MeshNormals', 'MeshTextureCoords', 'MeshMaterialList', 'Material', 'TextureFilename']
	for i in fn:
		print i
		f=open('../../../IndieLib_resources'+i, 'r')
    		lines=f.readlines()
    		f.close()
    		txt=string.join(lines,'')
		txt=txt.replace(';;;',' %endblock% ')
		txt=txt.replace(';;',' %endblock% ')
		txt=txt.replace(';,','')
		txt=txt.replace(';',' ')
		txt=txt.replace(',',' ')
		txt=txt.replace('TextureFileName','TextureFilename')
		#txt=txt.replace('TextureFilename',' %endblock% TextureFilename')
		v=txt.split()
		txt=txt.replace('}',' ')
		v=txt.split()
		'''
		temp=[]
		temp.append([float(z) for z in v if is_number(z)])
		print temp
		'''
		#print 'len(txt)=', len(txt)
		#print 'len(v)=', len(v)
		for symbol in symbols:
			#n=len([k for k in xrange(len(v)) if v[k]==symbol])
			if True:
				if symbol in ['Mesh', 'MeshNormals', 'MeshTextureCoords', 'MeshMaterialList']:
					indices=[j for j in xrange(len(v)) if v[j]==symbol]
					for indice_ in indices:
						if (indice_) and (v[indice_-1]=='template'):
							continue
						ln= symbol+': '
						TextureFilenames=[]
						if (symbol in ['Material','FrameTransformMatrix']):
							continue
						elif (v[indice_]=='Mesh') and (v[indice_+2]=='{'):
							c = 3*int(v[indice_+3],10)#+28 # +28 ???
							'''
							print 'c:', c
							print 'idx2:',indice_
							print 'idx2:',indice_+3+c
							'''
							ln='%s %s VN: %s IN: %s' % (ln, v[indice_+1], v[indice_+3], v[indice_+5+c])
						elif (symbol=='MeshMaterialList'):
							ln='%s MN: %s FN: %s' % (ln, v[indice_+2], v[indice_+3])
						elif v[indice_]=='MeshTextureCoords':
							ln='%s VN: %s' % (ln, v[indice_+2])
						else:
							c = 3*int(v[indice_+2],10) # +28 ???
							'''
							print 'c:', c
							print 'idx2:',indice_
							print 'idx2:',indice_+2+c
							'''
							ln='%s VN: %s IN: %s' % (ln, v[indice_+2], v[indice_+4+c])
						print ln
				
				texs=[]
				if symbol in ['TextureFilename']:
					indices=[j for j in xrange(len(v)) if v[j]==symbol]
					for indice_ in indices:
						if (indice_) and (v[indice_-1]=='template'):
							continue
						texs.append(v[indice_+2].replace('\"',''))				
						texs = list(dict.fromkeys(texs))
				if len(texs):
					ln=''
					if len(texs)>1:
						ln='TextureFiles N: %d' % (len(texs))
					elif len(texs[0]):
						ln='TextureFile: %s' % (texs[0])
					print ln
run()

