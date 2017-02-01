proxy for hanging mpi socket server.

serial client send messages over a socket to a parallel mpi server.
only master rank binds, sends, and receives. bcasts to other ranks.

term 1:
    > make pserver
    > mpirun -n 10 pserver
    listening for client
    got connection: [127.0.0.1, 59797]
    0 - o wat up
    8 - o wat up
    1 - o wat up
    2 - o wat up
    4 - o wat up
    6 - o wat up
    3 - o wat up
    5 - o wat up
    9 - o wat up
    7 - o wat up

term 2:
    > python client.py
    > o wat up
    o wat up
