# opensource

This is the simplest challenge. Not only is the protocol not as advanced, you get the source code too! When doing security tests in the real world, it's really great to get the source code to an application you're testing. This really speeds the test up, especially when you have a limited time to test and return results to the customer. (If a customers complains that this unrealistic, remind them that you only have weeks to test, while hacker might have weeks to years! Access to source code can be key to finding critical vulnerabilities. Still, sometimes, you may have to make do without source code. We'll get to that later...)

## What You'll Need

* A Linux system
* A code editor (I like [vscode](https://code.visualstudio.com/) or [Geany](https://www.geany.org/))

## Running the Server

1. Download the build binaries from the GitHub repo.
2. Run the binary with `./opensource`

The server will now be running on port 9999, listening only locally.

## Python Libraries

For these challenges, the two main libraries you'll use are `struct` and `socket`. `struct` provides a way for Python to write and read structured binary data, while `socket` provides low-level networking for Python. The first step would be to check out their respective Python documentation pages:

* [socket](https://docs.python.org/3/library/socket.html)
* [struct](https://docs.python.org/3/library/struct.html)

Not only is Python documentation great for being comprehensive, many times they give examples too. Don't be afraid to use these samples to get started. (I've done it on the job!)

## Getting Communications Up

To communicate with the challenge server, the basic steps are to:

1. Create socket
2. Connect to the server using the socket

It might look something like this:
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

Note that you tell how much data to read back from the server, and the server has to do the same for your data. This is important to know for network communications. Each side needs to know when the other is done sending data. This can be done through sending the size of the data as part of the protocol (what we're using here), or having a certain character or set of characters tell you when the other side is done (HTTP uses this, with a set of new-lines and carriage return characters indicating the end of the request/response: `\n\r\n\r`).

> If you're not getting a response from a server, and the server hasn't closed connection, it may be waiting for more data from you.

Looking at the code for `opensource`, you can tell it uses the "sending size" method. Craft your messages so that you tell the server right amount of data you're sending so you can get a response. Then code your Python tool to correctly get the size from the server and use it to read the response.

## Structured Data

When using the `struct` library, you'll "pack" data into structs to send to the server, and "unpack" data you get back.

```python
numbers_as_bytes = struct.pack("BB", 1, 1)
# unpack returns data as a tuple, even if just one value!
number_tuple = struct.unpack("BB", numbers_as_bytes)
```

I usually reference the library's documentation a lot, especially for the different sizes of data and their corresponding letters.

## Other Hints

* Don't forget about endianness of numbers. Most numbers, when put into a binary form, will be little endian (e.g. "backwards"). If you're looking at a number that seems too big or too small, try reversing the endianness of the number. (You probably won't use this in this challenge, but it's something to remember.)

## Exploiting the Server

When looking at source code for vulnerabilities, look for common coding mistakes or interesting functions (such as `system`, which runs commands).

* Look around the code for mistakes in writing data into static buffers. Perhaps you can get the server to crash?
* Look for functions that might give you cool functionality. Looks like this code calls commands `popen`, can you craft a message to run commands without any authentication.