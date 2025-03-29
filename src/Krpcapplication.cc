#include "Krpcapplication.h"

KrpcConfig KrpcApplication::m_config;//全局变量
std::mutex KrpcApplication::m_mutex;
KrpcApplication* KrpcApplication::m_applicaiton = nullptr;  //懒汉模式

void KrpcApplication::Init(int argc, char **argv) {
    if(argc < 2) {
        std::cout << "format: command -i <configfile>" << std::endl;
        exit(EXIT_FAILURE);
    }
    int o;
    std::string config_file;
    while(-1 != (o = getopt(argc, argv, "i:"))) {
        switch(o) {
            case 'i':
                config_file = optarg;       //将i后的配置文件的路径赋给config_file
                break;
            case '?':
                std::cout <<"format:command -i <configfile>" << std::endl;
                exit(EXIT_FAILURE);
                break;
            case ':':                       //出现了i，但是没有参数
                std::cout << "format:command -i <configfile>" << std::endl;
                exit(EXIT_FAILURE);
                break;
            default:
                break;
        }
    }
    m_config.LoadConfigFile(config_file.c_str());
}

KrpcApplication &KrpcApplication::GetInstance() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if(m_applicaiton == nullptr) {
        m_applicaiton = new KrpcApplication();
        atexit(deleteInstance);
    }

    return *m_applicaiton;
}

void KrpcApplication::deleteInstance() {
    if(m_applicaiton) {
        delete m_applicaiton;
    }
}

KrpcConfig& KrpcApplication::getConfig() {
    return m_config;
}