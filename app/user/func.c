/*
 * func.c
 *
 *  Created on: 2019��6��2��
 *      Author: Administrator
 */

#include "func.h"

day_Weather weather_Info;
#define GET_WEATHER_TIMES 200
#define RESET_TIMES 18
os_timer_t os_Get_Weather5min_Temp3s;

void string_Packet_Publish(MQTT_Client* client,char *data)
{

	uint8 size = os_strlen(data);
	char *payload = (char *)os_zalloc(size+3);
	payload[0] = 0x05 & 0xff;
	payload[1] = (size>>8) & 0xff;
	payload[2] = size & 0xff;
	os_memcpy(payload+3, data, size);

	MQTT_Publish(client, "$dp", payload, size+3, 0, 0);
	save_Mqtt_Info(&mqtt_Info);
	os_free(payload);


}
void ICACHE_FLASH_ATTR save_Mqtt_Info(MQTT_Info *pdata)
{
	spi_flash_erase_sector(0x98);						// ��������
	spi_flash_write(0x98*4096, (uint32 *)pdata, sizeof(MQTT_Info));	// д������
}
// SmartConfig״̬�����ı�ʱ������˻ص�����
//--------------------------------------------
// ����1��sc_status status / ����2��������ָ�롾�ڲ�ͬ״̬�£�[void *pdata]�Ĵ�������ǲ�ͬ�ġ�
//=================================================================================================================
void ICACHE_FLASH_ATTR smartconfig_done(sc_status status, void *pdata)
{
	os_printf("\r\n------ smartconfig_done ------\r\n");	// ESP8266����״̬�ı�

    switch(status)
    {
    	// CmartConfig�ȴ�
		//����������������������������������������
		case SC_STATUS_WAIT:		// ��ʼֵ
			os_printf("\r\nSC_STATUS_WAIT\r\n");
		break;
		//����������������������������������������

		// ���֡�WIFI�źš���8266������״̬�µȴ�������
		//��������������������������������������������������������������������������
		case SC_STATUS_FIND_CHANNEL:
			os_printf("\r\nSC_STATUS_FIND_CHANNEL\r\n");

			os_printf("\r\n---- Please Use WeChat to SmartConfig ------\r\n\r\n");

		break;
		//��������������������������������������������������������������������������

        // ���ڻ�ȡ��SSID����PSWD��
        //��������������������������������������������������������������������������
        case SC_STATUS_GETTING_SSID_PSWD:
            os_printf("\r\nSC_STATUS_GETTING_SSID_PSWD\r\n");

            // ��SC_STATUS_GETTING_SSID_PSWD��״̬�£�����2==SmartConfig����ָ��
            //-------------------------------------------------------------------
			sc_type *type = pdata;		// ��ȡ��SmartConfig���͡�ָ��

			// ������ʽ == ��ESPTOUCH��
			//-------------------------------------------------
            if (*type == SC_TYPE_ESPTOUCH)
            { os_printf("\r\nSC_TYPE:SC_TYPE_ESPTOUCH\r\n"); }

            // ������ʽ == ��AIRKISS��||��ESPTOUCH_AIRKISS��
            //-------------------------------------------------
            else
            { os_printf("\r\nSC_TYPE:SC_TYPE_AIRKISS\r\n"); }

	    break;
	    //��������������������������������������������������������������������������

	    // �ɹ���ȡ����SSID����PSWD��������STA������������WIFI
	    //��������������������������������������������������������������������������
        case SC_STATUS_LINK:
            os_printf("\r\nSC_STATUS_LINK\r\n");

            // ��SC_STATUS_LINK��״̬�£�����2 == STA����ָ��
            //------------------------------------------------------------------
            struct station_config *sta_conf = pdata;	// ��ȡ��STA������ָ��

            // ����SSID����PASS�����浽���ⲿFlash����
            //--------------------------------------------------------------------------
			spi_flash_erase_sector(Sector_STA_INFO);						// ��������
			spi_flash_write(Sector_STA_INFO*4096, (uint32 *)sta_conf, 96);	// д������
			//--------------------------------------------------------------------------

	        wifi_station_set_config(sta_conf);			// ����STA������Flash��
	        wifi_station_disconnect();					// �Ͽ�STA����
	        wifi_station_connect();						// ESP8266����WIFI


	    break;
	    //��������������������������������������������������������������������������


        // ESP8266��ΪSTA���ɹ����ӵ�WIFI
	    //��������������������������������������������������������������������������
        case SC_STATUS_LINK_OVER:
            os_printf("\r\nSC_STATUS_LINK_OVER\r\n");

            smartconfig_stop();		// ֹͣSmartConfig
			if(	wifi_softap_set_config(&AP_Config) == true)
				{

					ESP8266_AP_NetCon_Init();
				}

			 ESP8266_STA_NetCon_Init();
			 ESP8266_SNTP_Init();
			os_printf("\r\n---- ESP8266 Connect to WIFI Successfully ----\r\n");


	    break;
	    //��������������������������������������������������������������������������

    }
}


void ICACHE_FLASH_ATTR os_Get_Weather_Temp_Cb()
{
	static uint8 timesOfWeather = 0;
	static uint8 timesOfRestart = 0;
	if(DHT11_Read_Data_Complete() == 0)		// ��ȡDHT11��ʪ��ֵ
		{
			// ���������ʪ��
			//---------------------------------------------------------------------------------
			if(DHT11_Data_Array[5] == 1)			// �¶� >= 0��
			{
/*				os_printf("\r\nʪ�� == %d.%d%RH\r\n",DHT11_Data_Array[0],DHT11_Data_Array[1]);
				os_printf("\r\n�¶� == %d.%d ��\r\n\r\n", DHT11_Data_Array[2],DHT11_Data_Array[3]);*/
				os_printf("@0x02: T: %d %d H: %d %d \r\n",DHT11_Data_Array[2],DHT11_Data_Array[3],DHT11_Data_Array[0],DHT11_Data_Array[1]);
			}
			else // if(DHT11_Data_Array[5] == 0)	// �¶� < 0��
			{
				/*os_printf("\r\nʪ�� == %d.%d %RH\r\n",DHT11_Data_Array[0],DHT11_Data_Array[1]);
				os_printf("\r\n�¶� == -%d.%d ��\r\n",DHT11_Data_Array[2],DHT11_Data_Array[3]);*/
				os_printf("@0x02: T: %d %d H: %d %d \r\n",DHT11_Data_Array[2],DHT11_Data_Array[3],DHT11_Data_Array[0],DHT11_Data_Array[1]);
			}
			char *str;
			str = os_zalloc(os_strlen(TEMPER_HUM));
			os_sprintf(str, TEMPER_HUM,DHT11_Data_Array[0],DHT11_Data_Array[1],
					DHT11_Data_Array[2],DHT11_Data_Array[3]);
			string_Packet_Publish(&mqttClient, str);
			//DHT11_NUM_Char();	// DHT11����ֵת���ַ���
			os_free(str);
		}
	timesOfWeather++;
/*	os_printf("%d\r\n",timesOfWeather);*/
	if(timesOfWeather == GET_WEATHER_TIMES)
	{
		espconn_connect(&STA_NetCon);
		timesOfWeather = 0;
		timesOfRestart ++;

	}
	if(timesOfRestart == RESET_TIMES)
		system_restart();
}

void ICACHE_FLASH_ATTR os_Get_Weather_Temp_Init(uint32 time_ms, uint8 time_repetitive)
{
	os_timer_disarm(&os_Get_Weather5min_Temp3s);
	os_timer_setfn(&os_Get_Weather5min_Temp3s, (os_timer_func_t *)os_Get_Weather_Temp_Cb, NULL);
	os_timer_arm(&os_Get_Weather5min_Temp3s, time_ms, time_repetitive);
}

void ICACHE_FLASH_ATTR write_today_weather(cJSON * json)
{
	cJSON * json_value;
	json_value = cJSON_GetObjectItem(json, "date");
	os_strcpy(weather_Info.today_Weather.date, json_value->valuestring);

	json_value = cJSON_GetObjectItem(json, "week");
	os_strcpy(weather_Info.today_Weather.week, json_value->valuestring);

	json_value = cJSON_GetObjectItem(json, "dayweather");
	os_strcpy(weather_Info.today_Weather.dayweather, json_value->valuestring);

	json_value = cJSON_GetObjectItem(json, "nightweather");
	os_strcpy(weather_Info.today_Weather.nightweather, json_value->valuestring);

	json_value = cJSON_GetObjectItem(json, "daytemp");
	os_strcpy(weather_Info.today_Weather.daytemp, json_value->valuestring);

	json_value = cJSON_GetObjectItem(json, "nighttemp");
	os_strcpy(weather_Info.today_Weather.nighttemp, json_value->valuestring);



}

void ICACHE_FLASH_ATTR write_nextday_weather(cJSON * json)
{
	cJSON * json_value;
	json_value = cJSON_GetObjectItem(json, "date");
	os_strcpy(weather_Info.next_Weather.date, json_value->valuestring);

	json_value = cJSON_GetObjectItem(json, "week");
	os_strcpy(weather_Info.next_Weather.week, json_value->valuestring);

	json_value = cJSON_GetObjectItem(json, "dayweather");
	os_strcpy(weather_Info.next_Weather.dayweather, json_value->valuestring);

	json_value = cJSON_GetObjectItem(json, "nightweather");
	os_strcpy(weather_Info.next_Weather.nightweather, json_value->valuestring);

	json_value = cJSON_GetObjectItem(json, "daytemp");
	os_strcpy(weather_Info.next_Weather.daytemp, json_value->valuestring);

	json_value = cJSON_GetObjectItem(json, "nighttemp");
	os_strcpy(weather_Info.next_Weather.nighttemp, json_value->valuestring);



}

void ICACHE_FLASH_ATTR write_afterday_weather(cJSON * json)
{
	cJSON * json_value;
	json_value = cJSON_GetObjectItem(json, "date");
	os_strcpy(weather_Info.after_Weather.date, json_value->valuestring);
	json_value = cJSON_GetObjectItem(json, "week");
	os_strcpy(weather_Info.after_Weather.week, json_value->valuestring);
	json_value = cJSON_GetObjectItem(json, "dayweather");
	os_strcpy(weather_Info.after_Weather.dayweather, json_value->valuestring);
	json_value = cJSON_GetObjectItem(json, "nightweather");
	os_strcpy(weather_Info.after_Weather.nightweather, json_value->valuestring);
	json_value = cJSON_GetObjectItem(json, "daytemp");
	os_strcpy(weather_Info.after_Weather.daytemp, json_value->valuestring);
	json_value = cJSON_GetObjectItem(json, "nighttemp");
	os_strcpy(weather_Info.after_Weather.nighttemp, json_value->valuestring);

}

void ICACHE_FLASH_ATTR write_weather(cJSON *json, weather *day_weather)
{
	cJSON * json_value;
	json_value = cJSON_GetObjectItem(json, "date");
	os_strcpy(day_weather->date, json_value->valuestring);
	json_value = cJSON_GetObjectItem(json, "week");
	os_strcpy(day_weather->week, json_value->valuestring);
	json_value = cJSON_GetObjectItem(json, "dayweather");
	os_strcpy(day_weather->dayweather, json_value->valuestring);
	json_value = cJSON_GetObjectItem(json, "nightweather");
	os_strcpy(day_weather->nightweather, json_value->valuestring);
	json_value = cJSON_GetObjectItem(json, "daytemp");
	os_strcpy(day_weather->daytemp, json_value->valuestring);
	json_value = cJSON_GetObjectItem(json, "nighttemp");
	os_strcpy(day_weather->nighttemp, json_value->valuestring);

}
//============================================================================
// MQTT����  API���ӻ�ȡ�¶� ���� ������SNTP������
//============================================================================

void ICACHE_FLASH_ATTR ESP8266_Func_Cmd()
{
	MQTT_Connect(&mqttClient);
	ESP8266_STA_NetCon_Init();
	os_Get_Weather_Temp_Init(5000,1);
}

// OS_Timer_1_cb �ص�����
void ICACHE_FLASH_ATTR OS_Timer_1_Cb()
{
	os_printf("@0X01: SW0: %s TH: %s TM: %s\r\n",mqtt_Info.switch_state, mqtt_Info.hour,mqtt_Info.min);
		u8 S_WIFI_STA_Connect;			// WIFI����״̬��־

		struct ip_info ST_ESP8266_IP;	// ESP8266��IP��Ϣ
		u8 ESP8266_IP[4];				// ESP8266��IP��ַ

		// ��ѯSTA����WIFI״̬
		//-----------------------------------------------------
		S_WIFI_STA_Connect = wifi_station_get_connect_status();
		//---------------------------------------------------
		// Station����״̬��
		// 0 == STATION_IDLE -------------- STATION����
		// 1 == STATION_CONNECTING -------- ��������WIFI
		// 2 == STATION_WRONG_PASSWORD ---- WIFI�������
		// 3 == STATION_NO_AP_FOUND ------- δ����ָ��WIFI
		// 4 == STATION_CONNECT_FAIL ------ ����ʧ��
		// 5 == STATION_GOT_IP ------------ ���IP�����ӳɹ�
		//---------------------------------------------------

		switch(S_WIFI_STA_Connect)
		{
			case 0 : 	os_printf("\nSTATION_IDLE\n");				break;
			case 1 : 	os_printf("\nSTATION_CONNECTING\n");		break;
			case 2 : 	os_printf("\nSTATION_WRONG_PASSWORD\n");	break;
			case 3 : 	os_printf("\nSTATION_NO_AP_FOUND\n");		break;
			case 4 : 	os_printf("\nSTATION_CONNECT_FAIL\n");		break;
			case 5 : 	os_printf("\nSTATION_GOT_IP\n");			break;
		}

		// �ɹ�����WIFI��STAģʽ�£��������DHCP(Ĭ��)����ESO8266��IP��ַ��WIFI·�����Զ����䡿
		//----------------------------------------------------------------------------------------
		if( S_WIFI_STA_Connect == STATION_GOT_IP )	// �ж��Ƿ��ȡIP
		{
			// ��ȡESP8266_Stationģʽ�µ�IP��ַ
			// DHCP-ClientĬ�Ͽ�����ESP8266����WIFI����·��������IP��ַ��IP��ַ��ȷ��
			//--------------------------------------------------------------------------
			wifi_get_ip_info(STATION_IF,&ST_ESP8266_IP);	// ����2��IP��Ϣ�ṹ��ָ��

			// ESP8266_AP_IP.ip.addr��32λ�����ƴ��룬ת��Ϊ���ʮ������ʽ
			//----------------------------------------------------------------------------
			ESP8266_IP[0] = ST_ESP8266_IP.ip.addr;		// IP��ַ�߰�λ == addr�Ͱ�λ
			ESP8266_IP[1] = ST_ESP8266_IP.ip.addr>>8;	// IP��ַ�θ߰�λ == addr�εͰ�λ
			ESP8266_IP[2] = ST_ESP8266_IP.ip.addr>>16;	// IP��ַ�εͰ�λ == addr�θ߰�λ
			ESP8266_IP[3] = ST_ESP8266_IP.ip.addr>>24;	// IP��ַ�Ͱ�λ == addr�߰�λ

			// ��ʾESP8266��IP��ַ
			//-----------------------------------------------------------------------------------------------
			os_printf("ESP8266_IP = %d.%d.%d.%d\n",
					ESP8266_IP[0],ESP8266_IP[1],ESP8266_IP[2],ESP8266_IP[3]);

			os_timer_disarm(&OS_Timer_1);
			wifi_set_opmode(0x03);
			if(	wifi_softap_set_config(&AP_Config) == true)
				{

					ESP8266_AP_NetCon_Init();
				}
			ESP8266_SNTP_Init();
		}
		// ESP8266�޷�����WIFI
			//------------------------------------------------------------------------------------------------
			else if(	S_WIFI_STA_Connect==STATION_NO_AP_FOUND 	||		// δ�ҵ�ָ��WIFI
						S_WIFI_STA_Connect==STATION_WRONG_PASSWORD 	||		// WIFI�������
						S_WIFI_STA_Connect==STATION_CONNECT_FAIL		)	// ����WIFIʧ��
			{
				os_timer_disarm(&OS_Timer_1);			// �رն�ʱ��

				os_printf("\r\n---- ESP8266 Can't Connect to WIFI-----------\r\n");


				// ΢��������������
				//��������������������������������������������������������������������������������������������
				//wifi_set_opmode(STATION_MODE);		// ��ΪSTAģʽ							//���ڢٲ���

				smartconfig_set_type(SC_TYPE_AIRKISS); 	// ESP8266������ʽ��AIRKISS��			//���ڢڲ���

				smartconfig_start(smartconfig_done);	// ���롾��������ģʽ��,�����ûص�����	//���ڢ۲���
				//��������������������������������������������������������������������������������������������

			}
}
