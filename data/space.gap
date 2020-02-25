# Test GAP file
# src/gap --mount="/home/ade/source/pixelart" --mount="./data" space.gap
# src/gap --mount="~/source/pixelart" --mount="./data" space.gap

export,filename=assets.gbin,type=gbin,format=binary

loadimage,src="/sprites/ship1.png"
imagegroup,group=0
image,width=32,height=32,x=0,y=0,xorigin=16,yorigin=0,format=ARGB1555

loadimage,src="/fonts/major-title.png"
tileset,id=1,name="font1-horz",width=8,height=8,format=ARGB1555
tilearray,width=95,height=1
tileset,id=2,name="font1-vert",width=8,height=8,format=ARGB1555
tilearray,width=95,height=1,rotate=270

