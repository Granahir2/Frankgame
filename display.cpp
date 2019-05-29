#include "easing.h"

#include <iostream>
#include <vector>
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
      bool isNotFinished();
    private :
      bool orientation,ended, active, animFinished;
      sf::Texture defaultTxt, capeTxt, animAtxt, animBtxt, animCtxt, animDtxt, painTxt, arrowTxt ;
      sf::Sprite sprite, cape, arrow;
      std::string animationType;
      unsigned int iter, animCape, animNum;
      sf::Clock clock;
      sf::Time animStart;
  };
  int playing(sf::RenderWindow& window, sf::Sprite bg, sf::String questionTitle, std::vector<std::string> choicesR, unsigned int correctAnswer, bool player);
  sf::String toSfString(std::string theStdString);
  std::vector<std::string> split(std::string string, char search);

// ---- PROGRAM ----

int main(){
  /*
   * Main Menu
   */
  sf::RenderWindow window(sf::VideoMode(1280, 720), "FrankGame");
  window.setFramerateLimit(60);
  // gui
    guiSelect menu;
    menu.displaying = true;
    menu.choices = {"Jouer","Ajouter des questions", "Quitter"};
    menu.type(false);
    menu.position(sf::Vector2f(50,75));
  sf::Texture bgTxt;
    bgTxt.loadFromFile("res/Fond_1.png");
  sf::Sprite bg;
    bg.setTexture(bgTxt);
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
                sf::String questionT = "Lorem ispum dodor sit amet consectuer ?";
                unsigned int correctA = 3;
                std::vector<std::string> c = {"A : Lorem ispum dodor sit amet consectuer", "B : Lorem ispum dodor sit amet consectuer", "C : Lorem ispum dodor sit amet consectuer", "D : Lorem ispum dodor sit amet consectuer"};
                playing(window, bg, questionT, c, correctA, true);
                returned = playing(window, bg, questionT, c, correctA, false);
                break; }
              case 1: {
                std::cout<<"2\n";
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
    sf::Texture boxtxt;
    sf::Sprite box;
    boxtxt.loadFromFile("res/cadre.png");
    box.setTexture(boxtxt);
    box.setPosition(283,247);
    guiSelect choices;
    choices.displaying = true;
    choices.position(sf::Vector2f(300,245));
    choices.choices = choicesR;
  // Question title
    sf::Font fontQ;
    fontQ.loadFromFile("res/URW");
    sf::Text titleQ;
    titleQ.setFont(fontQ);
    titleQ.setString(questionTitle);
    titleQ.setCharacterSize(50);
    titleQ.setPosition(640 - titleQ.getLocalBounds().width/2, 15);
    titleQ.setOutlineThickness(2);
  //Characters + init
    Character chara1(true), chara2(false);
    bool playing = true, animating=true, oldanimating, block = false;
    int selection;
    sf::Clock clock;
    sf::Time startPain;
    const std::string selectLetters = "ABCD";
  while (playing){
    sf::Event event;
    while(window.pollEvent(event)) {
      if(event.type == sf::Event::Closed) {
        window.close();
        return 0;
      } else if (event.type == sf::Event::KeyReleased){
        if (event.key.code == sf::Keyboard::Return && choices.displaying) { // Main Menu
          selection = choices.enter();
          if (player) chara1.animate(selectLetters.substr(selection,1));
          else chara2.animate(selectLetters.substr(selection,1));
        }
      }
      else if (event.key.code == sf::Keyboard::Up && choices.displaying) choices.change(true);
      else if (event.key.code == sf::Keyboard::Down && choices.displaying) choices.change(false);
    }

    window.clear(sf::Color::White);
    window.draw(bg);
    chara1.display(window);
    chara2.display(window);
    if (choices.displaying) {
      window.draw(titleQ);
      window.draw(box);
      choices.draw(window);
    } else {
      oldanimating = animating;
      animating = (player)? chara1.isNotFinished(): chara2.isNotFinished();
      if (oldanimating != animating && !block){ //Make the final anim
        if (!player) {
          chara1.animate("pain");
          if (selection%2==1) chara2.animate("restart");
        } else {
          chara2.animate("pain");
          if (selection%2==1) chara1.animate("restart");
        }
        block = true;
      }
      if (!animating)//Wait the final anim
        playing = !(player^selection%2==1) ? chara1.isNotFinished(): chara2.isNotFinished();
    }
    window.display();
  }
  return 0;
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
  painTxt.loadFromFile("res/pain.png");

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
  ended = false;
  turn(player);
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
        arrowy = easeInBack(getms-700>700 ? 700:getms-700,315, -200, 700),
        arrowx = easeLinear(getms-700>700 ? 700:getms-700,0, 810, 700),
        angle  = easeInOutSine(getms-700>500 ? 500:getms-700,-10, 40, 500);
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
            x = easeLinear(getms-700>400 ? 400:getms-700,600, 300, 400);
        sprite.setPosition(orientation ? 175+x : 1105-x,543-y);
        if      (getms>1300) ended = true;
        else if (getms>1000) sprite.setTextureRect(sf::Rect<int>(250*2,0,250,250));
      } else if (getms>0){
        int y = easeOutCubic(getms>700 ? 700:getms,0, 300, 700),
            x = easeLinear(getms>700 ? 700:getms,0, 600, 700);
        sprite.setPosition(orientation ? 175+x : 1105-x,543-y);
        sprite.setTextureRect(sf::Rect<int>(250,0,250,250));
      }
  } else if (animationType == "C"){
    if     (getms>1000) ended = true;
    else if (getms>800) sprite.setTextureRect(sf::Rect<int>(250*3,0,250,250));
    else if (getms>600) sprite.setTextureRect(sf::Rect<int>(250*2,0,250,250));
    else if (getms>500) sprite.setTextureRect(sf::Rect<int>(250,0,250,250));
  } else if (animationType == "D"){
    getms -= 400;
    if (getms>600){
      ended = true;
    } else if (getms>0){
      int x = easeOutElastic(getms>500 ? 500:getms,0, 800, 500);
      sprite.setPosition(orientation ? 175+x : 1105-x,543);
      sprite.setTextureRect(sf::Rect<int>(350,0,350,250));
    }
  } else if (animationType == "pain"){
    sprite.setPosition(orientation ? 175 : 1105, 543+8*std::sin((getms-getms%10)*15) );
    if (getms>= 500) ended = true;
  } else if (animationType == "restart"){
    int x = easeOutSine(getms>700 ? 700:getms,900, -900, 700);
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
    sprite.setTexture(painTxt);
  else if (animationType == "D"){
    sprite.setTexture(animDtxt);
    sprite.setTextureRect(sf::Rect<int>(0,0,350,250));
  } else if (animationType == "restart"){
    ended = false;
    sprite.setTexture(defaultTxt);
  }
  return 0;
}

bool Character::turn (bool t){
  const unsigned int cVal = t?255:150;
  sprite.setColor(sf::Color(cVal,cVal,cVal));
  active = t;
}

bool Character::isNotFinished(){return !ended;}

// GuiSelect class -----

guiSelect::guiSelect(){
  if (!fontDeja.loadFromFile("res/Bradley.ttf"))
    std::cerr<<"INTERNAL ERROR : Can't load default font [resources/fonts/DejaVuSans.ttf] !!";
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