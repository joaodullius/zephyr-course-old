# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

set(SHIELD x_nucleo_iks01a2)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(sensor)

target_sources(app PRIVATE src/main.c)
