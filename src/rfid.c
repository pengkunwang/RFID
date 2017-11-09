
#include "rfid.h"

/*
 @fd	:文件描述符
 @FrameLen:	数据帧长度
 @CmdType:	命令类型
 @Cmd	:命令
 @InfoLen:数据信息的长度
 @info	:指向数据到指针
 */
void send_base(int fd, unsigned char FrameLen, char CmdType,
		char Cmd, char InfoLen, char *info)
{
	static char SEQ = 0;
	if(SEQ >= 16)
		SEQ = 0;
	char buff[FrameLen];
	bzero(buff,sizeof(buff));
	buff[0] = FrameLen;
	char tmp = 0;
	tmp = SEQ << 4;
	tmp |= CmdType;
	buff[1] = tmp;
	buff[2] = Cmd;
	buff[3] = InfoLen;
	if(InfoLen != 0)
		memcpy(buff+4, info, InfoLen);
	char BCC = 0;
	char i;
	for(i=0; i<buff[0]-2; i++)
	{
		BCC ^= buff[i];
	}
	buff[buff[0]-2] = ~BCC;
	buff[buff[0]-1] = 0x03;

	write(fd, buff, buff[0]);
	SEQ++;
	//printf("SEQ:%.2x\n", tmp);
}

//发送读设备信息的指令
void Get_Dev_Info(int fd)
{
	send_base(fd, 0x06, 0x01, 0x41, 0, NULL);
}

//发送配置读卡芯片的指令
void PCD_Config(int fd)
{
	send_base(fd, 0x06, 0x01, 0x42, 0, NULL);
}

//发送关闭读卡芯片的指令
void PCD_Close(int fd)
{
	send_base(fd, 0x06, 0x01, 0x43, 0, NULL);
}

//发送请求命令
void PiccRequest(int fd)
{
	static char info = 0x52; //ALL
	send_base(fd, 0x07, 0x02, 0x41, 0x01, &info);
}

//发送防碰撞命令
void PiccAnticoll(int fd)
{
	static char info[2] = {0x93, 0x00};
	send_base(fd, 0x08, 0x02, 0x42, 0x02, info);
}


//解析接收数据
int parse_recv(char byte)
{
	static int stat = 0;
	switch(stat)
	{
		case 0:
			bzero(&frame, sizeof(frame));
			bzero(Info, sizeof(Info));
			frame.FrameLen = byte;
			stat++;
			break;
		case 1:
			frame.CmdType = byte;
			stat++;
			break;
		case 2:
			frame.CmdStats = byte;
			stat++;
			break;
		case 3:
			frame.InfoLength = byte;
			stat++;
			break;
		case 4:
			Info[frame.data_num++] = byte;
			if(frame.data_num >= frame.InfoLength)   //数据接受完毕
			{
				
				stat++;
			}
			break;
		case 5:
			stat++;
			break;

		case 6:
			stat++;
			if(byte == 0x03)
			{
				stat = 0;
				frame.recvSucess = 1;
				return 0;
			}
			break;
		default:
			if(byte == 0x03)
				stat = 0;
			break;
	}
	return -1;
}

//接收rfid数据   如正确读取则能返回carID 否则返回0
int Recv_rfid(int fd)
{
	char RBuf[2];
	bzero(RBuf, sizeof(RBuf));
	read(fd, RBuf, 1);
	int carID = 0;
		//printf("0x%.2x  ", RBuf[0]);
	if(parse_recv(RBuf[0]) == 0) //接收完一条完整到帧
	{
		static int stat = 0;
		switch(stat)
		{
			case 0:    //接收请求信息状态
				if(frame.CmdStats == 0)
				{
					stat = 1;
					PiccAnticoll(fd);
				}
				else
				{
					stat = 0;
				}
				break;

			case 1:    //接收防碰撞信息状态
				carID = 0;
				if(frame.CmdStats == 0)
				{
					int i;
					char *p = (char *)&carID;
					for(i=0; i<frame.InfoLength; i++)
					{
						memcpy(p+i, &Info[i], 1);
						//printf("0x%.2x  ", Info[i]);
					}
					printf("\n");
				}
				stat = 0;
				return carID;
				break;

			default:
				stat = 0;
				break;
		}
	}
	return 0;
}


void init_tty(int fd)
{
	struct termios Opt;
	bzero(&Opt, sizeof(Opt));
	tcgetattr(fd, &Opt);
	cfmakeraw(&Opt);
	cfsetispeed(&Opt, B9600);
	cfsetospeed(&Opt, B9600);

	Opt.c_cflag |= CLOCAL | CREAD;

	Opt.c_cflag &= ~PARENB; 
	Opt.c_cflag &= ~CSTOPB; 
	Opt.c_cflag &= ~CSIZE; 
	Opt.c_cflag |= CS8;

	Opt.c_cc[VTIME] = 0;
	Opt.c_cc[VMIN] = 1;

	tcflush(fd, TCIFLUSH);

	if(tcsetattr(fd, TCSANOW, &Opt))
		printf("Setting the serial1 faild!\n");
}
