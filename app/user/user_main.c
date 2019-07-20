/* main.c -- MQTT client example
*
* Copyright (c) 2014-2015, Tuan PM <tuanpm at live dot com>
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* * Redistributions of source code must retain the above copyright notice,
* this list of conditions and the following disclaimer.
* * Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
* * Neither the name of Redis nor the names of its contributors may be used
* to endorse or promote products derived from this software without
* specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/




// ͷ�ļ�
//==============================

#include "func.h"

//=================================
//Ԥ����
//=================================

//==============================
// ���Ͷ���
//=================================
typedef unsigned long 		u32_t;

//=================================
// ȫ�ֱ���
//============================================================================
MQTT_Client mqttClient;			// MQTT�ͻ���_�ṹ�塾�˱����ǳ���Ҫ��
MQTT_Info mqtt_Info;
extern day_Weather weather_Info;
uint32 priv_param_start_sec;
ip_addr_t IP_Server;
struct espconn AP_NetCon;
struct espconn STA_NetCon;
os_timer_t OS_Timer_1;		// �����ʱ����ѯip����
os_timer_t OS_Timer_Dht11;   //dht11
os_timer_t OS_Timer_SNTP;
struct softap_config AP_Config;
struct station_config STA_Config;	// STA�����ṹ��

char waether_buf[1000]="";
uint8 times_rec =0;
//============================================================================
//�Զ���delay_ms
//============================================================================
void ICACHE_FLASH_ATTR delay_ms(uint32 ms)
{
	uint32 i = 0;
	for(i = 0; i < ms ; i++)
	{
		os_delay_us(1000);
	}
}
//============================================================================
// DNS_������������_�ص�����������1�������ַ���ָ�� / ����2��IP��ַ�ṹ��ָ�� / ����3���������ӽṹ��ָ�롿
//============================================================================
void ICACHE_FLASH_ATTR DNS_Over_Cb(const char * name, ip_addr_t *ipaddr, void *arg)
{
	struct espconn * T_arg = (struct espconn *)arg;	// �����������ӽṹ��ָ��
	if(ipaddr == NULL)		// ��������ʧ��
	{
		os_printf("\r\n---- DomainName Analyse Failed ----\r\n");
		espconn_gethostbyname(&STA_NetCon, DNS_Server, &IP_Server, DNS_Over_Cb);
		return;
	}

	else if (ipaddr != NULL && ipaddr->addr != 0)		// ���������ɹ�
	{
		os_printf("\r\n---- DomainName Analyse Succeed ----\r\n");

		IP_Server.addr = ipaddr->addr;					// ��ȡ������IP��ַ
		// ���������ķ�����IP��ַ��ΪTCP���ӵ�Զ��IP��ַ
		os_memcpy(T_arg->proto.tcp->remote_ip, &IP_Server.addr, 4);	// ���÷�����IP��ַ
		// ��ʾIP��ַ
		//------------------------------------------------------------------------------
		os_printf("\r\nIP_Server = %d.%d.%d.%d\r\n",						// ���ڴ�ӡ
				*((u8*)&IP_Server.addr),	*((u8*)&IP_Server.addr+1),
				*((u8*)&IP_Server.addr+2),	*((u8*)&IP_Server.addr+3));
		espconn_connect(T_arg);	// ����TCP-server

	}
}
//============================================================================
// STA TCP�������ӶϿ��ɹ��Ļص�����
//============================================================================
void ICACHE_FLASH_ATTR ESP8266_TCP_Disconnect_Cb(void *arg)
{
	delay_ms(1000);
	os_printf("@0x04: C:%s TD: %s TW: %s TDW: %s TNW: %s TDT: %s TNT: %s\r\n",
			weather_Info.city,
			weather_Info.today_Weather.date,
			weather_Info.today_Weather.week,
			weather_Info.today_Weather.dayweather,
			weather_Info.today_Weather.nightweather,
			weather_Info.today_Weather.daytemp,
			weather_Info.today_Weather.nighttemp
	);
	delay_ms(500);
	os_printf("@0x04: ND: %s NW: %s NDW: %s NNW: %s NDT: %s DNT: %s\r\n",
			weather_Info.next_Weather.date,
			weather_Info.next_Weather.week,
			weather_Info.next_Weather.dayweather,
			weather_Info.next_Weather.nightweather,
			weather_Info.next_Weather.daytemp,
			weather_Info.next_Weather.nighttemp
	);
	delay_ms(500);
	os_printf("@0x04: AD: %s AW: %s ADW: %s ANW: %s ADT: %s ANT: %s\r\n",
			weather_Info.after_Weather.date,
			weather_Info.after_Weather.week,
			weather_Info.after_Weather.dayweather,
			weather_Info.after_Weather.nightweather,
			weather_Info.after_Weather.daytemp,
			weather_Info.after_Weather.nighttemp
	);
/*	os_printf("@0x04: C:%s TD: %s TW: %s TDW: %s TNW: %s TDT: %s TNT: %s"
						  " ND: %s NW: %s NDW: %s NNW: %s NDT: %s DNT: %s"
						  " AD: %s AW: %s ADW: %s ANW: %s ADT: %s ANT: %s\r\n",
			weather_Info.city,
			weather_Info.today_Weather.date,
			weather_Info.today_Weather.week,
			weather_Info.today_Weather.dayweather,
			weather_Info.today_Weather.nightweather,
			weather_Info.today_Weather.daytemp,
			weather_Info.today_Weather.nighttemp,
			weather_Info.next_Weather.date,
			weather_Info.next_Weather.week,
			weather_Info.next_Weather.dayweather,
			weather_Info.next_Weather.nightweather,
			weather_Info.next_Weather.daytemp,
			weather_Info.next_Weather.nighttemp,
			weather_Info.after_Weather.date,
			weather_Info.after_Weather.week,
			weather_Info.after_Weather.dayweather,
			weather_Info.after_Weather.nightweather,
			weather_Info.after_Weather.daytemp,
			weather_Info.after_Weather.nighttemp

	);*/
	delay_ms(1000);
}
//============================================================================
// STA TCP & AP UDP�ɹ������������ݵĻص�����
//============================================================================
void ICACHE_FLASH_ATTR ESP8266_WIFI_Send_Cb(void *arg)
{
	/*os_printf("\nESP8266_WIFI_Send_OK\n");*/	// ��Է�����Ӧ��   ��������
}
//============================================================================
// STA TCP���ջص�����  ��ȡ������Ϣ
//============================================================================
void ICACHE_FLASH_ATTR ESP8266_Weather_Recv_Cb(void * arg, char * pdata, unsigned short len)
{

	cJSON *json;
	char* pc =waether_buf;

	if(pdata == NULL)
	{
		os_printf("erro\r\n");
		return;
	}
	//os_strcpy(temp,pdata);

	if(times_rec==0){
	pdata = (char *)os_strstr((char *)pdata, "{");
	if(pdata == NULL)
		return;
	os_memcpy(pc,pdata,800);
	times_rec++;
	}
	else{
	os_strcat(pc,pdata);
	times_rec =0;
	}
/*	os_printf("%d\r\n",os_strlen(pc));
	os_printf("%s\r\n",pc);*/
	pc = (char *)os_strstr((char *)pc, "city");
	if(pc == NULL)
		return;
	char *headOfCity = (char *)os_strstr((char *)pc, ":\"");
	if (headOfCity == NULL)
		INFO("ERRO1\r\n");

	char *tailOfCity = NULL;
	tailOfCity =(char *)os_strstr((char *)headOfCity, "\",");
	if(tailOfCity == NULL)
		INFO("erro2\r\n");

	os_memset(weather_Info.city,0,20);
	//os_bzero(weather_Info.city,sizeof(weather_Info.city));
	os_memcpy(weather_Info.city,(headOfCity+2),(tailOfCity)-(headOfCity+2));

	if(pc == NULL)
		return;
	int i = 0;
	for(i = 0; i < 3; i++)
	{
		pc = (char *)os_strstr((char *)pc, "{");
		if(pc == NULL)
			return;
		json = cJSON_Parse(pc);
		if(json == NULL)
			return;
		if(i == 0)
			write_weather(json,&weather_Info.today_Weather);
		else if(i == 1)
			write_weather(json,&weather_Info.next_Weather);
		else
			write_weather(json,&weather_Info.after_Weather);
		pc +=5;
	}

	cJSON_Delete(json);

}
//============================================================================
// STA TCP �������ӽ����ɹ��Ļص�����
//============================================================================
void ICACHE_FLASH_ATTR ESP8266_TCP_Connect_Cb(void *arg)
{
	espconn_regist_sentcb((struct espconn *)arg, ESP8266_WIFI_Send_Cb);			// ע���������ݷ��ͳɹ��Ļص�����
	espconn_regist_recvcb((struct espconn *)arg, ESP8266_Weather_Recv_Cb);			// ע���������ݽ��ճɹ��Ļص�����
	espconn_regist_disconcb((struct espconn *)arg,ESP8266_TCP_Disconnect_Cb);	// ע��ɹ��Ͽ�TCP���ӵĻص�����

	os_printf("\n--------------- ESP8266_TCP_Connect_OK ---------------\n");
	espconn_send((struct espconn *)arg, HTTP_GET_Weather1, os_strlen(HTTP_GET_Weather1));		// ����HTTP����

}
//============================================================================
// STA TCP�����쳣�Ͽ�ʱ�Ļص�����
//============================================================================
void ICACHE_FLASH_ATTR ESP8266_TCP_Break_Cb(void *arg,sint8 err)
{
	os_printf("\nESP8266_TCP_Break\n");
	espconn_connect(&STA_NetCon);	// ����TCP-server
}
//============================================================================
// ESP8266_STA_NetCon_Init �������ӳ�ʼ�� ����������
//============================================================================
void ICACHE_FLASH_ATTR ESP8266_STA_NetCon_Init()
{
	// �ṹ�帳ֵ
	//--------------------------------------------------------------------------
	STA_NetCon.type = ESPCONN_TCP ;				// ����ΪTCPЭ��
	STA_NetCon.proto.tcp = (esp_tcp *)os_zalloc(sizeof(esp_tcp));	// �����ڴ�
	// ESP8266��ΪTCP_Client����Ҫ����TCP_Server������֪��TCP_Server��IP��ַ
	STA_NetCon.proto.tcp->local_port = espconn_port() ;	// ���ض˿ڡ���ȡ���ö˿ڡ�
	STA_NetCon.proto.tcp->remote_port = 80;				// Ŀ��˿ڡ�HTTP�˿ں�80��

	// ��ȡ��������Ӧ��IP��ַ
	//������1���������ӽṹ��ָ�� / ����2�������ַ���ָ�� / ����3��IP��ַ�ṹ��ָ�� / ����4���ص�������
	//-------------------------------------------------------------------------------------------------
	espconn_gethostbyname(&STA_NetCon, DNS_Server, &IP_Server, DNS_Over_Cb);

	// ע�����ӳɹ��ص��������쳣�Ͽ��ص�����
	//--------------------------------------------------------------------------------------------------
	espconn_regist_connectcb(&STA_NetCon, ESP8266_TCP_Connect_Cb);	// ע��TCP���ӳɹ������Ļص�����
	espconn_regist_reconcb(&STA_NetCon, ESP8266_TCP_Break_Cb);		// ע��TCP�����쳣�Ͽ��Ļص�����

	// ���� TCP server
	//----------------------------------------------------------
	//espconn_connect(&STA_NetCon);	// ����TCP-server
}
//============================================================================
//UDP AP ���յĻص�����;arg��espconnָ��;pdata������ָ��;len�����ݳ���
//============================================================================
void ICACHE_FLASH_ATTR ESP8266_WIFI_Recv_Cb(void * arg, char * pdata, unsigned short len)
{
	struct espconn *T_arg = arg;		// �����������ӽṹ��ָ��

	remot_info *P_port_info = NULL;	// Զ��������Ϣ�ṹ��ָ��

/*
	// ������������LED����/��
	//-------------------------------------------------------------------------------
	if(pdata[0] == 'k' || pdata[0] == 'K')
	{LED_ON;}
	else if(pdata[0] == 'g' || pdata[0] == 'G')
	{LED_OFF;}
*/
	if(pdata[0]=='H')
	{
		if(os_strcmp(mqtt_Info.switch_state,",;switch0,1;")==0)
			{espconn_send(T_arg,"sw0:K",os_strlen("sw0:K"));}
		else
			{espconn_send(T_arg,"sw0:G",os_strlen("sw0:G"));}
	}

	os_printf("\nESP8266_Receive_Data = %s\n",pdata);		// ���ڴ�ӡ���յ�������


	// ��ȡԶ����Ϣ��UDPͨ���������ӵģ���Զ��������Ӧʱ���ȡ�Է���IP/�˿���Ϣ��
	//------------------------------------------------------------------------------------
	if(espconn_get_connection_info(T_arg, &P_port_info, 0)==ESPCONN_OK)	// ��ȡԶ����Ϣ
	{
		T_arg->proto.udp->remote_port  = P_port_info->remote_port;		// ��ȡ�Է��˿ں�
		T_arg->proto.udp->remote_ip[0] = P_port_info->remote_ip[0];		// ��ȡ�Է�IP��ַ
		T_arg->proto.udp->remote_ip[1] = P_port_info->remote_ip[1];
		T_arg->proto.udp->remote_ip[2] = P_port_info->remote_ip[2];
		T_arg->proto.udp->remote_ip[3] = P_port_info->remote_ip[3];
		//os_memcpy(T_arg->proto.udp->remote_ip,P_port_info->remote_ip,4);	// �ڴ濽��
	}
	os_printf("�Է�IP:%d.%d.%d.%d\r\n",
			    T_arg->proto.udp->remote_ip[0],
				T_arg->proto.udp->remote_ip[1],
				T_arg->proto.udp->remote_ip[2],
				T_arg->proto.udp->remote_ip[3]);


	if(pdata[0]=='H')
	{
		if(os_strcmp(mqtt_Info.switch_state,",;switch0,1;")==0)
			{espconn_send(T_arg,"sw0:K",os_strlen("sw0:K"));}
		else
			{espconn_send(T_arg,"sw0:G",os_strlen("sw0:G"));}
	}
}
//============================================================================
// UDP AP �������ӳ�ʼ��
//============================================================================
void ICACHE_FLASH_ATTR ESP8266_AP_NetCon_Init()
{
	// �ڣ��ṹ�帳ֵ
	//��������������������������������������������������������������������������
	AP_NetCon.type = ESPCONN_UDP;		// ͨ��Э�飺UDP

	// AP_NetCon.proto.udpֻ��һ��ָ�룬����������esp_udp�ͽṹ�����
	AP_NetCon.proto.udp = (esp_udp *)os_zalloc(sizeof(esp_udp));	// �����ڴ�
	// �˴���������Ŀ��IP/�˿�(ESP8266��ΪServer������ҪԤ��֪��Client��IP/�˿�)
	//--------------------------------------------------------------------------
	//AP_NetCon.proto.udp->local_port  = espconn_port();	// ��ȡ8266���ö˿�
	AP_NetCon.proto.udp->local_ip[0] = 192;		// ���ñ���IP��ַ
	AP_NetCon.proto.udp->local_ip[1] = 168;
	AP_NetCon.proto.udp->local_ip[2] = 66;
	AP_NetCon.proto.udp->local_ip[3] = 1;
	AP_NetCon.proto.udp->local_port  = 8266 ;		// ���ñ��ض˿�

	//AP_NetCon.proto.udp->remote_port = 8888;		// ����Ŀ��˿�
	//AP_NetCon.proto.udp->remote_ip[0] = 192;		// ����Ŀ��IP��ַ
	//AP_NetCon.proto.udp->remote_ip[1] = 168;
	//AP_NetCon.proto.udp->remote_ip[2] = 4;
	//AP_NetCon.proto.udp->remote_ip[3] = 2;
	// �ۣ�ע��/����ص�����
	//-------------------------------------------------------------------------------------------
	espconn_regist_sentcb(&AP_NetCon,ESP8266_WIFI_Send_Cb);	// ע���������ݷ��ͳɹ��Ļص�����
	espconn_regist_recvcb(&AP_NetCon,ESP8266_WIFI_Recv_Cb);	// ע���������ݽ��ճɹ��Ļص�����
	// �ܣ�����UDP��ʼ��API
	//----------------------------------------------
	espconn_create(&AP_NetCon);	// ��ʼ��UDPͨ��

}
//============================================================================
//OS_Timer_1�����ʱ����ʼ��(ms����) ���ķǳ���Ҫ ������΢��������Ĭ������wifi
//============================================================================
void ICACHE_FLASH_ATTR OS_Timer_1_Init(u32 time_ms, u8 time_repetitive)
{
	os_timer_disarm(&OS_Timer_1);	// �رն�ʱ��
	os_timer_setfn(&OS_Timer_1,(os_timer_func_t *)OS_Timer_1_Cb, NULL);	// ���ö�ʱ��
	os_timer_arm(&OS_Timer_1, time_ms, time_repetitive);  // ʹ�ܶ�ʱ��
}
//============================================================================
// SNTP��ʱ��������ȡ��ǰ����ʱ��
//============================================================================
void ICACHE_FLASH_ATTR OS_Timer_SNTP_Cb()
{
	 uint32	TimeStamp;		// ʱ���
	 char * Str_RealTime;	// ʵ��ʱ����ַ���
	 // ��ѯ��ǰ�����׼ʱ��(1970.01.01 00:00:00 GMT+8)��ʱ���(��λ:��)
	 //-----------------------------------------------------------------
	 TimeStamp = sntp_get_current_timestamp();

	 if(TimeStamp)		// �ж��Ƿ��ȡ��ƫ��ʱ��
	 {
		 //os_timer_disarm(&OS_Timer_SNTP);	// �ر�SNTP��ʱ��

		 // ��ѯʵ��ʱ��(GMT+8):������(����ʱ��)
		 //--------------------------------------------
		 Str_RealTime = sntp_get_real_time(TimeStamp);

		 // ��ʵ��ʱ�䡿�ַ��� == "�� �� �� ʱ:��:�� ��"
		 //------------------------------------------------------------------------
/*		 os_printf("\r\n----------------------------------------------------\r\n");
		 os_printf("SNTP_TimeStamp = %d\r\n", TimeStamp);		// ʱ���
		 os_printf("\r\nSNTP_InternetTime = %s", Str_RealTime);	// ʵ��ʱ��
		 os_printf("ʱ����%d\r\n", sntp_get_timezone());
		 os_printf("--------------------------------------------------------\r\n");*/
		os_printf("@0x03:%s\r\n",Str_RealTime);

	 }
}
//============================================================================
// OS_Timer_SNTP�����ʱ��ʼ��
//============================================================================
void ICACHE_FLASH_ATTR OS_Timer_SNTP_Init(uint32 time_ms, uint8 time_repetitive)
{
	os_timer_disarm(&OS_Timer_SNTP);
	os_timer_setfn(&OS_Timer_SNTP, (os_timer_func_t *)OS_Timer_SNTP_Cb, NULL);
	os_timer_arm(&OS_Timer_SNTP, time_ms, time_repetitive);
}
//============================================================================
// SNTP��ʼ��
//============================================================================
void ICACHE_FLASH_ATTR ESP8266_SNTP_Init()
{
	ip_addr_t * addr = (ip_addr_t *)os_zalloc(sizeof(ip_addr_t));
	sntp_setservername(0, "us.pool.ntp.org");	// ������_0��������
	sntp_setservername(1, "ntp.sjtu.edu.cn");	// ������_1��������
	ipaddr_aton("210.72.145.44", addr);			// ���ʮ���� => 32λ������
	sntp_setserver(2, addr);					// ������_2��IP��ַ��
	os_free(addr);								// �ͷ�addr
	sntp_init();	// SNTP��ʼ��API
	OS_Timer_SNTP_Init(1000,1);				// 1���ظ���ʱ(SNTP)
	ESP8266_Func_Cmd();
}
//============================================================================
// ESP8266 ģʽ���õĳ�ʼ��
//============================================================================
void ICACHE_FLASH_ATTR ESP8266_AP_STA_Init()
{
	os_memset(&STA_Config, 0, sizeof(struct station_config));	// STA�����ṹ�� = 0
	spi_flash_read(Sector_STA_INFO*4096,(uint32 *)&STA_Config, 96);	// ������STA������(SSID/PASS)
	STA_Config.ssid[31] = 0;		// SSID�����'\0'
	STA_Config.password[63] = 0;	// APSS�����'\0'
	wifi_set_opmode(0x01);
	if(wifi_station_set_config(&STA_Config) == true)	// ����STA�����������浽Flash
	{
		OS_Timer_1_Init(1000, 1);
	}
	os_memset(&AP_Config, 0, sizeof(struct softap_config));
	os_strcpy(AP_Config.ssid, ESP8266_AP_SSID);
	os_strcpy(AP_Config.password, ESP8266_AP_PASSWORD);
	AP_Config.ssid_len = os_strlen(ESP8266_AP_SSID);
	AP_Config.channel = 8;
	AP_Config.authmode = AUTH_WPA_WPA2_PSK;
	AP_Config.ssid_hidden = 0;
	AP_Config.max_connection = 4;
	AP_Config.beacon_interval = 100;
}
//============================================================================
// MQTT�ѳɹ����ӣ�ESP8266���͡�CONNECT���������յ���CONNACK��
//============================================================================
void mqttConnectedCb(uint32_t *args)
{
    MQTT_Client* client = (MQTT_Client*)args;	// ��ȡmqttClientָ��

    INFO("MQTT: Connected\r\n");

    // ������2����������� / ����3������Qos��
    //-----------------------------------------------------------------
	//MQTT_Subscribe(client, "$creq", 0);	// ��������"SW_LED"��QoS=0
	//MQTT_Subscribe(client, "$dp", 0);
//	MQTT_Subscribe(client, "SW_LED", 1);
//	MQTT_Subscribe(client, "SW_LED", 2);

	// ������2�������� / ����3��������Ϣ����Ч�غ� / ����4����Ч�غɳ��� / ����5������Qos / ����6��Retain��
	//-----------------------------------------------------------------------------------------------------------------------------------------

//	MQTT_Publish(client, "SW_LED", "ESP8266_Online", strlen("ESP8266_Online"), 2, 0);
}
//============================================================================
// MQTT�ɹ��Ͽ�����
//============================================================================
void mqttDisconnectedCb(uint32_t *args)
{
    MQTT_Client* client = (MQTT_Client*)args;
    INFO("MQTT: Disconnected\r\n");
}
//============================================================================
// MQTT�ɹ�������Ϣ
//============================================================================
void mqttPublishedCb(uint32_t *args)
{
    MQTT_Client* client = (MQTT_Client*)args;
    INFO("MQTT: Published\r\n");

}
//============================================================================
// ������MQTT��[PUBLISH]���ݡ�����		������1������ / ����2�����ⳤ�� / ����3����Ч�غ� / ����4����Ч�غɳ��ȡ�
//===============================================================================================================
void mqttDataCb(uint32_t *args, const char* topic, uint32_t topic_len, const char *data, uint32_t data_len)
{
    char *topicBuf = (char*)os_zalloc(topic_len+1);		// ���롾���⡿�ռ�
    char *dataBuf  = (char*)os_zalloc(data_len+1);		// ���롾��Ч�غɡ��ռ�
    char *str;
    MQTT_Client* client = (MQTT_Client*)args;	// ��ȡMQTT_Clientָ��

    os_memcpy(topicBuf, topic, topic_len);	// ��������
    topicBuf[topic_len] = 0;				// �����'\0'
    os_memcpy(dataBuf, data, data_len);		// ������Ч�غ�
    dataBuf[data_len] = 0;					// �����'\0'
    INFO("Receive topic: %s, data: %s \r\n", topicBuf, dataBuf);	// ���ڴ�ӡ�����⡿����Ч�غɡ�
    INFO("Topic_len = %d, Data_len = %d\r\n", topic_len, data_len);	// ���ڴ�ӡ�����ⳤ�ȡ�����Ч�غɳ��ȡ�

// ����С�¡����
//########################################################################################
    // ���ݽ��յ���������/��Ч�غɣ�����LED����/��
    //-----------------------------------------------------------------------------------
    if( os_strncmp(topicBuf,"$creq",5) == 0 )			// ���� == "SW_LED"
    {
    	if(os_strncmp(dataBuf,"sw",2) == 0 )
    	{

			if( os_strcmp(dataBuf,"sw1") == 0 )		// ��Ч�غ� == "LED_ON"
			{
				LED_ON;		// LED��
				os_sprintf(mqtt_Info.switch_state, SWITCH0,1);
				string_Packet_Publish(&mqttClient, mqtt_Info.switch_state);
	        	espconn_send(&AP_NetCon,"sw0:K",os_strlen("sw0:K"));
	        	os_printf("@0X01: SW0: %s TH: %s TM: %s\r\n",mqtt_Info.switch_state, mqtt_Info.hour,mqtt_Info.min);
			}

			else if( os_strcmp(dataBuf,"sw0") == 0 )	// ��Ч�غ� == "LED_OFF"
			{
				LED_OFF;			// LED��
				os_sprintf(mqtt_Info.switch_state, SWITCH0,0);
				string_Packet_Publish(&mqttClient, mqtt_Info.switch_state);
	        	espconn_send(&AP_NetCon,"sw0:G",os_strlen("sw0:G"));
	        	os_printf("@0X01: SW0: %s TH: %s TM: %s\r\n",mqtt_Info.switch_state, mqtt_Info.hour,mqtt_Info.min);
			}
    	}
    	else if(os_strncmp(dataBuf,"th",2) == 0)
		{

			os_sprintf(mqtt_Info.hour,SETHOUR,dataBuf+2);
			string_Packet_Publish(&mqttClient, mqtt_Info.hour);
			os_printf("@0X01: SW0: %s TH: %s TM: %s\r\n",mqtt_Info.switch_state, mqtt_Info.hour,mqtt_Info.min);
		}
    	else if(os_strncmp(dataBuf,"tm",2)==0)
		{

			os_sprintf(mqtt_Info.min,SETMIN,dataBuf+2);
			string_Packet_Publish(&mqttClient, mqtt_Info.min);
			os_printf("@0X01: SW0: %s TH: %s TM: %s\r\n",mqtt_Info.switch_state, mqtt_Info.hour,mqtt_Info.min);
		}


    }
//########################################################################################
    os_free(str);
    os_free(topicBuf);	// �ͷš����⡿�ռ�
    os_free(dataBuf);	// �ͷš���Ч�غɡ��ռ�
}
//===============================================================================================================


// user_init��entry of user application, init user function here
//===================================================================================================================
void user_init(void)
{
	uart_init(74880,115200);// ���ڲ�������Ϊ115200
	delay_ms(10);
	os_printf("\r\n=======================================\r\n");
	ESP8266_AP_STA_Init();
	//����GPIO����
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U,FUNC_GPIO0);
	PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO0_U);
	GPIO_DIS_OUTPUT(GPIO_ID_PIN(0));
	//��GPIO����
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
	GPIO_OUTPUT_SET(GPIO_ID_PIN(2),1);
	//���ڴ��wifi �˺�����
	os_memset(&STA_Config,0,sizeof(struct station_config));			// STA_INFO = 0
	spi_flash_read(Sector_STA_INFO*4096,(uint32 *)&STA_Config, 96);	// ������STA������(SSID/PASS)
	spi_flash_read(0x98*4096,(uint32 *)&mqtt_Info,sizeof(MQTT_Info));
	STA_Config.ssid[31] = 0;		// SSID�����'\0'
	STA_Config.password[63] = 0;	// APSS�����'\0'

    CFG_Load();	// ����/����ϵͳ������WIFI������MQTT������

    // �������Ӳ�����ֵ�������������mqtt_test_jx.mqtt.iot.gz.baidubce.com�����������Ӷ˿ڡ�1883������ȫ���͡�0��NO_TLS��
	//-------------------------------------------------------------------------------------------------------------------
	MQTT_InitConnection(&mqttClient, sysCfg.mqtt_host, sysCfg.mqtt_port, sysCfg.security);

	// MQTT���Ӳ�����ֵ���ͻ��˱�ʶ����..����MQTT�û�����..����MQTT��Կ��..������������ʱ����120s��������Ự��1��clean_session��
	//----------------------------------------------------------------------------------------------------------------------------
	MQTT_InitClient(&mqttClient, sysCfg.device_id, sysCfg.mqtt_user, sysCfg.mqtt_pass, sysCfg.mqtt_keepalive, 1);

	// ����MQTT��غ���
	//--------------------------------------------------------------------------------------------------
	MQTT_OnConnected(&mqttClient, mqttConnectedCb);			// ���á�MQTT�ɹ����ӡ���������һ�ֵ��÷�ʽ
	MQTT_OnDisconnected(&mqttClient, mqttDisconnectedCb);	// ���á�MQTT�ɹ��Ͽ�����������һ�ֵ��÷�ʽ
	MQTT_OnPublished(&mqttClient, mqttPublishedCb);			// ���á�MQTT�ɹ���������������һ�ֵ��÷�ʽ
	MQTT_OnData(&mqttClient, mqttDataCb);					// ���á�����MQTT���ݡ���������һ�ֵ��÷�ʽ


	INFO("\r\nSystem started ...\r\n");
}
//===================================================================================================================
/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
 *******************************************************************************/
uint32 ICACHE_FLASH_ATTR
user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        case FLASH_SIZE_64M_MAP_1024_1024:
            rf_cal_sec = 2048 - 5;
            break;
        case FLASH_SIZE_128M_MAP_1024_1024:
            rf_cal_sec = 4096 - 5;
            break;
        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}
