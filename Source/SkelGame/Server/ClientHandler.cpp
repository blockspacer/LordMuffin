#include "SkelPCH.h"
#include <Objects/Player/PlayerObject.h>
#include "ClientHandler.h"

namespace Skel::Net {
	ClientHandler::ClientHandler()
	{
		m_ClientSlots.reserve(MAX_PLAYERS);
		for (int i = 0; i < MAX_PLAYERS; ++i) {
			m_SlotAvailability[i] = true;
			m_LatestTick[i] = 0;	// acceptable cache-thrash
			m_InputAcks[i] = 0;
		}
	}
	uint16 ClientHandler::AddPlayer(Address playerAddress)
	{
		ASSERT(RemainingSlots() > 0, "No remaining slots when trying to add player");

		uint16 playerIndex = FindAvailableSlotIndex();
		ASSERT(m_SlotAvailability[playerIndex] == true, "Cannot add to filled up slot");
		m_SlotAvailability[playerIndex] = false;

		m_ClientSlots.emplace_back(playerIndex, playerAddress);

		m_ActivePlayers++;

		return playerIndex;
	}
	uint16 ClientHandler::GetClientIndex(const Address& address) const
	{
		auto slot = std::find_if(m_ClientSlots.begin(), m_ClientSlots.end(),
			[&addr = address](const ClientSlot& s) -> bool { return addr == s.ClientAddress; });
		return slot->ID;
	}
	uint64 ClientHandler::GetClientTick(uint16 clientIndex) const
	{
		return m_LatestTick[clientIndex];
	}
	void ClientHandler::UpdateClientTick(uint16 clientIndex, uint64 tick)
	{
		TryInputAck(clientIndex, tick);

		m_LatestTick[clientIndex] = tick;
	}
	// Input acks. Will shift if needed. Returns true if not input acked, and thus successfully acks
	bool ClientHandler::TryInputAck(uint16 clientIndex, uint64 tick)
	{
		uint64 latest = GetClientTick(clientIndex);
		uint64& ack = m_InputAcks[clientIndex];
		ASSERT(latest != tick, "Should not Ack current tick");
		// Shifts to make space and then acks
		if (tick > latest) {
			uint16 offset = tick - latest;
			ack <<= offset;
			ack |= 1;
		}
		// Finds and acks appropriate bit
		else {
			uint16 offset = latest - tick;
			// Return false if already acked
			uint64 bitState = m_InputAcks[clientIndex] & (uint64(1) << offset);
			if (bitState > 0) return false;

			ack |= (uint64(1) << offset);
		}
		return true;
	}
	void ClientHandler::RemovePlayer(uint16 clientIndex)
	{
		ASSERT(!m_SlotAvailability[clientIndex], "Cannot remove empty slot");

		m_SlotAvailability[clientIndex] = true;
		m_LatestTick[clientIndex] = 0;

		auto toBeRemoved = std::find_if(m_ClientSlots.begin(), m_ClientSlots.end(),
			[&index = clientIndex] (const ClientSlot& s) -> bool { return index == s.ID; });
		
		m_ClientSlots.erase(toBeRemoved);

		m_ActivePlayers--;
	}
	bool ClientHandler::IsActive(uint16 clientID) const
	{
		return !m_SlotAvailability[clientID];
	}
	const PlayerObject* ClientHandler::GetPlayerObject(uint16 clientID) const
	{
		if (!IsActive(clientID))
			return nullptr;
		return &m_PlayerObjectArray[clientID];
	}
	// Used to get addresses of players
	const std::vector<ClientSlot>& ClientHandler::GetClientSlots() const
	{
		return m_ClientSlots;
	}
	uint16 ClientHandler::FindAvailableSlotIndex() const
	{
		for (int i = 0; i < MAX_PLAYERS; ++i) {
			if (m_SlotAvailability[i]) {
				return i;
			}
		}
		return -1;
	}
}