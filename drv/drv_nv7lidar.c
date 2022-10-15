/**
 * @file drv_nv7lidar.c
 * @author shenzan (shenzan@foxmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-10-15
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <rtthread.h>
#include <rtdevice.h>
#define DBG_TAG "drv_nv7lidar"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
#include <rt_nv7lidar.h>

#ifndef NV7LIDAR_UART_NAME
#define NV7LIDAR_UART_NAME     "uart3"
#endif

/* 用于接收消息的信号量 */
static struct rt_semaphore rx_sem;

/* 接收数据回调函数 */
static rt_err_t nv7_lidar_input(rt_device_t dev, rt_size_t size)
{
    /* 串口接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
    if(size>0)
    {
        rt_sem_release(&rx_sem);
    }

    return RT_EOK;
}

static rt_err_t nv7_lidar_get_char(struct rt_nv7lidar_device *nv7lidar, char* ch, rt_uint32_t timeout)
{
    rt_device_t serial = nv7lidar->parent.user_data;
    while (rt_device_read(serial, 0, ch, 1) == 0)
    {
        rt_sem_control(&rx_sem, RT_IPC_CMD_RESET, RT_NULL);
        // rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
        if(rt_sem_take(&rx_sem, rt_tick_from_millisecond(timeout)) != RT_EOK)
        {
            return -RT_ETIMEOUT;
        }
        else
        {
            continue;
        }
    }
    return RT_EOK;
}

struct nv7lidar_device
{
    struct rt_nv7lidar_device nv7lidar;
    char *name;
};

static struct nv7lidar_device nv7lidar_obj =
{
    .name = "nv7lidar"
};

static rt_err_t nv7lidar_init(struct rt_nv7lidar_device *nv7lidar)
{
    /* 初始化信号量 */
    rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);

    return RT_EOK;
}

static rt_err_t nv7lidar_open(struct rt_nv7lidar_device *nv7lidar)
{
    rt_err_t ret;
    rt_device_t serial = nv7lidar->parent.user_data;
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

    config.baud_rate = BAUD_RATE_9600;
    config.data_bits = DATA_BITS_8;
    config.stop_bits = STOP_BITS_1;
    config.bufsz = 512;
    config.parity = PARITY_NONE;

    rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);

    /* 以中断接收及轮询发送模式打开串口设备 */
    ret = rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);

    if(ret == RT_EOK)
    {
        /* 设置接收回调函数 */
        rt_device_set_rx_indicate(serial, nv7_lidar_input);
    }

    return ret;
}

static rt_err_t nv7lidar_close(struct rt_nv7lidar_device *nv7lidar)
{
    rt_device_t serial = nv7lidar->parent.user_data;
    rt_device_close(serial);

    return RT_EOK;
}

static rt_size_t nv7lidar_read(struct rt_nv7lidar_device *nv7lidar, rt_off_t pos, void *buffer, rt_size_t size)
{
    rt_size_t read_count = 0;
    char* buffer_ch = (char*) buffer;
    for(rt_size_t i = 0; i < size; i++)
    {
        char ch = 0;
        if(nv7_lidar_get_char(nv7lidar, &ch, 1000) == RT_EOK)
        {
            buffer_ch[pos + read_count] = ch;
            // LOG_I("Read %02X", buffer_ch[pos + read_count]);
            read_count++;
        }
        else
        {
            LOG_E("Read error");
        }
    }
    return read_count;
}

static rt_size_t nv7lidar_write(struct rt_nv7lidar_device *nv7lidar, rt_off_t pos, const void *buffer, rt_size_t size)
{
    rt_device_t serial = (rt_device_t)(nv7lidar->parent.user_data);
    rt_size_t ret = rt_device_write(serial, 0, buffer, size);

    return ret;
}

static rt_err_t nv7lidar_control(struct rt_nv7lidar_device *nv7lidar, rt_uint32_t cmd, void *args)
{
    rt_err_t result = RT_EOK;
    /*
    switch (cmd)
    {
        case rplidar_CMD_ENABLE:
            break;
        case rplidar_CMD_DISABLE:
            break;
        default:
            result = -RT_ENOSYS;
            break;
    }
    */
    return result;
}

static const struct rt_nv7lidar_ops _ops =
{
    .init    = nv7lidar_init,
    .open    = nv7lidar_open,
    .close   = nv7lidar_close,
    .read    = nv7lidar_read,
    .write   = nv7lidar_write,
    .control = nv7lidar_control,
};

static int hw_nv7lidar_init(void)
{
    int result = RT_EOK;
    rt_device_t serial;

    nv7lidar_obj.nv7lidar.type = UNKNOWN_RPLIDAR_TYPE;
    nv7lidar_obj.nv7lidar.ops  = &_ops;

    serial = rt_device_find(NV7LIDAR_UART_NAME);
    if (!serial)
    {
        LOG_E("find %s failed!\n", NV7LIDAR_UART_NAME);
        return -RT_ERROR;
    }


    if (rt_device_nv7lidar_register(&nv7lidar_obj.nv7lidar, nv7lidar_obj.name, serial) != RT_EOK)
    {
        LOG_E("%s register failed", nv7lidar_obj.name);
        result = -RT_ERROR;
    }

    return result;
}
INIT_BOARD_EXPORT(hw_nv7lidar_init);
