from datetime import datetime
import requests
import json
from library import *
import threading
from time import sleep

numCols = 112
numRows = 16
city = "West Lafayette"
openWeatherMapAPIKey = "c7ca991b7196262e0be5185bce9e776f"
openWeatherMapBaseURL = "https://api.openweathermap.org/data/2.5/weather?"
openWeatherMapURL = openWeatherMapBaseURL + "appid=" + openWeatherMapAPIKey + "&q=" + city + "&units=metric"


class display:
    mode = 1

    @staticmethod
    def _sendSerial(string):
        print(string)

    shownMinute = -1
    timeDisplayLength = 0
    shownDay = -1
    weatherUpdatedMinute = -30

    @staticmethod
    def dayTimeTemp():
        now = datetime.now()
        if now.minute != display.shownMinute: # get time
            timeDisplay = []
            current_time = now.strftime("%#I:%M%p").lower()[0:-1]
            for l in current_time:
                timeDisplay.append("0")
                for c in time14[l]:
                    timeDisplay.append(str(c))
            timeDisplay.append("0")
            display.shownMinute = now.minute
            timeCommand = f"({'{' + ','.join(timeDisplay) + '}'},{len(timeDisplay)},14,1,1,20)"
            display._sendSerial(timeCommand)
            display.timeDisplayLength = len(timeDisplay)
            display.shownMinute = now.minute
        if now.day != display.shownDay: #get date
            dateDisplay = []
            current_date = now.strftime("%A, %B !!")
            current_date = current_date.replace('!!', str(now.day) + {1:'st',2:'nd',3:'rd'}.get(now.day%20, 'th'))
            for l in current_date:
                dateDisplay.append("0")
                for c in ascii7[l]:
                    dateDisplay.append(str(c))
            dateDisplay.append("0")
            dateCommand = f"({'{' + ','.join(dateDisplay) + '}'},{len(dateDisplay)},7,{display.timeDisplayLength+2},1,50)"
            display._sendSerial(dateCommand)
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
            weatherCommand = f"({'{' + ','.join(weatherDisplay) + '}'},{len(weatherDisplay)},7,{display.timeDisplayLength + 2},8,50)"
            display._sendSerial(weatherCommand)
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
    threading.Thread(target=displayLoop).start()
    main()