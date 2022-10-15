/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-10-15     沈瓒       the first version
 */
#ifndef APPLICATIONS_RT_NV7LIDAR_H_
#define APPLICATIONS_RT_NV7LIDAR_H_

/* rplidar type */
enum rt_nv7lidar_type
{
    UNKNOWN_RPLIDAR_TYPE = 0x00    /* Unknown rplidar type */

};

struct rt_nv7lidar_device;

struct rt_nv7lidar_ops
{
    rt_err_t (*init)(struct rt_nv7lidar_device *nv7lidar);
    rt_err_t (*open)(struct rt_nv7lidar_device *nv7lidar);
    rt_err_t (*close)(struct rt_nv7lidar_device *nv7lidar);
    rt_size_t (*read)(struct rt_nv7lidar_device *nv7lidar, rt_off_t pos, void *buffer, rt_size_t size);
    rt_size_t (*write)(struct rt_nv7lidar_device *nv7lidar, rt_off_t pos, const void *buffer, rt_size_t size);
    rt_err_t (*control)(struct rt_nv7lidar_device *nv7lidar, rt_uint32_t cmd, void *args);
};

struct rt_nv7lidar_device
{
    struct rt_device parent;
    const struct rt_nv7lidar_ops *ops;
    enum rt_nv7lidar_type type;
};

rt_err_t rt_device_nv7lidar_register(struct rt_nv7lidar_device *nv7lidar, const char *name, void *user_data);

#endif /* APPLICATIONS_RT_NV7LIDAR_H_ */
