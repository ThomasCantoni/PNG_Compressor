import struct
import zlib
def read_chunk(f):
    # Returns (chunk_type, chunk_data)
    chunk_length, chunk_type = struct.unpack('>I4s', f.read(8))
    chunk_data = f.read(chunk_length)
    checksum = zlib.crc32(chunk_data, zlib.crc32(struct.pack('>4s', chunk_type)))
    chunk_crc, = struct.unpack('>I', f.read(4))
    if chunk_crc != checksum:
        raise Exception('chunk checksum failed {} != {}'.format(chunk_crc,
            checksum))
    return chunk_type, chunk_data

def PaethPredictor(a, b, c):
    p = a + b - c
    pa = abs(p - a)
    pb = abs(p - b)
    pc = abs(p - c)
    if pa <= pb and pa <= pc:
        Pr = a
    elif pb <= pc:
        Pr = b
    else:
        Pr = c
    return Pr

f = open("test.png",'rb')

# i need to see if the png i opened has the png signature
PngSignature = b'\x89PNG\r\n\x1a\n' 
if f.read(len(PngSignature)) != PngSignature:
    raise Exception('Invalid PNG Signature')




chunks = []
while True:
    chunk_type, chunk_data = read_chunk(f)
    chunks.append((chunk_type, chunk_data))
    if chunk_type == b'IEND':
        break

print([chunk_type for chunk_type, chunk_data in chunks])


_internal, IHDR_data = chunks[0] # IHDR is always first chunk
width, height, bitd, colort, compm, filterm, interlacem = struct.unpack('>IIBBBBB', IHDR_data)
if compm != 0:
    raise Exception('invalid compression method')
if filterm != 0:
    raise Exception('invalid filter method')

print(width, height)

raw_image_data = [
    [255, 0, 0, 255, 0, 0, 255, 255], # First scanline
    [0, 255, 0, 255, 255, 255, 255, 255], # Second scanline
]

filtered_image_data = [
    [0, 255, 0, 0, 255, 0, 0, 255, 255], # First scanline, first item (0) specifies filter type 0
    [0, 0, 255, 0, 255, 255, 255, 255, 255], # Second scanline, first item (0) specifies filter type 0
]
IDAT_data = b''.join(chunk_data for chunk_type, chunk_data in chunks if chunk_type == b'IDAT')
IDAT_data = zlib.decompress(IDAT_data)
print(len(IDAT_data))

Recon = []
bytesPerPixel = 4
stride = width * bytesPerPixel # the stride is the number of bytes in each line of the image (padding included)

def Recon_a(r, c):
    return Recon[r * stride + c - bytesPerPixel] if c >= bytesPerPixel else 0

def Recon_b(r, c):
    return Recon[(r-1) * stride + c] if r > 0 else 0

def Recon_c(r, c):
    return Recon[(r-1) * stride + c - bytesPerPixel] if r > 0 and c >= bytesPerPixel else 0

i = 0
for r in range(height): # for each scanline
    filter_type = IDAT_data[i] # first byte of scanline is filter type
    i += 1
    for c in range(stride): # for each byte in scanline
        Filt_x = IDAT_data[i]
        i += 1
        if filter_type == 0: # None
            Recon_x = Filt_x
        elif filter_type == 1: # Sub
            Recon_x = Filt_x + Recon_a(r, c)
        elif filter_type == 2: # Up
            Recon_x = Filt_x + Recon_b(r, c)
        elif filter_type == 3: # Average
            Recon_x = Filt_x + (Recon_a(r, c) + Recon_b(r, c)) // 2
        elif filter_type == 4: # Paeth
            Recon_x = Filt_x + PaethPredictor(Recon_a(r, c), Recon_b(r, c), Recon_c(r, c))
        else:
            raise Exception('unknown filter type: ' + str(filter_type))
        Recon.append(Recon_x & 0xff) # truncation to byte

import matplotlib.pyplot as plt
import numpy as np
plt.imshow(np.array(Recon).reshape((height, width, 4)))
plt.show()