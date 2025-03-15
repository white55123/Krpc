#include "Krpcapplication.h"
#include "../user.pb.h"
#include "Krpccontroller.h"
#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include "KrpcLogger.h"

//发送RPC请求的函数，模拟客户端调用远程服务
void send_request(int thread_id, std::atomic<int> &success_count, std::atomic<int>& fail_count) {
    //创建一个UserServiceRpc_Stub对象， 用于调用远程的RPC方法
    Kuser::UserServiceRpc_Stub stub(new KrpcChannel(false));

    //设置RPC方法的请求参数
    Kuser::LoginRequest request;
    request.set_name("zhangsan");   //设置用户名
    request.set_pwd("123456");      //设置密码

    //定义RPC方法的响应参数
    Kuser::LoginResponse response;
    Krpccontroller controller;  //创建控制器对象， 用于处理RPC调用过程中的错误

    //调用远程的Login方法
    stub.Login(&controller, &request, &response, nullptr);

    //检查RPC调用是否成功
    if(controller.Failed()) //调用失败
    {
        std::cout << controller.ErrorText() << std::endl;   //打印错误信息
        fail_count++;   
    } else {    //调用成功
        if(0 == response.result().errcode()) {  //检查错误码
            std::cout << "rpc login response success : " << response.success() << std::endl;  
            success_count++;
        } else {
            std::cout << "rpc login response error : " << response.result().errmsg() << std::endl;
            fail_count++;
        }
    }
}

int main(int argc, char **argv) {
    //初始化RPC框架，解析命令行，加载配置文件
    KrpcApplication::Init(argc, argv);

    //创建日志对象
    KrpcLogger logger("MyRPC");

    const int thread_count = 10;    //并发线程数
    const int requests_per_thread = 1;  //每个线程发送的请求数量

    std::vector<std::thread> threads;   //存储线程对象的容器
    std::atomic<int> success_count(0);  //成功请求的计数器
    std::atomic<int> fail_count(0);     //失败请求的计数器

    auto start_time = std::chrono::high_resolution_clock::now();    //记录测试开始的时间

    //启动多线程进行并发测试
    for(int i = 0; i < thread_count; ++i) {
        threads.emplace_back([argc, argv, i, &success_count, &fail_count, requests_per_thread]() {
            for(int j = 0; j < requests_per_thread; ++j) {
                send_request(i, success_count, fail_count);
            }
        });
    }

    //等待线程执行完毕
    for(auto &t : threads) {
        t.join();
    }

    time_t end_time = std::chrono::high_resolution_clock::now();    //记录测试结束时间
    std::chrono::duration<double> elapsed = end_time - start_time;  //计算测试耗时

    //输出统计结果
    LOG(INFO) << "Total requests: " << thread_count * requests_per_thread;  //总请求数
    LOG(INFO) << "Success count: " << success_count;                        //成功请求数
    LOG(INFO) << "Fail count" << fail_count;                                //失败请求数
    LOG(INFO) << "Elapsed time: " << elapsed.count() << " seconds";         //测试耗时
    LOG(INFO) << "QPS: " << (thread_count * requests_per_thread) / elapsed.count(); //每秒请求数

    return 0;
}