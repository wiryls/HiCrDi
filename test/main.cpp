
/* headers */
#include <libheartbeat/libheartbeat.hpp>
#include <iostream>

/* libraries */
#if _MSC_VER > 1000
#ifdef _DEBUG
#pragma comment(lib, "../Debug/libheartbeat.lib")
#else
#pragma comment(lib, "../Release/libheartbeat.lib")
#endif
#endif

/* function */


int main(void)
{
	std::cout << "ver:" << hb::version() << std::endl;

	//hb::Heart *   heart = hb::i_come();
	//hb::i_see    (heart, nullptr, 0U, 0U);
	//hb::i_sleep(heart);
	//heart = nullptr;

	hb::test(nullptr, nullptr, 0, 0);

	return 0;
}
