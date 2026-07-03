# LSRPG

LSRPG is an Unreal Engine 5.4 C++ RPG sample project built around Gameplay Ability System (GAS). It includes character abilities, replicated attributes, equipment, inventory, damage feedback, and basic UI flows.

## Requirements

- Unreal Engine 5.4
- Visual Studio 2022 / MSVC
- Git LFS

Binary Unreal assets such as `.uasset`, `.umap`, textures, and audio files are tracked with Git LFS. After cloning the repository, run:

```bash
git lfs install
git lfs pull
```

## Project Layout

- `Source/LSRPG/AbilitySystem`: GAS component, gameplay abilities, attribute sets, and damage execution logic
- `Source/LSRPG/Character`: base character and player character code
- `Source/LSRPG/Component`: combo, health, equipment, and inventory components
- `Source/LSRPG/Item`: equipment, potion, and token item types
- `Source/LSRPG/UI`: character info, equipment, inventory, damage number, and HUD widgets
- `Content/LSRPG`: Blueprints, abilities, items, maps, and UI assets

## Opening The Project

1. Clone the repository and pull Git LFS assets.
2. Open `LSRPG.uproject` with Unreal Engine 5.4.
3. If needed, regenerate Visual Studio project files from `LSRPG.uproject`.
4. Build and run the `LSRPGEditor` target.

The default map is configured in `Config/DefaultEngine.ini` as `/Game/LSRPG/Map/L_TestMap`.

## Notes

Generated folders and local build artifacts are ignored, including `Binaries/`, `Intermediate/`, `Saved/`, `DerivedDataCache/`, packaged output, and IDE cache files.
