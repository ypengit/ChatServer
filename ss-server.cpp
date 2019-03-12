#include <getopt.h>
#include <vector.h>
#include <event2/event.h>
#include <pthread.h>
#include <thread>
#include <signal.h>
#include <memory>





class Event{
    public:
    Event():
        base(event_base_new())
    {

    }

    //往event_base中添加一个handler,往event_base中
    //注册这个事件
    void addHandler(int fd, void(*) cb){
        struct event * ev = event_new(base, fd, cb, (void *)base);
        event_add(ev);
    }

    void loop(){
        event_add(evsignal_new(base, SIGINT, quit, event_self_cbargs()));
        event_base_dipatch(base);
    }

    void quit(int fd, short event, void * arg){
        struct event_base * base = (struct event_base *)arg;
        event_base_loopbreak(base);
        event_del(event);
    }

    void startToQuit(){
        quiting = true;
        event_base_loopbreak(base);
        exit(0);
    }


    struct event_base *base;
    bool quiting = false;
};

class EventThreadPool{
    public:
    EventThreadPool(int num){
        for(int i = 0; i < num; ++i){
            vec.push_back(new EventThread());
            vec[i].start();
        }
    }
    ~EventThreadPool(){

    }

    vector<shared_ptr<EventThread>> vec;
};



class Thread(){
    public:
        Thread(){

        }


};


class EventThread{
    public:
        EventThread(){

        }
        ~EventThread(){
            stop();
        }

        void start(){
            t.join();
        }

        void stop(){
            event.startToQuit();
        }

        void loopFunc(shared_ptr<Event> event){
            //[TODO]这里分配了event对象又该怎么做？
            //
            //自动就handle了呀，因为这些事件注册在Event这个基本对象中，
            //libevent这个库只要fd可用，就自动执行回调函数
            //loop->handleEvents();
            //
            //
            //那么，每个线程应该保有一个Event对象！！！！
            event.loop();
        };
        shared_ptr<Event> event;
        thread t(loopFunc, event);
};



//编码和解码的工作留到后面做，这里先假设不加密的情况[TODO]
//class EncryptDecrypt{
//    public:
//    static encrypt(
//};

class Server{
    public:
    //初始化服务器信息
    Server(char * server_ip, int server_port){
    Server (const char * server_ip,
            const char * server_port,
            const char * password,
            const char * method,
            Event * mainEvent):
    {

    }

    //创建一个线程池，用于处理来自于主线程的消息
    void start(){
        eventThreadPool.start();
        int socket_fd = listenPort();
        mainEvent->addHandler(socket_fd, handleMainConn, "MainThreadConnection");
    }

    int listenPort(){
        int socket_fd = socket(AF_INET, communicateMethod, 0);

        struct socketaddr_in serverAddr;
        serverAddr.sin_family(AF_INET);
        serverAddr.sin_port(htons(server_port));
        serverAddr.sin_addr.s_addr(htonl(server_ip));

        if(bind(socket_fd, (struct socketaddr *)serverAddr, sizeof(serverAddr)) == -1){
            perror("bind");
            exit(1);
        }
        if(listen(socket_fd, 2048) == -1){
            perror("listen");
            exit(1);
        }
        //此处就结束了，将accept操作放到主线程的那个循环中去
        return socket_fd;
    }

    int threadNum;
    EventLoopThreadPool eventLoopThreadPool;
    bool quit;
    int communicateMethod;

    struct status{
        int registedEventNum;
        struct event_base *base;
    }

    struct event_base  * selectEventBase(){
        if(threadStatus.top().registedEventNum == 0){
            threadStatus.top().registedEventNum += 1;
            return threadStatus.top().base;
        }
        for(int i = 0; i < threadNum; ++i){

        }
        struct event_base * base = threadStatus.top().base;
    }

    //小根堆线程负载均衡
    priority_queue<status, std::vector<status>, [](status &a, status &b){
        return std::greater<int>(a.registedEventNum, b.registedEventNum)}> threadStatus;
    vector<struct event_base *> events;

    void dispatchToWorkThread(int fd, short event, void * arg){
        char buffer[2048], res[2048];
        int validLen = read(fd, buff, 2048);
        strcpy(res, buffer);
        struct sockaddr_in *client_addr = (struct sockaddr_in*)arg;

    }

    void handleMainConn(int fd, short event, void * arg){
        struct event_base *base = (struct event_base *)arg;

        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);

        int accept_fd = accept(fd, (struct sockaddr*)&client_addr, &len);
        if(accept_fd < 0){
            perror("accept error");
            exit(1);
        }

        //此时应该在主线程中监听，如果监听到事件到来，则解析并把事件派遣到工作线程
        struct event * listenEvent;
        listenEvent = event_new(base, accept_fd, EV_PERSIST, dispatchToWorkThread, (void *)client_addr);
        struct timeval tv;
        evutil_timerclear(&tv);
        tv.tv_sec = 2;
        event_add(listenEvent, &tv);
        event_base_dispatch(base);
    };

//Epoll对象应该是谁持有？[TODO]
class Epoll(){

};




void quit(){

}

int main(int argc, char ** argv){
    signal(SIGINT, quit);
    int opt;
    int load_config = false;

    char * server_ip;
    char * local_address_ip;
    char * password;
    char * method;

    int server_port;
    int local_port;
    int timeout;

    const char * str = "c|s|p|l|k|m|t|h";
	while(opt = getopt(argc, argv, str)){
		switch(opt){
			case 'c':
				load_config = true;
			case 'h':/*{{{*/
				std::cout << "" << std::endl;
					<< "  usage:" << std::endl
					<< "" << std::endl
					<< "    ss-server" << std::endl
					<< "" << std::endl
					<< "       -s <server_host>           Host name or IP address of your remote server." << std::endl
					<< "       -p <server_port>           Port number of your remote server." << std::endl
					<< "       -l <local_port>            Port number of your local server." << std::endl
					<< "       -k <password>              Password of your remote server." << std::endl
					<< "       -m <encrypt_method>        Encrypt method: rc4-md5, " << std::endl
					<< "                                  aes-128-gcm, aes-192-gcm, aes-256-gcm," << std::endl
					<< "                                  aes-128-cfb, aes-192-cfb, aes-256-cfb," << std::endl
					<< "                                  aes-128-ctr, aes-192-ctr, aes-256-ctr," << std::endl
					<< "                                  camellia-128-cfb, camellia-192-cfb," << std::endl
					<< "                                  camellia-256-cfb, bf-cfb," << std::endl
					<< "                                  chacha20-ietf-poly1305," << std::endl
					<< "                                  salsa20, chacha20 and chacha20-ietf." << std::endl
					<< "                                  The default cipher is rc4-md5." << std::endl
					<< "" << std::endl
					<< "       [-a <user>]                Run as another user." << std::endl
					<< "       [-f <pid_file>]            The file path to store pid." << std::endl
					<< "       [-t <timeout>]             Socket timeout in seconds." << std::endl
					<< "       [-c <config_file>]         The path to config file." << std::endl
					<< "       [-n <number>]              Max number of open files." << std::endl
					<< "       [-i <interface>]           Network interface to bind." << std::endl
					<< "       [-b <local_address>]       Local address to bind." << std::endl
					<< "" << std::endl
					<< "       [-u]                       Enable UDP relay." << std::endl
					<< "       [-U]                       Enable UDP relay and disable TCP relay." << std::endl
					<< "       [-6]                       Resovle hostname to IPv6 address first." << std::endl
					<< "" << std::endl
					<< "       [-d <addr>]                Name servers for internal DNS resolver." << std::endl
					<< "       [--reuse-port]             Enable port reuse." << std::endl
					<< "       [--fast-open]              Enable TCP fast open." << std::endl
					<< "                                  with Linux kernel > 3.7.0." << std::endl
					<< "       [--acl <acl_file>]         Path to ACL (Access Control List)." << std::endl
					<< "       [--manager-address <addr>] UNIX domain socket address." << std::endl
					<< "       [--mtu <MTU>]              MTU of your network interface." << std::endl
					<< "       [--mptcp]                  Enable Multipath TCP on MPTCP Kernel." << std::endl
					<< "       [--no-delay]               Enable TCP_NODELAY." << std::endl
					<< "       [--key <key_in_base64>]    Key of your remote server." << std::endl
					<< "       [--plugin <name>]          Enable SIP003 plugin. (Experimental)" << std::endl
					<< "       [--plugin-opts <options>]  Set SIP003 plugin options. (Experimental)" << std::endl
					<< "" << std::endl
					<< "       [-v]                       Verbose mode." << std::endl
					<< "       [-h, --help]               Print this message." << std::endl;/*}}}*/
				break;
			case 's':
                server_ip = new char[sizeof(optarg)];
                strcpy(server_ip, optarg);
				break;
            case 'p':
				server_port = atoi(optarg);
                break;
			case 'l':
				local_port = atoi(optarg);
                break;
            case 'k':
                password = new char[sizeof(optarg)];
                strcpy(passward, optarg);
                break;
            case 'm':
                method = new char[sizeof(optarg)];
                strcpy(method, optarg);
                break;
            //先做这些选项[TODO],剩下的以后做
        }
    }


    //创建监听服务器，监听事件的到来,服务器的设计
    //要考虑多个使用者的情况
    Event mainEvent;

    Server server(server_ip, server_port, password, method, &mainEvent);
    server.start();


    //使用事件循环，来处理sock5报文,它负责将从端口
    //接收到的报文转给其他线程来处理,这个event
    //为主线程,负责将socket_fd注册到event上
    //  事件的处理用来监听主线程上端口信息的变化情
    //  况，并把信息传递给一个全局对象,这个对象是
    //  epoll的包装
    mainEvent.loop();

    delete [] server_ip;
    delete [] local_ip;
    delete [] password;
    delete [] method;

    return 0;
}
