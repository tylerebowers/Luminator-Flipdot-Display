#include <iostream>
#include "LinkedList.h"
#include <cstdint>

using namespace std;


uint16_t largeTime[13][10] = {		//14r x 10c
	{4092,8190,14343,12291,12291,12291,12291,14343,8190,4092},	//0
	{0,12300,12302,12303,16383,16383,12288,12288,12288,0},		//1
	{0,14342,15367,15875,14083,13187,12739,12543,12414,0},		//2
	{0,6150,14343,12291,12483,12483,12483,14535,8190,3900},		//3
	{127,255,192,192,192,192,192,16383,16383,192},				//4
	{12415,12415,12387,12387,12387,12387,12387,14563,8131,3971},//5
	{4094,8191,14563,12387,12387,12387,12387,14563,8135,3974},	//6
	{3,3,3,3,14339,16131,1987,243,63,15},						//7
	{3868,8190,14823,12483,12483,12483,12483,14823,8190,3900},	//8
	{60,6270,14567,12483,12483,12483,12483,14567,8190,4092},	//9
	{0,0,0,0,1560,1560,0,0,0,0},								//:
	{0,3584,2560,1536,0,3584,512,3584,512,3584},				//am
	{0,15872,2560,3584,0,3584,512,3584,512,3584},				//pm
};

void writeTime(uint8_t hour, uint8_t minute, uint8_t x = 0, uint8_t y = 0, bool mt = false){
    LinkedList<uint16_t> out = LinkedList<uint16_t>();
    uint8_t t[6] = {hour / 10, hour % 10, 10, minute / 10, minute % 10, 11+(hour>=12)};
    uint8_t d = 0;
    if(!mt){
       if(hour > 12){t[0]-=1;t[1]-=2;} 
       else if(hour == 0){t[0]=1;t[1]+=2;}
       if(t[0] == 0){d = 1;}
    }
    for(d; d < 6-mt; d++){
        for(uint8_t i = 0; i < 10; i++){
            if(largeTime[t[d]][i] != 0){
                out.add(largeTime[t[d]][i]);
            }
        }
        out.add(0);
    }
    
    for(uint8_t r = 0; r<16; r++){
        for(uint16_t c = 0; c < out.size(); c++){
          if((out[c] >> r) & 1){
            cout << '#';
          } else {
            cout << ' ';
          }
        }
        cout << '\n';
    }
}

int main()
{
    writeTime(0, 0, 0, 0, false);
    writeTime(0, 59, 0, 0, false);
    writeTime(1, 0, 0, 0, false);
    writeTime(11, 59, 0, 0, false);
    writeTime(12, 0, 0, 0, false);
    writeTime(12, 1, 0, 0, false);
    writeTime(13, 0, 0, 0, false);
    writeTime(23, 59, 0, 0, false);
    return 0;
}
