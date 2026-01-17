# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.7.1] - 2026-01-17

### Fixed

- Fixed `LibSM64Mario`'s linear intepolation calculations, so that it doesn't rely on the raw tick number of the Godot Engine. In practice, this means that interpolation will now behave properly when pausing the `LibSM64Mario` node or when `Engine.time_scale` is diferrent than `1.0`.

## [2.7.0] - 2026-01-02

### Breaking Changes

- Removed `LibSM64.register_debug_print_function()` and `LibSM64.register_play_sound_function()` methods. They were replaced with equivalent `LibSM64.debug_print_function` and `LibSM64.play_sound_function` properties.

### Added

- More functions in the GDExtension module have been documented.

### Changed

- CI: compile the GNU/Linux GDExtension binaries in Ubuntu 22.04. This ensures that the release binaries works on glibc 2.35+ systems.

### Fixed

- Set `vertex_color_is_srgb` to `true` on Mario's materials. He should look less pale now.

## [2.6.1] - 2025-09-22

### Added

- Added UID files for Godot 4.4+.

### Fixed

- Reordered Mario's materials shader pass for Godot 4.5 compatibility.

## [2.6.0] - 2025-09-20

### Breaking Changes

- Removed `LibSM64Mario.set_forward_velocity()` function in favor of a `LibSM64Mario.forward_velocity` property. Replace all calls to this function with a statement to set the new property.

### Added

- Added `forward_velocity`, `anim_id`, and `anim_frame` properties to `LibSM64MarioState`.
- Added `forward_velocity`, `anim_id`, and `anim_frame` properties to `LibSM64Mario`.

### Removed

- Removed `LibSM64Mario.set_forward_velocity()` function (replaced by `LibSM64Mario.forward_velocity` property).

### Changed

- Update `godot-cpp` to the latest commit in the 4.3 branch.
- Update `libsm64` to latest commit. This latest commit exposes `forwardVelocity`, `animID`, and `animFrame` in `outState`.

## [2.5.1] - 2025-03-18

### Fixed

- Use `render_priority` in Mario's materials to make sure that the textures render over the vertices' colors. Not using this property was breaking Mario's texture rendering in Godot 4.4 ([#32](https://github.com/Brawmario/libsm64-godot/issues/32)).
- Remove the final newline in the `MARIO_INFO_FORMAT` format constant in `hud.gd`. This was causing an extra blank line in the HUD in Godot 4.4.

## [2.5.0] - 2025-03-10

### Added

- Add `LibSM64.MarioAnimID` enum, defining all available animations for Mario. `LibSM64.set_mario_animation` now uses this enum.

### Changed

- Update godot-cpp to the latest commit in the 4.3 branch. Notably, LTO optimization has been added to the build options (set to auto).

## [2.4.1] - 2025-02-10

### Changed

- Add `.clang-format`, `.clang-tidy`, and `.editorconfig` files (copied from the Godot Engine project) to project and apply their formating.
- Improve documentation in `LibSM64Global` and avoid creating default objects explicitly.
- Improve `LibSM64Mario` documentation and better segment the process functions behaviour in auxiliary functions.

## [2.4.0] - 2025-02-04

### Added

- Add `LibSM64.SoundBits` enum, defining all available sounds in `libsm64`. `LibSM64.play_sound` and `LibSM64.play_sound_global` now use this enum.
- Add documentation for `LibSM64MarioState` and `LibSM64MarioInterpolator`.

### Fixed

- Fix `LibSM64Mario.invicibility_time` typo to `LibSM64Mario.invincibility_time`.

## [2.3.1] - 2025-01-29

### Changed

- Use a dictionary internally in `LibSM64SurfaceObjectsHandler` instead of two arrays.
- Update `libsm64` to latest commit. Includes the changes necessary to fix `atan2f()` in the WebAssembly build and avoid exposing its public functions in the WebAssembly build (didn't work lol).
- Passing a null `Callable` to `LibSM64.register_debug_print_function` and `LibSM64.register_play_sound_function` now silently deactivates them.
- CI: Use `libsm64` commit SHA to avoid using stale caches.

## [2.3.0] - 2025-01-27

### Added

- New WebAssembly build target, targeting both single thread and multi thread exports. This allows the library to be used in web browsers.
- CI: Add a job to build the WebAssembly target.

### Changed

- CI: move the compilation of the GDExtension to its own action. Refractored the build workflow to use this action.
- Update godot-cpp to latest commit in the 4.3 branch.
- Change `BobOmbMinimal` to `BobOmbBattlefield`.

## [2.2.2] - 2025-01-26

### Changed

- Move `icon.svg` inside `libsm64_godot_demo` folder. This way the file is actually included in the Godot Asset Library package.

## [2.2.1] - 2025-01-25

### Changed

- All demo scenes can now be run directly from the editor. They will now query the user for the path to the SM64 ROM if it hasn't been loaded yet and set the necessary input actions at runtime.
- Completely reorganize the demo project to make it more discorverable.

### Fixed

- Fix internal angle conversion in the `LibSM64.set_mario_angle()` function.
- Remove `toggle_mouse_lock` input action from the example project. Replace its usage with `ui_cancel`.

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
