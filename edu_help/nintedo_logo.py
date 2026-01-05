import matplotlib.pyplot as plt
import numpy as np

logo_bytes = np.array(
    [
        0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,
        0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
        0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
        0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
        0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
        0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E,
    ],
    dtype=np.uint8,
)

TILE_SIZE = 8
BYTES_PER_TILE = 16
num_tiles = len(logo_bytes) // BYTES_PER_TILE  # = 3 tiles

# Create empty image for 3 horizontal tiles
image = np.zeros((TILE_SIZE, num_tiles * TILE_SIZE), dtype=np.uint8)

# Process each tile
for tile_num in range(num_tiles):
    tile_start = tile_num * BYTES_PER_TILE
    
    # Process each row in the tile (8 rows per tile)
    for row in range(TILE_SIZE):
        lsb = logo_bytes[tile_start + row * 2]
        msb = logo_bytes[tile_start + row * 2 + 1]
        
        # Process each bit in the row
        for bit in range(8):
            low = bool(lsb & (1 << (7 - bit)))
            high = bool(msb & (1 << (7 - bit)))
            value = (1 if low else 0) + (2 if high else 0)
            
            # Place pixel in correct position
            image[row, tile_num * TILE_SIZE + bit] = value

print(f"Image shape: {image.shape}")

plt.figure(figsize=(8, 3))
plt.imshow(image, cmap='gray', vmin=0, vmax=3, interpolation='nearest')
plt.axis('off')
plt.title('Game Boy Logo')
plt.tight_layout()
plt.show()