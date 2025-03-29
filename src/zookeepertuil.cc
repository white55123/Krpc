#include "zookeeperutil.h"
#include "Krpcapplication.h"
#include <mutex>
#include <condition_variable>
#include "KrpcLogger.h"
std::mutex cv_mutex;   //全局锁
std::condition_variable _cv;//信号量
bool is_connected = false;

//全局的watcher观察器，zkserver给zkclient的通知
void global_watcher(zhandle_t *zh, int type, int status, const char *path, void *watcherCtx) {
    if(type == ZOO_SESSION_EVENT)   {      
        if(status == ZOO_CONNECTED_STATE) {         // //连接成功
            std::lock_guard<std::mutex> lock(cv_mutex);
            is_connected = true;
        }
    }   
    _cv.notify_all();
}

ZkClient::ZkClient() : m_zhandle(nullptr) {

}

ZkClient::~ZkClient()
{
    if(m_zhandle != nullptr) {
        zookeeper_close(m_zhandle);
    }
}

void ZkClient::Start() {
    std::string host = KrpcApplication::GetInstance().getConfig().Load("zookeeperip");
    std::string port = KrpcApplication::GetInstance().getConfig().Load("zookeeperport");
    std::string connstr = host + ":" + port;
    m_zhandle = zookeeper_init(connstr.c_str(), global_watcher, 3000, nullptr, nullptr, 0);
    if(nullptr == m_zhandle)
    {
        LOG(ERROR) << "zookeeper_init error";
        exit(EXIT_FAILURE);
    }

    std::unique_lock<std::mutex> lock(cv_mutex);
    _cv.wait(lock,[] {return is_connected;});
    LOG(INFO) << "zookeeper_init success";
}

void ZkClient::Create(const char* path, const char* data, int datalen, int state)
{
    //创建znode节点，可以选择永久性节点还是临时节点
    char path_buffer[128];
    int bufferlen = sizeof(path_buffer);
    int flag = zoo_exists(m_zhandle, path, 0, nullptr);
    if(flag == ZNONODE) //节点不存在
    {
        //创建节点
        flag = zoo_create(m_zhandle, path, data, datalen, &ZOO_OPEN_ACL_UNSAFE, state, path_buffer, bufferlen);
        if(flag == ZOK) {
            LOG(INFO) << "znode create success... path: " << path;
        } else {
            LOG(ERROR) << "znode create success... path: " << path;
            exit(EXIT_FAILURE);
        }
    }
}

std::string ZkClient::GetData(const char *path)
{
    char buf[64];
    int bufferlen = sizeof(buf);
    int flag = zoo_get(m_zhandle, path, 0, buf, &bufferlen, nullptr);
    if (flag != ZOK)
    {
        LOG(ERROR) << "zoo_get error";
        return "";
    }
    else
    {
        return buf;
    }
    return "";
}