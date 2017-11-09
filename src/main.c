#include "rfid.h"
#include "data.h"

int main(int argc, char **argv)
{
	//打开串口
	int fd = open(SERIAL1, O_RDWR | O_NOCTTY);
	init_tty(fd);
	
	//打开数据库
	sqlite3 *db;
	int rc;
	rc = sqlite3_open_v2("parking.db", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
	if(rc != SQLITE_OK)
	{
		printf("open datebase err!!!\n");
		sqlite3_close(db);
		return 0;
	}
	printf("[[open sucessful!]]\n");

	char *table_name;
	table_name = creat_table(db);
	printf("table name: %s\n", table_name);
/***********************************************/
	char choice;
	int flag;
	
	fd_set rset;
	struct timeval tv, tv2;
	tv.tv_sec = 0;
	tv.tv_usec = 500000;


	while(1)
	{
		FD_ZERO(&rset);         
		FD_SET(fd, &rset);
		FD_SET(STDIN_FILENO, &rset);
		memcpy(&tv2, &tv, sizeof(struct timeval));

		flag = select(fd+1, &rset, NULL, NULL, &tv2);

		if(FD_ISSET(fd, &rset))
		{
			int carID = Recv_rfid(fd);   
			if(carID != 0)	//获取到卡的正确的ID
			{
				//printf("carID = %X\n", carID);
				char ID[9];
				char licence[9];
				char date[20];
				bzero(ID, sizeof(ID));
				bzero(licence, sizeof(licence));
				bzero(date, sizeof(date));
				sprintf(ID, "%X", carID);
				ID[9] = '\0';
				//printf("%s\n", ID);
				if((find(db, ID) == 0))   //如果数据库中没有找到该卡的信息
				{
					printf("please input licence:");
					scanf("%s", licence);
					insert(db, ID, licence, gettime(), 0);
					printf("add car sucessful!\n");
				}
				else
				{
					scan_card(db, ID);
				}
			}
		}


		if(FD_ISSET(STDIN_FILENO, &rset))
		{
			scanf("%c", &choice);
			if(choice == 'a')
			{
				insert(db, "ABCD782F", "yueA2589", "2017-9-13-20-03", "/home/peng");
			}
			else if(choice == 'd')
			{
				delete(db, "ABCD782F");
			}
			else if(choice == 's')
			{
				show_all(db);
			}
			else if(choice == 'f')
			{
				find(db, "ABCD782F");
			}
			else if(choice == 't')
			{
				printf("%s\n", gettime());
			}
			else if(choice == '1')
			{	
				//Get_Dev_Info(fd);
				//PiccRequest(fd);
			}
			else if(choice == '2')
			{
				//PiccAnticoll(fd);
			}
		}

		if(flag == 0)  //超时
		{
			PiccRequest(fd);
		}


	}

	return 0;
}
