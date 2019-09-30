Binary File Format
==================


+----------------+
|     HEADER     |
+----------------+
|     CHUNK-0    |
+----------------+
|     CHUNK-1    |
+----------------+
:       .        :
:       .        :
:       .        :
+----------------+
|     CHUNK-n    |
+----------------+


HEADER
------

          -------------------->
        0        1        2        3
    +--------+--------+--------+--------+
$00 |   G    |   B    |   I    |   N    |			FourCC 'GBIN' (Game-Assets Binary) - 4 Bytes
    +--------+--------+--------+--------+
$04 | FLAGS  |      Version Number      |			Flags - 1 Byte. Version Number - 3 Bytes (ASCII Characters)
    +--------+--------------------------+
$08 |               CRC32               |			CRC32 of ALL data after this field - 4 Bytes 
    +-----------------------------------+

    +--------+--------+--------+--------+
$0C |   x    |   x    |   x    |   x    |			First Chunk
    +--------+--------+--------+--------+

HEADER FLAGS
------------

    +---+---+---+---+---+---+---+---+
    | 7 | 8 | 5 | 4 | 3 | 2 | 1 | 0 |  -  Bit Number
    +---+---+---+---+---+---+---+---+
    |   |   |   |   |   |   |   | * |  -  Endian Type. 0 = Little Endian, 1 = Big Endian
    +---+---+---+---+---+---+---+---+



CHUNK HEADER
------------

          -------------------->
        0        1        2        3
    +--------+--------+--------+--------+
$00 |   C    |   H    |   N    |   K    |			FourCC defines chunk type - 4 Bytes
    +-----------------------------------+
$04 |               SIZE                |			Chunk Size - 4 Bytes
    +-----------------------------------+
$08 |            CHUNK DATA             |			Chunk Data - 4 * n Bytes (Aligned to 4 byte boundary)
    :                                   :
		:                                   :
		|                                   |
    +-----------------------------------+


CHUNKS
------

+------+----------------------------------------------------------------------+
| IMGD | Image Data                                                           |
+------+----------------------------------------------------------------------+
| SIMG | Source Images - Dimensions, Pixel Format, Data Offset ...            |
|      | Defines the layout of the image data. This is different to the       |
|      | sub images used to draw sprites and tilemaps etc.                    |
+------+----------------------------------------------------------------------+
| IMAG | Images - Dimensions, Pixel Format, Data Offset ...                   |
|      | Defines individual image frames used to draw sprites and tilemaps etc|
+------+----------------------------------------------------------------------+
| CMAP | Colour Map - Used with indexed mode images                           |
+------+----------------------------------------------------------------------+
|      |                                                                      |
+------+----------------------------------------------------------------------+
|      |                                                                      |

