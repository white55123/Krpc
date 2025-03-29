#ifndef _Krpcprovider_H__
#define _Krpcprovider_H__
#include "google/protobuf/service.h"
#include "zookeeperutil.h"
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpConnection.h>
#include <google/protobuf/descriptor.h>
#include <unordered_map>

class KrpcProvider
{
public:
    //提供给外部使用，可以发布rpc方法的函数接口
    void NotifyService(google::protobuf::Service* service);
    ~KrpcProvider();
    //启动rpc服务节点，提供rpc远程网络调用服务
    void Run();
    //开启提供rpc网络调用的服务
private:
    muduo::net::EventLoop event_loop;
    
    struct ServiceInfo
    {
        google::protobuf::Service* service;
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> method_map;
    };
    //存储注册成功的服务对象和其服务方法的所有信息
    std::unordered_map<std::string,ServiceInfo> service_map;


    void OnConnection(const muduo::net::TcpConnectionPtr& conn);
    //新的socket连接回调
    void OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buffer, muduo::Timestamp receive_time);
    //已建立连接用户的读写事件回调
    void SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response);
    //用于序列化rpc的响应和网络发送
};

#endif