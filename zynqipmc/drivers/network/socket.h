/*
 * This file is part of the ZYNQ-IPMC Framework.
 *
 * The ZYNQ-IPMC Framework is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The ZYNQ-IPMC Framework is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the ZYNQ-IPMC Framework.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SRC_COMMON_ZYNQIPMC_DRIVERS_NETWORK_SOCKET_H_
#define SRC_COMMON_ZYNQIPMC_DRIVERS_NETWORK_SOCKET_H_

#include "socket_address.h"
#include <string>
#include <lwip/sockets.h>
#include <libs/except.h>

#define SOCKET_DEFAULT_KEEPALIVE ///< All new sockets start with keep alive enabeld by default

/**
 * C++ socket wrapper with automatic cleanup.
 *
 * Simplifies the process of connecting to or hosting over-the-network services
 * without the need to deal with low-level interfaces and memory management.
 *
 * @note Check ServerSocket and ClientSocket as well.
 */
class Socket {
public:
	/**
	 * Creates a socket instance based upon an already existing
	 * socket file descriptor and sockaddr_in structure.
	 * Used for example after a call to ServerSocket::accept()
	 * @param socket socket file descriptor
	 * @param sockaddr address structure
	 */
	Socket(int socket, struct sockaddr_in sockaddr);

	/**
	 * Creates a new socket based on an address, port and TCP or UDP
	 * @param address The target address.
	 * @param port The port to use.
	 * @param useTCP true if TCP/IP, false if UDP/IP.
	 * @throws except::host_not_found if DNS fails for an address that is an URL.
	 */
	Socket(const std::string& address, unsigned short port, bool useTCP = true);
	virtual ~Socket();

	/**
	 * Receives data if present.
	 * @param ptr The data buffer.
	 * @param len The length of the buffer in buffer.
	 * @return The total number of read bytes.
	 */
	int recv(void *ptr, size_t len);

	/**
	 * Same as Socket::recv but with a custom timeout. For permanent timeout in
	 * all operation use Socket::setRecvTimeout instead and then the regular
	 * Socket::recv function.
	 * @param buf The data buffer.
	 * @param len The length of the buffer in buffer.
	 * @param timeout_ms Timeout in milliseconds.
	 * @return The total number of read bytes.
	 * @throws except::timeout_error if there was a timeout.
	 */
	int recv(void* buf, int len, unsigned int timeout_ms);

	/**
	 * Will receive the number of requested bytes or until there is a termination.
	 * @param ptr The data buffer.
	 * @param len Number of bytes available in buffer and quantity to read.
	 * @return Number of received bytes if positive or negative if error.
	 */
	int recvn(void *ptr, size_t len);

	/**
	 * Sends an array of characters to the client.
	 * @param buf The character buffer.
	 * @param len The starting position.
	 */
	int send(const void* buf, size_t len);

	/**
	 * Sends an array of characters to the client with a timeout set. This
	 * will override the timeout (if set) by Socket::setSendTimout.
	 * @param buf The character buffer.
	 * @param len The starting position.
	 * @param timeout_ms Timeout in milliseconds.
	 * @return The total number of read bytes.
	 * @throws except::timeout_error if there was a timeout.
	 */
	int send(const void* buf, size_t len, unsigned int timeout_ms);

	/**
	 * Sends a string to the client.
	 * @param str The string to send.
	 */
	int send(const std::string& str);

	/**
	 * Sends a string to the client with a timeout. This
	 * will override the timeout (if set) by Socket::setSendTimout.
	 * @param str The string to send.
	 * @param timeout_ms Timeout in milliseconds.
	 * @throws except::timeout_error if there was a timeout.
	 */
	int send(const std::string& str, unsigned int timeout_ms);

	void setBlocking(); ///< Sets the socket in blocking mode.
	void setNonblocking(); ///< Sets the socket in non-blocking mode.

	//! Sets the receiving timeout in milliseconds.
	void setRecvTimeout(uint32_t ms);

	//! Disables receiving timeout.
	inline void disableRecvTimeout() { setRecvTimeout(0); };

	//! Retrieve the current receive timeout configuration.
	inline uint32_t getRecvTimeout() { return this->recvTimeout; };

	//! Sets the transmitting timeout in milliseconds.
	void setSendTimeout(uint32_t ms);

	//! Disables transmitting timeout.
	inline void disableSendTimeout() { setRecvTimeout(0); };

	//! Retrieve the current transmit timeout configuration.
	inline uint32_t getSendTimeout() { return this->sendTimeout; };

	/**
	 * Enables no delay on TCP incoming packets.
	 * Useful when all incoming packets will be likely small in size.
	 */
	void enableNoDelay();
	void disableNoDelay(); ///< Disable no delay on TCP incoming packets.

	/**
	 * Enable keep alive packets.
	 * Allows detection of dropped connections and permits long lasting inactive connections.
	 */
	void enableKeepAlive();
	void disableKeepAlive(); ///< Disable keep alive packets.

	//! Closes the socket connection
	void close();

	//! Returns true if the socket is valid.
	inline bool isValid() { return socketfd != -1; }

	/**
	 * Checks if the socket is configured for TCP/IP operation,
	 * if false it means it is UDP
	 * @return true if TCP/IP socket, UDP/IP otherwise.
	 */
	bool isTCP();

	//! Returns the socket file descriptor.
	inline int getSocket() { return socketfd; }

	/**
	 * Gets the socketaddress instance of the socket, which contains
	 * information about the socket's address and port
	 * @return The socketaddress instance.
	 */
	inline const SocketAddress& getSocketAddress() { return sockaddr; }

	//! Operator overload for int assignment, returns socketfd
	inline operator int() { return this->getSocket(); }

protected:
	int socketfd;				///< The socket file descriptor number.
	SocketAddress sockaddr;		///< The socket address and port.
	unsigned int recvTimeout;	///< Receive timeout in ms.
	unsigned int sendTimeout;	///< Transmit timeout in ms.
};

#endif /* SRC_COMMON_ZYNQIPMC_DRIVERS_NETWORK_SOCKET_H_ */