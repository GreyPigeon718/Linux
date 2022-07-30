#include "select_server.hpp"

using namespace ns_select;

int main()
{
    SelectServer* svr = new SelectServer();
    svr->InitSelectServer();
    svr->Loop();

    // std::cout << sizeof(fd_set) << std::endl;

    return 0;
}