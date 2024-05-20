# Sistema IoT para Evaluar el Desempeño del Proceso de Enseñanza y Aprendizaje en el Aula

## Descripción
Este proyecto mejora el proceso de enseñanza-aprendizaje mediante un dispositivo IoT desarrollado con Wio Terminal. Monitorea en tiempo real variables ambientales y dinámicas educativas, ofreciendo apoyo analítico al profesorado.

## Características
- **Monitoreo ambiental**: Temperatura, humedad, calidad del aire y luminosidad.
- **Seguimiento de actividades**: Registro de dinámicas de clase y cambios en tiempo real.
- **Interfaz intuitiva**: Configurable y fácil de usar para los docentes.
- **Alertas y notificaciones**: En tiempo real sobre condiciones desfavorables.
- **Análisis de datos**: Dashboards detallados con métricas y estadísticas mediante Grafana.

## Conectividad y Analítica de Datos
El dispositivo utiliza las siguientes tecnologías para la conectividad y análisis de datos:
- **MQTT**: Protocolo de comunicación para IoT.
- **Mosquitto**: Broker MQTT que gestiona la comunicación entre dispositivos.
- **Grafana**: Plataforma de visualización de datos.
- **Matplotlib y Pandas**: Bibliotecas de Python para la visualización y análisis de datos desde archivos CSV mediante un Script.

### Instalación del Contenedor de Docker para Grafana
1. Clona el repositorio:
    ```sh
    git clone https://github.com/mkbaraka/WioMK.git
    ```
2. Navega al directorio del proyecto:
    ```sh
    cd WioMK
    ```
3. Navega al directorio `grafana`:
    ```sh
    cd grafana
    ```
4. Construye y ejecuta el contenedor de Docker:
    ```sh
    docker-compose up -d
    ```
5. Accede a Grafana en tu navegador web:
    ```sh
    http://localhost:3000
    ```

## Instalación del Dispositivo
1. Configura el Wio Terminal:
    - Sigue las instrucciones en la Wiki para configurar el hardware y cargar el firmware en el dispositivo.
2. Configura el broker Mosquitto:
    - Asegúrate de que Mosquitto esté instalado y configurado correctamente en tu servidor.
3. Configura el servidor MQTT en el Wio Terminal:
    - Modifica el archivo de configuración `wifi_mqtt.h` en el firmware del Wio Terminal con la dirección IP y el puerto de tu broker Mosquitto.

## Uso
1. Enciende el dispositivo Wio Terminal.
2. Configura los parámetros de la clase a través de la interfaz.
3. Monitorea las condiciones ambientales y dinámicas de clase.
4. Revisa los datos y alertas en tiempo real.
5. Accede a los análisis detallados mediante Grafana.

## Scripts
Este proyecto utiliza dos scripts principales para gestionar las diferentes funcionalidades del dispositivo.

### SavaDataInterface
Este script muestra una interfaz donde interactúan los profesores y se almacenan los datos en dos archivos CSV.

- **measure_data.csv**: Guarda los datos ambientales obtenidos por los sensores.
- **class_data.csv**: Guarda los datos de las clases realizadas.

### Script de Conectividad via MQTT
Este script se encarga de la integración de la conectividad via MQTT y no tiene capacidad de interfaz gráfica debido a las limitaciones de las librerías.

- **Funcionalidad**:
    - Envía los datos via WiFi utilizando la librería `rcpWifi`.
    - Los datos son enviados al broker MQTT y posteriormente mostrados en Grafana.
- **Limitaciones**:
    - No es posible utilizar la librería `TFT_LCD` junto con la librería `rcpWifi`, por lo que este script no incluye una interfaz gráfica.

## Contribución
1. Haz un fork del proyecto.
2. Crea una rama para tu feature:
    ```sh
    git checkout -b feature/nueva-feature
    ```
3. Realiza los cambios y haz commits:
    ```sh
    git commit -m "Descripción de los cambios"
    ```
4. Empuja tu rama:
    ```sh
    git push origin feature/nueva-feature
    ```
5. Abre un Pull Request.

## Licencia
Este proyecto está bajo la Licencia MIT. Para más detalles, consulta el archivo [LICENSE](LICENSE).

Proyecto dirigido por [Diego Casado Mansilla](https://www.deusto.es/es/inicio/somos-deusto/equipo/investigadores/1363/investigador).

[Repositorio en GitHub](https://github.com/mkbaraka/WioMK)
