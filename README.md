# Zeitdice application

Qt 5.4.1 / FFmpeg / x264

# Build Notes Mac

- Build-> Clean All
- Build->Run gmake
- In build....-Release folder, edit makefile: LIBS = .... line, take out all -framework Qt* entries
- Run make in release folder



### Future development notes

- Rendering pipeline long term perspective
  - **Current state** - Rendering through hardware-accelerated QGLWidget / Possibly costly QImage detour
  - Rendering through hardware-accelerated QGLWidget / Direct transfer to QPixmap with reconstructed in-buffer image headers (rather odd though)
  - Rendering through hardware-accelerated QGLWidget / Texture transfer through OpenGL FBOs/PBOs (e.g. http://www.songho.ca/opengl/gl_pbo.html#unpack)
  - Reboot the caching approach?

- Feature future perspective
  - Video upload straight to the internets and social media
  - Filter tweaking and generic, expandable interface
  - Ken-Burns Effect
