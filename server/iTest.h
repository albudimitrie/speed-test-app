#include <iostream>

class iTest
{
public:
    virtual ~iTest() = default;
    virtual void run(int client_socket) = 0;

};