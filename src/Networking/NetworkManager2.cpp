#include "NetworkManager2.h"

#include "NetworkManager2.h"

namespace Minecraft::Server {
	NetworkManager::NetworkManager(ServerSocket* socket)
	{
		m_Socket = socket;
	}
	void NetworkManager::AddPacket(PacketOut* p)
	{
		utilityPrint("Clearing Packet Queue", Utilities::LOGGER_LEVEL_DEBUG);
		packetQueue.push(p);
	}
	void NetworkManager::ClearPacketQueue()
	{
		utilityPrint("Clearing Packet Queue", Utilities::LOGGER_LEVEL_DEBUG);
		for (int i = 0; i < packetQueue.size(); i++) {
			delete packetQueue.front();
			packetQueue.pop();
		}
	}
	void NetworkManager::SendPackets()
	{
		utilityPrint("Sending Network Packet Queue", Utilities::LOGGER_LEVEL_DEBUG);
		std::vector<byte> endByteBuffer;
		int len = packetQueue.size();
		for (int i = 0; i < len; i++) {
			endByteBuffer.clear();

			int packetLength = packetQueue.front()->bytes.size() + 2;

			//Header
			encodeVarInt(packetLength, endByteBuffer);
			encodeShort(packetQueue.front()->ID, endByteBuffer);

			//Add body
			for (int x = 0; x < packetQueue.front()->bytes.size(); x++) {
				endByteBuffer.push_back(packetQueue.front()->bytes[x]);
			}

			utilityPrint("Sending packet with ID: " + std::to_string(packetQueue.front()->ID), Utilities::LOGGER_LEVEL_DEBUG);
			//Send over socket
			m_Socket->Send(endByteBuffer.size(), endByteBuffer.data());

			delete packetQueue.front();
			packetQueue.pop();
		}
	}
	bool NetworkManager::ReceivePacket()
	{
		PacketIn* p = m_Socket->Recv();
		if (p != NULL) {
			unhandledPackets.push(p);
		}
		return (p != NULL);
	}
	void NetworkManager::HandlePackets()
	{
		utilityPrint("Handling Packets...", Utilities::LOGGER_LEVEL_TRACE);

		int len = unhandledPackets.size();
		for (int i = 0; i < len; i++) {
			PacketIn* p = unhandledPackets.front();

			if (packetHandlers.find(p->ID) != packetHandlers.end()) {
				packetHandlers[p->ID](p);
				delete p;
				unhandledPackets.pop();
			}
		}
	}
	void NetworkManager::AddPacketHandler(int id, PacketHandler h)
	{
		utilityPrint("Added Packet Handler for ID: " + std::to_string(id), Utilities::LOGGER_LEVEL_DEBUG);
		packetHandlers.emplace(id, h);
	}
	void NetworkManager::ClearPacketHandlers()
	{
		utilityPrint("Clearing Packet Handlers", Utilities::LOGGER_LEVEL_DEBUG);
		packetHandlers.clear();
	}
}