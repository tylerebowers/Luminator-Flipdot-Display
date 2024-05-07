numCols = 112
numRows = 16


class simulator:

    def __init__(self):
        self.shown = [0] * numCols

    def writeDot(self, c, r, state):
        self.shown[c] = (self.shown[c] & (~(1 << r))) | (state << r)

    def write(self, array, colLimit = numCols, rowLimit = numRows, x = 0, y = 0, delayTime = 2, byCol=True):
        for c in range(x, numCols):  # For each column
            if c - x < colLimit:
                for r in range(y, numRows):
                    if r - y < rowLimit:  # do each row
                        newDot = ((array[c - x] >> r - y) & 1)
                        if newDot != ((self.shown[c] >> r) & 1):
                            self.writeDot(c, r, newDot)

    def allOff(self):
        self.shown = [0] * numCols

    def print(self):
        print()
        for r in range(numRows):
            print('|', end="")
            for c in range(numCols):
                if (self.shown[c] >> r) & 1:
                    print("#", end='')
                else:
                    print(" ", end='')
            print('|')

    def readString(self, string):
        if string[0] == '{':
            string = string[string.index('{')+1:string.index('}')]
            array = list(map(int, string.split(",")))
            self.write(array)
        else:
            if string[0:2] == '({':
                temp = string[string.index('}')+2:string.index(')')]
                params = [0,0,0,0,1]
                temp = list(map(int, temp.split(",")))
                for i in range(len(temp)):
                    params[i] = temp[i]
                array = string[string.index('{')+1:string.index('}')]
                array = list(map(int, array.split(",")))
                self.write(array, params[0], params[1], params[2], params[3], params[4])
            elif string[0] == '(':
                string = string[string.index('(')+1:string.index(')')]
                array = string.split(",")
                params = list(map(int, array))
                self.writeDot(params[0], params[1], params[2])