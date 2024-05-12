### Ejercicio sobre Memoria Compartida y Sincronización de Procesos

**Objetivo:**
Implementar un programa en C que utilice memoria compartida para compartir una matriz entre varios procesos hijos. Cada proceso hijo realizará una operación en la matriz y es necesario sincronizarlos para evitar condiciones de carrera.

**Descripción:**
1. Crear un programa en C que utilice memoria compartida para compartir una matriz de enteros entre varios procesos.
2. El programa principal deberá crear la matriz compartida y forkear varios procesos hijos. El número de procesos hijos debe ser igual al número de filas de la matriz.
3. Cada proceso hijo deberá realizar la siguiente operación en la matriz compartida:
   - Sumar 1 a cada elemento de su fila.
4. Después de que todos los procesos hijos hayan terminado, el programa principal debe imprimir la matriz resultante.

**Requisitos Técnicos:**
- Utilizar memoria compartida para compartir la matriz entre los procesos.
- Implementar la sincronización de procesos para evitar condiciones de carrera al modificar la matriz compartida.
- La matriz debe ser de tamaño configurable, pero debe tener al menos 3 filas y 3 columnas.
- Asegurar la correcta liberación de recursos al finalizar el programa.

