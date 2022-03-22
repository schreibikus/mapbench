#include <boost/unordered/unordered_map.hpp>
#include <boost/bimap.hpp>
#include <unordered_map>
#include <iostream>
#include <cxxabi.h>
#include <cstdlib>
#include <chrono>
#include <map>

class ClientInfo
{
public:
    ClientInfo() : countConnections(0) {}
    ClientInfo(int conecctions) : countConnections(conecctions) {}
    void incCon() { countConnections ++; }
    int countConnections;
};

template<typename F, typename... Args>
double funcTime(F func, Args&&... args)
{
    auto t1 = std::chrono::high_resolution_clock::now();
    func(std::forward<Args>(args)...);
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - t1).count();
}

template<typename Container>
void insertMap(Container &a, unsigned int keys_range)
{
    for(auto i = 0; i < keys_range; ++i)
        a.insert({i, 1});
}

template<typename Container>
void insertMapRandom(Container &a, unsigned int keys_range)
{
    for(auto i = 0; i < keys_range; ++i)
        a.insert({std::rand(), 1});
}

template<typename Container>
void clientStatistics(Container &a, unsigned int keys_range, unsigned int clientConnections)
{
    for(auto i = 0; i < clientConnections; ++i) {
        auto key = std::rand() % keys_range;
        auto iterator = a.find(key);
        if(iterator != a.end()) {
            iterator->second.incCon(); //increase count of requests
        } else {
            a.insert({key, 1});
        }
    }
#if 0
    auto allCon = 0;
    for(auto iterator = a.begin(); iterator != a.end(); ++iterator) {
        if(allCon < 1000)
            std::cout << "For " << iterator->first << " present " << iterator->second.countConnections << " connections" << std::endl;
        allCon += iterator->second.countConnections;
    }
    if(allCon != clientConnections)
        std::cout << "Error " << allCon << " != " << clientConnections << std::endl;
#endif
}

template<typename Container>
void bench()
{
    const unsigned int keys_range = 1000000;
    const unsigned int connections = 10000000;
    Container a;

    auto realname = abi::__cxa_demangle(typeid(Container).name(), 0, 0, 0);
    std::cout << "Benchmark for " << realname << std::endl;
    free(realname);

    std::cout << "Insert " << keys_range << " values " << funcTime(insertMap<Container>, a, keys_range) << " us" << std::endl;
    a.clear();
    std::cout << "Insert " << keys_range << " random values " << funcTime(insertMapRandom<Container>, a, keys_range) << " us" << std::endl;
    a.clear();
    std::cout << "Simulation of " << keys_range << " users witt a total of " << connections << " connections " << funcTime(clientStatistics<Container>, a, keys_range, connections) << " us" << std::endl;
    a.clear();
}

int main()
{
    std::srand(std::time(nullptr));

    bench<std::map<int, ClientInfo>>();
    bench<std::unordered_map<int, ClientInfo>>();
    bench<boost::unordered_map<int, ClientInfo>>();
    return 0;
}
