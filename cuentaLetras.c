#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi/mpi.h>

void inicializaCadena(char *cadena, int n){
    int i;
    for(i=0; i<n/2; i++){
        cadena[i] = 'A';
    }
    for(i=n/2; i<3*n/4; i++){
        cadena[i] = 'C';
    }
    for(i=3*n/4; i<9*n/10; i++){
        cadena[i] = 'G';
    }
    for(i=9*n/10; i<n; i++){
        cadena[i] = 'T';
    }
}

int main(int argc, char *argv[]){
    // Inicializacion de MPI
    MPI_Init(&argc, &argv);
    if(argc != 3){
        printf("Numero incorrecto de parametros\nLa sintaxis debe ser: program n L\n  program es el nombre del ejecutable\n  n es el tamaño de la cadena a generar\n  L es la letra de la que se quiere contar apariciones (A, C, G o T)\n");
        exit(1);
    }

    // Declaracion de variables MPI
    int numprocs, rank, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    int i, n, count=0;
    char *cadena;
    char L;

    int num;
    char letra;

    // Obtenemos el numero de procesos, el rango del proceso y el nombre del procesador
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(processor_name, &namelen);

    // Para las letras
    if(rank == 0) {
        L = *argv[2];
        letra = L;
        for (i = 1; i < numprocs; i++)
            MPI_Send(&letra, 1, MPI_CHAR, i, 0, MPI_COMM_WORLD);
    }else{
        MPI_Recv(&letra, 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        L = letra;
    }
    //Para el tamaño de la cadena
    if(rank == 0){
        n = atoi(argv[1]);
        num = n;
        for (i = 1; i < numprocs; i++)
            MPI_Send(&num, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
    else{
        MPI_Recv(&num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        n = num;
    }

    cadena = (char *) malloc(n*sizeof(char));
    inicializaCadena(cadena, n);

    for(i=rank*n/numprocs; i<(rank+1)*n/numprocs; i++){//otra opcion es i=rank; i<n; i+=numprocs
        if(cadena[i] == L){
            count++;
        }
    }

    // Para el proceso 0 recibe los datos de los demas procesos
    if(rank == 0){
        int aux;
        for(i=1; i<numprocs; i++){
            MPI_Recv(&aux, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            count += aux;
        }

    }else{
        MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }


    if(rank == 0) {
        printf("El numero de apariciones de la letra %c es %d\n", L, count);
    }
    free(cadena);
    MPI_Finalize();
    exit(0);
}