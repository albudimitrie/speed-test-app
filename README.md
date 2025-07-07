# speed-test-app

## TCP Test Components

### 1. Latency (TCP)
This test measures the time it takes for a message to complete a round trip from client → server → client:

- The client sends a message to the server.
- The server immediately replies.
- The total response time is recorded.

The test is repeated multiple times:
- **Final latency** is the lowest measured value.
- **Jitter** is the variation between successive response times.

---

### 2. TCP Download (Server -> Client)

- The client establishes one or more TCP connections to the server.
- The server sends fixed-size data chunks to the client for the configured duration.
- The client measures:
  - Total amount of data received
  - Total transfer time
  - Average download speed (MBps)

---

### 3. TCP Upload (Client -> Server)

- The client sends data to the server over a TCP connection.
- The server receives the data and calculates:
  - Total received data size
  - Total time taken
  - Average upload speed

---

## UDP Test Components

### 1. UDP Upload (Client -> Server)

- The client sends UDP packets to the server at a fixed rate (e.g., 1 MBps).
- Each packet contains:
  - Unique ID
  - Timestamp of sending

The server calculates:
- Number of lost packets
- Jitter
- Effective upload speed

---

### 2. UDP Download (Server -> Client)

- The server sends UDP packets to the client at the configured rate.
- The client tracks:
  - Number of packets received
  - Lost packets (missing IDs)
  - Jitter
  - Average download speed

---

### 3. UDP Latency

- A small UDP packet is sent and a reply is awaited (ping-pong).
- The client measures:
  - RTT (Round-Trip Time)
  - Timeout if no reply is received within the limit
  - Jitter

---

## Disk I/O Test

- A temporary file of configurable size (e.g., 100 MB) is created.
- The test measures:
  - Write time → write speed
  - Read time → read speed

The file is automatically deleted after the test.

---

## Application Structure

### Server

- Listens on dedicated TCP and UDP ports.
- Handles each client in a separate thread.
- Collects and sends statistics for each test.

### Client GUI/CLI (still thinking about it)

- Graphical interface using Tkinter / or command-line based

- Displays real-time results of each test.

---

## Notes

- The application simulates the behavior of an online speed test.
- Tests can be run independently (upload/download/ping/disk).
