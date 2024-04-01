extends MeshInstance3D

func _on_area_3d_area_entered(area):
	if not area.get_parent() is SM64Mario:
		return
	
	var mario = area.get_parent() as SM64Mario
	if mario.action in [SM64MarioAction.GROUND_POUND_LAND, SM64MarioAction.GROUND_POUND]:
		self.queue_free()
		
		# ya stomp, ya win.
		mario.action = SM64MarioAction.STAR_DANCE_NO_EXIT
		await get_tree().create_timer(1).timeout
