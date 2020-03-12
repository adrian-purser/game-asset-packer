# Test GAP file
# src/gap --mount="/home/ade/source/pixelart" --mount="./data" tilemap_test.gap

export,name=tilemap_test,filename=tilemap_test.gbin,type=gbin,format=binary

imagegroup,group=0

loadimage,src="/fonts/major-title.png"

#image,width=16,height=16,x=0,y=0,xorigin=0,yorigin=0,format=ARGB1555

tileset,id=1,name="font1-horz",width=8,height=8,format=ARGB1555
tilearray,width=95,height=1
tileset,id=2,name="font1-vert",width=8,height=8,format=ARGB1555
tilearray,width=95,height=1,rotate=270

loadimage,src="tilesets/test-tileset.png"
tileset,id=3,name="tiles",width=16,height=16,format=ARGB1555
tilearray,width=64,height=1

