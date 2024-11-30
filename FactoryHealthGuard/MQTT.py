import paho.mqtt.client as mqtt
import base64
import struct
import json

# TTN MQTT configuration
BROKER = "eu1.cloud.thethings.network"
PORT = 1883  # Using an unencrypted port
APPLICATION_ID = "xiao-dtu-34465@ttn"  # Replace with your TTN application ID
ACCESS_KEY = "NNSXS.RJSOTDLGE7ZVPX5UVJLQ2PUFHBCH5XD3HKEQA7I.EUJLMP2I77FCI6VOCEDMEOVD2RK6GTZJLKDRFCS2LR4RTRHJ4LFQ"  # Replace with your API key
DEVICE_ID = "xiao-34365-iot"  # Replace with your device ID

# MQTT topic
TOPIC = f"v3/{APPLICATION_ID}/devices/{DEVICE_ID}/down/push"

# Convert an array of floats to IEEE 754 standard HEX byte array
def floats_to_ieee754_bytes(values):
    """
    Convert an array of floats to an IEEE 754 standard byte array.
    """
    byte_array = b"".join(struct.pack(">f", value) for value in values)  # '>f' indicates big-endian single-precision float
    return byte_array


# Prepare the downlink message
float_values = [100, 200, 200]  # Example of three float values
hex_bytes = floats_to_ieee754_bytes(float_values)  # Convert to byte array
base64_payload = base64.b64encode(hex_bytes).decode()  # Encode as Base64

# Downlink message content
payload = {
    "downlinks": [
        {
            "f_port": 2,
            "frm_payload": base64_payload,  # Use Base64 encoded byte array
            "priority": "NORMAL"
        }
    ]
}


# MQTT callback functions
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("MQTT connected successfully!")
        # Send downlink message
        try:
            result = client.publish(TOPIC, payload=json.dumps(payload), qos=1)
            print(f"Downlink message sent! Result: {result}")
        except Exception as e:
            print(f"Error publishing message: {e}")
    else:
        print(f"Failed to connect, return code {rc}")

def on_publish(client, userdata, mid):
    print("Downlink message published!")

def on_disconnect(client, userdata, rc):
    print(f"Disconnected with return code {rc}")

# Configure the MQTT client
client = mqtt.Client(client_id=None, clean_session=True, userdata=None)
client.username_pw_set(APPLICATION_ID, ACCESS_KEY)  # Set username and password

client.on_connect = on_connect
client.on_publish = on_publish
client.on_disconnect = on_disconnect

try:
    # Connect to the TTN MQTT server
    client.connect(BROKER, PORT, 60)
    client.loop_forever()
except Exception as e:
    print(f"Error connecting to MQTT broker: {e}")
