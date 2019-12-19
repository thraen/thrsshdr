###A linux sound visualization engine using openGl and pulseaudio.

A simple toy for playing with fragment shaders much like 
https://www.shadertoy.com/. Fourier analyzed sound data captured from an pulseaudio 
source is made available to a fragment shader on a window spanning rect.

The last two rendered images are also fed into the shader 
as textures, i.e. time derivative information is available which allows 
implementation of 2D-finite-differences schemes for time-2nd-order PDEs.

Examples for simple solvers for advection, diffusion and wave equations are
included. 

[video1] (https://www.youtube.com/embed/uh3lMbHAPew)
[video2] (https://www.youtube.com/watch?v=9KmWyDt9fkk)

The visualization can be modified at runtime by changing 
link.frag and pressing 'r' in the visualization window.

###Dependencies:

It links against: pulseaudio, glut, glew, openGl, fftw3.

Your graphics card must support openGL 4.5.

Usage:
```
git clone https://github.com/thraen/pde_sound_visualization_engine.git
cd pde_sound_visualization_engine; make
./a.out <pulse-audio-source>
```

for example `./a.out alsa_output.pci-0000_00_1f.3.analog-stereo.monitor`

pulseaudio makes monitor sources readily available,
making it easy to visualize sound currently played.
