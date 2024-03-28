extends MeshInstance3D

func _on_area_3d_area_entered(area):
	if not area.get_parent() is SM64Mario:
		return
		
	var _mario = area.get_parent() as SM64Mario
	if _mario.action == SM64MarioAction.JUMP:
		self.queue_free()
