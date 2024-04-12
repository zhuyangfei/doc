# genapicontrollableprocess
use SIGTERM and SIGINT signals

## implement
// ServiceProviderPlugin 构造
ServiceProviderPlugin::ServiceProviderPlugin(std::shared_ptr<genapi::PluginServices> pluginServices,
                                             const std::string& serviceName,
                                             std::unique_ptr<lightweightservicediscovery::Connection> connection):
    pluginServices(pluginServices),
    serviceName(serviceName),
    fdMonitor(pluginServices->getFDMonitor()),
    connection(std::move(connection)),
    advertising(false)
{
    // 1. 添加监测this->connection->fd()
    fdMonitor.addFD(this->connection->fd(),
                    genapi::FDMonitor::EVENT_IN,
                    std::bind(&ServiceProviderPlugin::eventHandler,
                              this));
}

void ServiceProviderPlugin::setAttributes(const genapi::serviceprovider::Attributes& attrs)
{
    // 1. 通过构造AtScopeExitAdvertiseAttributes对象，把fd加入到epoll中
    => AtScopeExitAdvertiseAttributes updater( *this );
    => fdMonitor.addFD(fd, FDMonitor::EVENT_IN, std::bind(&EventFD::handleEvents, this);
}

void ServiceProviderPlugin::startAdvertisingAsync(const StartAdvertisingAck& startAdvertisingAck)
{
    //1. 向lightweightservicediscovery::Connection注册service对应的callback
    // std::unique_ptr<lightweightservicediscovery::Connection> connection
    connection->registerServiceInstanceAsync( serviceName,
                                              convert( attributes ),
                                              [startAdvertisingAck](const std::error_code& status, const std::string& instanceName)
                                              {
                                                  startAdvertisingAck( status, instanceName );
                                              });
    // 2. 更registrations的数据
    if(! registrations.insert( std::make_pair(typeName,registration) ).second )
         throw Connection::AlreadyRegistered( constructAlreadyRegisteredMsg( typeName ));
    // 3. 更新EventFD维护的callback列表
    // 4. 通过系统函数write，进行一次write操作
    // 5. [option], 把registration更新给daemon

}


void Engine::waitAndHandleEvents()
{
    //1. 调用系统函数epoll_wait, 死等
    const int count(system.epoll_wait(epfd, ebuffer.data(), static_cast<int>(ebuffer.size()), timeout));
    //2. 有事件发生时，触发callback函数
    // using EventHandler = std::function<void(unsigned int events)>;
    =>callHandler(i);
}

std::shared_ptr<GenAPI> GenAPI::create()
{
    // 1. 创建一个engine对象
    auto engine(std::make_shared<Engine>(System::getSystem()));
    // Ensure that EventFD (callback queue) exists and has been added to epoll set
    engine->getCallbackQueue();
    return engine;
}

Engine::Engine(System& system):
    system(system),
    mode(Mode::WITH_EPOLL),
    epfd(system.epoll_create1(EPOLL_CLOEXEC)),
    tracer(new Tracer(getTraceSize(), useStdErrTrace(), useSyslogTrace()))
{
    //1. 通过调用epoll_create1创建一个epoll实例, 并保存到epfd中
    //2.
    traceDisclaimer(*tracer);
    tracer->output() << PACKAGE_STRING " create Engine as GenAPI, epfd: " << epfd
                     << std::endl;
}
## REF
https://linux.die.net/man/2/epoll_create1
[epoll tutorial](https://www.suchprogramming.com/epoll-in-3-easy-steps/)

1、callback如何设置到engin的callback队列？

2、监测哪一个fd？
std::unique_ptr<lightweightservicediscovery::Connection>
fdMonitor.addFD(this->connection->fd(),
                    genapi::FDMonitor::EVENT_IN,
                    std::bind(&ServiceProviderPlugin::eventHandler,
                              this));

template <typename PluginType>
std::shared_ptr<PluginType> Plugin::create(std::shared_ptr<GenAPIEventLoop> genAPIEventLoop, const std::string&functionName, const void* extraArgs)
{
    auto ret(std::dynamic_pointer_cast<PluginType>(createImpl(genAPIEventLoop, typeid(PluginType).name(), typei(PluginType).hash_code(), functionName, extraArgs)));
    if (ret == nullptr)
        abort();
    return ret;
}

## 为什么实现设计成plugin的方式，而不是动态库？


