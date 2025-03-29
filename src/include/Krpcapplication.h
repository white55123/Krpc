#ifndef _Krpcapplication_H
#define _Krpcapplication_H

#include "KrpcConfig.h"
#include "Krpccontroller.h"
#include <mutex>
//Krpc基础类，负责进行框架的一些初始化操作
class KrpcApplication
{
public:
    static void Init(int argc, char **argv);
    static KrpcApplication & GetInstance();
    static void deleteInstance();
    static KrpcConfig& getConfig();
private:
    static KrpcConfig m_config;
    static KrpcApplication *m_applicaiton;      //单例
    static std::mutex m_mutex;
    KrpcApplication() {}
    ~KrpcApplication() {}
    KrpcApplication(const KrpcApplication&)=delete;
    KrpcApplication& operator=(const KrpcApplication&)=delete;
};

#endif