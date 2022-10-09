# Network Hacking with Python

This repo contains code for learning how to use Python to discover, implement, and exploit custom network protocols. This is based off of real-world scenarios I've found while working in cybersecurity. You can download the binaries from the Releases section.

## What's with .base64? Where's the Code?

For most challenges, except for "opensource" obviously, part of the challenge is figuring out the protocol without access to source. So that nobody gets tempted, the code for those challenges is encrypted. If you really need it, the key is available upon request.

## Challenges

### opensource

With this challenge, the source code is available to you. This is the most basic challenge.

You'll learn:

* Reading source code to determine the network protocol
* Using Python's `socket` and `struct` libraries to reconstruct and interact with the protocol
* Attacking the network service through buffer overflows and command injection

### messages

With this challenge, the binary produces detailed log messages you can use to figure out the protocol. 

You'll learn:

* Using the binary's logging messages to work out the protocol
* Using Python to loop and brute-force protocol elements 


