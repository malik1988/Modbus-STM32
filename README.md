# Modbus-STM32
modbus protocol app for STM32
# ��modbus-TCP֧��Ϊ��

## ����
- STM32
    - ETH
    - LWIP 
    - FREERTOS
    
## FreeModbus
### �ص�
- ����Ƕ��ʽ

## libmodbus
### �ص�
- ��ƽ̨��Ӧ�÷�Χ�㣨Linux,windows,mac ��ϵͳ������ʹ�ã��������������
- Դ��ṹ����������ֲ
- ��Ҫ����ϵͳ�⺯���϶࣬��ҪFreeRTOS֧��
��������Դ�룬���Ƶ�libmodbus�ļ�����

### 1. ����Դ�ļ����Ƴ�����Ҫ��RTUԴ��
CUBE���Ƴ�RTU��ص�Դ�ļ���
- modbus-rtu.c
- modbus-rtu.h
- modbus-rtu-private.h

### 2. ���config.h ��modbus-version.h
����Դ��Ľ̵̳��config.js����config.h��modbus-version.h�ļ�
- [x] config.h
- [x] modbus-version.h

### 3. ���libmodbus�ļ���Դ�����Ŀ¼��includeĿ¼
STM32CubeIDE�����libmodbus�ļ��У�include path��� libmodbus�ļ��С�


### 4. struct addrinfo �ṹ�����ʧ��
struct addrinfo ��ҪLWIP�� DNSģ�鿪��������ԭ����Կ�netdb.hԴ������������`LWIP_DNS_API_DECLARE_STRUCTS`

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

### 5. ʹ��static��̬����modbus_t ����
Ƕ��ʽ��RTOS���о�������malloc������̬���������ԭ�򣬽�modbus_new_tcp��д��modbus_new_tcp_static������

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
��Ӧ��modbus_free����Ҳ�����ˡ�

### 6. ����

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
		{//��������
			modbus_close(mb);
			modbus_connect(mb);
		}
		src++;
  }
  /* USER CODE END 5 */ 
}
```