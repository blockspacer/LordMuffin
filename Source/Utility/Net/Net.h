#pragma once

#include <WinSock2.h>
#include <ws2ipdef.h>
#include <WS2tcpip.h>

namespace Skel::Net
{
	class Address;
	inline constexpr uint16	 MAX_PLAYERS = 8;
	// Not const because ImGui editable
	inline			float	 FAKE_LAG_S = 0.6f; //seconds
	inline			float	 FAKE_JITTER_S = 0.1f; //seconds
	inline			float	 FAKE_PACKET_LOSS = 0.3f; //seconds


	inline constexpr uint16	 SYNC_SAMPLES = 30; // how many sync packets are sent out
	inline constexpr uint8	 SNAPSHOT_PER_SEC = 20; // how many snapshot packets are sent out

	inline constexpr double	 SNAPSHOT_RATE = 1.0 / double(SNAPSHOT_PER_SEC);

	void Init();
	Address GetServerAddress();
}