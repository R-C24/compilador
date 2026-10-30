Instrucciones para ejecutar el programa:
1. make
2. Se pueden utilizar varios argumentos según lo que se desee:

    2.1 ./compiladorC prueba.c  -----> Es la instrucción básica para ejecutar el compilador de forma predeterminada (en este caso el preproceso está desactivado por defecto y tendrá el estilo propio).

    2.2 Si incluye -r ----> Se sobreescribe el archivo fuente con el prettyprint.

    2.3 Si incluye -p -----> Activa el preproceso (en este caso, desactivado por defecto).       

    2.4 Si incluye -b -----> La salida tendrá el estilo de BSD. 

    2.5 Si incluye -g -----> La salida tendrá el estilo de GNU.

3. make clean

Archivos de prueba:

Estos se encuentran en la misma carpeta. Los nombres de los casos exitosos corresponden a "pruebaGenerada" seguidos por algún número (con la excepción de la primera prueba, la cual es solo "pruebaGenerada". Ya que, como es posible notar, la enumeración comienza en dos.) Con respecto a los archivos de prueba con errores, estos están nombrados como "pruebaError", seguidos de algún número, esta vez sí comenzando en 1.
