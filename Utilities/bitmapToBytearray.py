import math

#numberOfColumns = int(input("Number of Columns: "))
numberOfColumns = 8
#numberOfRows = int(input("Number of Rows: "))
numberOfRows = 8
toConvert = [0] * numberOfColumns
convertedArray = [0] * numberOfColumns
for i in range(0,numberOfRows):
    while(True):
        row = input("Enter horizontal row "+str(i)+": ")
        if(len(row) == numberOfColumns): break
        else: print("Please enter a row such as 00100100")
    toConvert[i] = list(row)
#print(toConvert)
for i in range(0,numberOfColumns):
    sum = 0
    for j in range(0,numberOfRows):
        if(toConvert[j][i] == '1'):
            sum += math.pow(2,j)
    convertedArray[i] = sum
print("{",end='')
for i in range(0,numberOfColumns):
    print(convertedArray[i],end='')
    if(i!=numberOfColumns-1): print(', ',end='')
print("};")

