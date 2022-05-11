# Test GAP file
# gap --mount="/home/ade/source/pixelart" --mount="../data" luatest.gap

export,name=luatest,filename=luatest.gbin,type=gbin,format=binary

imagegroup,group=0,name="PLAYER"

loadimage,src="/sprites/ship1.png"
image,xorigin=16,yorigin=16,format=ARGB1555


colourmap,src="testfiles/palette/zenith.gpl",name="zenith"

file,src="/testfiles/main.luac",name="main.luac",type="LUAC"


 