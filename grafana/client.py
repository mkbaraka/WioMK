# import paho.mqtt.client as mqtt

# # MQTT broker information
# broker_address = "localhost"
# port = 1883

# client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION1)
# client.connect(broker_address, port)
# client.publish("test", 1)
# client.disconnect()

import paho.mqtt.client as mqtt
from time import sleep
from ping3 import ping
import random

MQTT_HOST = 'localhost'
MQTT_PORT = 1883
MQTT_TOPIC = 'ping'

HOST_TO_PING = 'ww.google.com'

client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION1)
client.connect(MQTT_HOST, MQTT_PORT)
client.loop_start()

print(f"Publishing to {MQTT_HOST}:{MQTT_PORT} on topic '{MQTT_TOPIC}'")

topics = ["air", "light", "temperature", "humidity"]

while True:
    for topic in topics:
        p = random.randint(0, 120)
        encoded = str(p).encode('utf-8')
        client.publish(topic, encoded)
    sleep(0.2)