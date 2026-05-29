# Garantía sin accidentes: 
## ¿Por qué es arquitecturalmente imposible que dos vehículos crucen simultáneamente en la versión MPI? ¿Necesitamos detectar accidentes como en la versión con semáforos? Justifique.
Porque con MPI la carretera funciona por exclusión mutua del cordinador, donde el coordinador solo maneja un proceso al mismo tiempo y ninguno de ellos se encontrará directamente como para una colición, por ende; no se necesita detectar accidentes como en los semáforos porque los procesos respetan el orden.


# MPI vs Semáforos: 
## Compare las dos versiones del simulador. ¿Cuál es más fácil de razonar sobre su correctitud? ¿Cuál escalaría mejor a 100 carriles? ¿Por qué?
Es más fácil razonar su correctitud en el MPI ya existe un coordinador central y esto evita un race condition, que no posee pausas de procesos como con Semáforos. A pesar de ello, 100 carriles generaría un cuello de botella con MPI. 


# El coordinador como cuello de botella: 
## ¿Puede el coordinador convertirse en un cuello de botella? ¿Bajo qué condiciones? ¿Cómo podría diseñarse un sistema MPI sin coordinador central que igualmente garantice un solo vehículo a la vez?
Si, ya que es el único proceso que maneja la instrucción de seguir o avanzar, es como si ese fragmento de decisión fuese secuencial, esto bajo las condiciones de muchos carriles, alta demanda causando una saturación del coordinador. 
Un MPI sin coordinador utilizaría exclusión mutua, con algo que de los permisos necesarios. 


# MPI_ANY_SOURCE: 
## El coordinador usa MPI_ANY_SOURCE para recibir solicitudes. ¿Qué ventaja tiene? ¿Podría causar starvation para algún carril? ¿Cómo lo detectaría en los datos del reporte?
MPI_ANY_SOURCE permite que el coordinador atienda solicitudes apenas llegan, evitando tiempos de espera. Starvation puede implicar mucho gasto de tiempo, detectando cuando en un carril cruzan menos carros que otros por la falta de equidad al acceso.


# Bonus (+0.5 pts): 
## Modifique el coordinador para usar MPI_Irecv (no bloqueante) en lugar de MPI_Recv bloqueante. ¿Qué ventaja aporta? ¿Cambia el comportamiento observable?
