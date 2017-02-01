import socket

def main():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(('127.0.0.1', 1234))
    while (1):
        msg = raw_input("> ")
        s.send(msg)
        echo = s.recv(1024)
        print(echo)


if __name__ == "__main__":
    main()
