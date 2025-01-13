TODO
====

- [ ] Remove group number from IMAGEGROUP command.
      - [ ] Remove GROUP keyword from IMAGEGROUP command parser.
      - [ ] Change asset group array to a vector instead of fixed size array.
            Each time a group is added it will be appended to the group array.
- [ ] Add 'hflip' and 'vflip' parameters to 'image' and 'imagearray' commands.
- [ ] Add 'verbose' and 'quiet' flags to limit the amount of output generated.
- [ ] Load palette files. format type examples are paint.net(txt), JASC(pal), Gimp(gpl)
- [ ] Export 'CMAP' colour map chunks
- [ ] Sample Sounds
      - [ ] Define sample sound chunks
      - [ ] Define sample sound commands
      - [ ] Load RAW sound sample files
      - [ ] Generate SWAV chunk


DONE
----

- [x] Add header guard to generated C++ headers.

