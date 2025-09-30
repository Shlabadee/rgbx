# RGBX Encoder and Decoder
A barebones but functional 16-bit PNG to RGBx binary image converter with dithering.

You set the bit depth per channel (including alpha) as long as the total number of bits does not exceed 16.

To build, all that's needed regarding external libraries is libpng. Argparse is already included (but pending replacement).
