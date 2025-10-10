# Test GAP file
export,filename=test.gbin,type=gbin,format=binary

loadtilemap, src="testfiles/tmx/tilemap.tmx", type="tiled:tmx"
tilemap, id=1, layer=1

soundsample, name="testwav", src="testfiles/audio/test_22050_pcm16_le.raw", format="S16", srcformat="S16", srcrate=22050, rate=22050
