class_name LibSM64Global
extends Object

## A helper class for managing the global state of the [code]libsm64[/code] library.
##
## Provides functionality to load and initialize the Super Mario 64 ROM file,
## manage the global state of the library, and handle Mario's texture.


## The SHA256 hash of the expected ROM file. [method load_rom_file] will fail if the hash of the loaded ROM file does not match this value.
## The expected ROM file is the US version of Super Mario 64.
## The hash can be calculated using the following command, for example:
## [code]sha256sum "Super Mario 64 (USA).z64"[/code]
const ROM_FILE_SHA256 := "17ce077343c6133f8c9f2d6d6d9a4ab62c8cd2aa57c40aea1f490b4c8bb21d91"

## The SHA256 hash of the base64 encoded string of the expected ROM file. [method load_rom_from_base64_string] will fail if the base64 encoded string of the loaded ROM file does not match this value.
## The expected ROM file is the US version of Super Mario 64.
## This is used when loading the ROM file in the Web export.
const ROM_BASE64_ENCODED_SHA256 := "4a07a7862d48234293aa2c47f1feb4672671c3ffa825f0fa2970b9c14bc75348"

## The loaded ROM file.
## The ROM file should be loaded using [method load_rom_file] or [method load_rom_from_base64_string] before calling [method init].
static var rom := PackedByteArray()

## Mario's texture resource. This texture is created when [method init] is called sucessfully. [LibSM64Mario] will automatically use this texture.
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
static func load_rom_file(rom_path: String, skip_checksum_validation := false) -> bool:
	if not FileAccess.file_exists(rom_path):
		push_error("ROM file not found")
		return false

	if not skip_checksum_validation:
		var rom_file_sha256 := FileAccess.get_sha256(rom_path)
		if rom_file_sha256 != ROM_FILE_SHA256:
			push_error("ROM file's SHA256 mismatch, expected: ", ROM_FILE_SHA256, " recieved: ", rom_file_sha256)
			return false

	rom = FileAccess.get_file_as_bytes(rom_path)
	if rom.is_empty():
		push_error("Failed to read ROM file, error: ", FileAccess.get_open_error())
		return false

	return true


## Loads the ROM file from the given base64 string. Useful for loading the ROM file in the Web export
## If [param skip_checksum_validation] is [code]true[/code], the checksum validation will be skipped ([b]Warning[/b]: invalid ROM files will cause the library to crash).
## Returns [code]true[/code] if the ROM file was loaded successfully.
static func load_rom_from_base64_string(rom_base64: String, skip_checksum_validation := false) -> bool:
	if not skip_checksum_validation:
		var rom_base64_sha256 := rom_base64.sha256_text()
		if rom_base64_sha256 != ROM_BASE64_ENCODED_SHA256:
			push_error("Base64 encoded ROM's SHA256 mismatch, expected: ", ROM_BASE64_ENCODED_SHA256, " recieved: ", rom_base64_sha256)
			return false

	rom = Marshalls.base64_to_raw(rom_base64)
	if rom.is_empty():
		push_error("Failed to decode base64 encoded ROM")
		return false

	return true
