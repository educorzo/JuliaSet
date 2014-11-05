#include <iostream>
#include <vector>
#include <cstdlib>
#include "mpi.h"
#define Ventx     -2.0                   /* Esquina inferior izquierda del espacio.*/
#define Venty     -2.0                   /* Esquina superior derecha del espacio.*/
using namespace std;


//mpicxx JuliaSet.cpp -o prueba
//mpirun -np 4 ./prueba 512 512 -0.8 0.156 4 4
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
    int contador=0;//Con esta variable se cuantos mensajes he recibido con el rank=0
    int flag=0;//Variable para saber cuando se ha recibido un mensaje
    float zr,zi;  /*Parte real e imaginaria de z.*/
    float zrs,zis;
    int color=0;
    fi=fopen("Salida","wb");
    double tiempo;
    if (!fi)
    {
        cout<<"No es posible abrir el fichero de salida"<<endl;
        exit(1);
    }
    tiempo=MPI_Wtime();
    MPI_Init(&argc,&argv);
    unsigned int  filas=atoi(argv[1]);
    unsigned int columnas=atoi(argv[2]);
    float cr=atof(argv[3]);//cr=-0.8 /*Parte real e imaginaria de c.*/
    float ci=atof(argv[4]);//cr=0.156
    float Tamx=atof(argv[5]);//4
    float Tamy=atof(argv[6]);//4

    float Incx  =    (float)Tamx/(float)columnas;
    float Incy  =   (float)Tamy/(float)filas;
   
    
	MPI_Comm_size(MPI_COMM_WORLD, &size); //Se obtiene el numero de procesos
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); //Se obtiene el rank que soy
    MPI_Request request;
    MPI_Status status;
   
    int tamano=filas/size;
    char ** resultado=reservarMatriz(filas, columnas);
    char ** local=reservarMatriz(tamano,columnas);
    char ** mensaje=reservarMatriz(tamano,columnas);
    
    if(rank==0){
         int a =  MPI_Irecv(mensaje[0], tamano*columnas, MPI_CHAR,MPI_ANY_SOURCE,  MPI_ANY_TAG, MPI_COMM_WORLD, &request);
    }
    /* PROCESO LOCAL DE CADA PROCESADOR */
    for(int i=0;i<tamano;i++){
        for(int j=0; j<columnas;j++){
            int r=rank*(tamano)+i; //r es el numero de fila de la matriz resultado
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
            
            local[i][j]=color-1;
            if(rank==0)
                resultado[i][j]=color-1;
        }//FOR COLUMNA
        
        //CONSULTAR EN CADA FILA//
        if(rank==0){
           
            MPI_Test (&request, &flag, &status);
            if(flag == 1) {
                int iterador=(filas/size)*status.MPI_SOURCE;
                int inicial=iterador;
                //JUNTAR//
                for(int i=0;i<tamano;i++){
                    iterador=inicial+i;
                    for(int j=0;j<columnas;j++){
                        
                        resultado[iterador][j]=mensaje[i][j];
                    }
                }
                //FIN JUNTAR//
                contador++;
                flag=0;
                int a =  MPI_Irecv(mensaje[0], tamano*columnas, MPI_CHAR,MPI_ANY_SOURCE,  MPI_ANY_TAG, MPI_COMM_WORLD, &request);
            }//IF FLAG ==0
            
        }//IF RANK==0
        //FIN DE CONSULTA EN CADA FILA
        
    }//FOR FILA
/*FIN DEL PROCESO LOCAL DE CADA PROCESADOR*/

    //Todos los mensajes le envian el resultado al procesador 0 , menos el procesador 0
    if(rank!=0){
        MPI_Isend(local[0],tamano*columnas, MPI_CHAR, 0,rank, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);

    }
    
    if(rank==0){
        while(contador<size-1){
            MPI_Test(&request, &flag, &status);
            if(flag == 1) {
                int iterador=(filas/size)*status.MPI_SOURCE;
                int inicial=iterador;
                //JUNTAR//
                for(int i=0;i<tamano;i++){
                    iterador=inicial+i;
                    for(int j=0;j<columnas;j++){
                        resultado[iterador][j]=mensaje[i][j];
                    }
                }
                //FIN JUNTAR//
                contador++;
                flag=0;
                int a =  MPI_Irecv(mensaje[0], tamano*columnas, MPI_CHAR,MPI_ANY_SOURCE,  MPI_ANY_TAG, MPI_COMM_WORLD, &request);
            }
            
            
        }//WHILE
        for (int j=0;j<filas;j++)
            fwrite(resultado[j],sizeof(char),columnas,fi);
        fclose(fi);
        tiempo=MPI_Wtime()-tiempo;
        cout<<"Tiempo Conjunto de Julia: "<<tiempo<<endl;
    }//IF RANK=0
    MPI_Finalize();
}
