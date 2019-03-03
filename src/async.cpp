#include "async.h"
#include "commandprocessorsmanager.h"
#include "multithreadlogger.h"

namespace async {

handle_t connect(std::size_t bulkSize)
{
    auto& instance = bulk::CommandProcessorsManager<bulk::MultiThreadLogger<2>>::instance();
    return instance.createCommandProcessor(bulkSize);
}

void receive(handle_t handle, const char *data, std::size_t size)
{
    bulk::CommandProcessorsManager<bulk::MultiThreadLogger<2>>::instance().processCommandBuffer(handle,data,size);
}

void disconnect(handle_t handle)
{
    bulk::CommandProcessorsManager<bulk::MultiThreadLogger<2>>::instance().removeCommandProcessor(handle);
}

}
