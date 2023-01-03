import tkinter as tk
import math
# numRows = int(input("The number of rows: "))
# numCols = int(input("The number of rows: "))
numRows = 16
numCols = 112
button_list = []
root = tk.Tk()
height = ((numRows * 40) + 60)
width = (numCols * 46)
if height > root.winfo_screenheight(): height = root.winfo_screenheight() - 50
if width > root.winfo_screenwidth(): width = root.winfo_screenwidth() - 50
root.geometry(str(width) + "x" + str(height))
# root.tk.call('tk', 'scaling', 1)
root.title("Luminator Design Canvas")

# Scrollbar
main_frame = tk.Frame(root)
main_frame.pack(fill="both", expand=1)

main_canvas = tk.Canvas(main_frame)
main_canvas.pack(side="top", fill="both", expand=1)

x_scrollbar = tk.Scrollbar(main_frame, orient="horizontal", command=main_canvas.xview)
x_scrollbar.pack(side="bottom", fill="x")

# y_scrollbar = tk.Scrollbar(main_frame, orient="vertical", command=main_canvas.yview)
# y_scrollbar.pack(side="right", fill="y")

main_canvas.configure(xscrollcommand=x_scrollbar.set)
# main_canvas.configure(yscrollcommand=y_scrollbar.set)
main_canvas.bind("<Configure>", lambda e: main_canvas.configure(scrollregion=main_canvas.bbox("all")))

inside_frame = tk.Frame(main_canvas)
main_canvas.create_window((0, 0), window=inside_frame, anchor="nw")
# Scrollbar /\


class button_box:
    def __init__(self, button, ID_number):
        self.ID_number = ID_number
        self.button = button

    def off(self):
        self.button.config(bg="black")

    def on(self):
        self.button.config(bg="yellow")

    def isOn(self):
        if self.button.config('bg')[-1] == "yellow":
            return True
        else:
            return False

    def getX(self):
        return int(self.button.config('text')[-1].split(",")[0])

    def gety(self):
        return int(self.button.config('text')[-1].split(",")[1])

    def toggle(self, event):
        if self.button.config('bg')[-1] == "black":
            print(f'Button {self.ID_number} toggled ON.')
            self.on()
        else:
            print(f'Button {self.ID_number} toggled OFF.')
            self.off()


def allon():
    print("Button ALL toggled ON.")
    for but in button_list:
        but.on()


def alloff():
    print("Button ALL toggled OFF.")
    for but in button_list:
        but.off()


def export():
    print("Button ALL exported:")
    toConvert = [[0]*numRows for i in range(numCols)]
    convertedArray = [0] * numCols
    for but in button_list:
        if but.isOn():
            toConvert[but.getX()][but.gety()] = 1
        else:
            toConvert[but.getX()][but.gety()] = 0
    for i in range(0, numCols):
        sum = 0
        for j in range(0, numRows):
            if (toConvert[i][j] == 1):
                sum += math.pow(2, j)
        convertedArray[i] = int(sum)
    print("{", end='')
    for i in range(0, numCols):
        print(convertedArray[i], end='')
        if (i != numCols - 1): print(', ', end='')
    print("};")

button = tk.Button(inside_frame, width=5, height=1, text="(X,Y)", bg='grey', relief="flat")
button.grid(row=0, column=0)
button = tk.Button(inside_frame, width=5, height=1, text="Export", bg='grey', command=export)
button.grid(row=0, column=1)
button = tk.Button(inside_frame, width=5, height=1, text="All On", bg='grey', command=allon)
button.grid(row=0, column=2)
button = tk.Button(inside_frame, width=5, height=1, text="All Off", bg='grey', command=alloff)
button.grid(row=0, column=3)
button = tk.Button(inside_frame, width=5, height=1, text="Exit", bg='grey', command=root.destroy)
button.grid(row=0, column=4)

button_number = 1
for y in range(numRows):
    for x in range(numCols):
        button = tk.Button(inside_frame, width=5, height=2, text=(str(x) + "," + str(y)), bg='black', fg='grey')
        button.grid(row=numRows - y, column=x)
        button_list.append(button_box(button, button_number))
        button.bind('<Button-1>', button_list[button_number - 1].toggle)
        button_number += 1

tk.mainloop()
