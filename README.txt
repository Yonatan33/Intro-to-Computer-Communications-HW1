Ofir Yoffe - 303166318
Yonatan Gartenberg - 311126205

Instructions:
1) Run Channel.c as specified in the assignment
2) Copy IPs & Ports
3) Run Sender.c with arguments copied
4) Run Receiver.c with arguments copied
5) Enter file name in Sender.c
6) Enter file name in Receiver.c
7) If you want to proceed:
   - Enter 'yes' in Channel.c
   - Go back to stage 5
8) Enter 'no' in Channel.c
9) Enter 'quit' in Sender.c
10) Enter 'quit' in Receiver.c

Code structure:
Every part (sender, channel, receiver) has a main loop:
- Sender loops while all the file hasn't been sent yet. A packet has 806 data bytes (divides with 26, 31).
- Channel loops while the sender socket is active. It receives a packet, adds noise and sends it to the receiver.
- Receiver loops while the channel socket is active. It receives a packet, decode it using hamming and save the data
  to the file.
Two additional files are:
- Common.h/Common.c - contain general utility functions and definitions.
- WinSock_handlers.c/WinSock_handlers.h - contain socket functions.

The code was written by us using examples from the official documentation of Winsock2:
https://docs.microsoft.com/en-us/windows/win32/winsock/windows-sockets-start-page-2

Bonus:
We've implemented an efficient operation of the random noise generation - we generate a random 32-bit number using rand_s() and use
the lower 16 bits for even indexes and upper 16-bit for odd indexes, doubling the Channel's efficiency - for every 2 bits we use 1 random generation.
Had rand_s() was off limits for any reason being, and the only random generation we could use was of 15 bits (0 - RAND_MAX), we would have implemented 2 random generations for the 1st bit, 
carrying the spare 14 bits to the next 14 generations, thus performing 16 random generations for every 15 bits (instead of 30), which is 47% more efficient. An example for this implementation is demonstrated in another branch of this project - https://github.com/Yonatan33/Intro-to-Computer-Communications-HW1/tree/Yonatan_branch

Limitations:
- As instructed, we assume inputs are valid.
- As instructed, we assume the input file size divides with 26.
