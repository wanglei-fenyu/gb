#include "endpoint_help.h"
#include <sstream>

namespace gb
{

std::string EndpointToString(const Endpoint& endpoint)
{
    std::stringstream ss;
    ss << endpoint;
    return ss.str();
}

std::string HostOfEndpoint(const Endpoint& endpoint)
{
    return endpoint.address().to_string();
}

uint32_t PortOfEndpoint(const Endpoint& endpoint)
{
     return endpoint.port();
}

bool ResolveAddress(IoService& io_service, const std::string& host, const std::string& svc, Endpoint* endpoint)
{
    Resolver resolver(io_service);
    Error_code ec;
    Resolver::iterator it,end;
    it = resolver.resolve(Resolver::query(host, svc), ec);
    if (it != end)
    {
        *endpoint = it->endpoint();
        return true;
    }
    
    LOG_WARN("resolve address [{}:{}] failed:{}", host, svc, ec.message());
    return false;

}

bool ResolveAddress(IoService& io_service, const std::string& address, Endpoint* endpoint)
{
    std::string::size_type pos = address.find(':');
    if (pos == std::string::npos)
    {
        LOG_WARN("invalid address: {}", address);
        return false;
    }
    
    std::string host = address.substr(0, pos);
    std::string svc = address.substr(pos + 1);
    return ResolveAddress(io_service, host, svc, endpoint);
}

}