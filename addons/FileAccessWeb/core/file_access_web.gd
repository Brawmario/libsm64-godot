class_name FileAccessWeb
extends RefCounted

signal load_started(file_name: String)
signal loaded(file_name: String, file_type: String, base64_data: String)
signal progress(current_bytes: int, total_bytes: int)
signal error()

var _file_uploading: JavaScriptObject

var _on_file_load_start_callback: JavaScriptObject
var _on_file_loaded_callback: JavaScriptObject
var _on_file_progress_callback: JavaScriptObject
var _on_file_error_callback: JavaScriptObject

func _init() -> void:
	if _is_not_web():
		_notify_error()
		return

	JavaScriptBridge.eval(js_source_code, true)
	_file_uploading = JavaScriptBridge.get_interface("godotFileAccessWeb")

	_on_file_load_start_callback = JavaScriptBridge.create_callback(_on_file_load_start)
	_on_file_loaded_callback = JavaScriptBridge.create_callback(_on_file_loaded)
	_on_file_progress_callback = JavaScriptBridge.create_callback(_on_file_progress)
	_on_file_error_callback = JavaScriptBridge.create_callback(_on_file_error)

	_file_uploading.setLoadStartCallback(_on_file_load_start_callback)
	_file_uploading.setLoadedCallback(_on_file_loaded_callback)
	_file_uploading.setProgressCallback(_on_file_progress_callback)
	_file_uploading.setErrorCallback(_on_file_error_callback)

func open(accept_files: String = "*") -> void:
	if _is_not_web():
		_notify_error()
		return

	_file_uploading.setAcceptFiles(accept_files)
	_file_uploading.open()

func _is_not_web() -> bool:
	return OS.get_name() != "Web"

func _notify_error() -> void:
	push_error("File Access Web worked only for HTML5 platform export!")

func _on_file_load_start(args: Array) -> void:
	var file_name: String = args[0]
	load_started.emit(file_name)

func _on_file_loaded(args: Array) -> void:
	var file_name: String = args[0]
	var splitted_args: PackedStringArray = args[1].split(",", true, 1)
	var file_type: String = splitted_args[0].get_slice(":", 1). get_slice(";", 0)
	var base64_data: String = splitted_args[1]
	loaded.emit(file_name, file_type, base64_data)

func _on_file_progress(args: Array) -> void:
	var current_bytes: int = args[0]
	var total_bytes: int = args[1]
	progress.emit(current_bytes, total_bytes)

func _on_file_error(args: Array) -> void:
	error.emit()

const js_source_code = """
function godotFileAccessWebStart() {
	var loadedCallback;
	var progressCallback;
	var errorCallback;
	var loadStartCallback;

	var input = document.createElement("input");
	input.setAttribute("type", "file")

	var interface = {
		setLoadedCallback: (loaded) => loadedCallback = loaded,
		setProgressCallback: (progress) => progressCallback = progress,
		setErrorCallback: (error) => errorCallback = error,
		setLoadStartCallback: (start) => loadStartCallback = start,

		setAcceptFiles: (files) => input.setAttribute("accept", files),
		open: () => input.click()
	}

	input.onchange = (event) => {
		var file = event.target.files[0];

		var reader = new FileReader();
		reader.readAsDataURL(file)

		reader.onloadstart = (loadStartEvent) => {
			loadStartCallback(file.name);
		}

		reader.onload = (readerEvent) => {
			if (readerEvent.target.readyState === FileReader.DONE) {
				loadedCallback(file.name, readerEvent.target.result);
			}
		}

		reader.onprogress = (progressEvent) => {
			if (progressEvent.lengthComputable)
				progressCallback(progressEvent.loaded, progressEvent.total);
		}

		reader.onerror = (errorEvent) => {
			errorCallback();
		}
	}

	return interface;
}

var godotFileAccessWeb = godotFileAccessWebStart();
"""
