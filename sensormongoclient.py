# Reading Temperature from Arduino in Python Serial module and store data at MongoDB Collection with 10 seconds of Intervals
# @Author - Chetan Khatri, Myboiler 09/02/2017
# kindly note before running this code - give command at linux machine pip install pymongo
import serial
import time
import datetime
from pymongo import MongoClient

# Configuration
serial_port          = 'COM15' #REPLACE YOUR PORT OVER HERE
mongodb_host         = '127.0.0.1'
mongodb_db           = 'gas-boiler'
temperature_location = "London-UK"
collection = 'temperature'
# Connect to Serial Port for communication
ser = serial.Serial(serial_port, 9600, timeout=0)

# Connect to MongoDB
client = MongoClient(mongodb_host, 27017)
db = client[mongodb_db]


# Setup a loop to send Temperature values at fixed intervals in seconds
fixed_interval = 10
while 1:
    try:
        # Temperature value obtained from Arduino + Temperature Sensor
        temp_string = ser.readline().rstrip()

        # If we received a measurement, print it and send it to MongoDB.
        if temp_string:
            temperature_c = float(temp_string)
            doc_id = db[collection].insert_one({ 'temperature': temperature_c,
                                             'datetime': datetime.datetime.now(),
                                             'location': temperature_location}).inserted_id
            print str(doc_id) + ': ' + str(temperature_c) + ',' + temperature_location
    except serial.SerialTimeoutException:
        print('Error! Could not read the Temperature Value from unit')
    except ValueError:
        print('Error! Could not convert temperature to float')
    finally:
        time.sleep(fixed_interval)
