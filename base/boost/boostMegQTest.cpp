#include<iostream>
#include<thread>
#include<string>
#include<sstream>
#include<boost/interprocess/ipc/message_queue.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <genapi/serviceprovider/ServiceProvider.hpp>
#include <chrono>

using namespace boost::interprocess;
using genapi::serviceprovider::Attributes;
using genapi::serviceprovider::NameAndValue;

// include/ServiceProviderNameAndValueSerialization.hpp
namespace boost {
namespace serialization {

template<class Archive>
void serialize(Archive & ar, NameAndValue & nameAndValue, const unsigned int version)
{
    (void)version;
    ar & nameAndValue.name;
    ar & nameAndValue.value;
}

} // namespace serialization
} // namespace boost

// user class
class Producer
{
public:
    explicit Producer(message_queue& mq)
        : m_mq(mq)
        {}
    ~Producer(){}
    void send()
    {
        std::cout << "sending data ...\n";
        Attributes rannic;
        rannic.push_back(genapi::serviceprovider::NameAndValue({"ipaddr", "192.168.0.1"}));
        rannic.push_back(genapi::serviceprovider::NameAndValue({"type", "rannic"}));
        std::stringstream oss;
        boost::archive::text_oarchive oa(oss);
        oa << rannic;

        std::string serialized_string(oss.str());
        m_mq.send(serialized_string.data(), serialized_string.size(), 0);
        std::cout << "send data done\n";
    }
private:
    message_queue& m_mq;
};

class Consumer
{
public:
    explicit Consumer(message_queue& mq)
        : m_mq(mq)
        {}
    ~Consumer(){}
    void receive()
    {
        std::cout << "wait receiving...\n";
        message_queue::size_type recvd_size = 0u;  // outref? need initailize
        unsigned int priority = 0u;
        Attributes rannic;
        std::string serialized_string;
        serialized_string.resize(1024);
        m_mq.receive(&serialized_string[0], 1024, recvd_size, priority);
        std::cout << "receivee data\n";
        std::stringstream iss;
        iss << serialized_string;

        boost::archive::text_iarchive ia(iss);
        ia >> rannic;
        for(auto it = rannic.begin(); it !=rannic.end(); ++it)
        {
            std::cout << it->name << " " << it->value << "\n";
        }
    }
private:
    message_queue& m_mq;
};

int main()
{
    const int MaxMsgLength = 1024;
    message_queue::remove("rannicMessage_queue");
    message_queue mq(create_only,
            "rannicMessage_queue",
            1,
            MaxMsgLength
        );
    Producer produce(mq);
    Consumer consumer(mq);

    std::thread produceTh(&Producer::send, &produce);
    std::thread consumerTH(&Consumer::receive, &consumer);
    produceTh.join();
    consumerTH.join();

    return 0;

}
