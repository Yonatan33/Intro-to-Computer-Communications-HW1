Ofir Yoffe - 303166318
Yonatan Gartenberg - 311126205

Instructions:
1) Run Channel.c
2) Copy IPs & Ports
3) Run Sender.c with arguments
4) Run Receiver.c with arguments
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
We've implemented an efficient operation of the noise generation - we random a 32-bit number using rand_s() and use
the lower 16-bit for even indexes and upper 16-bit for odd indexes (making the Channel as twice as efficient).

Limitations:
- As instructed, we assume inputs are valid.
- As instructed, we assume the input file size divides with 26.