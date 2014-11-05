JuliaSet
========
<img src="http://i.imgur.com/RAOgAer.png" />
Diferent solutions to draw a Julia set with different processors.
These solutions are part of one exercise of Computer Architecture 2 (Computer engineering, University of Granada).

There are different solutions:
<div>
<li>-Sequential</li>
<li>-Using gather and scatter</li>
<li>-Using just gather</li>
<li>-Using just gather with some improvements</li>
</div>
Instructions:

Install openmpi and raw2gif in your linux computer
Compile the file with: mpicxx JuliaSet.cpp -o julia
Execute the executable with:
mpirun -np [number of procesors] ./julia [rows] [columns] [real number] [imaginary number] [Tamx] [Tamy]
Example : mpirun -np 4 ./julia 512 512 -0.8 0.156 4 4

Draw the julia set with:
raw2gif -s [rows] [columns] -p -p vgaPalette.txt Salida > [nameOfFile].gif
Example : raw2gif -s 512 512 -p vgaPalette.txt Salida > salida.gif

