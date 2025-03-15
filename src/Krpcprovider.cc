#include "Krpcapplication.h"
#include "Krpcprovider.h"
#include "Krpccheader.pb.h"
#include "KrpcLogger.h"
#include <iostream>

//用来注册服务对象和其对应的rpc方法，以便于服务端处理客户端的请求
void KrpcProvider::NotifyService(google::protobuf::Service* service)
{
    //服务端需要知道对方想要调用的具体服务对象和方法，
    //这些信息会存储在ServiceInfo中
    ServiceInfo service_info;

    // 参数类型设置为 google::protobuf::Service，是因为所有由 protobuf 生成的服务类
    // 都继承自 google::protobuf::Service，这样我们可以通过基类指针指向子类对象，
    // 实现动态多态。

    //通过动态多态调用service->GetDescriptor();
    //GetDescriptor()方法会返回protobuf生成的服务类的描述信息(ServiceDescriptor)
    const google::protobuf::ServiceDescriptor *psd = service->GetDescriptor();

    //通过ServiceDescriptor,我们可以获取该服务类中定义的方法列表
    //并进行相应的注册和管理

    //获取服务的名称
    std::string service_name = psd->name();
    //获取服务端对象service的方法数量
    int method_count = psd->method_count();

    //打印service_name
    std::cout << "service_name= " << service_name << std::endl;

    for(int i = 0; i < method_count; ++i) {
        //获取服务中的方法描述
        const google::protobuf::MethodDescriptor *pmd = psd->method(i);
        std::string method_name = pmd->name();
        std::cout << "method_name= " << method_name << std::endl;
        service_info.method_map.emplace(method_name, pmd);
    }

    service_info.service = service;
    service_map.emplace(service_name, service_info);
}

//启动rpc服务节点，开始提供rpc远程网络调用服务
void KrpcProvider::Run()
{
    //读取配置文件rpcservice的信息
    std::string ip = KrpcApplication::GetInstance().getConfig().Load("rpcserverip");
    int port = atoi(KrpcApplication::GetInstance().getConfig().Load("rpcserverport").c_str());
    //使用muduo，创建address对象
    muduo::net::InetAddress address(ip,port);

    //创建tcpserver对象
    std::shared_ptr<muduo::net::TcpServer> server = std::make_shared<muduo::net::TcpServer>(&event_loop, address, "KrpcProvider");

    //绑定连接回调和消息回调
    server->setConnectionCallback(std::bind(&KrpcProvider::OnConnection, this, std::placeholders::_1));
    server->setMessageCallback(std::bind(&KrpcProvider::OnMessage, this, std::placeholders::_1, 
        std::placeholders::_2, std::placeholders::_3));

    //设置muduo库的线程数量
    server->setThreadNum(4);

    //把当前rpc节点上要发布的服务全部注册到zk上，让rpc client客户端在zk上发现服务
    ZkClient zkclient;
    zkclient.Start();   //连接zookeeper
    for(auto &sp : service_map) {
        std::string service_path = "/" + sp.first;
        zkclient.Create(service_path.c_str(), nullptr, 0);
        for(auto &mp : sp.second.method_map) {
            std::string method_path = service_path + "/" + mp.first;
            char method_path_data[128] = {0};
            sprintf(method_path_data, "%s:%d", ip.c_str(), port);   //ip和端口信息存入节点数据
            // ZOO_EPHEMERAL表示这个节点是临时节点，在客户端断开连接后，ZooKeeper会自动删除这个节点
            zkclient.Create(method_path.c_str(), method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);
        }
    }

    //RPC服务准备启动，打印信息
    std::cout << "RpcProvider start service at ip:" << ip << "port: " << port << std::endl;

    //启动网络服务
    server->start();
    event_loop.loop();  //进入事件循环
}