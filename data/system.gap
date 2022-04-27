# Test GAP file
# gap --mount="/home/ade/source/pixelart" --mount="../data" system.gap

export,name=system,filename=system.gbin,type=gbin,format=binary

imagegroup,group=0

loadimage,src="/fonts/namco-classic.png",format=L8
tileset,id=1,name="sysfont1-horz",width=8,height=8,format=L8
tilearray,width=95,height=1
tileset,id=2,name="sysfont1-vert",width=8,height=8,format=L8
tilearray,width=95,height=1,rotate=270

 