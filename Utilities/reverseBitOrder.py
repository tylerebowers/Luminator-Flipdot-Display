def reverse_mask(x, numOfBytes):
    if numOfBytes >= 1:
        x = ((x & 0x55555555) << 1) | ((x & 0xAAAAAAAA) >> 1)
        x = ((x & 0x33333333) << 2) | ((x & 0xCCCCCCCC) >> 2)
        x = ((x & 0x0F0F0F0F) << 4) | ((x & 0xF0F0F0F0) >> 4)
    if numOfBytes >= 2:
        x = ((x & 0x00FF00FF) << 8) | ((x & 0xFF00FF00) >> 8)
    if numOfBytes >= 4:
        x = ((x & 0x0000FFFF) << 16) | ((x & 0xFFFF0000) >> 16)
    return x

byteSize = input("Number byte size: ")
while(True):
    oldArray = input("C-style array: ")
    oldArray = oldArray.replace("{","").replace("}","").replace(";","").replace(" ","").split(",")
    print("{",end="")
    i = 0
    for number in oldArray:
        print(reverse_mask(int(float(number)), int(byteSize)), end="")
        i += 1
        if(len(oldArray)!=i): print(", ",end="")
    print("};")

