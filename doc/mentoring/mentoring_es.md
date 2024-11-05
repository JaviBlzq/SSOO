## Tutoría 05/11/24

En la tutoría que se ha tenido con el profesor se han preguntado varias dudas, y se han resuelto todas ellas:
- **Execv y control de errores**: Se aclara que exec solo retorna si ha sucedido algún fallo al ejecutar exec, ya sea por que el path es incorrecto, no se tienen los permisos necesarios para ejecutar un comando o por la razón que sea. Si execv funciona correctamente, retorna con el estado con el que retorna el comando ejecutado.

    Por tanto, como se quiere controlar los errores tenemos que tener en cuenta lo siguiente: 
    - Comprobar con que código de error retorna los comandos, normalmente 0 para Success, y valores bajos para Failure, normalmente 1. Así podremos recoger el estado del comando ejecutado.
    - Si es exec quien ha fallado, hacer un exit con un código un tanto especial, para recoger el estado especial.

    En el caso de querer anidar ifs en el control de errores con diferentes valores de estado, podemos recurrir a:

    ```
    if (WIFEXITED(status)){
        switch(WEXITSTATUS(status)){
            case 0:
                exit(EXIT_SUCCESS);
            case 1:
                /* Fallo del comando, no de exec*/
            case SPECIAL_VALUE_ERROR:
                /* Fallo de exec*/
        }
    }
    ```

    En algunos casos no hará falta distinguir entre ambos errores.

- **Fragmentación interna**: Cuando en la memoria ocurre fragmentación interna, el trozo de bloque de memoria que no está completo nunca va a ser confundido. 
    Con el control de gestión de ficheros del propio sistema operativo siempre se sabe cual es el tamaño máximo del fichero, por tanto, pese a que existe fragmentación interna, se sabrá cuando dejar de leer en un bloque y no leer memoria que no sea del fichero.

- **Consejos sobre código**: Se ha revisado el fichero _idall.c_ del ejercicio 3, el cual al momento de la tutoría no había sido entregada. Se han dado unos consejos pese a que el código tenía buena pinta:
    - Recordatorio de declaración de variables siempre al principio de las funciones, para que la depuración sea más sencilla. Es una buena práctica de programación.
    - Podemos usar cualquiera de `err(), errx(), warn(), warnx() o fprintf(stderr, "") con exit()`. Teniendo claro lo que hacen y cuando pintan errno. Con `err() y errx()` se sale directamente, no hace falta llamar a `exit()`.
    - El PACTO al que se ha llegado en la asignatura es que los recursos reservados deben estar todos liberados cuando hacemos `exit(EXIT_SUCCESS)`, es decir, cuando salimos con éxito. Esto ha sido comentado porque cuando salimos con error, no hace falta que liberemos. Pese a que está bien hecho, no penaliza si no se hace.
    - Los nombres de funciones deben ser nombradas por lo que hacen, si queremos calcular un coseno, que la función se llame coseno. Sin embargo, si es un procedimiento, puede ser `do_something()`.
    - Se menciona también que las constantes están nombradas de una manera que no siguen el estilo que se sigue en la asignatura, pero que no es importante.
    - Es buena práctica también hacer funciones cortas, por ejemplo, sacar el código del proceso hijo y del proceso padre, a funciones aparte.

