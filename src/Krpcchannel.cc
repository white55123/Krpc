#include <memory>
#include "Krpcchannel.h"
#include "KrpcLogger.h"
#include <mutex>
#include "Krpcapplication.h"
#include "Krpccontroller.h"

std::mutex g_data_mutex;    //全局互斥锁，用于保护共享数据的线程安全

//TODO
//RPC调用的核心方法，负责将客户端的请求序列化并且发送给服务端，同时接受服务端的响应
void KrpcChannel::CallMethod(const ::google::protobuf::MethodDescriptor *method,
    ::google::protobuf::RpcController *controller,
    const ::google::protobuf::Message *request,
    ::google::protobuf::Message *response,
    ::google::protobuf::Closure *done)
{
    if(-1 == m_clientfd) {
        //获取服务对象名称和方法名称
        const google::protobuf::ServiceDescriptor *sd = method->service();
        service_name = sd->name();
        method_name = method->name();

        //客户端需要查询zookeeper， 找到提供该服务的服务器地址
        ZkClient zkCli;
        zkCli.Start();  //连接zookeeper
        //查询服务地址
        std::string host_data = QueryServiceHost(&zkCli, service_name, method_name, m_idx);
        m_ip = host_data.substr(0, m_idx);  //提取IP地址
        std::cout << "ip: " << m_ip << std::endl;
        //提取端口号
        m_port = atoi(host_data.substr(m_idx + 1, host_data.size() - m_idx).c_str());
        std::cout <<"port: " << m_port << std::endl;

        //连接服务器
        auto rt = newConnect(m_ip.c_str(), m_port);
        if(!rt) {
            LOG(ERROR) << "connect server error";   //连接失败，记录错误日志
            return;
        } else {
            LOG(INFO) << "connect server success";  
        }
    }

    uint32_t args_size{};
    std::string args_str;
    if(request->SerializeToString(&args_str)) {
        args_size = args_str.size();
    } else {
        controller->SetFailed("serialize request fail");  // 序列化失败，设置错误信息
        return;
    }
}