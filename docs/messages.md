# Messages

With this challenge, you don't get the source code. You'll need another source of data to determine what effect your communications to the server are having.

## What You'll Need

* A Linux system
* A code editor (I like [vscode](https://code.visualstudio.com/) or [Geany](https://www.geany.org/))

## Running the Server

1. Download the build binaries from the GitHub repo.
2. Run the binary with `./messages`

The server will now be running on port 9999, listening only locally.

## Getting Our Information

If you send some data to this new server, you'll not it gives a lot of log messages. Log messages are a great source of information about how the application is responding to you data. This is great even if you have source code, as it also will show you where in the code your attacks have made the application go.

For this challenge, use the log messages to determine the protocol. You'll have to figure values out by brute forcing them, looping until you find a value that produces a different response or a response you want.

## Other Hints

* 

## Exploiting the Server

When looking logs vulnerabilities, look for logs that indicate interesting functionality, such as "command" or "execute".

* This challenge also has a way to execute commands through the server, but you'll have to brute force your way there.