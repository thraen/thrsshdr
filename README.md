###linux sound visualization with openGl and pulseaudio.

###Dependencies:

pulseaudio, glfw, glew, openGl, fftw3.

install dependencies on ubuntu: 
sudo apt install libglfw3-dev libfftw3-dev libpulse-dev

openGL 4.5

```
git clone https://github.com/thraen/thrshdr.git
cd thrshdr; make
./a.out <pulse-audio-source>
```

`./a.out alsa_output.pci-0000_00_1f.3.analog-stereo.monitor`
