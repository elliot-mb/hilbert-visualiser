# Hilbert Curve data visualiser
A useful way to visualise the data of any file - a Hilbert Curve is one of the best ways to map one dimension to two dimensions such that two data points close together in 1D remains close together in 2D space.\
This is important to preserve the structure of data, giving an intutive way to visualise it.
# Usage
Provide a file and the program writes a png ``hilbert.png``, which is the image.
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
