# Test GAP file
export,filename=assets.gbin,type=gbin,format=binary

loadimage,src="/fonts/outline.png"
imagegroup,group=0,base=33
imagearray,width=8,height=8,xcount=93,ycount=1,format=ARGB1555

loadimage,src="/3rd_party/ForgottenDungeon.png",format=RGB565
imagegroup,group=4
imagearray,width=16,height=16,xcount=3,ycount=3,format=ARGB1555
imagearray,width=16,height=16,xcount=3,ycount=1,format=ARGB1555,x=0,y=64

#image,x=16,y=16,width=16,height=16,name=TILE_01
#image,x=48,y=16,width=16,height=16,format=ARGB8888
