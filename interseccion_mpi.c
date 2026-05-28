//mpicc -Wall -Wextra -std=c11
#define _DEFAULT_SOURCE // Esto permite que usleep() funcione bajo el estándar c11
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>       // Agregamos esto para arreglar el error de time()

#define N_CARRILES 4
#define N_VEHICULOS 10
#define MASTER 0

#define TAG_SOLICITUD 1 /* carril → coordinador: pido cruzar */
#define TAG_PERMISO 2 /* coordinador → carril: ya puede cruzar */
#define TAG_CRUCE_FIN 3 /* carril → coordinador: terminé */
#define TAG_ESTADISTICA 4 /* carril → coordinador: mis contadores */

#define TAG_MENSAJE 5
const char *nombres[N_CARRILES +1] = {"Coordinador","Norte", "Sur", "Este", "Oeste"};

typedef struct {
    int carros_cruzados;
    double tiempo_carril; 
} mens_carril;

void reporte_final(double tiempo_total , int carros_cruzados, int accidentes, mens_carril *carriles){
    
    printf("======== REPORTE FINAL ======== \n\n");
    printf("Vehiculos cruzados: %d\n", carros_cruzados);
    printf("Accidentes: %d\n", accidentes);
    printf("Tiempo de simulacion: %.3f seg\n\n", tiempo_total);
    printf("Estadisticas por carril: \n");
    printf("Carril      Vehiculos   Espera acum. \n");
    printf("Norte      %d  %f \n", carriles[1].carros_cruzados, carriles[1].tiempo_carril); 
    printf("Sur:        %d  %f \n", carriles[2].carros_cruzados, carriles[2].tiempo_carril);
    printf("Este        %d  %f \n", carriles[3].carros_cruzados, carriles[3].tiempo_carril);
    printf("Oeste:      %d  %f \n\n", carriles[4].carros_cruzados, carriles[4].tiempo_carril);
        
    printf("Funciones MPI usadas: MPI_Send, MPI_Recv, MPI_Bcast, MPI_Barrier, MPI_Reduce, MPI_Gather");
} 


void hilo_carril(int rank, int n_carros) {
    srand(time(NULL) + rank);

    MPI_Send((void*)nombres[rank], 20, MPI_CHAR, MASTER, TAG_MENSAJE, MPI_COMM_WORLD);
    printf("[%s] listo. Esperando autorizacion para cruzar.\n", nombres[rank]);

    MPI_Barrier(MPI_COMM_WORLD);

    int respuesta;
    for (int i = 1; i <= n_carros; i++) {
        int id_vehiculo = i;

        MPI_Send(&id_vehiculo, 1, MPI_INT, MASTER, TAG_SOLICITUD, MPI_COMM_WORLD);
        MPI_Recv(&respuesta, 1, MPI_INT, MASTER, TAG_PERMISO, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        int tiempo_cruce = 2000 + rand() % 4000;
        usleep(tiempo_cruce);
        
        MPI_Send(&id_vehiculo, 1, MPI_INT, MASTER, TAG_CRUCE_FIN, MPI_COMM_WORLD);
    }

    int fin_cruce = -1;
    MPI_Send(&fin_cruce, 1, MPI_INT, MASTER, TAG_SOLICITUD, MPI_COMM_WORLD);
}

typedef struct {
  int carriles_activos;
  int cruce_libre;
  int cola_espera[N_CARRILES * N_VEHICULOS];
  int indice_frente;
  int indice_final;
} EstadoCruce;

void procesar_solicitud(int carril, int id, EstadoCruce *estado) {
  int permiso = 1;
  if (id == -1) {
    estado->carriles_activos--;
  } else if (estado->cruce_libre) {
    estado->cruce_libre = 0;
    printf("[%s-%03d] - solicita cruce\n", nombres[carril], id);
    MPI_Send(&permiso, 1, MPI_INT, carril, TAG_PERMISO, MPI_COMM_WORLD);
    printf("[Coordinador] %s-%03d autorizado. Cruce OCUPADO.\n", nombres[carril], id);
  } else {
    estado->cola_espera[estado->indice_final++] = carril;
    int esperando = estado->indice_final - estado->indice_frente;
    printf("[%s-%03d] - solicita cruce (en cola: %d esperando)\n", nombres[carril], id, esperando);
  }
}

void procesar_fin_cruce(int carril, int id, EstadoCruce *estado) {
  int permiso = 1;
  printf("[%s-%03d] ✓ cruce completado. Cruce LIBRE.\n", nombres[carril], id);
  
  if (estado->indice_frente < estado->indice_final) {
    int siguiente_carril = estado->cola_espera[estado->indice_frente++];
    MPI_Send(&permiso, 1, MPI_INT, siguiente_carril, TAG_PERMISO, MPI_COMM_WORLD);
    printf("[Coordinador] %s autorizado (era el siguiente en cola).\n", nombres[siguiente_carril]);
  } else {estado->cruce_libre = 1;}
}

void coordinador(int n_carros, int rank) {
  for (int i = 1; i <= N_CARRILES; i++)
  {
    char recibido[20];
    MPI_Recv(recibido, 20, MPI_CHAR, i, TAG_MENSAJE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  
  MPI_Barrier(MPI_COMM_WORLD);
  double inicio = MPI_Wtime();

  EstadoCruce estado = {N_CARRILES, 1, {0}, 0, 0};
  int id;
  MPI_Status estado_mpi;

  while (estado.carriles_activos > 0 || !estado.cruce_libre) {
    MPI_Recv(&id, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &estado_mpi);
    
    if (estado_mpi.MPI_TAG == TAG_SOLICITUD) {
      procesar_solicitud(estado_mpi.MPI_SOURCE, id, &estado);
    } 
    else if (estado_mpi.MPI_TAG == TAG_CRUCE_FIN) {
      procesar_fin_cruce(estado_mpi.MPI_SOURCE, id, &estado);
    }
  }

  printf("\nTiempo total: %.3f s\n", MPI_Wtime() - inicio);
}


void crear_mpi(int argc, char *argv[]){
  //Fase 1, base
  int rank, size;
  MPI_Init(&argc, &argv);
  //Nùmero de proceso/rank/ carril
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  //Tamaño total del rank
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (size != N_CARRILES + 1){
    if (rank == MASTER)printf("ERROR en cantidad de procesos");
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  int n_carros = N_VEHICULOS; 
  if(rank == MASTER){
    MPI_Bcast(&n_carros, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
    printf("[%s] Sistema iniciado, N_VEHICULOS= %d, distribuido a %d", nombres[0], N_VEHICULOS, N_CARRILES);
  }else {
    MPI_Bcast(&n_carros, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
  }
  if(rank == MASTER){
    coordinador(n_carros, rank);
  } else{
    hilo_carril(rank, N_VEHICULOS);
  }
  

  MPI_Finalize();
  //free variables
}

int main(int argc, char *argv[]) {
  printf("========================================================\n");
  printf("SIMULADOR DE INTERSECCION DE TRAFICO CON MPI\n");
  printf("========================================================\n");
  printf("Procesos: %d | Carriles: %d | Vehiculos por carril: %d\n", N_CARRILES, N_CARRILES , N_VEHICULOS);
  printf("========================================================\n");
  
  crear_mpi(argc, argv);

  return 0;
}