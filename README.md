A linux sound visualization and/or simple PDE-solver engine using openGl and alsalib. 
One thread captures an alsa device and another thread feeds the fourier data into a simple opengl engine. 
By using a render to texture technique, it is capable of solving simple time-dependent partial differential with finite differences methods 
on the shader hardware.

Dependencies: alsalib, glut, glew, openGl, fftw3. 

To use it as a visualization, an alsa loopback device is required.

The visualization can be modified by changing active_fragment_shader.gl

