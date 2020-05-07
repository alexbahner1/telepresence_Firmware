import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((socket.gethostname(),1234))

s.listen(5)

while True:
    clientsocket, address = s.accept()
    print(f"Connetion form {address} has been established!")
    clientsocket.send(bytes("Welcome to the Server!", "utf-8"))

    while 1:
        msg = clientsocket.recv(1024)
        if msg:
            break
    print(msg.decode("utf-8"))

    print("listening for keys")
    clientsocket.send(bytes("Ready to play!", "utf-8"))

    while 1:
        msg = clientsocket.recv(1024)
        if msg:
            print(msg.decode("utf-8"))
            msg =""
    # print(msg.decode("utf-8"))
