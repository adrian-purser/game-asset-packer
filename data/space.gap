# Test GAP file
# src/gap --mount="/home/ade/source/pixelart" --mount="./data" space.gap
# src/gap --mount="~/source/pixelart" --mount="./data" space.gap

export,name=space,filename=assets.gbin,type=gbin,format=binary

imagegroup,group=0


loadimage,src="/sprites/ship1.png"
image,width=32,height=32,x=0,y=0,xorigin=16,yorigin=16,format=ARGB1555
image,width=32,height=32,x=0,y=0,xorigin=16,yorigin=16,format=ARGB1555,rotate=30
image,width=32,height=32,x=0,y=0,xorigin=16,yorigin=16,format=ARGB1555,rotate=45
image,width=32,height=32,x=0,y=0,xorigin=16,yorigin=16,format=ARGB1555,rotate=60
image,width=32,height=32,x=0,y=0,xorigin=16,yorigin=16,format=ARGB1555,rotate=75
image,width=32,height=32,x=0,y=0,xorigin=16,yorigin=16,format=ARGB1555,rotate=90
image,width=32,height=32,x=0,y=0,xorigin=16,yorigin=16,format=ARGB1555,rotate=105
image,width=32,height=32,x=0,y=0,xorigin=16,yorigin=16,format=ARGB1555,rotate=120
image,width=32,height=32,x=0,y=0,xorigin=16,yorigin=16,format=ARGB1555,rotate=135
image,width=32,height=32,x=0,y=0,xorigin=16,yorigin=16,format=ARGB1555,rotate=150
image,width=32,height=32,x=0,y=0,xorigin=16,yorigin=16,format=ARGB1555,rotate=165
image,width=32,height=32,x=0,y=0,xorigin=16,yorigin=16,format=ARGB1555,rotate=180
image,width=32,height=32,x=0,y=0,xorigin=16,yorigin=16,format=ARGB1555,rotate=195
image,width=32,height=32,x=0,y=0,xorigin=16,yorigin=16,format=ARGB1555,rotate=210
image,width=32,height=32,x=0,y=0,xorigin=16,yorigin=16,format=ARGB1555,rotate=225
image,width=32,height=32,x=0,y=0,xorigin=16,yorigin=16,format=ARGB1555,rotate=240
image,width=32,height=32,x=0,y=0,xorigin=16,yorigin=16,format=ARGB1555,rotate=255
image,width=32,height=32,x=0,y=0,xorigin=16,yorigin=16,format=ARGB1555,rotate=270
image,width=32,height=32,x=0,y=0,xorigin=16,yorigin=16,format=ARGB1555,rotate=285
image,width=32,height=32,x=0,y=0,xorigin=16,yorigin=16,format=ARGB1555,rotate=300
image,width=32,height=32,x=0,y=0,xorigin=16,yorigin=16,format=ARGB1555,rotate=315
image,width=32,height=32,x=0,y=0,xorigin=16,yorigin=16,format=ARGB1555,rotate=330
image,width=32,height=32,x=0,y=0,xorigin=16,yorigin=16,format=ARGB1555,rotate=345

#loadimage,src="/sprites/test32x32.png"
#image,width=32,height=32,x=0,y=0,xorigin=24,yorigin=0,format=ARGB1555
#image,width=32,height=32,x=0,y=0,xorigin=24,yorigin=16,format=ARGB1555,rotate=90

loadimage,src="/fonts/major-title.png"
tileset,id=1,name="font1-horz",width=8,height=8,format=ARGB1555
tilearray,width=95,height=1
tileset,id=2,name="font1-vert",width=8,height=8,format=ARGB1555
tilearray,width=95,height=1,rotate=270

