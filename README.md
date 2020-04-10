# Modbus-STM32
modbus protocol app for STM32
# 以modbus-TCP支持为例

## 环境
- STM32
    - ETH
    - LWIP 
    - FREERTOS
    
## FreeModbus
### 特点
- 面向嵌入式

## libmodbus
### 特点
- 跨平台，应用范围广（Linux,windows,mac 等系统都可以使用），相对容易上手
- 源码结构清晰方便移植
- 需要操作系统库函数较多，需要FreeRTOS支持
官网下载源码，复制到libmodbus文件夹中

### 1. 精简源文件，移除不需要的RTU源码
CUBE中移除RTU相关的源文件：
- modbus-rtu.c
- modbus-rtu.h
- modbus-rtu-private.h

### 2. 添加config.h 和modbus-version.h
按照源码的教程点击config.js生成config.h和modbus-version.h文件
- [x] config.h
- [x] modbus-version.h

### 3. 添加libmodbus文件到源码编译目录和include目录
STM32CubeIDE中添加libmodbus文件夹，include path添加 libmodbus文件夹。


### 4. struct addrinfo 结构体编译失败
struct addrinfo 需要LWIP的 DNS模块开启。具体原因可以看netdb.h源码中是用来了`LWIP_DNS_API_DECLARE_STRUCTS`

```C

#if LWIP_DNS_API_DECLARE_STRUCTS
struct hostent {
    char  *h_name;      /* Official name of the host. */
    char **h_aliases;   /* A pointer to an array of pointers to alternative host names,
                           terminated by a null pointer. */
    int    h_addrtype;  /* Address type. */
    int    h_length;    /* The length, in bytes, of the address. */
    char **h_addr_list; /* A pointer to an array of pointers to network addresses (in
                           network byte order) for the host, terminated by a null pointer. */
#define h_addr h_addr_list[0] /* for backward compatibility */
};

struct addrinfo {
    int               ai_flags;      /* Input flags. */
    int               ai_family;     /* Address family of socket. */
    int               ai_socktype;   /* Socket type. */
    int               ai_protocol;   /* Protocol of socket. */
    socklen_t         ai_addrlen;    /* Length of socket address. */
    struct sockaddr  *ai_addr;       /* Socket address of socket. */
    char             *ai_canonname;  /* Canonical name of service location. */
    struct addrinfo  *ai_next;       /* Pointer to next in list. */
};
#endif /* LWIP_DNS_API_DECLARE_STRUCTS */
```

### 5. 使用static静态声明modbus_t 对象。
嵌入式（RTOS）中尽量少用malloc方法动态申请变量的原则，将modbus_new_tcp改写成modbus_new_tcp_static方法。

```c

static modbus_t _modbus_dev_fd;
static modbus_tcp_t _modbus_tcp_backend_data;

modbus_t* modbus_new_tcp_static(const char *ip, int port)
{
	modbus_t *ctx;
	modbus_tcp_t *ctx_tcp;
	size_t dest_size;
	size_t ret_size;
	ctx = &_modbus_dev_fd;
	_modbus_init_common(ctx);

	/* Could be changed after to reach a remote serial Modbus device */
	ctx->slave = MODBUS_TCP_SLAVE;

	ctx->backend = &_modbus_tcp_backend;

	ctx->backend_data = &_modbus_tcp_backend_data;

	ctx_tcp = (modbus_tcp_t*) ctx->backend_data;

	if (ip != NULL)
	{
		dest_size = sizeof(char) * 16;
		ret_size = strlcpy(ctx_tcp->ip, ip, dest_size);
		if (ret_size == 0)
		{
			fprintf(stderr, "The IP string is empty\n");
			modbus_free(ctx);
			errno = EINVAL;
			return NULL;
		}

		if (ret_size >= dest_size)
		{
			fprintf(stderr, "The IP string has been truncated\n");
			modbus_free(ctx);
			errno = EINVAL;
			return NULL;
		}
	}
	else
	{
		ctx_tcp->ip[0] = '0';
	}
	ctx_tcp->port = port;
	ctx_tcp->t_id = 0;

	return ctx;
}
```
相应的modbus_free函数也不用了。

### 6. 测试

```c
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for LWIP */
  MX_LWIP_Init();
  /* USER CODE BEGIN 5 */
//	udp_echoclient_connect();
	osDelay(2000);
	modbus_t *mb = modbus_new_tcp_static("192.168.1.100", 502);
	modbus_set_slave(mb, 1);
	int ret = modbus_connect(mb);
	uint16_t src = 0;
	uint16_t dest = 0;
  /* Infinite loop */
  for(;;)
  {
//		udp_echoclient_send();
	  
		osDelay(1000);
		ret = modbus_write_and_read_registers(mb, 1, 1, &src, 1, 1, &dest);
		if (ret == 1)
		{
			src == dest;
		}
		else if (ret < 0)
		{//断线重连
			modbus_close(mb);
			modbus_connect(mb);
		}
		src++;
  }
  /* USER CODE END 5 */ 
}
```