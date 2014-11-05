#include <iostream>
#include <vector>
#include <cstdlib>
#include "mpi.h"
using namespace std;
#define Ventx     -2.0                   /* Esquina inferior izquierda del espacio.*/
#define Venty     -2.0                   /* Esquina superior derecha del espacio.*/
#define Tamx      4                      /* Ancho de la ventana.*/
#define Tamy      4                       /* Alto de la ventana.*/
//mpicc juliaGather.c -o prueba
//mpirun -np 4 ./prueba
//raw2gif -s 512 512 -p vgaPalette.txt Salida > salida.gif
//ESTE EJERCICIO HACE EL GATHER de un conjunto de Julia
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
    MPI_Comm_size(MPI_COMM_WORLD, &size); //Se obtiene el numero de procesos
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); //Se obtiene el rank que soy
    char matriz[filas][columnas];
    char resultado[filas][columnas];
    char filaLocal[filas/size][columnas];
    float Incx  =    (float)Tamx/(float)columnas;
    float Incy  =   (float)Tamy/(float)filas;
    float zr,zi;  /*Parte real e imaginaria de z.*/
    double tInicio, tFin;
    
    
    MPI_Scatter(matriz[0],(filas/size)*columnas, MPI_CHAR, filaLocal, (filas/size)*columnas, MPI_CHAR,0,MPI_COMM_WORLD);//envio matriz 1 elemento para cada filaLocal que espera recibir 1 elemento
    
    
    int color=0;
    /* PROCESO LOCAL DE CADA PROCESADOR */
    for(int i=0;i<filas/size;i++){
        for(int j=0; j<columnas;j++){
            int r=rank*(filas/size)+i; //r es el numero de fila de la matriz resultado
            
            zi=(float)Venty+(float)r*Incy; // z0=coordenadas del punto (parte imaginaria).
            
            zr=(float)Ventx+(float)j*Incx; // z0=coordenadas del punto (parte real).
            
           // if(j==9)printf(" |%i | zi %f, zr %f , Incy %f \n",r,zi,zr,Incy);
            
            zrs=zis=(float)0; // Se inicializan los cuadrados de z, su parte real al cuadrado, zrs y su parte imaginaria al cuadrado (zis).
            color=0; // Cada punto se colorea según el número de iteraciones necesarias para escapar.
            
            while (zrs+zis<(float)4 && color < 256){
                //Calculo z^2 + c como zr^2-zi^2+2*zr*zi+c.
                zrs=zr*zr; // Calculo zr^2.
                zis=zi*zi; // Calculo zi^2.
                zi=2*zr*zi+ci;
                zr=zrs-zis+cr; // Calculo la parte real de z --> x^2-y^2+cr.
                color++; // Cada punto se colorea según el número de iteraciones necesarias para escapar.
            }//whiel
            
            filaLocal[i][j]=color-1;
        }
    }
    
    
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(filaLocal,(filas/size)*columnas, MPI_CHAR, resultado, (filas/size)*columnas, MPI_CHAR, 0,MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
    
    
    
    if(rank==0){
        for (int j=0;j<filas;j++)
            fwrite(resultado[j],sizeof(char),columnas,fi);
        fclose(fi);
        tiempo=MPI_Wtime()-tiempo;
        cout<<"Tiempo Conjunto de Julia: "<<tiempo<<endl;
    }
    
}
