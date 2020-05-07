import socket
import keyboard
import pygame

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s.connect((socket.gethostname(),1234))


s.sendall(bytes("Hey bot!!", "utf-8"))
msg = s.recv(1024) #
print(msg.decode("utf-8"))

pygame.init()
pygame.display.set_mode((10,10))
# events = pygame.event.get()
while 1:
    # events = pygame.event.get()
    events = pygame.event.get()
    for event in events:
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_u:
                s.sendall(bytes("U_D","utf-8"))
            elif event.key == pygame.K_j:
                s.sendall(bytes("J_D","utf-8"))
            if event.key == pygame.K_i:
                s.sendall(bytes("I_D","utf-8"))
            elif event.key == pygame.K_k:
                s.sendall(bytes("K_D","utf-8"))
        if event.type == pygame.KEYUP:
            if event.key == pygame.K_u:
                s.sendall(bytes("U_U","utf-8"))
            if event.key == pygame.K_j:
                s.sendall(bytes("J_U","utf-8"))
            if event.key == pygame.K_i:
                s.sendall(bytes("I_U","utf-8"))
            if event.key == pygame.K_k:
                s.sendall(bytes("K_U","utf-8"))
