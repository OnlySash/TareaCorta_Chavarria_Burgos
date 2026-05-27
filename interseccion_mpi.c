//mpicc -Wall -Wextra -std=c11
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
  //unsigned int semilla = rank + time(NULL);

  //Crear espacio para nombre y enviarlo
  //char nombre_local[20];
  //sprintf(nombre_local, "%s-%d", nombres[rank]);
  MPI_Send((void*)nombres[rank], 20, MPI_CHAR, MASTER, TAG_MENSAJE, MPI_COMM_WORLD);
  printf("[%s] listo. Esperando autorizacion para cruzar. \n", nombres[rank]);

  /*int vehiculos_cruzados_local = 0;
  int accidentes_local = 0;
  int en_cruce_local = 0;
  
  for (int i = 1; i <= N_VEHICULOS; i++) {
    cola[numero_de_carril]++;

    // aqui es donde se da la Race condition en Fase 1
    //sem_wait(&semaforo_cruce);
    printf("[%s] cruzando\n", nombre);

    //La variable en_cruce_sem ya no tiene forma de ser 1
    if (en_cruce_sem == 1) {
      accidentes_sem++;
      printf("ACCIDENTE detectado con %s\n", nombre);
    }

    int time = 2500 + rand_r(&semilla) % 3000;
    en_cruce_sem = 1; //marcar que hay un vehiculo en el cruce (sin proteccion)
    usleep(time);
    en_cruce_sem = 0;
    printf("[%s] cruzo\n", nombre);
    //sem_post(&semaforo_cruce);

    //sem_wait(&mutex_contadores);
    cola[numero_de_carril]--;
    vehiculos_cruzados_sem++;
    cruzados[numero_de_carril]++;
    //sem_post(&mutex_contadores);
  }
  return NULL;*/
}

void coordinador(int n_carros, int rank){
  double inicio = MPI_Wtime();  
   
  for (int i = 1; i <= N_CARRILES; i++)
  {
    char recibido[20];
    MPI_Recv(recibido, 20, MPI_CHAR, i, TAG_MENSAJE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  if (rank == MASTER) {
        //Inicializar desde rank 0
    }
   
    //mpi_recv
    if (rank == MASTER) {
        //Recibir solicitudes de cruce
    }

    if (rank == MASTER) {
        //en el rank 0
        double fin = MPI_Wtime();
        //tiempo total
        //reporte_final();
      }
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