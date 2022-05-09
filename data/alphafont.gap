# Test GAP file
# gap --mount="/home/ade/source/pixelart" --mount="../data" system.gap

export,name=system,filename=alphafont.gbin,type=gbin,format=binary

imagegroup,group=0

loadimage,src="/fonts/namco-classic.png",format=L8
tileset,id=1,name="font",width=8,height=8,format=L8
tilearray,width=95,height=1

 