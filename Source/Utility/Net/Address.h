#pragma once
#include <Macros.h>
#include "Net.h"


namespace Skel::Net
{
	enum AddressType
	{
		ADDRESS_UNDEFINED,
		ADDRESS_IPV4,
		ADDRESS_IPV6
	};

	class Address
	{
		AddressType m_type;

		union
		{
			uint32_t m_address4;
			uint16_t m_address6[8];
		};

		uint16_t m_port;

	public:

		Address();

		Address(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint16_t port = 0);

		explicit Address(uint32_t address, int16_t port = 0);

		explicit Address(uint16_t a, uint16_t b, uint16_t c, uint16_t d,
			uint16_t e, uint16_t f, uint16_t g, uint16_t h,
			uint16_t port = 0);

		explicit Address(const uint16_t address[], uint16_t port = 0);

		explicit Address(const sockaddr_storage& addr);

		explicit Address(const sockaddr_in6& addr_ipv6);

		explicit Address(addrinfo* p);

		explicit Address(const char* address);

		explicit Address(const char* address, uint16_t port);

		void Clear();

		uint32_t GetAddress4() const;

		const uint16_t* GetAddress6() const;

		const sockaddr_in GetSockAddr4() const;

		const sockaddr_in6 GetSockAddr6() const;

		void SetPort2(uint64_t port);

		uint16_t GetPort() const;

		AddressType GetType() const;

		const char* ToString(char buffer[], int bufferSize) const;

		bool IsValid() const;

		bool operator ==(const Address& other) const;

		bool operator !=(const Address& other) const;

	protected:

		void Parse(const char* address);
	};
}