# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.0.1] - 2025-01-21

### Breaking Changes

- Renamed addon folder from `libsm64-godot` to `libsm64_godot`.

### Changed

- Default value for `LibSM64.scale_factor` has been changed to `100.0`. All demo scenes have been updated to reflect this change.
- `LibSM64.get_current_background_music` and `LibSM64.play_music` now use plain `int` instead of `LibSM64.SeqId` due to how sequence priority flags work.

### Fixed

- Stop demo scenes from crashing if ROM wasn't provided.

## [2.0.0] - 2025-01-18

- Initial release of plugin version 2.
