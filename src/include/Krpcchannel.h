#ifndef _Krpcchannel_H_
#define _Krpcchannel_H_
//此类继承google::protobuf::RpcChannel
//目的是为了让客户端调用方法时，统一进行接收
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include "zookeeperutil.h"

class KrpcChannel : public google::protobuf::RpcChannel
{
public:
    KrpcChannel(bool connectNow);
    virtual ~KrpcChannel()
    {

    }
    void CallMethod(const ::google::protobuf::MethodDescriptor *method,
        ::google::protobuf::RpcController *controller,
        const ::google::protobuf::Message *request,
        ::google::protobuf::Message *response,
        ::google::protobuf::Closure *done) override;
private:
        int m_clientfd; //客户端套接字
        std::string service_name;
        std::string m_ip;
        uint16_t m_port;
        std::string method_name;
        int m_idx;      //划分服务器IP和PORT的下标
        bool newConnect(const char *ip, uint16_t port);
        std::string QueryServiceHost(ZkClient *zkclient, std::string service_name, std::string method_name, int &idx);
};
#endif