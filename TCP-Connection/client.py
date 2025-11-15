import socket

# Create a TCP/IP socket
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect to the server
client_socket.connect(('127.0.0.1', 65432))

# Take user input
num1 = input("Enter first number: ")
num2 = input("Enter second number: ")

# Send data to server
client_socket.sendall(f"{num1} {num2}".encode())

# Receive acknowledgment
ack = client_socket.recv(1024).decode()
print(f"Acknowledgment: {ack}")

# Receive result
result = client_socket.recv(1024).decode()
print(f"Result from server: {result}")

# Close connection
client_socket.close()
