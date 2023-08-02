#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <sys/ioctl.h>

//#define CHIP_ADDR 0x68
#define I2C_DEV "/dev/i2c-1"

static int iic_read(int fd, char buff[], int addr, int count)
{
    int res;
    char sendbuffer1[2];
    sendbuffer1[0]=addr>>8;
    sendbuffer1[1]=addr;
    write(fd,sendbuffer1,2);      
        res=read(fd,buff,count);
        printf("read %d byte at 0x%x/n", res, addr);
        return res;
}

static int iic_write(int fd, char buff[], int addr, int count)
{
        int res;
        int i,n;
        static char sendbuffer[100];
        memcpy(sendbuffer+2, buff, count);
        sendbuffer[0]=addr>>8;
    sendbuffer[1]=addr;
        res=write(fd,sendbuffer,count+2);
        printf("write %d byte at 0x%x/n", res, addr);
}
int main(void){
    int fd;
    int res;
    char ch;
    char buf[50];
    int regaddr,i,slaveaddr;
    fd = open(I2C_DEV, O_RDWR);// I2C_DEV /dev/i2c-0
        if(fd < 0){
                printf("####i2c test device open failed####/n");
                return (-1);
        }
    printf("please input slave addr:");
    scanf("%x",&slaveaddr);
    printf("input slave addr is:%x/n",slaveaddr);
    printf("please input reg addr:");
    scanf("%x", &regaddr);
    printf("input slave addr is:%x/n",regaddr);
    res = ioctl(fd,I2C_TENBIT,0);   //not 10bit
           res = ioctl(fd,I2C_SLAVE,slaveaddr);    //����I2C���豸��ַ[6:0]
        while((ch=getchar())!='0'){
        switch(ch){
            case '1':
                printf("getch test success/n");
                break;
            case '2':
                buf[0]=0xFF;
                buf[1]=0xFF;
                buf[2]=0xFF;
                buf[3]=0xFF;
                buf[4]=0xFF;
                printf("read i2c test/n");
                res=iic_read(fd,buf,regaddr,5);
                printf("%d bytes read:",res);
                for(i=0;i<res;i++){
                    printf("%d ",buf[i]);
                }
                printf("/n");
                break;
            case '3':
                buf[0]=0xAA;
                buf[1]=0x55;
                buf[2]=0xAA;
                printf("write i2c test/n");
                res=iic_write(fd,buf,regaddr,2);
                printf("%d bytes write success/n");
                break;
            default:
                printf("bad command/n");
                break;
        }
    }
    return 0;
}
