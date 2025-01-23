# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.2.0] - 2025-01-22

### Added

- Added `LibSM64Mario.reset_interpolation()` method.

### Changed

- `LibSM64MarioInterpolator` doesn't populate its arrays and instantiate its properties in the constructor anymore. This eliminates an editor warning. `LibSM64Mario` has been updated to reflect this change.

### Fixed

- Check for `return_to_menu()` method in the `HUD` node.
- Surpress unused variable warning in the `camera_rig.tscn` scene.
- `LibSM64Mario.create()` method now accounts for the node's `global_rotation` property.
- Properly reset `LibSM64Mario`'s interpolation when setting properties.
- Only tick `LibSM64Mario`'s when `LibSM64Mario.create()` has been called sucessfully.
- Fix angles conversions between Godot angles and `libsm64` angles.

## [2.1.0] - 2025-01-21

### Added

- Add `"libsm64/scale_factor"` project setting. This setting can be used to set `LibSM64.scale_factor` project wide (`LibSM64.scale_factor` can still be set directly in code if needed).

## [2.0.1] - 2025-01-21

### Breaking Changes

- Renamed addon folder from `libsm64-godot` to `libsm64_godot`.

### Changed

- Change value for `LibSM64.scale_factor` to `100.0`. All demo scenes have been updated to reflect this change.
- `LibSM64.get_current_background_music()` and `LibSM64.play_music()` methods now use plain `int` instead of `LibSM64.SeqId` due to how sequence priority flags work.

### Fixed

- Stop demo scenes from crashing if ROM wasn't provided.

## [2.0.0] - 2025-01-18

- Initial release of plugin version 2.
