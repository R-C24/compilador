Instrucciones para ejecutar el programa:
1. make
2. Se pueden utilizar varios argumentos según lo que se desee:

    2.1 ./compiladorC archivo.c  -----> Es la instrucción básica para ejecutar el compilador de forma predeterminada (en este caso el preproceso está desactivado por defecto y despliegan las tablas de símbolos para cada uno por defecto).

    2.2 Si incluye -s ----> Despliega cada tabla de símbolos al cerrar su contexto.

    2.3 Si incluye -o -----> Permite escoger el nombre del archivo de salida.       

3. make clean

Archivos de prueba:

Estos se encuentran en la misma carpeta. Los nombres de los casos exitosos corresponden a "sinErrores" seguidos por algún número. Con respecto a los archivos de prueba con errores, los semánticos y sintácticos ambos están nombrados de esa forma (sin las tildes) seguidos de algún número comenzando en 1.
