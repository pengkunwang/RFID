#include "data.h"

//创建表
char *creat_table(sqlite3 *db)
{
	char *ErrMsg;
	int rc;
	char *sql = "create table carinfo(cardid INT PRIMARY KEY, licence TEXT NOT NULL, time_in TEXT, photo TEXT);";
	rc = sqlite3_exec(db, sql, NULL, 0, &ErrMsg);
	if(rc != SQLITE_OK)
	{
		sqlite3_free(ErrMsg);
	}
	return "carinfo";
}
//查找所有信息的回调函数
int show_cabk(void *num, int argc, char **argv, char **colname)
{
	int i;
	int *p = (int *)num;
	(*p) += 1;
	for(i=0; i<argc; i++)
	{
		printf("%s\t", argv[i]);
	}
	printf("\n");
	return 0;
}

//刷卡的回调函数
int scan_card_cbak(void *time, int argc, char **argv, char **colname)
{
	int i;
	int mony = 0;
	int *p = (int *)time;
	for(i=0; i<argc; i++)
	{
		if(strcmp(colname[i], "licence") == 0)
		{
			printf("%s\t", argv[i]);
		}
		if(strcmp(colname[i], "time_in") == 0)
		{
			
			if(strcmp(argv[i], "NO") == 0)   //进场
			{
				*p = 0;
			}
			else   //出场
			{
				printf("intput time: %s\t now time: %s\n", argv[i], gettime());
				*p = 2;
			}
		}
	}

	return 0;
}
 
void show_all(sqlite3 *db)
{
	int rc;
	int find_num = 0;
	char *zErrMsg = 0;
	rc = sqlite3_exec(db, "select * from carinfo;",
					show_cabk, &find_num, &zErrMsg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error:%s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
}

int find(sqlite3 *db, char *cardid)
{
	int find_num = 0;
	int rc;
	char *zErrMsg = 0;
	char buff[512];
	bzero(buff, sizeof(buff));
	sprintf(buff, "select * from carinfo where cardid = '%s'", cardid);
	rc = sqlite3_exec(db, buff, show_cabk, &find_num, &zErrMsg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "find SQL error:%s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	return find_num;
}


//刷卡进场
int scan_card(sqlite3 *db, char *cardid)
{
	int time = -1;
	int rc;
	char *zErrMsg = 0;
	char buff[512];
	bzero(buff, sizeof(buff));
	sprintf(buff, "select * from carinfo where cardid = '%s'", cardid);
	rc = sqlite3_exec(db, buff, scan_card_cbak, &time, &zErrMsg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "scan_card SQL error:%s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}

	if(time == 0)   //进场
	{
		printf("welcome\n");
		update(db, cardid, gettime());
	}
	else if(time > 0)   //出场time为停车时间
	{
		update(db, cardid, "NO");
		printf("you cann out!!\n");
	}
	return time;

}

void insert(sqlite3 *db, char *carid, char *licence, char *time_in, char *photo)
{
	int rc;
	char *zErrMsg = 0;
	char buff[512];
	bzero(buff, sizeof(buff));
	sprintf(buff, "insert into carinfo values('%s', '%s', '%s', '%s')",
			carid, licence, time_in, photo);

	rc = sqlite3_exec(db, buff, NULL, NULL, &zErrMsg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error:%s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
}

void delete(sqlite3 *db, char *cardid)
{
	int rc;
	char *zErrMsg = 0;
	char buff[512];
	bzero(buff, sizeof(buff));
	sprintf(buff, "delete from carinfo where cardid = '%s'", cardid);

	rc = sqlite3_exec(db, buff, NULL, NULL, &zErrMsg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error:%s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
}

void update(sqlite3 *db, char *cardid, char *time)
{
	int rc;
	char *zErrMsg = 0;
	char buff[512];
	bzero(buff, sizeof(buff));
	sprintf(buff, "update carinfo set time_in = '%s' where cardid = '%s'",time, cardid);

	rc = sqlite3_exec(db, buff, NULL, NULL, &zErrMsg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error:%s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	
}

char *gettime()
{
	char *timebuf = malloc(25);
	bzero(timebuf,25);
	struct tm *p;
	time_t seconds = time((time_t *)NULL);
	p = gmtime(&seconds);
	sprintf(timebuf, "%d-%d-%d %d:%d.%d", (1900+p->tm_year), (1+p->tm_mon), (p->tm_mday),
										(p->tm_hour), (p->tm_min), (p->tm_sec));
	return timebuf;
}