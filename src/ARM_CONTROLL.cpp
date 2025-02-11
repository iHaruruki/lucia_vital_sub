#include "ARM_CONTROLL.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <iostream>
using namespace std;
#define SERIAL_PORT "/dev/ttyUSB0"
#define BAUDRATE 2000000
#define BUFSIZE 100

void ARM_CONTROLL::arm_controll(int angle){

        unsigned char txData[BUFSIZE]={};
	unsigned char txData_r[BUFSIZE]={};
        int fd;

        struct termios oldtio, newtio;

        fd = open(SERIAL_PORT, O_RDWR);

        if(fd < 0){
                printf("open error");
        }
        cout << "fd:" << fd << endl;

	ioctl(fd, TCGETS, &oldtio);
	newtio.c_cflag = BAUDRATE | CS8 | CREAD;
	tcsetattr(fd, TCSANOW, &newtio);
	ioctl(fd, TCSETS, &newtio);

        printf("\n\rstart\n\r\n\r");


        	char buf[9]; // バッファ
        	char send[11];         //コマンドを保存する配列

        	char rollpitch='p';

        	int num_of_modules=6;

        	/*
		for(int i=0;i<8;i++)scanf("%c",&buf[i]);
	    	printf("Received command:");
	    	for(int i=0;i<7;i++)printf("%c",buf[i]);
	    	printf("\n\r");
	    	*/
	    	/*
	    	if(buf[3]=='+')angle=angle;
	    	else if(buf[3]=='-')angle=-angle;
	    	else angle=1000;
	    	*/

	    	buf[2]=rollpitch;

	    	if(angle>0)buf[3]='+';
	    	else {
	    		buf[3]='-';
	    		angle=-angle;
	    	}



	    	if((-30*num_of_modules)<=angle && angle<=(30*num_of_modules)){
	    		for(int i=1; i<=num_of_modules; i++){
	    			send[0] = 0xAA;
				send[1] = 0xC6;
				send[2] = 0x00;
				send[3] = 0x00;
	    			send[4]='C';
	    			send[5]=i+'0';
	    			send[6]=buf[2];
	    			send[7]=buf[3];
	    			send[8]=(angle/(10*num_of_modules))+'0';
	    			send[9]=((angle/num_of_modules)-((angle/(10*num_of_modules))*10))+'0';
	    			send[10] = 0x55;

	    			write(fd, send, sizeof(send));  //保存したコマンドを送信
				usleep(1.0*50000);
				//printf("send:");             //送信確認
				//for(int i=4;i<10;i++)printf("%c",send[i]);
				//printf(" length:%d\n\r",sizeof(send));
    			}
	    	}

	    	//else printf("Invalid value!\n\r");
	    	//printf("\n\r");

        ioctl(fd, TCSETS, &oldtio);
        close(fd);
}
