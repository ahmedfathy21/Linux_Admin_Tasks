# Inter-Process Communication (IPC) - TCP Socket Implementation

## 📋 Project Overview

This project demonstrates **Inter-Process Communication (IPC)** using **TCP sockets** in Python. The implementation consists of a client-server architecture where the client sends two numbers to the server, and the server performs multiplication and returns the result.

---

## 🔍 Key Concepts

### 1. Inter-Process Communication (IPC)

**IPC** is a mechanism that allows processes to communicate with each other and synchronize their actions. Processes can be running on the same machine or across different machines.

#### Types of IPC:
- **Pipes** - Unidirectional communication
- **Message Queues** - Asynchronous message passing
- **Shared Memory** - Direct memory access (fastest)
- **Sockets** - Network-based communication (used in this project)
- **Signals** - Event notification
- **Semaphores** - Synchronization

### 2. Socket Programming

A **socket** is an endpoint for sending or receiving data across a network. Think of it as a "phone connection" between two processes.

#### Socket Types:
- **TCP (SOCK_STREAM)** - Connection-oriented, reliable (used here)
- **UDP (SOCK_DGRAM)** - Connectionless, faster but less reliable

### 3. TCP Connection Process

TCP uses a **three-way handshake** to establish a connection:

```
CLIENT                           SERVER
  |                                |
  |-------- SYN ------------------>|  (Synchronize)
  |                                |
  |<------- SYN-ACK --------------|  (Synchronize-Acknowledge)
  |                                |
  |-------- ACK ------------------>|  (Acknowledge)
  |                                |
  |===== CONNECTION ESTABLISHED ===|
```

---

## 🏗️ Architecture

### Visual Flow Diagram

```
┌─────────────────┐                    ┌─────────────────┐
│     CLIENT      │                    │     SERVER      │
│   (client.py)   │                    │   (server.py)   │
└────────┬────────┘                    └────────┬────────┘
         │                                      │
         │ 1. Create Socket                     │ 1. Create Socket
         │    socket.socket()                   │    socket.socket()
         │                                      │
         │                                      │ 2. Bind to Address
         │                                      │    bind(localhost, 65432)
         │                                      │
         │                                      │ 3. Listen for Connections
         │                                      │    listen()
         │                                      │
         │ 4. Connect to Server                 │
         │────────────────────────────────────> │
         │         connect(127.0.0.1:65432)     │
         │                                      │ 5. Accept Connection
         │<──────────── Connection OK ──────────│    accept()
         │                                      │
         │ 6. Send Data: "5 10"                 │
         │═════════════════════════════════════>│
         │                                      │ 7. Receive & Decode
         │                                      │    recv(1024)
         │                                      │
         │ 8. Receive ACK                       │ Send ACK: "Data received"
         │<═════════════════════════════════════│
         │    Display: "Data received"          │
         │                                      │ 9. Calculate: 5 * 10 = 50
         │                                      │
         │ 10. Receive Result                   │ Send Result: "50.0"
         │<═════════════════════════════════════│
         │    Display: "Result: 50.0"           │
         │                                      │
         │ 11. Close Connection                 │ 12. Close Connection
         │    close()                           │     close()
         │                                      │
```

---

## 💻 Implementation Details

### Server Side (`server.py`)

**Key Operations:**
1. **Socket Creation**: `socket.socket(socket.AF_INET, socket.SOCK_STREAM)`
   - `AF_INET`: IPv4 address family
   - `SOCK_STREAM`: TCP socket type

2. **Binding**: `bind(('localhost', 65432))`
   - Binds socket to IP address and port
   - Port 65432: Custom port number (above 1024 to avoid privileged ports)

3. **Listening**: `listen()`
   - Puts server in listening mode
   - Waits for incoming connections

4. **Accepting**: `accept()`
   - Blocks until a client connects
   - Returns connection object and client address

5. **Data Processing**:
   ```python
   data = conn.recv(1024).decode()  # Receive up to 1024 bytes
   num1, num2 = map(float, data.split())  # Parse numbers
   result = num1 * num2  # Perform calculation
   conn.sendall(str(result).encode())  # Send result back
   ```

### Client Side (`client.py`)

**Key Operations:**
1. **Socket Creation**: Same as server

2. **Connection**: `connect(('127.0.0.1', 65432))`
   - `127.0.0.1`: Loopback address (localhost)
   - Initiates connection to server

3. **Sending Data**: `sendall(f"{num1} {num2}".encode())`
   - Converts string to bytes
   - Sends reliably (ensures all data is sent)

4. **Receiving Data**: 
   ```python
   ack = recv(1024).decode()  # First receive: acknowledgment
   result = recv(1024).decode()  # Second receive: calculation result
   ```

---

## 🔄 Communication Protocol

Our custom protocol works as follows:

1. **Client → Server**: Two space-separated numbers (e.g., "5 10")
2. **Server → Client**: Acknowledgment ("Data received")
3. **Server → Client**: Multiplication result (e.g., "50.0")

### Why Two recv() Calls?

The client must call `recv()` **twice** because the server sends **two separate messages**:
- **First message**: Acknowledgment
- **Second message**: Result

Each `recv()` call reads one message from the TCP stream.

---

## 🚀 How to Run

### Step 1: Start the Server

```bash
python3 server.py
```

**Expected Output:**
```
Server is waiting for connection...
```

### Step 2: Run the Client (in a new terminal)

```bash
python3 client.py
```

**Example Interaction:**
```
Enter first number: 5
Enter second number: 10
Acknowledgment: Data received
Result from server: 50.0
```

### Step 3: Server Output

```
Server is waiting for connection...
Connected by ('127.0.0.1', 54321)
Received data: 5 10
```

---

## 🛠️ Technical Details

### Port Number: 65432
- **Range**: 0-65535
- **0-1023**: Well-known ports (HTTP: 80, HTTPS: 443)
- **1024-49151**: Registered ports
- **49152-65535**: Dynamic/Private ports
- **65432**: Our custom port (in dynamic range)

### Loopback Interface (127.0.0.1)
- Special IP address that refers to "this computer"
- Data never leaves the machine
- Used for local testing
- Also known as "localhost"

### Buffer Size (1024 bytes)
- Maximum amount of data to read at once
- Sufficient for our small messages
- Larger applications may need bigger buffers

### TCP Features Used:
- ✅ **Reliable delivery** - Guaranteed data delivery
- ✅ **Ordered delivery** - Data arrives in order sent
- ✅ **Error checking** - Built-in checksums
- ✅ **Connection-oriented** - Established before data transfer

---

## 🐛 Common Issues & Solutions

### Issue 1: "Address already in use"
**Cause**: Server port still bound from previous run

**Solution:**
```bash
# Find process using port
lsof -i :65432

# Kill the process
kill -9 <PID>
```

Or add to server code:
```python
server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
```

### Issue 2: Client shows "Data received" instead of result
**Cause**: Client only calls `recv()` once

**Solution**: Call `recv()` twice to get both acknowledgment and result (already implemented)

### Issue 3: Connection refused
**Cause**: Server not running or wrong IP/port

**Solution**: Ensure server is running first and IP/port match

---

## 📚 Learning Outcomes

After completing this project, you understand:

1. ✅ How TCP socket communication works
2. ✅ Client-server architecture patterns
3. ✅ Inter-process communication concepts
4. ✅ Network programming basics in Python
5. ✅ Data encoding/decoding (bytes ↔ strings)
6. ✅ Blocking I/O operations
7. ✅ Connection lifecycle management

---

## 🔧 Possible Enhancements

1. **Multi-client Support**: Handle multiple clients simultaneously using threading
2. **More Operations**: Add subtraction, division, etc.
3. **Error Handling**: Better validation and error messages
4. **Persistent Server**: Keep server running for multiple requests
5. **Encryption**: Add SSL/TLS for secure communication
6. **GUI Interface**: Create a graphical user interface
7. **Logging**: Add detailed logging for debugging

---

## 📖 References

- [Python Socket Documentation](https://docs.python.org/3/library/socket.html)
- [TCP/IP Protocol Suite](https://www.rfc-editor.org/rfc/rfc793)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)

---

## 👨‍💻 Author

**Project**: IPC with TCP Sockets  
**Language**: Python 3  
**Topic**: Operating Systems & Network Programming

---

## 📝 License

This is an educational project for learning Linux administration and network programming concepts.
