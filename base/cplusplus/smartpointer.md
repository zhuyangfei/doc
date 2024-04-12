# smart pointer

- [std::unique\_ptr](#stdunique_ptr)
  - [使用场景](#使用场景)
- [std::shared\_ptr](#stdshared_ptr)
  - [如何实现](#如何实现)
  - [std::unique\_ptr转成std::shared\_ptr](#stdunique_ptr转成stdshared_ptr)
- [std::weak\_ptr](#stdweak_ptr)
  - [使用场景](#使用场景-1)


## std::unique_ptr
独享对象所有权，所以，只能移动，不能拷贝。
被管理对象，在指针调用`reset`或者指针被重新构造时释放。
`std::unique_ptr`性能接近raw pointer。

### 使用场景
1. 对象工厂
2. 资源管理
```cpp
std::unique_ptr<T> createObject();
std::unique_ptr<int> p = std::make_unique<int>(2);
std::unique_ptr<T> p = std::make_unique<T>();
```

## std::shared_ptr
共享对象所有权，当每次构造时，拷贝时，引用计数加一，析构时，引用计数减一，当引用计数等于0时，对象被释放。

### 如何实现
`std::shared_ptr`相当于包含两个指针，一个指向对象，另一个指向`control block`, `control block`包含引用计数，弱引用计数及析构的function对象

### std::unique_ptr转成std::shared_ptr
```cpp
auto t = std::make_unique<T>();
std::shared_ptr<T> a = std::move(t);
```

## std::weak_ptr
不直接拥有对象所有权。使用时，通过`lock `接口，获取`std::shared_ptr`，然后，操作对象。
std::weak_ptr可以感知`std::shared_ptr`是否悬空

### 使用场景
1. 仅使用资源，不延长对象的声明周期，如：使用`std::weak_ptr`定义观察者模式中的观察者
2. A，B两个对象，使用`std::shared_ptr`的循环引用，导致的内存泄漏
3. 资源需要及时释放的场景
```cpp
// Entity only use resource, but doesn't extend lifetime of Resource
class Entity {
public:
    Entity(const std::string& name, const std::weak_ptr<Resource>& resource) : name(name), resource(resource) {}

    void useResource() {
        if (auto res = resource.lock()) { // Check if resource still exists
            std::cout << name << " is using resource." << std::endl;
        } else {
            std::cout << name << " cannot use resource. It has been unloaded." << std::endl;
        }
    }

private:
    std::string name;
    std::weak_ptr<Resource> resource;
};

// observe patten
// Message broker class
class MessageBroker {
public:
    // Subscribe a subscriber to a message type
    void subscribe(const std::shared_ptr<Subscriber>& subscriber, const std::string& messageType) {
        subscribers[messageType].push_back(subscriber);
    }

    // Publish a message to all subscribers interested in its type
    void publish(const std::shared_ptr<Message>& message, const std::string& messageType) {
        auto it = subscribers.find(messageType);
        if (it != subscribers.end()) {
            for (const auto& weakPtr : it->second) {
                if (auto subscriber = weakPtr.lock()) {
                    subscriber->receiveMessage(message);
                }
            }
        }
    }

private:
    std::unordered_map<std::string, std::vector<std::weak_ptr<Subscriber>>> subscribers;
};
```
https://chat.openai.com/share/40369f4e-03f1-4898-944b-1d9aa42bdaf6
