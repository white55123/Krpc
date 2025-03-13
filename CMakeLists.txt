#设置最低版本和项目名称
cmake_minimum_required(VERSION 3.0)
project(Krpc)

#设置全局的C++标准
set(CMAKE_CXX_STANDARD 11)
#强制使用指定的的标准
set(CMAKE_CXX_STANDARD_REQUIRED True)

#设置头文件的目录，给所有的子项目提供
include_directories(${CMAKE_SOURCE_DIR}/src/include)

#查找protobuf包
find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

#设置全局链接库
set(LIBS
    Protobuf
    pthread
    zookeeper_mt
    muduo_net
    muduo_base
    glog
)

#添加子目录
add_subdirectory(src)
add_subdirectory(example)