a simple, work-in-Progress Renderer for OpenGL

To do next:
- Add Point Shadows
- IMPROVE PERFORMANCE! Ideas:
    -> reduce number of Framebuffers by re-using framebuffers with several color-attachments
    -> improve cascaded Shadow Map: Draw all cascades in one texture with offsets
    -> do not update cascade view Frustum Matrix every frame, but only when the camera's fov changed
