/*
 * sdcard.c
 *
 *  Created on: 2018. 8. 2.
 *      Author: S.S.C
 */

#include "sdcard.h"

extern FATFS SDFatFS;
extern uint8_t retSD;
static FATFS *fs;

DIR 		dirs;
FILINFO 	Finfo;
FIL			file;

char file_name[100]={0,};

#define MAX_REPORT_LOG	128

char rep_str_buff[256];

FIL File_Sensor_W;
char fname[128];
char dname[128];
char sbuff[256];

char rep_fname[256];
char rep_buff[256];

FILINFO repFinfo;
DIR dirs;


uint8_t test_text[10];
uint32_t bytesWritten, bytesRead;

void sdcardInit()
{
	memset(&s, 0x00, sizeof(SDCardStatus_T));

    fs = (FATFS *)&SDFatFS;

    sds.isInit = sds.getfree = sds.mount = FR_NOT_READY;

    sds.isInit = retSD;
}


void logicSDCardTask()
{
	FRESULT res;

	/*
	if (sds.isInit != FR_OK) {
		sds.isInit = FATFS_LinkDriver(&SD_Driver, (TCHAR const*)SDPath);

		if (sds.isInit != FR_OK) {
			printf("[SD CARD] FatFs Init Error[%d] Error!!!\r\n", sds.isInit);
			osDelay(100);
			return;
		}
	}*/

	if (GET_SD_DET() == FR_OK) {
		sds.sd_det = FR_OK;
		//ls.lampSDDetFlag = pdTRUE;
	} else {
		sds.sd_det = FR_NOT_READY;
		//printf( "[SD CARD] No SD Card !!!\r\n");
	}

	if (sds.sd_det == FR_OK) {
		SET_SD_POWER(GPIO_PIN_SET);
		sds.sd_power = FR_OK;
	} else {
		SET_SD_POWER(GPIO_PIN_RESET);
		sds.sd_power = FR_NOT_READY;
	}

	if ((sds.sd_power == FR_OK) && (sds.mount != FR_OK)) {
		sds.mount = f_mount(&SDFatFS, "", 1);

		if (sds.mount != FR_OK) {
			printf( "[SD CARD] Mount Error[%d]!!!\r\n", sds.mount);
			//ls.lampSDMountFlag = pdFALSE;
		} else {
			printf( "[SD CARD] Mount OK!!!\r\n");
			//ls.lampSDMountFlag = pdTRUE;

			f_opendir_scan();
		}
	}

	/*
	if ((sds.mount == FR_OK) && (sds.getfree != FR_OK)) {
		sds.getfree = f_getfree("", &sds.fre_clust, &SDFatFS);

		if(res == FR_OK){
			int32_t fre_clust, fre_sect, tot_sect;

			tot_sect = (fs->n_fatent - 2) * fs->csize;
			fre_sect = sds.fre_clust * fs->csize;

			printf("\r\n%10lu KB total drive space.\r\n%10lu KB available.\r\n",
					tot_sect / 2, fre_sect / 2);
		}
		else{
			printf("\r\nf_getfree error [%d]\r\n", res);
		}
	}
*/

	if(sds.mount == FR_OK)
	{

		SensorLogSave();

/*
		printf( "[SD CARD] f_mkdir !!!\r\n");

		res = f_mkdir("0:/dir");
		if ((res != FR_OK) && (res != FR_EXIST))
			while (1);

		res = f_open(&file, "0:/YY/STM32.TXT", FA_CREATE_ALWAYS | FA_WRITE);
		if (res != FR_OK)
			while (1);

		UINT written;
		res = f_write(&file, "hello card", 10, &written);
		if (res != FR_OK)
			while (1);

		f_close(&file);
		if (res != FR_OK)
			while (1);

		FILINFO info;
		res = f_stat("0:/STM32.TXT", &info);
		if (res != FR_OK)
			while (1);
		printf( "[SD CARD] End !!!\r\n");*/

	}

    osDelay(1000);
}

void f_opendir_scan(void)
{
    #if _USE_LFN
    TCHAR lfn[_MAX_LFN + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof lfn;
    #endif
    TCHAR path[200] = "";
    FRESULT res;

    if (sds.mount == FR_OK)
    {
    	res = f_opendir(&dirs, path);
        printf("res f_open : %02X\n\r",res);

        if (res == FR_OK)
        {
        while(1)
        {
            char *fn;

            res = f_readdir(&dirs, &Finfo);

            if (res != FR_OK)
                printf("res = %d f_readdir\n\r", res);

            if ((res != FR_OK) || (Finfo.fname[0] == 0))
                break;

#if _USE_LFN
      fn = *fno.lfname ? fno.lfname : fno.fname;
#else
      fn = Finfo.fname;
#endif
            printf("%c%c%c%c ",
                ((Finfo.fattrib & AM_DIR) ? 'D' : '-'),
                ((Finfo.fattrib & AM_RDO) ? 'R' : '-'),
                ((Finfo.fattrib & AM_SYS) ? 'S' : '-'),
                ((Finfo.fattrib & AM_HID) ? 'H' : '-') );

            printf("%10d ", Finfo.fsize);

            printf("%s/%s\n\r", path, fn);
        }
        }

        //res = f_mount(0, "" , 0);
        //printf("SD Unmount : res f_mount : %02X\n\r",res);
    }
}


static LOG_EVENT_T sensor_log;

void loginit()
{
	memset((uint8_t*)&sensor_log, 0, sizeof(LOG_EVENT_T));
}

void logicLogTask()
{
	//SensorLogSave();
}

void report_append_str(RTC_DateTypeDef sDate, RTC_TimeTypeDef sTime, char *str)
{
	sensor_log.buff[sensor_log.wbp].sDate = sDate;
	sensor_log.buff[sensor_log.wbp].sTime = sTime;

	sprintf(sensor_log.buff[sensor_log.wbp].msg,
			"20%2.2u/%2.2u/%2.2u %2.2u:%2.2u:%2.2u, ", sDate.Year, sDate.Month,
			sDate.Date, sTime.Hours, sTime.Minutes, sTime.Seconds);

	strncat(sensor_log.buff[sensor_log.wbp].msg, str, strlen(str));
	printf("%s", sensor_log.buff[sensor_log.wbp].msg);

	printf("\r\n");

	sensor_log.wbp++;
	if (sensor_log.wbp >= MAX_REPORT_LOG) {
		sensor_log.wbp = 0;
	}
}

void SensorLogSave(void)
{
	uint16_t len;
	FRESULT rc;				/* Result code */
	unsigned int bw;
	DIR dir;				/* Directory object */

	if(sensor_log.rbp==sensor_log.wbp){
		return;
	}


	sprintf(fname, "/yy-%2.2u/MM%2.2u_%2.2u.txt",
			sensor_log.buff[sensor_log.rbp].sDate.Year, sensor_log.buff[sensor_log.rbp].sDate.Month, sensor_log.buff[sensor_log.rbp].sDate.Date);

	rc = f_open(&File_Sensor_W, fname, FA_OPEN_ALWAYS | FA_WRITE);
	if(rc){

		//printf( "[LogSave] f_open[0] Error[%d][%s]!!!\r\n", rc, fname);
		sprintf(dname, "/yy-%2.2u", sensor_log.buff[sensor_log.rbp].sDate.Year);
		rc= f_opendir(&dir, dname);
		if(rc){
			//printf( "[LogSave] f_opendir Error[%d]!!!\r\n", rc);
			rc= f_mkdir(dname);
			if(rc)
			{
				printf( "[LogSave] f_mkdir Error[%d]!!!\r\n", rc);
			}
			//else
				//printf( "[LogSave] f_mkdir[%s]!!!\r\n", dname);
		}
		else
		{
			sprintf(fname, "/MM%2.2u_%2.2u.txt",sensor_log.buff[sensor_log.rbp].sDate.Month, sensor_log.buff[sensor_log.rbp].sDate.Date);
		}

		rc = f_open(&File_Sensor_W, fname, FA_WRITE | FA_OPEN_ALWAYS);
		if(rc){
			//printf( "[LogSave] f_open[2] Error[%d][%s]!!!\r\n", rc, fname);
			return;
		}
	}

	rc = f_lseek(&File_Sensor_W, f_size(&File_Sensor_W));
	if(rc){
		f_close(&File_Sensor_W);

		return;
	}


	strcpy(sbuff, sensor_log.buff[sensor_log.rbp].msg);
	len= strlen(sbuff);

	rc = f_write(&File_Sensor_W, sbuff, len, &bw);
	if(rc){
		f_close(&File_Sensor_W);
		return;
	}

	rc = f_close(&File_Sensor_W);
	if(rc){
	}

	printf("[SensorLogSave] End [%d]\r\n", sensor_log.rbp);

	sensor_log.rbp++;
	if(sensor_log.rbp>=MAX_EVENT){
		sensor_log.rbp= 0;
	}
}

