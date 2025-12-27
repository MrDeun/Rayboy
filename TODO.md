# Rayboy — External TODO (non-invasive)

This file is a workspace-local TODO for tracking tasks without modifying project source files.

- [ ] Create external TODO file for project (non-invasive)
- [x] Document VRAM/Bus fixes
  - bus write: call `ppu_vram_write(address, value)` for 0x8000-0x9FFF
  - ppu_vram_write: use `address - 0x8000` (hex) as VRAM index
- [ ] Run emulator to verify VRAM tiles/map rendering

Notes:
- Do NOT edit existing source files here; this is a separate tracking file.
- To update this TODO, edit this file or request updates via the assistant.
