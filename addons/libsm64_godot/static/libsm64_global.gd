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

## The loaded ROM file.
## The ROM file should be loaded using [method load_rom_file] or [method load_rom_from_base64_string] before calling [method init].
static var rom: PackedByteArray

## Mario's texture resource. This texture is created when [method init] is called sucessfully. [LibSM64Mario] will automatically use this texture.
static var mario_texture: ImageTexture

static var _audio_init_once := false


## Whether the global state of the library is currently initialized.
## Set by [method init] and cleared by [method terminate].
static var is_initialized := false


## Initializes the global state and the audio systems of the library (ROM file should be loaded before calling this function).
## Calling this method while the library is already initialized does nothing (it only pushes a warning); initializing a
## second time would destroy all existing Marios and surfaces, since [code]libsm64[/code] rebuilds its global state.
## Returns [code]true[/code] if the global state was initialized successfully.
static func init() -> bool:
	if is_initialized:
		push_warning("LibSM64Global is already initialized")
		return true

	if rom.is_empty():
		push_error("ROM not loaded")
		return false

	var mario_image := LibSM64.global_init(rom)
	mario_texture = ImageTexture.create_from_image(mario_image)

	# Don't initialize audio more than once (it causes a memory leak)
	if not _audio_init_once:
		_audio_init_once = true
		LibSM64.audio_init(rom)

	is_initialized = true

	return true


## Terminates the global state of the library.
## Calling this method while the library is not initialized does nothing.
static func terminate() -> void:
	if not is_initialized:
		return

	LibSM64.global_terminate()
	mario_texture = null
	is_initialized = false


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
	# Line breaks and spaces are common in base64 payloads, and they don't change the decoded bytes.
	var cleaned_base64 := rom_base64.replace("\n", "").replace("\r", "").replace("\t", "").replace(" ", "")

	var rom_bytes := Marshalls.base64_to_raw(cleaned_base64)
	if rom_bytes.is_empty():
		push_error("Failed to decode base64 encoded ROM")
		return false

	if not skip_checksum_validation:
		var hashing_context := HashingContext.new()
		hashing_context.start(HashingContext.HASH_SHA256)
		hashing_context.update(rom_bytes)
		var rom_sha256 := hashing_context.finish().hex_encode()
		if rom_sha256 != ROM_FILE_SHA256:
			push_error("ROM's SHA256 mismatch, expected: ", ROM_FILE_SHA256, " recieved: ", rom_sha256)
			return false

	rom = rom_bytes

	return true
