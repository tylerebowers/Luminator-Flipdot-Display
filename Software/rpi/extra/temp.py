from datetime import datetime
"""
toConvert = [
    [0x00, 0x00, 0x00],  #
    [0x22, 0x20, 0x20],  # !
    [0x55, 0x00, 0x00],  # "
    [0x57, 0x57, 0x50],  ##
    [0x23, 0x63, 0x62],  # $
    [0x41, 0x24, 0x10],  # %
    [0x25, 0x35, 0x70],  # &
    [0x64, 0x00, 0x00],  # '
    [0x24, 0x44, 0x20],  # (
    [0x42, 0x22, 0x40],  # )
    [0x52, 0x72, 0x50],  # *
    [0x02, 0x72, 0x00],  # +
    [0x00, 0x00, 0x64],  # ,
    [0x00, 0x70, 0x00],  # -
    [0x00, 0x00, 0x20],  # .
    [0x11, 0x24, 0x40],  # /
    [0x35, 0x55, 0x60],  # 0
    [0x26, 0x22, 0x70],  # 1
    [0x61, 0x24, 0x70],  # 2
    [0x61, 0x21, 0x60],  # 3
    [0x15, 0x71, 0x10],  # 4
    [0x74, 0x61, 0x60],  # 5
    [0x24, 0x65, 0x20],  # 6
    [0x71, 0x32, 0x20],  # 7
    [0x25, 0x25, 0x20],  # 8
    [0x25, 0x31, 0x20],  # 9
    [0x00, 0x20, 0x20],  #:
    [0x00, 0x20, 0x64],  # ;
    [0x12, 0x42, 0x10],  # <
    [0x00, 0x70, 0x70],  # =
    [0x42, 0x12, 0x40],  # >
    [0x61, 0x20, 0x20],  # ?
    [0x75, 0x54, 0x70],  # @
    [0x25, 0x75, 0x50],  # A
    [0x65, 0x65, 0x60],  # B
    [0x34, 0x44, 0x30],  # C
    [0x65, 0x55, 0x60],  # D
    [0x74, 0x64, 0x70],  # E
    [0x74, 0x64, 0x40],  # F
    [0x34, 0x55, 0x30],  # G
    [0x55, 0x75, 0x50],  # H
    [0x72, 0x22, 0x70],  # I
    [0x11, 0x15, 0x20],  # J
    [0x55, 0x65, 0x50],  # K
    [0x44, 0x44, 0x70],  # L
    [0x57, 0x75, 0x50],  # M
    [0x57, 0x55, 0x50],  # N
    [0x25, 0x55, 0x20],  # O
    [0x65, 0x64, 0x40],  # P
    [0x25, 0x57, 0x30],  # Q
    [0x65, 0x65, 0x50],  # R
    [0x34, 0x71, 0x60],  # S
    [0x72, 0x22, 0x20],  # T
    [0x55, 0x55, 0x70],  # U
    [0x55, 0x55, 0x20],  # V
    [0x55, 0x77, 0x50],  # W
    [0x55, 0x25, 0x50],  # X
    [0x55, 0x22, 0x20],  # Y
    [0x71, 0x24, 0x70],  # Z
    [0x64, 0x44, 0x60],  # [
    [0x44, 0x21, 0x10],  # \
    [0x62, 0x22, 0x60],  # ]
    [0x25, 0x00, 0x00],  # ^
    [0x00, 0x00, 0x0F],  # _
    [0x62, 0x00, 0x00],  # `
    [0x00, 0x35, 0x70],  # a
    [0x44, 0x65, 0x60],  # b
    [0x00, 0x34, 0x30],  # c
    [0x11, 0x35, 0x30],  # d
    [0x00, 0x76, 0x30],  # e
    [0x12, 0x72, 0x20],  # f
    [0x00, 0x75, 0x17],  # g
    [0x44, 0x65, 0x50],  # h
    [0x20, 0x22, 0x20],  # i
    [0x20, 0x22, 0x26],  # j
    [0x44, 0x56, 0x50],  # k
    [0x22, 0x22, 0x20],  # l
    [0x00, 0x77, 0x50],  # m
    [0x00, 0x65, 0x50],  # n
    [0x00, 0x25, 0x20],  # o
    [0x00, 0x65, 0x64],  # p
    [0x00, 0x35, 0x31],  # q
    [0x00, 0x64, 0x40],  # r
    [0x00, 0x32, 0x60],  # s
    [0x02, 0x72, 0x30],  # t
    [0x00, 0x55, 0x70],  # u
    [0x00, 0x55, 0x20],  # v
    [0x00, 0x57, 0x70],  # w
    [0x00, 0x52, 0x50],  # x
    [0x00, 0x55, 0x24],  # y
    [0x00, 0x62, 0x30],  # z
    [0x32, 0x62, 0x30],  # {
    [0x22, 0x22, 0x20],  # |
    [0x62, 0x32, 0x60],  # }
    [0x5A, 0x00, 0x00],  # ~
    [0x02, 0x57, 0x00],  #
]


for l in toConvert:
    s = "".join(format(i, '08b') for i in l)
    arr = []
    for i in range(0, 4):
        b = ""
        for j in range(0, 6):
            b += s[(j*4)+i]

        arr.append(int(b[::-1], 2))
    print(arr)

    #f bin(i[0]) bin(i[1]), bin(i[2]), bin(i[3]), bin(i[4]), bin(i[5])

"""
"""
now = datetime.now()
print(now.strftime("%I:%M%p").lower()[0:-1])
current_date = now.strftime("%A, %B !!")
current_date = current_date.replace('!!', str(now.day) + {1:'st',2:'nd',3:'rd'}.get(now.day%20, 'th'))
print(current_date)
"""
"""
import requests
city = "West Lafayette"
openWeatherMapAPIKey = "c7ca991b7196262e0be5185bce9e776f"
openWeatherMapBaseURL = "https://api.openweathermap.org/data/2.5/weather?"
openWeatherMapURL = openWeatherMapBaseURL + "appid=" + openWeatherMapAPIKey + "&q=" + city + "&units=imperial"
response = requests.get(openWeatherMapURL)
print(response.json())
if response.ok:
    response = response.json()
    current_temperature = response["main"]["temp"]
    print(current_temperature)
"""
print(datetime.now().strftime("%#I:%M%p").lower()[0:-1])