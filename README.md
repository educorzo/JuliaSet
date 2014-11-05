JuliaSet
========
Diferent solutions to draw a Julia set with different processors.
<img src="http://i.imgur.com/RAOgAer.png" />


These solutions are part of one exercise of Computer Architecture 2 (Computer engineering, University of Granada).

There are different solutions:

* Sequential
* Using gather and scatter
* Using just gather
* Using just gather with some improvements

Instructions:

Install openmpi and raw2gif in your linux computer.

Compile the file with: 

>mpicxx JuliaSet.cpp -o julia

Execute the executable with:
>mpirun -np [number of procesors] ./julia [rows] [columns] [real number] [imaginary number] [Tamx] [Tamy]

Example : > mpirun -np 4 ./julia 512 512 -0.8 0.156 4 4

Draw the julia set with:

>raw2gif -s [rows] [columns] -p -p vgaPalette.txt Salida > [nameOfFile].gif

Example : raw2gif -s 512 512 -p vgaPalette.txt Salida > salida.gif

