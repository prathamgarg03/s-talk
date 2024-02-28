# s-talk
s-talk is a simple chat-like facility that enables communication between users on different terminals. It allows users to exchange messages in real-time over a network connection.

## Getting Started

To initiate an s-talk session, follow these steps:

1. Agree with the other user(s) on:
   - The machine each user will be running on
   - The port number each user will use

2. Start the s-talk session by running the following command:

```
s-talk [my port number] [remote machine name] [remote port number]
```

For example:

- If Fred is on machine "csil-cpu1" and will use port number 6060, and Barney is on machine "csil-cpu3" using port number 6001:
  - Fred should type: `s-talk 6060 csil-cpu3 6001`
  - Barney should type: `s-talk 6001 csil-cpu1 6060`

## Functionality

- Characters will appear character-by-character on the sender’s terminal as they type it.
- Messages will appear on the receiver’s terminal once the sender presses enter.
- An s-talk session is terminated when either user enters the complete line of text consisting of just one ‘!’ and presses enter.

## Project Structure

This project is implemented using pthreads, a kernel-level thread implementation for LINUX.

### Required Threads (in each process):

1. **Keyboard Input Thread**: Awaits input from the keyboard and adds it to the list of messages to be sent to the remote s-talk client.
2. **UDP Datagram Thread**: Awaits UDP datagrams.
3. **Screen Output Thread**: Prints characters to the screen.
4. **UDP Output Thread**: Sends data to the remote UNIX process over the network using UDP.

All threads share access to a list ADT, carefully controlled to prevent concurrent access using mutexes and signaling/waiting on condition variables.
