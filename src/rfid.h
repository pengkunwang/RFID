#ifndef __RFID_H_
#define __RFID_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/unistd.h>
#include <sys/select.h>

#define SERIAL1 "/dev/ttySAC1"

struct
{
	unsigned char recvSucess; //非零表示成功接收
	unsigned char FrameLen; //帧长度
	unsigned char CmdType;//命令类型
	unsigned char CmdStats;//命令状态
	unsigned char InfoLength;//信息到长度
	unsigned char data_num; //当前读取到的数据位

}frame;

char Info[55]; //接收到的信息

/*
 @fd	:文件描述符
 @FrameLen:	数据帧长度
 @CmdType:	命令类型
 @Cmd	:命令
 @InfoLen:数据信息的长度
 @info	:指向数据到指针
 */
void send_base(int fd, unsigned char FrameLen, char CmdType,
		char Cmd, char InfoLen, char *info);


//发送读设备信息的指令
void Get_Dev_Info(int fd);


//发送配置读卡芯片的指令
void PCD_Config(int fd);

//发送关闭读卡芯片的指令
void PCD_Close(int fd);

//发送请求命令
void PiccRequest(int fd);

//发送防碰撞命令
void PiccAnticoll(int fd);

//解析接收数据
int parse_recv(char byte);

//初始化串口
void init_tty(int fd);

//接收rfid数据   如正确读取则能返回carID 否则返回0
int Recv_rfid(int fd);
#endif