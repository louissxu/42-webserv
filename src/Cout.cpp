// #include "Cout.hpp"

// void Cout::output( const char *colour, const char *str, int outfd )
// {
// 	switch (outfd)
// 	{
// 	case 1:
// 		std::cout << colour << str << RESET << std::endl;
// 		break ;
// 	case 2:
// 		std::cerr << colour << str << RESET << std::endl;
// 		break ;
// 	default:
// 		std::cerr << RED << "Wrong file descriptor given" << RESET << std::endl;
// 		break;
// 	}
// }