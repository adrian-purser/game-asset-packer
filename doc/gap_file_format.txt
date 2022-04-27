GAP File Format
===============


Each line that is not empty or a comment contains a command. The structure of the command can be seen below -

<command>[,<parameter]...

where parameter is a key/value pair -

<key>[=<value>]

e.g.,

loadimage,src="test.png"

Note: The value is optional for certain parameters.

Commands
========

+-----------------+---------------------------------------------------------------------------------------------+
| Command         |  Description                                                                                |
+-----------------+---------------------------------------------------------------------------------------------+
| EXPORT          | Export the data to a file in a specified format                                             |
| IMAGE           | Add an image to the current image group.                                                    |
| IMAGEARRAY      | Add a series of images from a 2 dimensional array within the source image.                  | 
| IMAGEGROUP      | Set the active image group. Following images will be added to this group.                   |
| LOADIMAGE       | Load a source image. Following image related commands will use this as source.              |
| TILE            | Add a tile to the current tileset                                                           |
| TILEARRAY       | Add a series of tiles from a 2 dimensional array of images from the source image.           |
| TILESET         | Create an empty tileset or select an existing tileset.                                      |
+-----------------+---------------------------------------------------------------------------------------------+



EXPORT
------

Export the data to a file in a specified format.

FILENAME or PATH             The filename of the target file.
NAME                         The name of the package.
TYPE                         The type of file. Currently only type GBIN is supported.
FORMAT                       The format or encoding of the output file. Supported values are BINARY, C_ARRAY, 
                             CPP_VECTOR and CPP_STDARRAY.

FILE
----

Add a file to the package. The file will be loaded as binary and added as uncompressed data.  The files will be added
to the package in a single list. There is no folder structure so all files should be given a unique name.

SRC                          The filename/path of the file on the local filesystem.
NAME                         The name that the file will be given in the package. If not specified then the
                             source filename will be used. There is a limit of 15 characters. A file extension
                             is not required.
TYPE                         The type of the file. This is a 4CC so there is a maximum 4 characters.


IMAGE
-----

Add an image to the current imagegroup. The image is taken as a subimage of the current source image if the subimage 
coordinates are specified, otherwise the whole source image is used.

X                    X coordinate in pixels of the top left of the source region within the parent image
Y                    Y coordinate in pixels of the top left of the source region within the parent image
W or WIDTH           Width of the image in pixels
H or HEIGHT          Height of the image in pixels
XO or XORIGIN        X Origin of the image in pixels. The image will be rendered with the origin placed at 
                     the specified coordinate. Rotations will use this as the origin of rotation
YO or YORIGIN        Y Origin of the image in pixels. The image will be rendered with the origin placed at 
                     the specified coordinate. Rotations will use this as the origin of rotation
ANGLE ot ROTATE      Angle in degrees to rotate the image. The image will be rotated around the origin specified by XO,YO
PF or FORMAT         Pixel format pf the output image. If not specified then the format of the source image will be used.
NAME                 Name of the image so that it can be found by a search


IMAGEARRAY
----------

Create a series of images from a square 2 dimensional array within the source image. Each image will have the same 
dimensions and pixel format.


X                    X coordinate in pixels of the top left of the source region within the parent image
Y                    Y coordinate in pixels of the top left of the source region within the parent image
W or WIDTH           Width of each image in pixels
H or HEIGHT          Height of each image in pixels
XO or XORIGIN        X Origin of each image in pixels. The image will be rendered with the origin placed at the specified 
                     coordinate. Rotations will use this as the origin of rotation
YO or YORIGIN        Y Origin of each image in pixels. The image will be rendered with the origin placed at the specified 
                     coordinate. Rotations will use this as the origin of rotation
XC or XCOUNT         The width of the image array measured in number of images. This value multiplied by YC will determine 
                     the total number of images.
YC or YCOUNT         The height of the image array measured in number of images. This value multiplied by XC will determine 
                     the total number of images.
PF or FORMAT         Pixel format pf the output image. If not specified then the format of the source image will be used.


IMAGEGROUP
----------

Images are placed into groups. This command selects the group that following images will be inserted into.  The BASE parameter 
can be used to specify the index that the following images will start at in the group.

IMAGEGROUP [,GROUP=<group-number>] [,BASE=<base>]

GROUP	               Group number
BASE                 Base index for images added to the group


LOADIMAGE
---------

LOADIMAGE [,SRC=<filename>] [,FORMAT=<output pixel format>]


SRC                  Filename of the image file
FORMAT               Desired output pixel format


TILE
----

Add a tile to the current tileset. Use the TILESET command to create or set the current tileset.

X                    X coordinate in pixels of the top left of the tiles source region within the parent image
Y                    Y coordinate in pixels of the top left of the tiles source region within the parent image
HFLIP                Flip the tile image horizontally. This parameter does not require a value.
VFLIP                Flip the tile image vertically. This parameter does not require a value.
ROTATE or ROTATION   Rotate the tile image. Valid values are 0, 90, 180 & 270 degrees.


TILEARRAY
---------

Add a series of tiles to the current tileset. The tiles are taken from a 2 dimensional array of sub-images
within the source image. 

X                            X coordinate in pixels of the top left of the tiles source region within the parent image
Y                            Y coordinate in pixels of the top left of the tiles source region within the parent image
TW or WIDTH or TILES-WIDE    The width of the 2 dimensional array measured in tiles.
TH or HEIGHT or TILES-HIGH   The height of the 2 dimensional array measured in tiles.
HFLIP                        Flip each tile image horizontally. This parameter does not require a value.
VFLIP                        Flip each tile image vertically. This parameter does not require a value.
ROTATE or ROTATION           Rotate each tile image. Valid values are 0, 90, 180 & 270 degrees.


TILESET
-------

Create an empty tileset or select an existing tileset.

ID                   Numerical identifier of the tileset.
NAME                 Name of the tileset that can be used for searching.
W or WIDTH           Width of each tile in pixels.
H or HEIGHT          Height of each tile in pixels.
PF or FORMAT         Pixel format pf the timeset image data. If not specified then the format of the source image will be used.

