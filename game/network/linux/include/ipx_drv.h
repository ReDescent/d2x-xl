/* $Id: ipx_drv.h,v 1.5 2003/10/12 09:17:47 btb Exp $ */
/*
 *
 * IPX driver interface
 *
 * parts from:
 * ipx[HA] header file for IPX for the DOS emulator
 * 		Tim Bird, tbird@novell.com
 *
 */

#ifndef _IPX_DRV_H
#define _IPX_DRV_H
#include <sys/types.h>

#define IPX_MANUAL_ADDRESS

#ifndef _WIN32
#define UINT_PTR uint32_t
#define INT_PTR int32_t
#endif

typedef struct IPXAddressStruct {
    uint8_t Network[4]; // __attribute__((packed));
    uint8_t Node[6]; // __attribute__((packed));
    uint8_t Socket[2]; // __attribute__((packed));
} IPXAddress_t;

typedef struct IPXPacketStructure {
    uint16_t Checksum; // __attribute__((packed));
    uint16_t Length; // __attribute__((packed));
    uint8_t TransportControl; // __attribute__((packed));
    uint8_t PacketType; // __attribute__((packed));
    IPXAddress_t Destination; // __attribute__((packed));
    IPXAddress_t Source; // __attribute__((packed));
} IPXPacket_t;

typedef struct ipx_socket_struct {
#ifdef DOSEMU
    struct ipx_socket_struct *next;
    far_t listenList;
    int listenCount;
    far_t AESList;
    int AESCount;
    uint16_t PSP;
#endif
    uint16_t socket;
    int16_t fd;
} ipx_socket_t;

class CPacketAddress : public CNetworkAddress {
    /* all network order */
    private:
    // tNetworkNode	src_addr;
    int32_t m_nType;
    uint16_t m_srcSocket;
    uint16_t m_destSocket;

    public:
    CPacketAddress() : m_nType(0), m_srcSocket(0), m_destSocket(0) {}
    CPacketAddress(tNetworkAddress &address) : CNetworkAddress(address), m_nType(0), m_srcSocket(0), m_destSocket(0) {}

    inline void SetType(int32_t nType) { m_nType = nType; }
    inline void SetSockets(uint16_t srcSocket, uint16_t destSocket) {
        m_srcSocket = srcSocket;
        m_destSocket = destSocket;
    }
    inline void GetSockets(uint16_t &srcSocket, uint16_t &destSocket) {
        srcSocket = m_srcSocket;
        destSocket = m_destSocket;
    }
    inline tNetworkAddress &Address(void) { return m_address; }

    inline bool operator==(CPacketAddress &other) {
        return !memcmp(&Address(), &other.Address(), sizeof(tNetworkNode));
    }
    inline CPacketAddress &operator=(CPacketAddress &other) {
        memcpy(this, &other, sizeof(*this));
        return *this;
    }
};

struct ipx_driver {
    int32_t (*GetMyAddress)();
    int32_t (*OpenSocket)(ipx_socket_t *, int32_t);
    void (*CloseSocket)(ipx_socket_t *);
    int32_t (*SendPacket)(ipx_socket_t *, IPXPacket_t *, uint8_t *, int32_t);
    int32_t (*ReceivePacket)(ipx_socket_t *, uint8_t *, int32_t, CPacketAddress *);
    int32_t (*PacketReady)(ipx_socket_t *s);
    void (*InitNetGameAuxData)(ipx_socket_t *, uint8_t[]);
    int32_t (*HandleNetGameAuxData)(ipx_socket_t *, const uint8_t[]);
    void (*HandleLeaveGame)(ipx_socket_t *);
    int32_t (*SendGamePacket)(ipx_socket_t *s, uint8_t *, int32_t);
};

int IPXGeneralPacketReady(ipx_socket_t *s);

extern CNetworkAddress ipx_MyAddress;

#endif /* _IPX_DRV_H */
