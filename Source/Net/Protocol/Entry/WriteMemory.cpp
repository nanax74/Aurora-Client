#include "Net/Protocol/Entry.hpp"
#include "Net/Transport/Connection.hpp"
#include "Data/ByteWriter.hpp"
#include "Data/ByteReader.hpp"
#include <memory>

namespace Net::Protocol::Entry
{
    WriteMemory::WriteMemory(uint32_t address, std::vector<std::byte> data) : address(address), data(data) {}

    ID WriteMemory::getId()
    {
        return ID::WriteMemory;
    }

    std::vector<std::byte> WriteMemory::serialize()
    {
        Data::ByteWriter writer;
        writer.write<uint32_t>(address);
        writer.writeVector(data);
        return writer.copy();
    }

    std::unique_ptr<WriteMemory> WriteMemory::deserialize(const std::vector<std::byte>& data)
    {
        Data::ByteReader reader(data);
        if(reader.remain() < sizeof(uint32_t) * 2) return nullptr;
        uint32_t address = reader.read<uint32_t>();
        std::vector<std::byte> d = reader.readVector();
        return std::unique_ptr<WriteMemory>(new WriteMemory(address, d));
    }

    void WriteMemory::process(std::shared_ptr<Transport::Connection> connection) {}
}