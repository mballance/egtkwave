Analog support is new as of 1.3.81 and should be considered experimental.  
The analog waves are auto-scaling.  Manual scaling will be added in a 
future release.  If you wish to make an analog wave taller, simply 
highlight the wave you want to make larger and right click-select Insert
Analog Height Extension from the popup menu.

Note that currently double floats are not written in a platform
independent manner with GHW files.  This should not be an issue unless you
swap across architectures between ghdl and the viewer such as powerpc on
AIX and x86.

-11dec05ajb
