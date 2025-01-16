class_name LibSM64Global
extends Object

## A helper class for managing the global state of the [code]libsm64[/code] library.
##
## Provides functionality to load and initialize the Super Mario 64 ROM file,
## manage the global state of the library, and handle Mario's texture.


## The SHA256 hash of the expected ROM file. [member load_rom_file] will fail if the hash of the loaded ROM file does not match this value.
## The expected ROM file is the US version of Super Mario 64 (U) [!].zip.
## The hash can be calculated using the following command, for example:
## [code]sha256sum "Super Mario 64 (USA).z64"[/code]
const ROM_SHA256 := "17ce077343c6133f8c9f2d6d6d9a4ab62c8cd2aa57c40aea1f490b4c8bb21d91"

## The loaded ROM file.
## The ROM file should be loaded using [member load_rom_file] before calling [member init].
static var rom := PackedByteArray()

## Mario's texture resource. This texture is created when [member init] is called sucessfully. [LibSM64Mario] will automatically use this texture.
static var mario_texture := ImageTexture.new()

static var _audio_init_once := false


## Initializes the global state and the audio systems of the library (ROM file should be loaded before calling this function).
## Returns [code]true[/code] if the global state was initialized successfully.
static func init() -> bool:
	if rom.is_empty():
		push_error("ROM not loaded")
		return false

	var mario_image := LibSM64.global_init(rom)
	mario_texture = ImageTexture.create_from_image(mario_image)

	# Don't initialize audio more than once (it causes a memory leak)
	if not _audio_init_once:
		_audio_init_once = true
		LibSM64.audio_init(rom)

	return true


## Terminates the global state of the library.
static func terminate() -> void:
	LibSM64.global_terminate()
	mario_texture = null


## Loads the ROM file from the given path.
## If [param skip_checksum_validation] is [code]true[/code], the checksum validation will be skipped ([b]Warning[/b]: invalid ROM files will cause the library to crash).
## Returns [code]true[/code] if the ROM file was loaded successfully.
static func load_rom_file(path: String, skip_checksum_validation := false) -> bool:
	if not FileAccess.file_exists(path):
		push_error("ROM file not found")
		return false

	if not skip_checksum_validation:
		var rom_file_sha256 := FileAccess.get_sha256(path)
		if rom_file_sha256 != ROM_SHA256:
			push_error("ROM file's SHA256 mismatch, expected: ", ROM_SHA256, " recieved: ", rom_file_sha256)
			return false

	rom = FileAccess.get_file_as_bytes(path)
	if rom.is_empty():
		push_error("Failed to read ROM file, error: ", FileAccess.get_open_error())
		return false

	return true
