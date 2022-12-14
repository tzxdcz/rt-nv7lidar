/**
 * @file rt_nv7lidar.c
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
#define DBG_TAG "rt_nv7lidar"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
#include <rt_nv7lidar.h>

static rt_err_t rt_lidar_init(struct rt_device *dev)
{
    struct rt_nv7lidar_device *lidar = (struct rt_nv7lidar_device *)dev;

    if (lidar->ops->init)
    {
        return lidar->ops->init(lidar);
    }
    else
    {
        return -RT_ENOSYS;
    }
}

static rt_err_t rt_lidar_open(struct rt_device *dev, rt_uint16_t oflag)
{
    struct rt_nv7lidar_device *lidar = (struct rt_nv7lidar_device *)dev;

    if (lidar->ops->open)
    {
        return lidar->ops->open(lidar);
    }
    else
    {
        return -RT_ENOSYS;
    }
}

static rt_err_t rt_lidar_close(struct rt_device *dev)
{
    struct rt_nv7lidar_device *lidar = (struct rt_nv7lidar_device *)dev;

    if (lidar->ops->close)
    {
        return lidar->ops->close(lidar);
    }
    else
    {
        return -RT_ENOSYS;
    }
}

static rt_size_t rt_lidar_read(struct rt_device *dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    struct rt_nv7lidar_device *lidar = (struct rt_nv7lidar_device *)dev;

    if (lidar->ops->read)
    {
        return lidar->ops->read(lidar, pos, buffer, size);
    }
    return 0;
}

static rt_size_t rt_lidar_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    struct rt_nv7lidar_device *lidar = (struct rt_nv7lidar_device *)dev;

    if (lidar->ops->write)
    {
        return lidar->ops->write(lidar, pos, buffer, size);
    }
    return 0;
}

static rt_err_t rt_lidar_control(struct rt_device *dev, int cmd, void *args)
{
    struct rt_nv7lidar_device *lidar = (struct rt_nv7lidar_device *)dev;

    rt_err_t result = RT_EOK;
    result = lidar->ops->control(lidar, cmd, args);

    return result;
}

#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops lidar_ops =
{
    rt_lidar_init,
    rt_lidar_open,
    rt_lidar_close,
    rt_lidar_read,
    rt_lidar_write,
    rt_lidar_control
};
#endif

rt_err_t rt_device_nv7lidar_register(struct rt_nv7lidar_device *nv7lidar, const char *name, void *user_data)
{
    struct rt_device *device;

    RT_ASSERT(nv7lidar != RT_NULL);
    RT_ASSERT(nv7lidar->ops != RT_NULL);

    device = &(nv7lidar->parent);

    device->type        = RT_Device_Class_Miscellaneous;
    device->rx_indicate = RT_NULL;
    device->tx_complete = RT_NULL;

#ifdef RT_USING_DEVICE_OPS
    device->ops         = &lidar_ops;
#else
    device->init        = rt_lidar_init;
    device->open        = rt_lidar_open;
    device->close       = rt_lidar_close;
    device->read        = rt_lidar_read;
    device->write       = rt_lidar_write;
    device->control     = rt_lidar_control;
#endif
    device->user_data   = user_data;

    return rt_device_register(device, name, RT_DEVICE_FLAG_RDWR);
}
