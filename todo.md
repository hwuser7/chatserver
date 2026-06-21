- [x] solve initialize to 0 issue with struct sockaddr_in 
```bash
main.cpp:25:23: error: missing field 'sin_port' initializer [-Werror,-Wmissing-field-initializers]
        sockaddr_in serv = {0};
```
```cpp
// change code to value-initialize
sockaddr_in serv = sockaddr_in();

// related 
// value-initialization in c++98
// default ctor
```



- [] make calls async