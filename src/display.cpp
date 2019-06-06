#include "easing.h"


#include <cstdlib>
#include <algorithm>
#include <string>
#include <array>
#include <cmath>
#include <vector>
#include <fstream>
#include <exception>
#include <iostream>
#include <random>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>

// ---- INIT ---- (add this to the header)
  class guiSelect{
    public:
      guiSelect();
      bool draw(sf::RenderWindow& src);
      sf::Vector2f position(sf::Vector2f position);
      bool type(bool type);
      bool change(bool direction);
      unsigned int enter();
      bool displaying = false;
      std::vector<std::string> choices;
    private:
      sf::Font fontDeja;
      sf::Texture barTxt, smallBarTxt;
      sf::Sprite bar, smallBar, barSelected, smallBarSelected;

      bool valType;
      sf::Vector2f valPosition;
      unsigned int iter, choicePos;
  };

  class Character{
    public :
      Character(bool player);
      bool display(sf::RenderWindow& scr);
      bool animate(std::string animationType);
      bool turn(bool t);
      bool failAttack(bool f);
      bool getFailAttack();
      bool isNotFinished();
    private :
      bool orientation,ended, active, animFinished, animFail;
      sf::Texture defaultTxt, capeTxt, animAtxt, animBtxt, animCtxt, animDtxt, finalTxt, arrowTxt ;
      sf::Sprite sprite, cape, arrow;
      std::string animationType;
      unsigned int iter, animCape, animNum;
      sf::Clock clock;
      sf::Time animStart;
  };
    
  int playing(sf::RenderWindow& window, sf::Sprite bg, sf::String questionTitle, std::vector<std::string> choicesR, unsigned int correctAnswer, bool player);
  sf::String toSfString(std::string theStdString);
  std::vector<std::string> split(std::string string, char search);

  class Backend {
    public:
      Backend(std::string filename, int HP_penalty = 7, int HP_gain = 3);
      void enterPlayLoop(sf::RenderWindow& rw, sf::Sprite& bg);
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

      std::vector<int> order_pool;
      int return_random_q();
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

    question_pool.push_back(q);
  }

  for(int i = 0; i < question_pool.size(); ++i) {
    order_pool.push_back(i);
  }
}

int Backend::return_random_q() {
  static int index = order_pool.size();

  if(index == order_pool.size()) {
      std::random_device dev;
      std::mt19937 rng(dev());
  
      std::shuffle(order_pool.begin(), order_pool.end(), rng);
      index = 0;
  }

  auto retval = order_pool[index];
  ++index;
  return retval;

}

void Backend::enterPlayLoop(sf::RenderWindow& rw, sf::Sprite& bg) {
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<int> dist(0, question_pool.size() - 1); // Random index in question pool
  bool curr_player = true;
  int retval = 0, i(0);
  sf::Font font;
  font.loadFromFile("res/DejaVuSans.ttf");
  std::string dialog = "";
  sf::Text text(dialog,font,30), criticTxt("",font, 50);
  sf::SoundBuffer buffer;
  sf::Sound criticHP;
  buffer.loadFromFile("res/criticHP.wav");
  criticHP.setBuffer(buffer);
  criticHP.setLoop(true);

  while(retval != -1) {
    //init
      int question_ID = return_random_q();
      i++;
      std::array<std::string, 4> rep = question_pool[question_ID].reponses;
      std::shuffle(rep.begin(), rep.end(), rng);
      dialog = " --- TOUR N°"+std::to_string(i)+" ---\n\n";
    // rep contient les reponses mélangées.
      int correct_answer = 0;
      for(;correct_answer < 4; ++correct_answer)
        if(rep[correct_answer] == question_pool[question_ID].reponses[0]) break;
      std::vector<std::string> rep_vec;
      for(auto a : rep) 
        rep_vec.push_back(a);
    //Jeu
      sf::Clock t;
      retval = playing(rw, bg, toSfString(question_pool[question_ID].prompt), rep_vec, correct_answer, curr_player);
      int delta_t = t.getElapsedTime().asSeconds()-7.5;
      switch(retval) {
        case 0:
          return;
          break;
        case 1: {
          const int n = std::ceil(7 + delta_t * 0.2f);
          joueur[curr_player].HP -= n; // Penalty grows wrt time
          dialog.append("perdu !! La bonne réponse était : \n> "+question_pool[question_ID].reponses[0]+" <\n"+
            "Equipe "+(curr_player ? '1':'2')+ " perds "+std::to_string(n)+" points de vie !\n"+
            "Elle a répondu en "+std::to_string(delta_t)+" secondes !\n\n");
          break; }
        case 2: {
          const int n = std::max((int)std::floor(3 - delta_t * 0.2f), 0);
          joueur[curr_player].HP += n;
          dialog.append("Bonne réponse !! Equipe ");
          dialog.append(curr_player ? "1":"2");
          dialog.append(" gagne "+std::to_string(n)+" points de vie !\n"+
            "Elle a répondu en "+std::to_string(delta_t)+" secondes !\n\n");
          break; }
      }
    // Petite pause :)
    dialog.append("Vie de l'équipe 1 : "+std::to_string(joueur[1].HP)+"");
    dialog.append("\nVie de l'équipe 2 : "+std::to_string(joueur[0].HP)+"\n");

    if (joueur[1].HP<=0)
      criticTxt.setString("L'EQUIPE 1 N'A PLUS DE VIE !!!");
    else if (joueur[0].HP<=0)
      criticTxt.setString("L'EQUIPE 2 N'A PLUS DE VIE !!!");
    else if (joueur[1].HP<=8) {
      criticTxt.setString("VIE DE L'EQUIPE 1 AU NIVEAU CRITIQUE !!!");
      criticHP.play();
    } else if (joueur[0].HP<=8){
      criticTxt.setString("VIE DE L'EQUIPE 2 AU NIVEAU CRITIQUE !!!");
      criticHP.play();
    }

    bg.setColor(sf::Color(30,30,30));
    bool wt(1);
    text.setString(toSfString(dialog));
    text.setPosition(640-text.getLocalBounds().width/2, 360-text.getLocalBounds().height/2);
    criticTxt.setPosition(640-criticTxt.getLocalBounds().width/2, 640);
    while (wt){
      sf::Event event;
      while(rw.pollEvent(event))
        if(event.type == sf::Event::Closed)
          return;
        else if (event.type == sf::Event::KeyReleased || event.type == sf::Event::MouseButtonPressed)
          wt=0;
      rw.draw(bg);
      rw.draw(text);
      rw.draw(criticTxt);
      rw.display();
    }
    bg.setColor(sf::Color::White);
    criticHP.stop();
    // Changement de joueurs.
      if(joueur[curr_player].HP <= 0) return;
      curr_player = !curr_player;
  }
}


// ---- PROGRAM ----

int main(){
  /*
   * Main Menu
   */
  sf::Music music;
  music.openFromFile("res/ost.ogg");
  music.setLoop(true);
  music.setVolume(20);
  music.play();
  Backend bck("questions.txt");
  sf::RenderWindow window(sf::VideoMode(1280, 720), "FrankGame");
  window.setFramerateLimit(60);
  // gui
    guiSelect menu;
    menu.displaying = true;
    menu.choices = {"Jouer","Ajouter des questions", "Quitter"};
    menu.type(false);
    menu.position(sf::Vector2f(50,75));
  sf::Texture bgTxt, bg2Txt;
    bgTxt.loadFromFile("res/menu.png");
    bg2Txt.loadFromFile("res/Fond_1.png");
  sf::Sprite bg,bg2;
    bg.setTexture(bgTxt);
    bg2.setTexture(bg2Txt);
  bool returned;
  while (window.isOpen()){
    sf::Event event;
    while(window.pollEvent(event)) {
      if(event.type == sf::Event::Closed) {
        window.close();
        return 0;
      } else if (event.type == sf::Event::KeyReleased){
        if (event.key.code == sf::Keyboard::Return) {
          if (menu.displaying) { // Main Menu
            switch (menu.enter()){
              case 0:{
                bck.enterPlayLoop(window, bg2);
                break; }
              case 1: {
                std::system("start ed.exe");
                break; }
              case 2:{
                window.close();
                return 0;
                break; }
              default: break;
            }
            menu.displaying = true; //never hide.
          }
        } else if (event.key.code == sf::Keyboard::Up) {
          if (menu.displaying) menu.change(true);
        } else if (event.key.code == sf::Keyboard::Down) {
          if (menu.displaying) menu.change(false);
        }
      }
    }

    window.clear(sf::Color::White);
    window.draw(bg);
    if (menu.displaying) menu.draw(window);
    window.display();
  }
  return 0;
}

int playing(sf::RenderWindow& window, sf::Sprite bg, sf::String questionTitle, std::vector<std::string> choicesR, unsigned int correctAnswer, bool player){
  /*
   * Question loop.
   */
  // Choice box + gui
    sf::Texture boxtxt, epicFailTxt;
    sf::Sprite box, epicFail;
    boxtxt.loadFromFile("res/cadre.png");
    box.setTexture(boxtxt);
    box.setPosition(283,247);
    epicFailTxt.loadFromFile("res/epicFail.png");
    epicFail.setTexture(epicFailTxt);
    epicFail.setOrigin(511,247);
    epicFail.setPosition(1280/2,720/2);
    guiSelect choices;
    choices.displaying = true;
    choices.position(sf::Vector2f(300,245));
    choices.choices = choicesR;

  // Question title
    sf::Font fontQ;
    fontQ.loadFromFile("res/URW");
    sf::Text titleQ(questionTitle,fontQ,30), timing("",fontQ, 40);
    titleQ.setPosition(640 - titleQ.getLocalBounds().width/2, 15);
    titleQ.setOutlineThickness(2);
    if(titleQ.getLocalBounds().width >= 1280) {
      float factor = (titleQ.getLocalBounds().width)/1280.0f;
      titleQ.setScale(1/factor, 1/factor);
      titleQ.setPosition(640 - titleQ.getLocalBounds().width/(2 * factor), 15);
    }
    timing.setOutlineThickness(2);
    
  //Characters + init
    Character chara1(true), chara2(false);
    chara1.turn(player); chara2.turn(!player);
    bool playing = true, animating=true, oldanimating, block = false;
    int selection,i(0);
    sf::Clock clock;
    sf::Time startPain;
    const std::string selectLetters = "ABCD";
    int retval = 2;
  //sound
    sf::SoundBuffer bufferFail, bufferDamage;
    sf::Sound soundFail, soundDamage;
    bufferFail.loadFromFile("res/fail.wav");
    bufferDamage.loadFromFile("res/Damage.wav");
    soundFail.setBuffer(bufferFail);
    soundDamage.setBuffer(bufferDamage);
  while (playing){
    const float elapsed = clock.getElapsedTime().asSeconds() - 5.;
    sf::Event event;
    while(window.pollEvent(event) && elapsed>0) {
      if(event.type == sf::Event::Closed)
        return 0;
      else if (event.type == sf::Event::KeyReleased){
        if (event.key.code == sf::Keyboard::Return && choices.displaying) { // Main Menu
          selection = choices.enter();
          if (player){
            if (selection!=correctAnswer) {
              chara1.failAttack(true);
              retval = 1;
            }
            chara1.animate(selectLetters.substr(selection,1));
          } else {
            if (selection!=correctAnswer) {
              chara2.failAttack(true);
              retval = 1;
            }
            chara2.animate(selectLetters.substr(selection,1));
          }
        }
        else if (event.key.code == sf::Keyboard::Up && choices.displaying) choices.change(true);
        else if (event.key.code == sf::Keyboard::Down && choices.displaying) choices.change(false);
        else if (event.key.code == sf::Keyboard::Escape) return 0;
      } else if(event.type == sf::Event::MouseButtonPressed) {
        const auto y = event.mouseButton.y;
        choices.enter();
        if(y < 720/4) {
          selection = 0;
        } else if(y >= 720/4 && y < 720/2) {
          selection = 1;
        } else if(y >= 720/2 && y < 3 * 720/4) {
          selection = 2;
        } else {
          selection = 3;
        }
        if (player){
            if (selection!=correctAnswer) {
              chara1.failAttack(true);
              retval = 1;
            }
            chara1.animate(selectLetters.substr(selection,1));
          } else {
            if (selection!=correctAnswer) {
              chara2.failAttack(true);
              retval = 1;
            }
            chara2.animate(selectLetters.substr(selection,1));
          }
      }
    }
    i++;
    window.clear(sf::Color::White);
    window.draw(bg);
    chara1.display(window);
    chara2.display(window);
    if (elapsed<0){
      timing.setCharacterSize(250+elapsed*30);
      timing.setString(std::to_string( -elapsed ).substr(0,1));
      timing.setPosition(640 - timing.getLocalBounds().width/2, 360 - timing.getLocalBounds().height/2);
      window.draw(titleQ);
      window.draw(timing);
      if (elapsed>-3){
        window.draw(box);
        choices.draw(window);
        timing.setPosition(0, 200);
      }
    } else if (choices.displaying){
      timing.setCharacterSize(60);
      timing.setString(std::to_string( std::abs(elapsed) ).substr(0,4));
      timing.setPosition(640 - timing.getLocalBounds().width/2, 550);
      window.draw(timing);
      window.draw(titleQ);
      window.draw(box);
      choices.draw(window);
    } else { //animating
      oldanimating = animating;
      animating = (player)? chara1.isNotFinished(): chara2.isNotFinished();
      if (block && selection!=correctAnswer){
        int toMove = 8*std::sin(i*15);
        epicFail.setPosition(1280/2 + toMove, 720/2 + toMove);
        window.draw(epicFail);
      }
      if (oldanimating != animating && !block){ //Make the final anim
        if (selection!=correctAnswer) soundFail.play();
        else soundDamage.play();
        if (!player) {
          chara1.animate(chara2.getFailAttack()? "victory": "pain");
          if (selection%2==1) chara2.animate("restart");
        } else {
          chara2.animate(chara1.getFailAttack()? "victory": "pain");
          if (selection%2==1) chara1.animate("restart");
        }
        block = true;
      }
      if (!animating)//Wait the final anim
        playing = !(player^selection%2==1) ? chara1.isNotFinished(): chara2.isNotFinished();
    }
    window.display();
  }
  return retval;
}


// ---- HELPERS ----
  sf::String toSfString(std::string theStdString) {
    std::basic_string<sf::Uint32> utf32line;
    sf::Utf8::toUtf32(theStdString.begin(), theStdString.end(), back_inserter(utf32line));
    return sf::String(utf32line);
  }

  std::vector<std::string> split(std::string string, char search) {
    std::vector<std::string> parsed;
    std::string command;
    std::istringstream spart(string);
    while(std::getline(spart,command,search)) parsed.push_back(command);
    return parsed;
  }

// Character class -----

Character::Character(bool player){
  // true : left to right; false : right to left.
  orientation = player;
  defaultTxt.loadFromFile("res/charaDefault.png");
  capeTxt.loadFromFile("res/cape.png");
  animAtxt.loadFromFile("res/charaA.png");
  animBtxt.loadFromFile("res/charaB.png");
  animCtxt.loadFromFile("res/charaC.png");
  animDtxt.loadFromFile("res/charaD.png");
  arrowTxt.loadFromFile("res/arrow.png");
  finalTxt.loadFromFile("res/final.png");

  sprite.setTexture(defaultTxt);
  sprite.setTextureRect(sf::Rect<int>(0,0,250,250));
  sprite.setOrigin(sf::Vector2f(175,175));
  sprite.setScale((player) ? 1.f : -1.f, 1.f);
  sprite.setPosition((player) ? 175 : 1105,543);
  cape.setTexture(capeTxt);
  cape.setTextureRect(sf::Rect<int>(0,0,250,250));
  cape.setOrigin(sf::Vector2f(175,175));
  cape.setScale((player) ? 1.f : -1.f, 1.f);
  cape.setPosition((player) ? 175 : 1105,543);
  arrow.setTexture(arrowTxt);
  arrow.setRotation(0);

  animationType = "default";
  animCape = animNum = 1;
  iter = 0;
  ended = animFail = false;
}

bool Character::display(sf::RenderWindow& scr){
  int getms = clock.getElapsedTime().asMilliseconds()-animStart.asMilliseconds();
  if (animationType == "default") {
    iter++;
    if (iter%10==0) { // each 10 frames = (6 fps)
      cape.setTextureRect(sf::Rect<int>(250*animCape,0,250,250));
      animCape++;
      if (animCape>3) animCape = 0;
    }
    if ( (animNum%2 == 1 && iter%60==0) || (animNum%2 == 0 && iter%60==20)  ) {
      sprite.setTextureRect(sf::Rect<int>(250*animNum,0,250,250));
      animNum++;
      if (animNum>3) animNum = 0;
    }
    scr.draw(cape);
  } else if (animationType == "A"){
      getms -= 400;
      if (getms>700){
        int  y = easeInCubic(getms-700>700 ? 700:getms-700,300, -300, 700),
        arrowy = easeInBack(getms-700>700 ? 700:getms-700,315, animFail?-300:-200, 700),
        arrowx = easeLinear(getms-700>700 ? 700:getms-700,0, animFail? 500 : 810, 700),
        angle  = easeInOutSine(getms-700>500 ? 500:getms-700,animFail? 50 : -10, 40, 500);
        sprite.setPosition(orientation ? 175 : 1105,543-y);
        sprite.setTextureRect(sf::Rect<int>(0,0,250,250));
        if (orientation){
          arrow.setPosition(250+arrowx,543-arrowy);
          arrow.setRotation(angle);
        } else {
          arrow.setPosition(1030-arrowx,543-arrowy);
          arrow.setRotation(180-angle);
        }
        scr.draw(arrow);
        if      (getms>1400) ended = true;
        else if (getms>1300) sprite.setTexture(defaultTxt);
      } else if (getms>0){
        sprite.setTextureRect(sf::Rect<int>(250,0,250,250));
        sprite.setPosition(orientation ? 175 : 1105,543-easeOutExpo(getms>700 ? 700:getms,0, 300, 700));
      }
  } else if (animationType == "B"){
      getms -= 400;
      if (getms>700){
        int y = easeInExpo(getms-700>400 ? 400:getms-700,300, -300, 400),
            x = easeLinear(getms-700>400 ? 400:getms-700,animFail? 400:600,300, 400);
        sprite.setPosition(orientation ? 175+x : 1105-x,543-y);
        if      (getms>1300) ended = true;
        else if (getms>1000) sprite.setTextureRect(sf::Rect<int>(250*2,0,250,250));
      } else if (getms>0){
        int y = easeOutCubic(getms>700 ? 700:getms,0, 300, 700),
            x = easeLinear(getms>700 ? 700:getms,0, animFail? 400:600, 700);
        sprite.setPosition(orientation ? 175+x : 1105-x,543-y);
        sprite.setTextureRect(sf::Rect<int>(250,0,250,250));
      }
  } else if (animationType == "C"){
    if     (getms>1000) ended = true;
    else if (getms>800&&!animFail) sprite.setTextureRect(sf::Rect<int>(250*3,0,250,250));
    else if (getms>600&&!animFail) sprite.setTextureRect(sf::Rect<int>(250*2,0,250,250));
    else if (getms>500) sprite.setTextureRect(sf::Rect<int>(250,0,250,250));
  } else if (animationType == "D"){
    getms -= 400;
    if (getms>600){
      sprite.setOrigin(sf::Vector2f(175,175));
      sprite.setRotation(0);
      ended = true;
    } else if (getms>0){
      int x;
      if (animFail) {
        x = easeOutCubic(getms>500 ? 500:getms,0, 500, 500);
        sprite.setRotation(easeOutExpo(getms>500 ? 500:getms,0,orientation? 80 : -80, 500));
      } else x = easeOutElastic(getms>500 ? 500:getms, 0, 800, 500);
      sprite.setPosition(orientation ? 175+x : 1105-x,543);
      sprite.setTextureRect(sf::Rect<int>(350,0,350,250));
    }
  } else if (animationType == "pain"){
    sprite.setPosition(orientation ? 175 : 1105, 543+8*std::sin((getms-getms%10)*15) );
    if (getms>= 500) ended = true;
  } else if (animationType == "victory"){
    if (getms>= 1000) ended = true;
  } else if (animationType == "restart"){
    int x = easeOutSine(getms>700 ? 700:getms,animFail? 600:900, animFail? -600:-900, 700);
    sprite.setPosition(orientation ? 175+x : 1105-x,543);
    if (getms>=700) ended = true;
  }
  scr.draw(sprite);
  return 0;
}

bool Character::animate(std::string animationType){
  // Animation type : 
  // default, A, B, C, D, A fail, B fail, C fail, D fail, pain
  this->animationType = animationType;
  animStart = clock.getElapsedTime();
  sprite.setTextureRect(sf::Rect<int>(0,0,250,250));
  if (animationType == "A")
    sprite.setTexture(animAtxt);
  else if (animationType == "B")
    sprite.setTexture(animBtxt);
  else if (animationType == "C")
    sprite.setTexture(animCtxt);
  else if (animationType == "pain")
    sprite.setTexture(finalTxt);
  else if (animationType == "victory"){
    sprite.setTexture(finalTxt);
    sprite.setTextureRect(sf::Rect<int>(250,0,250,250));
  } else if (animationType == "D"){
    sprite.setTexture(animDtxt);
    sprite.setTextureRect(sf::Rect<int>(0,0,350,250));
    if (animFail) sprite.setOrigin(sf::Vector2f(70,245));
    sprite.setPosition(orientation ? 175:1105,618);
  } else if (animationType == "restart"){
    ended = false;
    sprite.setTexture(defaultTxt);
  }
  return 0;
}

bool Character::turn (bool t){
  const unsigned int cVal = t?255:150;
  sprite.setColor(sf::Color(cVal,cVal,cVal));
  return active = t;
}
bool Character::failAttack(bool f){return animFail = f;}
bool Character::getFailAttack() {return animFail;}
bool Character::isNotFinished(){return !ended;}

// GuiSelect class -----

  guiSelect::guiSelect(){
    if (!fontDeja.loadFromFile("res/BLKCHCRY.TTF"))
      std::cerr<<"INTERNAL ERROR : Can't load default font [res/BLKCHCRY.TTF] !!";
    if (!barTxt.loadFromFile("res/choice.png"))
      std::cerr<<"ERROR : unable to load [resources/img/choice.png]\n";
    if (!smallBarTxt.loadFromFile("res/smallChoice.png"))
      std::cerr<<"ERROR : unable to load [resources/img/smallChoice.png]\n";
    bar.setTexture(barTxt);
    smallBar.setTexture(smallBarTxt);
    bar.setColor(sf::Color(0,0,0,0));
    smallBar.setColor(sf::Color(255,100,100));
    barSelected.setTexture(barTxt);
    smallBarSelected.setTexture(smallBarTxt);
    valType = true;
    iter = choicePos = 0;
  }

  bool guiSelect::change(bool direction){
    // up or down selection
    if (direction) choicePos = (choicePos==0) ? choices.size()-1 : choicePos-1;
    else choicePos = (choicePos+1>=choices.size()) ? 0 : choicePos+1;
    return direction;
  }
  unsigned int guiSelect::enter(){
    //enter key pressed
    displaying = false;
    return choicePos;
  }

  bool guiSelect::draw(sf::RenderWindow& scr){
    iter++;
    int colorIter = 32*std::sin(0.1*iter)+32;
    sf::Text tempTxt("",fontDeja,valType?38:27);
    tempTxt.setFillColor(valType? sf::Color(120,130,255) : sf::Color::Black);
    if (valType) barSelected.setColor( sf::Color(255,255,255,95+colorIter*2) );
    else smallBarSelected.setColor( sf::Color(255,colorIter,colorIter) );

    for (unsigned int i(0); i<choices.size(); i++){
    //draw bar
      sf::Vector2f position(valPosition.x,valPosition.y+i*50);
      if (i==choicePos) {
        if (valType){
          barSelected.setPosition(position);
          scr.draw(barSelected); 
        } else {
          smallBarSelected.setPosition(position);
          scr.draw(smallBarSelected); 
        }
      } else {
        if (valType){
          bar.setPosition(position);
          scr.draw(bar);
        } else {
          smallBar.setPosition(position);
          scr.draw(smallBar);
        }
      }
    //draw text
      tempTxt.setString(toSfString(choices[i]));
      sf::Rect<float> txtBounds = tempTxt.getGlobalBounds();
      if (valType)
        tempTxt.setPosition(valPosition.x + 75, valPosition.y+i*50+15);
      else
      tempTxt.setPosition(valPosition.x + 200 - txtBounds.width/2, valPosition.y+i*50 + (12-txtBounds.height/2) );
      scr.draw(tempTxt);
    }
    return true;
  }

  sf::Vector2f guiSelect::position(sf::Vector2f position){ return valPosition = position; } //set position
  bool guiSelect::type(bool type){ return valType = type; } //full or small msgbox