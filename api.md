# api

## makeSockNonBlocking

```cpp
int makeSockNonBlocking(
    int sock                // socket filde descriptor
);
```

Takes a socket and change it into a non blocking socket.

**RETURN**

` 0`: success
`-1`: failure



## setSockOptReuse

```cpp
int setSockOptReuse(
    int sock                // socket filde descriptor
);
```

Set socket's reuse option on. Default value is off and you need to wait
OS to release port to use between short time intervals.

**RETURN**

` 0`: success
`-1`: failure



## bindSock

```cpp
int bindSock(
    int sock                // socket filde descriptor
);
```

Bind listening socket to give ip port and other attributes
to the socket.

**RETURN**

` 0`: success
`-1`: failure



## startListeningSock

```cpp
int startListeningSock(
    int *mSock              // main socket pointer
);
```

Start to listening socket mSock is return parameter which will be main socket.

**RETURN**

` 0`: success
`-1`: failure



## acceptConnection

```cpp
int acceptConnection(
    std::vector<pollfd>& fds // socket storage for connections
);
```

When there are a pending events on listening socket
accept them and add them to the socket storage (fds)

**RETURN**

` 0` or `>0`: success (new socket number)
`-1`        : failure




## checkRevents

```cpp
int checkRevents(
    std::vector<pollfd>& fds // socket storage for connections
);
```

Check revents masks of sockets for:
- new connections (adds to fds)
- errors          (removes socket from fds)
- new event on existing socket (#todo)

**RETURN**

` 0`: success
`-1`: failure


## startPolling

```cpp
int startPolling(
    int sock        // listening socket
);
```

After creating a listening socket it is time to start polling
This function uses `poll` system function to listens sockets without blocking
and busy waiting

**RETURN**

` 0`: success
`-1`: failure
