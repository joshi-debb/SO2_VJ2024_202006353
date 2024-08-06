# Manual Técnico 

### Introducción
En este manual se busca comprender el funcionamiento de las llamadas al sistema en el contexto del sistema operativo Linux Mint. Se abordarán temas relacionados con la gestión de procesos y la capacidad de interceptar estas llamadas para monitorear y registrar su actividad. El objetivo principal es desarrollar un programa en lenguaje C que actúe como proceso padre y cree dos procesos hijos. Estos procesos hijos realizarán operaciones de manejo de archivos sobre un archivo específico, mientras que el proceso padre interceptará y registrará las llamadas al sistema realizadas por los hijos.

### Requerimientos para Linux Mint
- Sistema operativo: Linux Mint 21.2 Cinnamon.

- Procesador: Compatible con arquitectura de 32 bits (x86) o 64 bits (x64).
Memoria RAM: Al menos 1 GB para compilaciones simples y desarrollo básico en C. Para proyectos más complejos, se recomiendan 2 GB o más.

- Espacio en disco: Mínimo de 16 GB de espacio disponible.
Herramientas de desarrollo: Compilador de C compatible con Linux Mint, en este caso GCC.

- Instalación y Configuración de Herramientas de Desarrollo

- Compilador GCC: Generalmente viene preinstalado en la mayoría de las distribuciones de Linux, incluyendo Linux Mint.

- Si no está instalado, se puede instalar con el siguiente comando:

    ```bash
        sudo apt-get install build-essential
    ```

- Editor de texto o IDE: Puede utilizar cualquier editor de texto de su preferencia, como Vim, Emacs, o instalar un IDE como NetBeans, Code::Blocks, o Visual Studio Code.


### Iniciando la Ejecución del Programa

Para esta práctica, utilizaremos tres archivos:

- `Padre.c`: Se encarga de la creación de los procesos hijos y de la llamada a SystemTap.

- `Hijo.c`: Contiene la lógica necesaria para simular los procesos cuando el padre manda a llamar a los hijos.

- `Trace.stp`: Script de SystemTap, en el cual se monitorean todas las llamadas a los procesos hijos (Read, Write).


### Archivo `Padre.c`

#### Librerías Utilizadas
<div>
    <img src="https://github.com/crstn07/SO2_GRUPO1/assets/87725718/348e8f98-c1a9-4fe1-9200-af6762d85825" alt="librerias">
</div>

#### Inicio del Código
En esta parte declaramos los PID para llevar el control de los procesos y declaramos el método signal, el cual nos ayuda a capturar el ctrl + C. Más adelante explicaremos qué hace cuando se ejecuta esta acción.

<div>
    <img src="https://github.com/crstn07/SO2_GRUPO1/assets/87725718/236861b9-1a4a-4224-b48a-2b56a023f619" alt="main">
</div>

Luego, declaramos el primer fork, que es el primero que utilizaremos en el programa.

<div>
    <img src="https://github.com/crstn07/SO2_GRUPO1/assets/87725718/68544e52-49d5-4e8d-a96a-82fb00038d44" alt="fork1">
</div>

En esta parte del código, tendremos la lógica para ejecutar los procesos hijos. Si el primer if se iguala a -1, se trata de un error. Si el PID que viene es mayor a 0, es el proceso padre. En el else if creamos el segundo proceso hijo y seguimos la misma lógica para el primer hijo, verificando si el PID2 que viene es padre o hijo. Si no es mayor a 0, es el proceso hijo, entonces en el else del proceso hijo, mandamos a ejecutar el archivo `Hijo.bin` usando *execv*. Ambos hijos llaman al mismo archivo, ya que simulan el mismo proceso.

#### Llamada al Archivo SystemTap
Para la llamada de este archivo, utilizamos un char de 100 caracteres para almacenar el comando. Luego, con ayuda de un snprintf, guardamos el comando sudo stap `trace.stp`, el cual redirige toda su salida al archivo `syscall.log`. Este archivo `trace.stp` recibe dos parámetros: el PID del hijo 1 y el PID del hijo 2. Con *system(command)* mandamos a ejecutar el SystemTap y luego, con *waitpid*, esperamos a que terminen los procesos hijos.

<div>
    <img src="https://github.com/crstn07/SO2_GRUPO1/assets/87725718/11c979e4-c367-4b64-bed7-596620a2f60c" alt="llamada_systemtap">
</div>

#### Método que Captura la Señal Ctrl + C
En esta parte del código, tendremos la lógica para obtener el conteo de los procesos que se hicieron. Utilizamos fopen para abrir el archivo syscall.logs y recorremos el archivo con un while. Si encuentra las palabras read o write suma la variable dependiendo la condición encontrada. Con una serie de pasos simples, obtenemos el conteo total y ya imprimimos en consola nuestros procesos totales.

<div>
    <img src="https://github.com/crstn07/SO2_GRUPO1/assets/87725718/d1346e92-d194-4a03-8cf4-df4001139012" alt="signint">
</div>

### Archivo `Hijo.c`

#### Método que Escribe en el Archivo
En esta parte, tenemos un método llamado escribir, el cual recibe el archivo, que sería el open del archivo `practica1.txt`. Se crea un char de 8 caracteres y con un for que tiene un random, se decide entre escribir números o letras. Luego, con un write, escribimos en el archivo.

<div>
    <img src="https://github.com/crstn07/SO2_GRUPO1/assets/87725718/55d3e20b-099d-4df5-8929-0414b3eb7266" alt="hijo_escribir">
</div>

#### Método que Lee el Archivo
En este método, tenemos un char que nos ayuda a leer las 8 posiciones del archivo, donde se encuentre el apuntador.

<div>
    <img src="https://github.com/crstn07/SO2_GRUPO1/assets/87725718/b4904e00-acee-43a2-8333-aafd2b16cd1e" alt="hijo_leer">
</div>

#### Función Main
En esta función, creamos el archivo practica1.txt en el cual se realizan los procesos de escribir y leer. Con un while, hacemos que se repita el código. Luego, con un random de 1 a 3, ponemos un sleep para que espere un tiempo aleatorio y con un if y un random, elegimos qué función mandar a llamar (escribir o leer). Esto se repetirá hasta que se termine el proceso hijo.

<div>
    <img src="https://github.com/crstn07/SO2_GRUPO1/assets/87725718/d269c5d7-213f-41f9-a26d-df6957759225" alt="main_hijo">
</div>

### Archivo `trace.stp` para usar SystemTap 
SystemTap es una herramienta que nos ayuda a llevar el control de procesos en Linux. Podemos crear un script que nos ayude a saber qué procesos se hacen con el respectivo PID.

#### Método que Controla los Read
SystemTap nos da la opción de controlar las llamadas que se hacen. Con un probe syscall.read, podemos obtener todos los read. Para buscar los read que hacen los PID de los hijos, utilizamos un if que, igualando con los parámetros obtenidos del comando, encuentra el PID al cual pertenece, actuando como un tipo de filtro.

<div>
    <img src="https://github.com/crstn07/SO2_GRUPO1/assets/87725718/13099dba-0c7e-46ed-9a87-5025b59e827b" alt="systemtap_read">
</div>

#### Método que Controla los Write
De manera similar al método read, cambiamos syscall.read por syscall.write para buscar todos los procesos write que concuerden con el PID de los procesos hijos.

<div>
    <img src="https://github.com/crstn07/SO2_GRUPO1/assets/87725718/60d3552a-c8ef-40d0-8c81-0639dcc03ad0" alt="systemtrap_write">
</div>
