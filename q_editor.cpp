#include <fstream>
#include <iostream>
#include <regex>


int main() {
	std::ofstream file("questions.txt", std::ios_base::out | std::ios_base::app);
	if(!file.is_open()) {std::cerr << "Had trouble syncing to file...\n"; return 1;}

	const std::regex reg("(.+),(.+),(.+),(.+)."); // J'aime les regex

	for(;;) {
		std::string line;
		std::cout << "Veuillez insérer une question." << std::endl;
		std::cin >> line;

		if(line == std::string("")) break;

		std::smatch match;

		if(!std::regex_match(line, match, reg)) {
			std::cout << "Il semblerait que le format de votre entrée soit incorrect : il doit être de la forme \"Q,RC,RF,RF,RF\"\n";
		} else {
			file << line << std::endl;
		}
	}

	file.close();
	return 0;
}