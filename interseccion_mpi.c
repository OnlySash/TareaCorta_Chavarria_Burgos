//mpicc -Wall -Wextra -std=c11
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N_CARRILES 4
#define N_VEHICULOS 50

#define TAG_SOLICITUD 1 /* carril → coordinador: pido cruzar */
#define TAG_PERMISO 2 /* coordinador → carril: ya puede cruzar */
#define TAG_CRUCE_FIN 3 /* carril → coordinador: terminé */
#define TAG_ESTADISTICA 4 /* carril → coordinador: mis contadores */

const char *nombres[N_CARRILES] = {"Coordinador","Norte", "Sur", "Este", "Oeste"};

int vehiculos_cruzados = 0;
int accidentes = 0;
int en_cruce = 0;
double tiempo = 0;
int cola[N_CARRILES];
int cruzados[N_CARRILES];

void *hilo_carril(void *dato_del_carril) {
  int numero_de_carril = *(int *)dato_del_carril;
  unsigned int semilla = numero_de_carril + 1;

  for (int i = 1; i <= N_VEHICULOS; i++) {
    char nombre[20];
    sprintf(nombre, "%s-%d", nombres[numero_de_carril], i);

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
  return NULL;
}

void reporte_final(){
    double fin = MPI_Wtime();
    printf("======== REPORTE FINAL ======== \n\n");
    printf("Vehiculos cruzados: %d\n", vehiculos_cruzados);
    printf("Accidentes: %d\n", accidentes);
    printf("Tiempo de simulacion: %.3f seg\n\n", fin - inicio);
    printf("Estadisticas por carril: \n")
    printf("Carril      Vehiculos   Espera acum. \n")
    printf("Norte      %d  %f \n", cruzados[0], cruzados[0]); 
    printf("Sur:        %d  %f \n", cruzados[1], cruzados[1]);
    printf("Este        %d  %f \n", cruzados[2] ,cruzados[2]);
    printf("Oeste:      %d  %f \n\n", cruzados[3], cruzados[3]);
        
    printf("Funciones MPI usadas: MPI_Send, MPI_Recv, MPI_Bcast, MPI_Barrier, MPI_Reduce, MPI_Gather");
} 

void coordinador(){
    if (size != N_CARRILES +1){
    if (rank == 0)
    {
        prinf("Mayor cantidad de ranks");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    }
  double inicio = MPI_Wtime();

  if (rank == MASTER) {
        //Inicializar desde rank 0
    }

    MPI_Bcast(dna_chain, params.dna_length, MPI_CHAR, MASTER, MPI_COMM_WORLD);
   
    MPI_Barrier(MPI_COMM_WORLD);

    // process_data;
    // Impresiones por rank

    //mpi_recv
    if (rank == MASTER) {
        //Recibir solicitudes de cruce
    }

    //para impresiones locales, sincrinizar resultados
    MPI_Gather(local_results, params.k_patterns * 2, MPI_INT, 
               all_results,   params.k_patterns * 2, MPI_INT, 
               MASTER, MPI_COMM_WORLD);

    if (rank == MASTER) {
        //en el rank 0
        reporte_final();
      }
}

void crear_mpi(){
  //Fase 1, base
  int rank, size;
  MPI_INit(&argc, &argc);
  //Nùmero de proceso/rank/ carril
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  //Tamaño total del rank
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  coordinador();

  MPI_Finalize();
  //free variables
}

int main(void) {
  printf("========================================================\n");
  printf("SIMULADOR DE INTERSECCION DE TRAFICO CON MPI\n");
  printf("========================================================\n");
  printf("Procesos: %d | Carriles: %d | Vehiculos por carril: %d\n", ranks ,N_CARRILES, N_VEHICULOS);
  printf("========================================================\n");
  
  crear_mpi();

  return 0;
}