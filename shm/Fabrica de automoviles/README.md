### Ejercicio: Simulación de una Cadena de Producción

**Contexto:**
Imagina que estás trabajando en una fábrica de automóviles que tiene una cadena de producción compuesta por varias etapas. Cada etapa de la cadena de producción realiza una tarea específica, como ensamblaje de piezas, pintura, pruebas de calidad, etc.

**Descripción del Problema:**
El objetivo es desarrollar un programa en C que simule la cadena de producción de la fábrica de automóviles utilizando memoria compartida para coordinar el flujo de trabajo entre las diferentes etapas de la cadena.

#### Etapa de Ensamblaje de Piezas:
- Este proceso hijo se encarga de ensamblar las diferentes piezas del automóvil, como el chasis, el motor, las ruedas, etc.
- Trabaja con la lista de piezas necesarias para ensamblar un automóvil completo.
- Puede tener una cola de espera para las piezas que están esperando ser ensambladas.

#### Etapa de Pintura:
- Este proceso hijo se encarga de pintar el automóvil una vez que ha sido ensamblado.
- Trabaja con los colores de pintura disponibles en la fábrica.
- Puede tener una cola de espera para los automóviles que están esperando ser pintados.

#### Etapa de Pruebas de Calidad:
- Este proceso hijo se encarga de realizar pruebas de calidad en el automóvil para asegurarse de que cumple con los estándares de la fábrica.
- Trabaja con los criterios de calidad establecidos por la fábrica.
- Puede tener una cola de espera para los automóviles que están esperando ser probados.