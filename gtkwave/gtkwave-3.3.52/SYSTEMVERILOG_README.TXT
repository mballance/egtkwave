SV adds a number of data types however according to IEEE1800-2005 they can
be dumped by pretending to be existing VCD datatypes.

SystemVerilog		Verilog		Size
=============		=======		====
bit			reg		[total size of packed dimension]
logic			reg		[total size of packed dimension]
int			integer		32
shortint		reg		16
longint			reg		64
byte			reg		8
enum			integer		32
shortreal		real		--

...noncompliant dumpers may surface in the future; the above is meant to
be an alias guide for future incompatibility fixes.

-07apr07ajb
