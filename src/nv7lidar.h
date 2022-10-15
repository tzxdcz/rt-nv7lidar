/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-10-15     沈瓒       the first version
 */
#ifndef APPLICATIONS_NV7LIDAR_H_
#define APPLICATIONS_NV7LIDAR_H_

rt_device_t nv7_lidar_create(const char* lidar_name);
rt_err_t nv7_lidar_init(rt_device_t lidar);
rt_err_t nv7_lidar_get_single_measure(rt_device_t lidar, rt_uint16_t* result_t, rt_uint32_t timeout);
rt_err_t nv7_lidar_open_continue_measure(rt_device_t lidar, rt_uint32_t timeout);
rt_err_t nv7_lidar_close_continue_measure(rt_device_t lidar, rt_uint32_t timeout);
rt_err_t nv7_lidar_continue_measure_get(rt_device_t lidar, rt_uint16_t* result_t, rt_uint32_t timeout);
rt_err_t nv7_lidar_open_indicate_laser(rt_device_t lidar, rt_uint32_t timeout);
rt_err_t nv7_lidar_close_indicate_laser(rt_device_t lidar, rt_uint32_t timeout);

#endif /* APPLICATIONS_NV7LIDAR_H_ */
