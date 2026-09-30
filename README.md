Instrucciones para ejecutar el programa:
1. make
2. Se divide en tres posibilidades:
    2.1 ./compiladorC prueba.c  ___ para ejecutar el compilador de forma predeterminada (en este caso el preproceso está activado por defecto).
    2.2 ./compiladorC prueba.c -o nombreDelArchivo   ____ envía la salida al archivo indicado en lugar de la consola.
    2.3 ./compiladorC prueba.c -p    ____    activa el preproceso (en este caso, activado por defecto).       
3. make clean

Archivos de prueba:

Estos se encuentran en la misma carpeta. Los nombres de los casos corresponden a "prueba" seguidos por algún número. Como es posible notar, la enumeración comienza en dos. Esto se debe a que se planteó el primer caso como el main.c de este propio proyecto. 
Adicionalmente, hay casos de prueba que están diseñados para que algunos tiren diversos errores para poner a prueba el control del programa.