#ifndef KRPC_LOG_H
#define KRPC_LOG_H
#include <glog/logging.h>
#include <string>

class KrpcLogger
{
public:
    //自动初始glog
    explicit KrpcLogger(const char *argv0)
    {
        google::InitGoogleLogging(argv0);
        FLAGS_colorlogtostderr=true;    //彩色日志
        FLAGS_logtostderr=true;         //默认输出标准错误
    }

    ~KrpcLogger() {
        google::ShutdownGoogleLogging();
    }

    //静态日志方法
    static void Info(const std::string &message)
    {
        LOG(INFO) << message;
    }

    static void Warning(const std::string &message) 
    {
        LOG(WARNING) << message;
    }

    static void Error(const std::string &message)
    {
        LOG(ERROR) << message;
    }

    static void Fatal(const std::string &message) 
    {
        LOG(FATAL) << message;
    }

    private:
        //禁用拷贝构造和拷贝复制
        KrpcLogger(const KrpcLogger&)=delete;
        KrpcLogger& operator=(const KrpcLogger&)=delete;
};

#endif