cmake_minimum_required(VERSION 2.8)

project(demo)

add_executable(demo_base
	base_demo.c
	)

add_executable(demo_timer
	base_timer.c
	)

target_link_libraries(demo_timer
	rt
	)
