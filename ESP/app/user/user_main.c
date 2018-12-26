
#include "driver/uart.h"  //串口0需要的头文件
#include "osapi.h"  //串口1需要的头文件
#include "user_interface.h" //WIFI连接需要的头文件
#include "espconn.h"//TCP连接需要的头文件
#include "mem.h" //系统操作需要的头文件
#include "gpio.h"  //端口控制需要的头文件
#include "my_json.h"

//#include "at_custom.h"
//#include "c_types.h"
//#include "eagle_soc.h"
//#include "espnow.h"
//#include "ets_sys.h"
//#include "ip_addr.h"
//#include "os_type.h"
//#include "ping.h"
//#include "pwm.h"
//#include "queue.h"
//#include "smartconfig.h"
#include "sntp.h"
//#include "spi_flash.h"
//#include "upgrade.h"
//#include "user_config.h"
#include "user_json.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "enddevice.h"

#define GPIO_EN_OUTPUT(gpio_no) 	gpio_output_set(0,0, 1<<gpio_no,0)

#define  N   5

os_timer_t checkTimer_wifistate;

LOCAL os_timer_t sntp_timer;

struct espconn user_tcp_conn;

int   begincnt=0;
int   cnt=0;

void ICACHE_FLASH_ATTR rgbGpioInit(void)
{
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U,FUNC_GPIO15);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U,FUNC_GPIO12);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U,FUNC_GPIO13);
	GPIO_OUTPUT_SET(15, 0);
	GPIO_OUTPUT_SET(12, 0);
	GPIO_OUTPUT_SET(13, 0);
}
void ICACHE_FLASH_ATTR user_tcp_sent_cb(void *arg)  //发送
{
	os_printf("发送数据成功！");
}
void ICACHE_FLASH_ATTR user_tcp_discon_cb(void *arg)  //断开
{
	os_printf("断开连接成功！");
}
void ICACHE_FLASH_ATTR user_tcp_recv_cb(void *arg,  //接收
		char *pdata,
		unsigned short len)
{

	os_printf("收到数据：%s\r\n",pdata);

	if(pdata[0]=='1')
	{
		GPIO_OUTPUT_SET(5, 1);
		os_printf("继电器开\n");
	}
	else if(pdata[0]=='2')
	{
		GPIO_OUTPUT_SET(5, 0);
        os_printf("继电器关\n");
	}
	else if(pdata[0]=='3')
	{
		GPIO_OUTPUT_SET(15, 1);
	}
	else if(pdata[0]=='4')
	{
		GPIO_OUTPUT_SET(15, 0);
	}
	else if(pdata[0]=='5')
	{
		GPIO_OUTPUT_SET(12, 1);
	}
	else if(pdata[0]=='6')
	{
		GPIO_OUTPUT_SET(12, 0);
	}
	else if(pdata[0]=='7')
	{
		GPIO_OUTPUT_SET(13, 1);
	}
	else if(pdata[0]=='8')
	{
		GPIO_OUTPUT_SET(13, 0);
	}
}
void ICACHE_FLASH_ATTR user_tcp_recon_cb(void *arg, sint8 err)//注册 TCP 连接发生异常断开时的回调函数，可以在回调函数中进行重连
{
	os_printf("连接错误，错误代码为%d\r\n",err);
	espconn_connect((struct espconn *)arg);
}
void ICACHE_FLASH_ATTR user_tcp_connect_cb(void *arg)//注册 TCP 连接成功建立后的回调函数
{
	struct espconn *pespconn=arg;
	espconn_regist_recvcb(pespconn,user_tcp_recv_cb);//接收
	espconn_regist_sentcb(pespconn,user_tcp_sent_cb);//发送
	espconn_regist_disconcb(pespconn,user_tcp_discon_cb);//断开

}

void ICACHE_FLASH_ATTR my_station_init(struct ip_addr *remote_ip,struct ip_addr *local_ip,int remote_port)
{
	user_tcp_conn.proto.tcp=(esp_tcp *)os_zalloc(sizeof(esp_tcp));//分配空间
	user_tcp_conn.type=ESPCONN_TCP;//设置类型为TCP协议
	os_memcpy(user_tcp_conn.proto.tcp->local_ip,local_ip,4);
	os_memcpy(user_tcp_conn.proto.tcp->remote_ip,remote_ip,4);
	user_tcp_conn.proto.tcp->local_port=espconn_port();//本地端口
	user_tcp_conn.proto.tcp->remote_port=remote_port;//目标端口
	//注册连接成功回调函数和重新连接回调函数
	espconn_regist_connectcb(&user_tcp_conn,user_tcp_connect_cb);//注册 TCP 连接成功建立后的回调函数
	espconn_regist_reconcb(&user_tcp_conn,user_tcp_recon_cb);//注册 TCP 连接发生异常断开时的回调函数，可以在回调函数中进行重连
	//启用连接
	espconn_connect(&user_tcp_conn);
}


void GetDataFunc(void)
{
	uint8 *RequestData = "0";
	espconn_sent(&user_tcp_conn,RequestData,strlen(RequestData));
	   //  os_printf("%s\n",RequestData);
}

void Check_WifiState(void)
{
	 uint8 getState;

		//struct espconn *pespconn;


	 getState = wifi_station_get_connect_status();

	 //查询 ESP8266 WiFi station 接口连接 AP 的状态
	 		  //返回:
	 		  //enum{
	 		  //      STATION_IDLE = 0,    //未发现已保存的wifi
	 		  //      STATION_CONNECTING,  //正在连接路由
	 		  //      STATION_WRONG_PASSWORD, //密码错误
	 		  //      STATION_NO_AP_FOUND, //未发现已保存的wifi
	 		  //      STATION_CONNECT_FAIL, //加入路由失败
	 		  //      STATION_GOT_IP
	 		  //};
	 if(getState == STATION_GOT_IP)//如果状态正确，证明已经连接
	  {
		 os_printf("WIFI连接成功！");
		 os_timer_disarm(&checkTimer_wifistate);

        //183.230.40.33:80
		 struct ip_info info;
		 const char remote_ip[4]={106,12,132,115};//目标IP地址
		 wifi_get_ip_info(STATION_IF,&info);//查询 WiFi模块的 IP 地址
		 my_station_init((struct ip_addr *)remote_ip,&info.ip,10087);//连接到目标服务器的80端口

		  os_timer_setfn(&checkTimer_wifistate,(os_timer_func_t *)GetDataFunc,NULL);//设置定时器回调函数
		  os_timer_arm(&checkTimer_wifistate,2000,1);//启动定时器，单位：毫秒

	  }
}

void user_init() //初始化
{
	uint8 opmode;

	uart_init(115200,115200);
	//uart_init(57600, 57600);//设置串口0和串口1的波特率
	os_printf("enter here  2!\n");

	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U,FUNC_GPIO5);
	GPIO_OUTPUT_SET(5, 0);

	wifi_set_opmode(0x01);//设置为STATION模式

	struct station_config stationConf;
	os_strcpy(stationConf.ssid,"Sakila");
	os_strcpy(stationConf.password,"12345679");

	wifi_station_set_config(&stationConf);//设置WiFi station接口配置，并保存到 flash
	wifi_station_disconnect();//断开路由器
	wifi_station_connect();//连接路由器
	os_timer_disarm(&checkTimer_wifistate);//取消定时器定时
	rgbGpioInit();
	os_timer_setfn(&checkTimer_wifistate,(os_timer_func_t *)Check_WifiState,NULL);//设置定时器回调函数
	os_timer_arm(&checkTimer_wifistate,500,1);//启动定时器，单位：毫秒
}

void user_rf_pre_init()
{

}

