Let the dice roll ...

### Building

- Get Qt 5.3 here: http://qt-project.org/
- Get the repo and open `zeitdice.pro`
- Hit 'Ctrl/Cmd + B' (Build) and observe the Issues tab
- Paste the output for evaluation with me :)

### Agile Roadmap

- [x] Qt - Get the big picture and get going
- [x] FFMpeg libraries working inside Qt project
- [x] Decode images and reencode as video through libav* pipeline
- [x] Size and Pixelformat conversions through libswscale (Enable adaptive realtime performance)
- [x] High performance display pipeline from ffmpeg buffers to Qt Window
  - [x] Rendering through hardware-accelerated QGLWidget / Possibly costly QImage detour
  - (Rendering through hardware-accelerated QGLWidget / Direct transfer to QPixmap with reconstructed in-buffer image headers - dropped)
  - (Rendering through hardware-accelerated QGLWidget / Texture transfer through OpenGL FBOs/PBOs (e.g. http://www.songho.ca/opengl/gl_pbo.html#unpack) - dropped)
- [x] Demo filters through libavfilter
  - [x] Implement reference filter pipeline from buffersource to buffersink
  - [x] Get it to work (filter param initialization process cleanup)
  - [X] Wrap and expose 1-3 demo filters in the libzeit API
  - [X] Integrate the demo filters in the UX
- [x] Clean up and restructure libzeit API for realtime display/final render functions
  - [x] Threaded encoding and decoding, Signals and Slots based interface updates
  - [x] API/Program flow restructuring
- [x] Implement timeslider/selector for setting the timelapse speed in the UI
- (Implement Ken-Burns Effect - dropped)
- [ ] Round off the Demo UX
  - [x] Decoder and scaler memory audit and patch
  - [x] Fix for stray filter bug
  - [x] Filter pipeline memory audit and patch
  - [x] Draft movie export implementation
  - [x] Debug and round off movie exporting
  - [ ] Yet another filter memory audit
  - [ ] Folder select on startup?
  - (Vimeo upload if cheap - I'd rather in the next version)
- (Tackle a prototype implementation for startup on zeitdice device detection)
  - (Mac Support - I'd rather in the next version)
  - (Linux Support - dropped)
