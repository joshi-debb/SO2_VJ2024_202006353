# Manual Técnico

## Introducción
En este proyecto se realizó una aplicación destinada a comprender el funcionamiento del manejo de memoria en el sistema operativo Linux Mint 21.2 Cinnamon. Esta aplicación es capaz de monitorear el uso de memoria de cada proceso en ejecución, detectando las solicitudes de memoria realizadas al sistema operativo. Se utilizaron herramientas como SystemTap para la captura de llamadas al sistema y MySQL para almacenar la información obtenida.

## Herramientas de Desarrollo:
- **Compilador GCC**: El compilador GCC generalmente viene preinstalado en la mayoría de
las distribuciones de Linux, incluyendo Linux Mint. Si no está instalado, puede ser instalado a través del gestor de paquetes de su distribución con el siguiente comando:
    ```bash
    sudo apt-get install build-essential
    ```
- **Editor de texto o IDE**: Se puede utilizar cualquier editor de texto de su preferencia, en el desarrollo de este proyecto se utilizó Visual Studio Code.
- **SystemTap**: Es una herramienta de monitoreo y análisis de sistemas que permite a los usuarios escribir scripts para observar el comportamiento del sistema en tiempo real. Se puede instalar en Linux con el siguiente comando:
    ```bash
    sudo apt-get install systemtap
    ```
- **Python**: Es un lenguaje de programación interpretado de alto nivel y propósito general. Se utiliza para el desarrollo de aplicaciones web y scripts. Se puede instalar en Linux con el siguiente comando:
    ```bash
    sudo apt-get install python3
    ```
- **Pip**: Es un sistema de gestión de paquetes utilizado para instalar y administrar paquetes de software escritos en Python. Se puede instalar en Linux con el siguiente comando:
    ```bash
    sudo apt-get install python3-pip
    ```
- **Flask**: Es un framework web ligero y flexible para Python. se utiliza para construir aplicaciones web rapidas y escalables. Se instala con el siguiente comando:
    ```bash
    pip install Flask 
    ```
- **Flask-CORS**: Es una extension para Flask que proporciona un manejo sencillo de solicitudes de recursos cruzados
    ```bash
    pip install Flask-Cors
    ```
- **Mysql**: Es un sistema de gestion de bases de datos relacion de codigo abierto. para interactuar con una base de datos MYSQL desde python, se necesita un conector, en este caso se utilizó *mysql-connector-python*, se instala con el siguiente comando:
    ```bash
    pip install mysql-connector-python
    ```
- **Node.js**: Es un entorno de ejecución para JavaScript construido con el motor de JavaScript V8 de Chrome. Se utiliza para ejecutar aplicaciones web en el servidor. Se instala en Linux con el siguiente comando:
    ```bash
    sudo apt-get install nodejs
    ```
- **NPM**: Es el administrador de paquetes de Node.js. Se utiliza para instalar paquetes de Node.js y administrar dependencias. Se instala en Linux con el siguiente comando:
    ```bash
    sudo apt-get install npm
    ```
- **Vite**: Es una biblioteca de JavaScript para construir interfaces de usuarios. Es mantenida por facebook y una comunidad activa de desarrolladores, Vite es un entorno de desarollo web rapido y moderno que esta especialmente optimizado para el desarollo de aplicaciones web modernas utilizando tecnologias como React. Se instala con el siguiente comando:
    ```bash
    npm install -g create-vite
    # Crear un nuevo proyecto Vite
    npm create vite "nombre proyecto"
    npm install
    npm run dev
    ```

## Arquitectura del Sistema
![ArquiP2](https://github.com/crstn07/SO2_GRUPO1/assets/83626820/0fd2c8bf-b139-4d6a-bbd1-8251b72114ab)

Se utilizó una máquina virtual con Linux Mint 21.2 Cinnamon para el desarrollo del proyecto. Para el Lector en C se implementó un sistema de monitoreo de memoria que captura las llamadas al sistema realizadas por los procesos en ejecución. Se utilizó SystemTap para capturar las llamadas al sistema y Python para procesar los datos obtenidos. Se implementó un servidor web con Flask para visualizar los datos en un dashboard. Se utilizó MySQL para almacenar los datos obtenidos y Vite.js para el desarrollo de la interfaz de usuario.

## Api Python `app.py`
### Librerías Utilizadas
![LibreriasPython](https://github.com/crstn07/SO2_GRUPO1/assets/83626820/25e23511-0bc3-4fc8-851a-e6a74cfa8b7a)

### Método main
![MainPy](https://github.com/crstn07/SO2_GRUPO1/assets/83626820/c78749a7-7c60-4721-be84-9964d8d960f0)

Hace que la aplicación Flask esté disponible para recibir y procesar solicitudes HTTP en una dirección IP específica y un puerto específico.

### Endpoint /dashboard
![/dashboard](https://github.com/crstn07/SO2_GRUPO1/assets/83626820/9055eecd-5130-4d87-a12b-af71c949347f)

Esta función define una ruta en la aplicación Flask para la URL '/dashboard'. Cuando se accede a esta ruta utilizando el método GET, la función realiza una consulta a la base de datos y devuelve los resultados en formato JSON. Los resultados incluyen tres listas: una lista de 'pid', una lista de 'porcentaje' y una lista de diccionarios que representan filas de la tabla 'dashboard' con las claves 'pid', 'name', 'memoria' y 'cpu'.

### Endpoint /solicitud

![/solicitud](https://github.com/crstn07/SO2_GRUPO1/assets/83626820/4266368c-6265-4a4e-818b-381e793a24b9)

Esta función define una ruta en la aplicación Flask para la URL '/solicitud'. Cuando se accede a esta ruta utilizando el método GET, la función realiza una consulta a la base de datos y devuelve los resultados en formato JSON. Los resultados incluyen una lista de diccionarios que representan filas de la tabla 'llamada'. Cada diccionario tiene las claves 'pid', 'call', 'size' y 'fecha', donde 'pid' es el ID del proceso, 'call' es la llamada realizada, 'size' es el tamaño y 'fecha' es la fecha de la llamada.

## Lector en C `lector.c`
### Librerías Utilizadas
![LibreriasC](https://github.com/crstn07/SO2_GRUPO1/assets/83626820/24783181-aa84-498a-ac58-027daf809a9a)

### Método main
![MainC](https://github.com/crstn07/SO2_GRUPO1/assets/83626820/b7a4f698-bb32-429b-a87f-dff25af59384)

El programa está estructurado en varias secciones que cumplen funciones específicas:
- **Conexión a la Base de Datos**: Se establece una conexión con la base de datos MySQL utilizando la biblioteca MySQL C API. Se definen las credenciales de acceso (nombre del servidor, nombre de usuario, contraseña y nombre de la base de datos).
- **Creación del Pipe y Fork**: Se crea un pipe para la comunicación entre procesos. Luego se realiza un fork para crear un nuevo proceso hijo que ejecutará el script 'trace.stp' utilizando el comando 'sudo stap'.
- **Proceso Hijo (Lectura del archivo `trace.stp`)**: El proceso hijo redirige la salida estándar al pipe y ejecuta el script 'trace.stp' utilizando el comando 'sudo stap'. Este script está diseñado para monitorear actividades del sistema y producir una salida que será leída por el programa.
- **Proceso Padre (Lectura de la Salida del Pipe y Actualización de la Base de Datos)**: El proceso padre lee la salida del pipe, interpreta los datos recibidos y actualiza la base de datos MySQL con la información obtenida.
    - Se parsea la salida del pipe para extraer los datos relevantes, como el identificador de proceso (PID), el nombre del proceso, el tamaño, entre otros.
    - Se construye una consulta SQL para insertar o actualizar los datos en la base de datos MySQL.
    - La consulta SQL se ejecuta utilizando la función mysql_query().
- **Cierre de la Conexión y Finalización del Programa**: Una vez que se han procesado todos los datos, se cierra la conexión con la base de datos MySQL y el programa termina su ejecución.

### Script de SystemTap `trace.stp`
![stap](https://github.com/crstn07/SO2_GRUPO1/assets/83626820/8bfe4c8f-d5ae-43b0-9ce5-e7fd11360f7f)

- Monitoreo de la llamada *"mmap"*: cuando se llama a mmap2, se capturan los siguientes datos
    - **llamada**: Nombre de la llamada al sistema
    - **pid**: identificador del proceso
    - **nombre**: nombre del ejecutable
    - **size**: tamaño de la memoria mapeada
    - **tiempo**: tiempo actual en segundos
- Monitoreo de la llamada al sistema *"munmap"*:
    cuando se llama a munmap, se capturan los siguientes datos.
    - **llamada**: Nombre de la llamada al sistema
    - **pid**: identificador del proceso
    - **nombre**: nombre del ejecutable
    - **size**: tamaño de la memoria mapeada
    - **tiempo**: tiempo actual en segundos
- Monitoreo de actividades del sistema cada 250 milisegundos:
    - Se utiliza un temporizador para ejecutar la siguiente accion cada 250ms.
    - Se verifica si el proceso tiene un pid distinto de cero (para evitar errores).
    - Se utiliza el coomando *ps* para obtener el porcentaje de memoria utilizada por el proceso identificado por el pid.

## Base de Datos MySQL
![sql](https://github.com/crstn07/SO2_GRUPO1/assets/83626820/f7a0f7e4-503d-4af9-90c8-89db8b0cc369)
### Tabla `dashboard`
Esta tabla almacena los datos relacionaods con el rendimiento del sistema. Los campos son:
- id
- pid
- name
- size
- memoria
### Tabla `llamada`
Esta tabla registra las llamadas al sistema mmap2 y munmap. Los campos son:
- id
- pid
- name
- size
- feha_hora
### Procedimiento Almacenado `InsertOrUpdateDashboard`
Este procedimiento almacena inserta o actualiza registros en las tablas *llamada* y *dashboard* de la base de datos. Si ya existe un registro en dashboard con el mismo nombre, actualiza sus datos; de lo contrario, inserta un nuevo registro. Los parámetros de entrada son el ID del proceso (pid), el nombre del proceso (name), el tamaño del proceso (size), el porcentaje de memoria (memoria), el nombre del ejecutable (nombre), y la fecha y hora de la llamada al sistema (time).

