class_name LibSM64Global
extends Object


const ROM_SHA256 := "17ce077343c6133f8c9f2d6d6d9a4ab62c8cd2aa57c40aea1f490b4c8bb21d91"

static var rom := PackedByteArray()
static var mario_texture := ImageTexture.new()

static var _audio_init_once := false


static func init() -> bool:
	if rom.is_empty():
		push_error("ROM not loaded")
		return false

	var mario_image := LibSM64.global_init(rom)
	mario_texture = ImageTexture.create_from_image(mario_image)

	# Don't initialize audio more than once (it causes a memory leak)
	if not _audio_init_once:
		LibSM64.audio_init(rom)
		_audio_init_once = true

	return true


static func terminate() -> void:
	LibSM64.global_terminate()
	mario_texture = null


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
