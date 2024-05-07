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


class flipdots:
    serial = None
    serialPort = "/dev/ttyUSB0"
    baudRate = 115200

    @staticmethod
    def connect():
        try:
            flipdots.serial = serial.Serial(flipdots.serialPort, flipdots.baudRate)
            print("Connected to serial port: " + flipdots.serialPort)
        except:
            print("Failed to connect to serial port: " + flipdots.serialPort)
            flipdots.serial = None


    @staticmethod
    def send(string):
        print(string, end='')
        if flipdots.serial is None or not flipdots.serial.is_open:
            flipdots.connect()
        if flipdots.serial is not None and flipdots.serial.is_open:
            flipdots.serial.write(string.encode('ascii', 'ignore'))
            while flipdots.serial.readline()[0:5] != b'ready':
                sleep(0.5)

    @staticmethod
    def allOff():
        flipdots.send("allOff\n")

    @staticmethod
    def allOn():
        flipdots.send("allOn\n")

    @staticmethod
    def invert():
        flipdots.send("invert\n")


class display:
    interrupt = False

    @staticmethod
    def compileTextArray(string, mode, min = 0):
        tempArray = []
        if mode == "time14":
            for l in string:
                tempArray.append("0")
                for c in time14[l]:
                    tempArray.append(str(c))
            tempArray.append("0")
        if mode == "ascii7":
            for l in string:
                tempArray.append("0")
                for c in ascii7[l]:
                    tempArray.append(str(c))
            tempArray.append("0")
        while len(tempArray) < min:
            tempArray.append("0")
        return tempArray


    @staticmethod
    def timeDay():
        shownMinute = -1
        shownDay = -1
        while True:
            now = datetime.now()
            if display.interrupt:
                break
            if now.minute != shownMinute:  # get time
                current_time = now.strftime("%-I:%M%p").lower()[0:-1]
                tempArray = display.compileTextArray(current_time, "time14", 56)
                timeCommand = f"({'{' + ','.join(tempArray) + '}'},len(tempArray),14,0,1,20)\n"
                flipdots.send(timeCommand)
                # update the current minute that is shown
                shownMinute = now.minute

            if now.day != shownDay:
                # line 1 is the day of the week
                line1 = now.strftime("%A")
                tempArray = display.compileTextArray(line1, "ascii7")
                dateCommand = f"({'{' + ','.join(tempArray) + '}'},{len(tempArray)},7,56,0,50)\n"
                flipdots.send(dateCommand)
                # line 2 is the month and day
                line2 = now.strftime("%B !!")
                line2 = line2.replace('!!', str(now.day) + {1: 'st', 2: 'nd', 3: 'rd'}.get(now.day % 20, 'th'))
                tempArray = display.compileTextArray(line2, "ascii7")
                dateCommand = f"({'{' + ','.join(tempArray) + '}'},{len(tempArray)},7,56,8,50)\n"
                flipdots.send(dateCommand)
                # update the current day that is shown
                shownDay = now.day
            sleep(5)

    """
    weatherUpdatedMinute = -30
                if abs(now.minute - weatherUpdatedMinute) > 30:  # get weather
                newDisplay = []
                response = requests.get(openWeatherMapURL)
                if response.ok:
                    response = response.json()
                    celcius_temperature = response["main"]["temp"]
                    fahrenheit_temperature = (celcius_temperature * 1.8) + 32
                    text = f"{celcius_temperature:.0f}C ({fahrenheit_temperature:.0f}F)"
                else:
                    text = "API call error"
                for l in text:
                    newDisplay.append("0")
                    for c in ascii7[l]:
                        newDisplay.append(str(c))
                newDisplay.append("0")
                weatherCommand = f"({'{' + ','.join(newDisplay) + '}'},{len(newDisplay)},7,{timeDisplayLength + 2},8,50)\n"
                flipdots.send(weatherCommand)
                weatherUpdatedMinute = now.minute
    """

    @staticmethod
    def stocks():  # stocks
        print("stocks")

    @staticmethod
    def news():  # news
        print("news")

    class tests:
        @staticmethod
        def time():  # test
            separator = "({65535},1,16,56,0,50)\n"
            flipdots.send(separator)
            for h in range(0, 24):
                for m in [0,1,23,34,56,57,58,59]:
                    dt = datetime.strptime(f"{h:02d}:{m:02d}:00", "%H:%M:%S")
                    current_time = dt.strftime("%-I:%M%p").lower()[0:-1]
                    tempArray = display.compileTextArray(current_time, "time14", 56)
                    timeCommand = f"({'{' + ','.join(tempArray) + '}'},{len(tempArray)},14,0,1,20)\n"
                    flipdots.send(timeCommand)
                    sleep(0.5)


def displayLoop():
    #display.timeDay()
    display.tests.time()


def main():
    # start webApp and wait for interrupts
    return

if __name__ == "__main__":
    flipdots.connect()
    flipdots.allOff()
    threading.Thread(target=displayLoop).start()
    main()
