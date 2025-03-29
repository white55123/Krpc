#include <iostream>
#include <string>
#include "../user.pb.h"
#include "Krpcapplication.h"
#include "Krpcprovider.h"

class UserService : public Kuser::UserServiceRpc    //继承protobuf生产的RPC服务基类
{
public:
    //本地登陆方法
    bool Login(std::string name, std::string pwd) {
        std::cout << "doing local service Login" << std::endl;
        std::cout << "name: " << name << "pwd: " << pwd << std::endl;
        return true;
    }


/*
重写基类UserServiceRpc的虚函数，这些方法会被RPC框架调用
1. 调用者(caller)通过RPC框架发送Login请求
2. 服务提供者(callee) 收到请求后， 调用下面重写的Login
*/

void Login(::google::protobuf::RpcController* controller,
        const ::Kuser::LoginRequest* request,
        ::Kuser::LoginResponse* response,
        ::google::protobuf::Closure* done) {
            //获取用户名和密码
            std::string name = request->name();
            std::string pwd = request->pwd();

            //调用本地业务逻辑处理登录
            bool login_result = Login(name, pwd);

            //将响应结果写入response对象中
            Kuser::ResultCode *code = response->mutable_result();
            code->set_errcode(0);   //设置错误码为0，表示成功
            code->set_errmsg("");   //设置错误消息为空
            response->set_success(login_result);    //设置登录结果

            done->Run();
        }

};

int main(int argc, char **argv) {
    KrpcApplication::Init(argc, argv);

    KrpcProvider provider;

    provider.NotifyService(new UserService());

    //启动RPC服务节点，进入阻塞状态，等待远程调用
    provider.Run();
}