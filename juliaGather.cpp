#include <iostream>
#include <vector>
#include <cstdlib>
#include "mpi.h"
using namespace std;
#define Ventx     -2.0                   /* Esquina inferior izquierda del espacio.*/
#define Venty     -2.0                   /* Esquina superior derecha del espacio.*/
//SE QUITA EL SCATTER
//mpicxx juliaGather.c -o prueba
//mpirun -np 4 ./prueba 512 512 -0.8 0.156
//raw2gif -s 512 512 -p vgaPalette.txt Salida > salida.gif
//ESTE EJERCICIO HACE EL GATHER de un conjunto de Julia


char ** reservarMatriz(unsigned int filas, unsigned int columnas) {
    
	// Reserva de Memoria
	char **m;
	m = new char *[filas];
	for (int i=0;i<filas;i++)
    m[i] = new char[columnas];
    
	return m;
    
}


int main(int argc, char *argv[]){
    FILE *fi;
	int size, rank;
    double tiempo;
    float zrs,zis;
    
    fi=fopen("Salida","wb");
    if (!fi)
    {
        cout<<"No es posible abrir el fichero de salida"<<endl;
        exit(1);
    }
     tiempo=MPI_Wtime();
    MPI_Init(&argc,&argv);
    int filas=atoi(argv[1]);
    int columnas=atoi(argv[2]);
    float cr=atof(argv[3]);
    float ci=atof(argv[4]);  /*Parte real e imaginaria de c.*/
    float Tamx=atof(argv[5]);//4
    float Tamy=atof(argv[6]);//4
    MPI_Comm_size(MPI_COMM_WORLD, &size); //Se obtiene el numero de procesos
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); //Se obtiene el rank que soy

    
    int tamano=filas/size;
    char ** resultado=reservarMatriz(filas, columnas);
    char ** filaLocal=reservarMatriz(tamano,columnas);
    float Incx  =    (float)Tamx/(float)columnas;
    float Incy  =   (float)Tamy/(float)filas;
    float zr,zi;  /*Parte real e imaginaria de z.*/
    double tInicio, tFin;
    
    int color=0;
    cout<<tamano<<" "<<columnas<<endl;
    /* PROCESO LOCAL DE CADA PROCESADOR */
    for(int i=0;i<filas/size;i++){
        for(int j=0; j<columnas;j++){
            int r=rank*tamano+i; //r es el numero de fila de la matriz resultado
            
            zi=(float)Venty+(float)r*Incy; // z0=coordenadas del punto (parte imaginaria).
            
            zr=(float)Ventx+(float)j*Incx; // z0=coordenadas del punto (parte real).
            
            zrs=zis=(float)0; // Se inicializan los cuadrados de z, su parte real al cuadrado, zrs y su parte imaginaria al cuadrado (zis).
            color=0; // Cada punto se colorea según el número de iteraciones necesarias para escapar.
            
            while (zrs+zis<(float)4 && color < 256){
                //Calculo z^2 + c como zr^2-zi^2+2*zr*zi+c.
                zrs=zr*zr; // Calculo zr^2.
                zis=zi*zi; // Calculo zi^2.
                zi=2*zr*zi+ci;
                zr=zrs-zis+cr; // Calculo la parte real de z --> x^2-y^2+cr.
                color++; // Cada punto se colorea según el número de iteraciones necesarias para escapar.
            }//while
           // cerr<<i<<" "<<j<<endl;
            filaLocal[i][j]=color-1;
        }
    }
    
    
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(filaLocal[0],tamano*columnas, MPI_CHAR, resultado[0], tamano*columnas, MPI_CHAR, 0,MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
	cerr<<rank<<endl;
    
    if(rank==0){
        for (int j=0;j<filas;j++){
             cerr<<j<<endl;
            fwrite(resultado[j],sizeof(char),columnas,fi);
           
        }
        fclose(fi);
        tiempo=MPI_Wtime()-tiempo;
        cout<<"Tiempo Conjunto de Julia: "<<tiempo<<endl;
    }
    MPI_Finalize();
}
