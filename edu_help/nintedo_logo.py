import matplotlib.pyplot as plt
import numpy as np

logo_bytes = np.array([
    0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,
    0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
    0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
    0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
    0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
    0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
], dtype=np.uint8)

# Constants
TILE_SIZE = 8
BYTES_PER_TILE = 16
TILES_X = 3
TILES_Y = 2

# Assertions (correct)
assert len(logo_bytes) == 48
assert TILES_X * TILES_Y == len(logo_bytes) // BYTES_PER_TILE

# Image buffer
width = TILES_X * TILE_SIZE
height = TILES_Y * TILE_SIZE
image = np.zeros((height, width), dtype=np.uint8)

palette = [255, 170, 85, 0]

num_tiles = len(logo_bytes) // BYTES_PER_TILE  # = 6

for tile_index in range(num_tiles):
    tile_x = tile_index % TILES_X
    tile_y = tile_index // TILES_X

    tile_base = tile_index * BYTES_PER_TILE

    for row in range(8):
        low = logo_bytes[tile_base + row * 2]
        high = logo_bytes[tile_base + row * 2 + 1]

        for col in range(8):
            bit = 7 - col
            color_id = ((high >> bit) & 1) << 1 | ((low >> bit) & 1)

            x = tile_x * 8 + col
            y = tile_y * 8 + row
            image[y, x] = palette[color_id]

plt.imshow(image, cmap="gray", interpolation="nearest")
plt.axis("off")
plt.title("Nintendo Game Boy Boot Logo")
plt.show()
