from PIL import Image

'''
def add_logo(mfname, lfname, outfname):

    mimage = Image.open(mfname)
    limage = Image.open(lfname)

    # resize logo
    wsize = int(min(mimage.size[0], mimage.size[1]) * 0.25)
    wpercent = (wsize / float(limage.size[0]))
    hsize = int((float(limage.size[1]) * float(wpercent)))

    simage = limage.resize((wsize, hsize))
    mbox = mimage.getbbox()
    sbox = simage.getbbox()

    # right bottom corner
    box = (mbox[2] - sbox[2], mbox[3] - sbox[3])
    mimage.paste(simage, box)
    mimage.save(outfname)
'''
'''
i1=Image.open("../../IndieLib_resources/textures/ATI_SDK/256px-brick.bmp")
i2=Image.open("../../IndieLib_resources/textures/NVIDIA_Corporation/2d/relief_rockwall.jpg")
i2=i2.resize((256, 256))
i3=Image.open("../../IndieLib_resources/RadeonTerrainDemo/CastleDoor.bmp")
i3=i3.resize((85, 85))
print "", i1.size[0], i1.size[0]
print "", i2.size[0], i2.size[0]
print "", i3.size[0], i3.size[0]
i1.paste(i3, (85, 75))
i2.paste(i3, (85, 77))
i1.save("../../IndieLib_resources/RadeonTerrainDemo/CastleWindow256px.png")
i2.save("../../IndieLib_resources/RadeonTerrainDemo/CastleWindow256px2.png")
'''
i6=Image.open("../../IndieLib_resources/textures/ATI_SDK/Spectrum.png")
i6=i6.resize((256,18))
i0=Image.open("../../IndieLib_resources/VertexDisplacement/explosion.png") # 9x128
i0=i0.transpose(Image.ROTATE_90)
i1=Image.open("../../IndieLib_resources/textures/NVIDIA_Corporation/1D/fire_gradient3.png") # 256x1
i1=i1.resize((256,18))
i2=Image.open("../../IndieLib_resources/textures/NVIDIA_Corporation/1D/FireGrade.png") # 256x1 
i2=i2.resize((256,18))
#i3=Image.open("../../IndieLib_resources/textures/NVIDIA_Corporation/1D/grad1.png") # 256x32
#i4=Image.open("../../IndieLib_resources/textures/NVIDIA_Corporation/1D/grad2.png") # 256x32
i3=Image.open("../../IndieLib_resources/textures/NVIDIA_Corporation/1D/grad3.png") # 256x1 
i3=i3.resize((256,18))
i4=Image.open("../../IndieLib_resources/Smoke/FireTransferFunction.png") # 2048x1
i4=i4.resize((256,18))
i5=Image.open("../../IndieLib_resources/R2VB-Sort/FirePalette.png") # 256x1
i5=i5.resize((256,18))
i0.save("../../IndieLib_resources/textures/NVIDIA_Corporation/2d/explosion.png") # 9x128
i1.save("../../IndieLib_resources/textures/NVIDIA_Corporation/2d/fire_gradient3.png") # 256x1
i2.save("../../IndieLib_resources/textures/NVIDIA_Corporation/2d/FireGrade.png") # 256x1
i3.save("../../IndieLib_resources/textures/NVIDIA_Corporation/2d/grad3.png") # 256x1 
i4.save("../../IndieLib_resources/textures/NVIDIA_Corporation/2d/FireTransferFunction.png") # 2048x1
i5.save("../../IndieLib_resources/textures/NVIDIA_Corporation/2d/FirePalette.png")
i6.save("../../IndieLib_resources/textures/NVIDIA_Corporation/2d/Spectrum.png")
'''
out = Image.new("RGB", (768, 768), "white")

s=3
for y in range(0,s):
	for x in range(0,s):
		#out.paste(Image.open("../../IndieLib_resources/textures/ATI_SDK/256px-brick.bmp"), (x*256, y*256))
		out.paste(Image.open("../../IndieLib_resources/textures/NVIDIA_Corporation/2d/relief_rockwall.jpg"), (x*256, y*256))

out.paste(Image.open("../../IndieLib_resources/RadeonTerrainDemo/CastleDoor.bmp"), (256, 256))

out.resize((256, 256))

out.save("../../IndieLib_resources/RadeonTerrainDemo/CastleWindow256px2.png")
'''
'''
out = Image.new("RGB", (256, 1), "white")
pixels = out.load()
for i in range(out.size[0]):
    for j in range(1):
        pixels[i,j] = (0, 0 ,0)
out.save("../../IndieLib_resources/Colors256px1px.png")
out = Image.new("RGB", (256, 2), "white")
pixels = out.load()
for i in range(out.size[0]):
    for j in range(2):
        pixels[i,j] = (0, 0 ,0)
out.save("../../IndieLib_resources/Colors256px2px.png")
out = Image.new("RGB", (256, 3), "white")
pixels = out.load()
for i in range(out.size[0]):
    for j in range(3):
        pixels[i,j] = (0, 0 ,0)
out.save("../../IndieLib_resources/Colors256px3px.png")
out = Image.new("RGB", (256, 4), "white")
pixels = out.load()
for i in range(out.size[0]):
    for j in range(4):
        pixels[i,j] = (0, 0 ,0)
out.save("../../IndieLib_resources/Colors256px4px.png")
out = Image.new("RGB", (256, 5), "white")
pixels = out.load()
for i in range(out.size[0]):
    for j in range(5):
        pixels[i,j] = (0, 0 ,0)
out.save("../../IndieLib_resources/Colors256px5px.png")
'''

