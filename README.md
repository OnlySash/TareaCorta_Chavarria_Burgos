## Garantía sin accidentes: 
# ¿Por qué es arquitecturalmente imposible que dos vehículos crucen simultáneamente en la versión MPI? ¿Necesitamos detectar accidentes como en la versión con semáforos? Justifique.




## MPI vs Semáforos: 
# Compare las dos versiones del simulador. ¿Cuál es más fácil de razonar sobre su correctitud? ¿Cuál escalaría mejor a 100 carriles? ¿Por qué?



## El coordinador como cuello de botella: 
# ¿Puede el coordinador convertirse en un cuello de botella? ¿Bajo qué condiciones? ¿Cómo podría diseñarse un sistema MPI sin coordinador central que igualmente garantice un solo vehículo a la vez?



## MPI_ANY_SOURCE: 
# El coordinador usa MPI_ANY_SOURCE para recibir solicitudes. ¿Qué ventaja tiene? ¿Podría causar starvation para algún carril? ¿Cómo lo detectaría en los datos del reporte?



## Bonus (+0.5 pts): 
# Modifique el coordinador para usar MPI_Irecv (no bloqueante) en lugar de MPI_Recv bloqueante. ¿Qué ventaja aporta? ¿Cambia el comportamiento observable?