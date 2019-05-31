
#include <SFML/Graphics.hpp>

class Backend {
public:
	Backend(std::string filename, int HP_penalty = 7, int HP_gain = 3);
	void enterPlayLoop();
protected:
	struct Joueur {
		int HP = 40;
	} joueur[2];

	int m_HP_penalty;
	int m_HP_gain;

	struct Question {
		std::string prompt;
		std::array<std::string, 4> reponses; // La 1ere est toujours la bonne
	};

	std::vector<Question> question_pool;
};

Backend::Backend(std::string filename, int HP_penalty, int HP_gain) : m_HP_penalty(HP_penalty), m_HP_gain(HP_gain) {
	std::ifstream q_file(filename);

	if(!q_file.is_open()) {throw std::runtime_error("Couldn't attach to question file");}

	while(!q_file.eof()) {
		// Pas de validation d'entrée car on est des Pwnz0r
		char buf[255];
		q_file.getline(&buf[0], 255);
		std::string line(&buf[0]);

		Question q;

		auto comma_pos = line.find(",");
		q.prompt = line.substr(0, comma_pos);

		for(int i = 0; i < 4; ++i) {
			line.erase(0, comma_pos + 1);
			comma_pos = line.find(",");
			q.reponses[i] = line.substr(0, comma_pos);
		}

		//std::cout << q.prompt << '\n' << q.reponses[0] << '\n' << q.reponses[1] << '\n' << q.reponses[2] << '\n' << q.reponses[3] << '\n';
		//std::cout << "-----------------------------------------------------------" << std::endl;
	}
}

void Backend::enterPlayLoop(sf::RenderWindow& rw, sf::Sprite& bg) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, question_pool.size() - 1); // Random index in question pool

    bool curr_player = false;

	int stop = 0;
	while(stop == 0) {
		int question_ID = dist(rng);
		
		std::array<std::string, 4> rep = question_pool[question_ID].reponses;
		std::random_shuffle(rep.begin(), rep.end(), rng);

		// rep contient les reponses mélangées.

		int correct_answer = 0;

		for(;correct_answer < 4; ++correct_answer) {
			if(rep[correct_answer] == question_pool[question_ID].reponses[0]) break;
		}

		stop = playing(rw, bg, sf::String(question_pool[question_ID].prompt), rep, correct_answer, curr_player);

		curr_player = !curr_player;
	}
}