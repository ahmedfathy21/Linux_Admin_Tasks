import socket 

# Create a TCP/IP socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Bind the socket to the address and port
server_socket.bind(('localhost', 65432))  # localhost and port 65432

server_socket.listen()
print("Server is waiting for connection...")

# Wait for a connection
conn, addr = server_socket.accept()
print(f"Connected by {addr}")

data = conn.recv(1024).decode()  # receive up to 1024 bytes
print(f"Received data: {data}")
conn.sendall(b"Data received")  # send acknowledgment

try:
    num1,num2 = map(float, data.split())
    result = num1 * num2
    conn.sendall(str(result).encode())
except ValueError:
    conn.sendall(b"Error: Please send two numbers separated by space.")

conn.close()
server_socket.close()