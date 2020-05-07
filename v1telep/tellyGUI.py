

import tkinter as tk
import telly
import socket


# def server_up():{
#     s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#     s.bind((socket.gethostname(),1234))
#
#     s.listen(5)
#
# }

def ServerRUN(s,tel):

    while True:
        clientsocket, address = s.accept()
        print(f"Connetion form {address} has been established!")
        clientsocket.send(bytes("Welcome to the Server!", "utf-8"))

        # while 1:
        #     msg = clientsocket.recv(1024)
        #     if msg:
        #         break
        #         print(msg.decode("utf-8"))

        print("listening for keys")
        clientsocket.send(bytes("Ready to play!", "utf-8"))

        while 1:
            msg = clientsocket.recv(1024)
            if msg:
                msg = msg.decode("utf-8")
                print(msg)
                if(msg == "U_D"):
                    tel.forwardL()
                if(msg == "I_D"):
                    tel.forwardR()
                if(msg == "J_D"):
                    tel.backwardL()
                if(msg == "K_D"):
                    tel.backwardR()
                if(msg == "U_U"):
                    tel.stopL()
                if(msg == "I_U"):
                    tel.stopR()
                if(msg == "J_U"):
                    tel.stopL()
                if(msg == "K_U"):
                    tel.stopR()


                msg =""


if __name__=='__main__':
    # server_up()
    IP = "127.0.0.1"
    PORT = 1234
    tel = telly.telly()
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # s.bind((socket.gethostname(),1234))
    s.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDER, 1)
    s.bind((IP,PORT))
    s.listen(5)

    ServerRUN(s,tel)
    # gui = tellyGUI()
    # gui.root.mainloop()
