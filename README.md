# C Image Compressor/Encoder Demo

## Method

This image compressor follow the JPEG compression method of the following steps, and what has been completed as a first iteration in this repo:

- [x] RGB -> YCbCr (colour space conversion, see known issues)
- [ ] Downsampling (will achieve a further 50% compression when completed)
- [x] Discrete Cosine Transform (DCT)
- [x] Quantization
- [x] Huffman Coding (see future goals)

## Current issues

- Some artifcating on the image causing a blur effect

- Colour space conversion, not sure if the issue stems from the encoder or decoder side, but luminosity is reading perfectly, colour seems to be having issues.

## Important Goals

- [ ] Remove all dyanamic memory allocations
- [ ] Redo API
- [ ] Consolidate into one header and one source file for ease of use

### Huffman Coding

The Huffman Coding that is currently implemented uses predefined Huffman tables, which means they are not created at rnutime. This means the compression acheived by Huffman Coding is not optimal. A possible future development is passing the image twice to generate dynamic tables at runtime for further compression.
Dynamic tables will come at an expense of speed and memory, so it may not be something we would like to do.

## Known Limitations

This encoder is a (relatively) simple implementation of the JPEG method, and has the limitation of only processing an 8x8 chunk at a time. For example, a 1024x1024 fully black image would be much better compressed by modern file standards as this compressor can only "see" an 8x8 block, so the image in this example would have lots of unnecessary data.

## Image Example

Original:

![original](https://cdn.discordapp.com/attachments/922288944585015336/1000489307112480869/flower.jpg)

Compressed (~50% in the outputted .bin file, including the colour data which is currently not working):

![original](https://cdn.discordapp.com/attachments/922288944585015336/1000489307364134922/flower.bin_decoded.jpg)
