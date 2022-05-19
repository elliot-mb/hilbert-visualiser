# Hilbert curve data visualiser
A somewhat esoteric way to visualise the data of any file; this program produces a png (using the [stb_image_write](https://github.com/nothings/stb) header/lib) that displays the bytes of a file such that all "structure" is preseved (groups of bytes remain close in the image if they were in the original file). This is done by mapping the bytes as greyscale/colour pixels to successive vertices of a pseudo-Hilbert curve. 
# A pseudo-Hilbert curve <img align="right" height="250" src="https://user-images.githubusercontent.com/45922387/167229973-9ed7e180-04b8-44c0-a658-76196d71b486.png"></img>
As far as I'm aware, in pure mathematics, a [Hilbert curve](https://www.wikiwand.com/en/Hilbert_curve) is an infinite space-filling line that passes through every point in a two-dimensional (or larger) space. Computing a finite version of this curve happens to be one of the best ways to map one dimension to two dimensions such that two data points close together in 1D remains close together in 2D space. Alternative mappings like placing the pixels along rows from left to right down the image would not represent the data in the file as naturally wrt. localised groups of data.
# Usage
Provide a file and the program writes a png ``hilbert.png``, which is the image.\
**A word of warning** - the program mallocs an array equal to the file you give; only use this with files less than a couple hundered MB unless you know you have the memory.
```./a.out --help
Usage: ./[OUTFILE] [OPTN] [FILE]
The OUTFILE is the name of the compiled program (typically 'a.out' by default).
   -b -> shows file in bytes.
   -c -> shows file in colour (triplets of bytes).
The FILE argument is the path of any file stored as bytes.
```
## Examples
The binary of this program on my computer\
<img align="center" height="600" src="https://user-images.githubusercontent.com/45922387/167229933-55db91e3-330a-4093-916a-540f64d56881.png">

The C code\
<img align="center" height="200" src="https://user-images.githubusercontent.com/45922387/167229892-7ad317e5-bec6-42ba-9534-5f0df5a16cc7.png">

Some art I created using another program to make a "rainbow" in bytes\
<img align="center" height="400" src="https://user-images.githubusercontent.com/45922387/167229973-9ed7e180-04b8-44c0-a658-76196d71b486.png">
