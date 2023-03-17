###linux sound visualization with openGl and pulseaudio.

###Dependencies:

pulseaudio, glfw, glew, openGl, fftw3.

openGL 4.5

```
git clone https://github.com/thraen/pde_sound_visualization_engine.git
cd pde_sound_visualization_engine; make
./a.out <pulse-audio-source>
```

`./a.out alsa_output.pci-0000_00_1f.3.analog-stereo.monitor`
