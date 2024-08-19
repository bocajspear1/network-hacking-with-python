---
layout: default
title: opensource
---

This is the simplest challenge. Not only is the protocol not as advanced, you get the source code too! When doing security tests in the real world, it's really great to get the source code to an application you're testing. This really speeds the test up, especially when you have a limited time to test and return results to the customer. (If a customers complains that this unrealistic, remind them that you only have weeks to test, while hacker might have weeks to years! Access to source code can be key to finding critical vulnerabilities. Still, sometimes, you may have to make do without source code. We'll get to that later...)

## What You'll Need

* A Linux system
* A code editor (I like [vscode](https://code.visualstudio.com/) or [Geany](https://www.geany.org/))
* [Source code](https://github.com/bocajspear1/network-hacking-with-python/blob/main/opensource.c)

## Running the Server

1. Download the build binaries from the [GitHub repo](https://github.com/bocajspear1/network-hacking-with-python/releases/tag/latest).
2. Run the binary with `./opensource`

The server will now be running on port 9999, listening only locally.

## Python Libraries

For these challenges, the two main libraries you'll use are `struct` and `socket`. `struct` provides a way for Python to write and read structured binary data, while `socket` provides low-level networking for Python. The first step would be to check out their respective Python documentation pages:

* [socket](https://docs.python.org/3/library/socket.html)
* [struct](https://docs.python.org/3/library/struct.html)

Not only is Python documentation great for being comprehensive, many times they give examples too. Don't be afraid to use these samples to get started. (I've done it on the job!)

Don't forget you'll need to import them like this at the top of your source file:

```python
import socket
import struct
```

## Getting Communications Up

To communicate with the challenge server, the basic steps are to:

1. Create socket
2. Connect to the server using the socket, it might look something like this:
```python
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(("<ADDR>", <PORT>))
```

3. Send data with `send` method:

```python
sock.send(b"data")
```

4. Receive data with `recv`
```python
BYTES_TO_READ=5
data = sock.recv(5)
```

## Structured Data

When using the `struct` library, you'll "pack" data into structs to send to the server, and "unpack" data you get back.

```python
numbers_as_bytes = struct.pack("BB", 1, 1)
# unpack returns data as a tuple, even if just one value!
number_tuple = struct.unpack("BB", numbers_as_bytes)
```

I usually reference the library's documentation a lot, especially for the different sizes of data and their corresponding letters.

## Interacting with Servers

Note that you tell how much data to read back from the server, and the server has to do the same for your data. This is important to know for network communications. Each side needs to know when the other is done sending data. This can be done through sending the size of the data as part of the protocol or having a certain character or set of characters tell you when the other side is done (HTTP uses this, with a set of new-lines and carriage return characters indicating the end of the request/response: `\n\r\n\r`).

> If you're not getting a response from a server, and the server hasn't closed connection, it may be waiting for more data from you.

Looking at the code for `opensource`, you can tell it uses the "sending size" method. You will need to craft your messages so that you tell the server right amount of data you're sending so you can get a response. Then code your Python tool to correctly get the size from the server and use it to read the response.

## Getting Our Script Started

In our case, you're most likely connecting to your local system, so when building the connection, you'll use the loopback address of `127.0.0.1` and port 9999. This means our script will look like this:

```python
import socket
import struct

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(("127.0.0.1", 9999))
```

If we wanted to send the bytes "hello!" to the server (note the `b` in front of the string), then try to get back 10 bytes, we'd add the following:

```python
sock.send(b"hello!")
data = sock.recv(10)
```

This won't get us anywhere, as it doesn't fit the protocol. Looking at the code, we can see that it looks for a size. The size is stored in 16 bits (2 bytes), then the rest of message follows. It tries to read the exact size sent it. (Could that be dangerous? hmmmm...) We need to send this length for it to properly parse our message. To do this, we use `struct.pack`.

```python
data = b"hello"
sock.send(struct.pack("<H", len(data)) + data)
```

By giving it `<H` we tell it to convert the integer length of `data` to 2 bytes (a half-long) in little-endian format. If we send this message, the server should show the right size, but nothing happens. Investigate the code to see how it parses the data it gets.

> To write bytes manully in Python, you can put `\xYY`, where `YY` is the hexidecimal value of the byte you want to send. For example, `\xff` sends 255 in hex.

When we get a response from the server, it will use the same format as we sent. To unpack that length, we need to read the 2 bytes, then unpack it with `struct.unpack`.

```
size = struct.unpack("<H", sock.recv(2))[0]
```

We use `[0]` since `unpack` always returns a tuple, so we want to select the first item in that tuple. Once we parse the size, we can read that amount of data to get the response.

```
data = sock.recv(size)
```

Looking at the code, it may do this multiple times so craft your script to work with this.

You should now have the groundwork to exploit the server!

## Exploiting the Server

When looking at source code for vulnerabilities, look for common coding mistakes or interesting functions (such as `system`, `exec` or `popen`, which run commands).

* Look around the code for mistakes in writing data into static buffers. Perhaps you can get the server to crash?
* Look for functions that might give you cool functionality. Looks like this code calls commands `popen` to run the `du` command that gives usage output of the provided directory. Can you craft a message to inject arbitrary commands?
* Size fields can be manipulated to make the servers do unexpected things. What happens if you lie about the size of your message?

## Other Hints

* Don't forget about endianness of numbers. Most numbers, when put into a binary form, will be little endian (e.g. "backwards"). If you're looking at a number that seems too big or too small, try reversing the endianness of the number. (You probably won't use this in this challenge, but it's something to remember.)