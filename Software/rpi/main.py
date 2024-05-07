from datetime import datetime
from time import sleep
import threading
import requests
import serial
from library import *


numCols = 112
numRows = 16
city = "West Lafayette"
openWeatherMapAPIKey = "c7ca991b7196262e0be5185bce9e776f"
openWeatherMapBaseURL = "https://api.openweathermap.org/data/2.5/weather?"
openWeatherMapURL = openWeatherMapBaseURL + "appid=" + openWeatherMapAPIKey + "&q=" + city + "&units=metric"

"""
#test
from simulator import simulator  
sim = simulator()  
#test
"""

class luminator:
    serial = None
    serialPort = "/dev/ttyUSB0"
    baudRate = 115200

    @staticmethod
    def connect():
        try:
            luminator.serial = serial.Serial(luminator.serialPort, luminator.baudRate)
            print("Connected to serial port: " + luminator.serialPort)
        except:
            print("Failed to connect to serial port: " + luminator.serialPort)
            luminator.serial = None


    @staticmethod
    def send(string):
        """
        sim.readString(string)  #test
        sim.print()  #test
        """
        print(string, end='')
        if luminator.serial is None or not luminator.serial.is_open:
            luminator.connect()
        if luminator.serial is not None and luminator.serial.is_open:
            luminator.serial.write(string.encode('ascii', 'ignore'))
            while luminator.serial.readline() != b'ready':
                sleep(0.5)
        luminator.isReady = False

    @staticmethod
    def allOff():
        luminator.send("allOff\n")

    @staticmethod
    def allOn():
        luminator.send("allOn\n")

    @staticmethod
    def invert():
        luminator.send("invert\n")



class display:
    shownMinute = -1
    timeDisplayLength = 0
    shownDay = -1
    weatherUpdatedMinute = -30

    @staticmethod
    def dayTimeTemp():
        now = datetime.now()
        if now.minute != display.shownMinute:  # get time
            timeDisplay = []
            current_time = now.strftime("%-I:%M%p").lower()[0:-1]
            for l in current_time:
                timeDisplay.append("0")
                for c in time14[l]:
                    timeDisplay.append(str(c))
            timeDisplay.append("0")
            display.shownMinute = now.minute
            timeCommand = f"({'{' + ','.join(timeDisplay) + '}'},{len(timeDisplay)},14,1,1,20)\n"
            luminator.send(timeCommand)
            display.timeDisplayLength = len(timeDisplay)
            display.shownMinute = now.minute
        if now.day != display.shownDay:  # get date
            dateDisplay = []
            current_date = now.strftime("%A, %B !!")
            current_date = current_date.replace('!!',
                                                str(now.day) + {1: 'st', 2: 'nd', 3: 'rd'}.get(now.day % 20, 'th'))
            for l in current_date:
                dateDisplay.append("0")
                for c in ascii7[l]:
                    dateDisplay.append(str(c))
            dateDisplay.append("0")
            dateCommand = f"({'{' + ','.join(dateDisplay) + '}'},{len(dateDisplay)},7,{display.timeDisplayLength + 2},0,50)\n"
            luminator.send(dateCommand)
            display.shownDay = now.day
        if abs(now.minute - display.weatherUpdatedMinute) > 30:  # get weather
            weatherDisplay = []
            response = requests.get(openWeatherMapURL)
            if response.ok:
                response = response.json()
                celcius_temperature = response["main"]["temp"]
                farenheit_temperature = (celcius_temperature * 1.8) + 32
                text = f"{celcius_temperature:.0f}C ({farenheit_temperature:.0f}F)"
            else:
                text = "API call error"
            for l in text:
                weatherDisplay.append("0")
                for c in ascii7[l]:
                    weatherDisplay.append(str(c))
            weatherDisplay.append("0")
            weatherCommand = f"({'{' + ','.join(weatherDisplay) + '}'},{len(weatherDisplay)},7,{display.timeDisplayLength + 2},8,50)\n"
            luminator.send(weatherCommand)
            display.weatherUpdatedMinute = now.minute

    @staticmethod
    def stocks():  # stocks
        print("stocks")

    @staticmethod
    def news():  # news
        print("news")


def displayLoop():
    # day/time/temp
    while True:
        display.dayTimeTemp()
        sleep(10)


def main():
    return

if __name__ == "__main__":
    luminator.connect()
    threading.Thread(target=displayLoop).start()
    main()