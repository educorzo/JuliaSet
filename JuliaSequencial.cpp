#include <iostream>
#include <vector>
#include <cstdlib>
#include "mpi.h"
using namespace std;
#define Ventx     -2.0                   /* Esquina inferior izquierda del espacio.*/
#define Venty     -2.0                   /* Esquina superior derecha del espacio.*/
#define Tamx      4                      /* Ancho de la ventana.*/
#define Tamy      4                       /* Alto de la ventana.*/

/*****************************************
Para el conjunto de julia se resuelve:
          f_c(z) = z^2 + c 
Siendo c un valor constante complejo,
z una variable compleja donde 
z0 = coordenadas del punto
 
 
 
 mpicc conjuntojulia.c -o prueba

******************************************/


int main(int argc, char *argv[]) {
    float zr,zi;  /*Parte real e imaginaria de z.*/
    float zrs,zis;
    int i,j;
    int Fil=atoi(argv[1]);
    int Col=atoi(argv[2]);
    float cr=atof(argv[3]);  /*Parte real e imaginaria de c.*/
    float ci=atof(argv[4]);
    char im[Fil][Col];  /*Reserva de memoria estática para la imagen*/
    char color;
    float Incx =   Tamx/Col;
    float Incy = Tamy/Fil;

	 FILE *fi;
	 clock_t t0,t1;
	 double tiempo;

	 fi=fopen("Salida","wb");
	 if (!fi)
	    {
	     printf("No es posible abrir el fichero de salida\n");
	     exit(1);
	    }

         //Asignamos el valor de c para el conjunto de julia en particular, parte real e imaginaria.
         cr=-0.8;   
         ci=0.156;
    
	 t0=clock();
    
	 for (j=0;j<Fil;j++)
	     for (i=0;i<Col;i++){
              zi=(float)Venty+(float)j*Incy; // z0=coordenadas del punto (parte imaginaria).
	          zr=(float)Ventx+(float)i*Incx; // z0=coordenadas del punto (parte real).
	          zrs=zis=(float)0; // Se inicializan los cuadrados de z, su parte real al cuadrado, zrs y su parte imaginaria al cuadrado (zis).
	          color=0; // Cada punto se colorea según el número de iteraciones necesarias para escapar.
             
             while (zrs+zis<(float)4 && color<256){
                         //Calculo z^2 + c como zr^2-zi^2+2*zr*zi+c.
	                 zrs=zr*zr; // Calculo zr^2.
	                 zis=zi*zi; // Calculo zi^2.
                     zi=2*zr*zi+ci; // Calculo la parte imaginaria de z --> 2*zr*zi+ci.
	                 zr=zrs-zis+cr; // Calculo la parte real de z --> x^2-y^2+cr.
	                 color++; // Cada punto se colorea según el número de iteraciones necesarias para escapar.
                                  // Los puntos que no consiguen escapar se presentan en negro.
              }//while
	          im[j][i]=color-1; // Asigno el color.
         }
    

	 t1=clock();
	 // Paso los valores de la matriz al fichero Salida.
	 for (j=0;j<Fil;j++)
	     fwrite(im[j],sizeof(char),Col,fi);
	 fclose(fi);

	tiempo=(double)(t1-t0);
	 //fprintf(stderr,"\nTiempo Conjunto de Julia(%dx%d)=%.16g milisegundos\n",Col,Fil,tiempo);
    //tiempo=MPI_Wtime()-tiempo;
    cout<<"Tiempo Conjunto de Julia: "<<tiempo<<endl;
}
