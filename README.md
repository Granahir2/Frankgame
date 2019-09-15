# Frankgame

Un jeu en c++ semblable à un QCM interactif.


Testez votre culture littéraire de manière interactive avec ce jeu !

Adapté au programme de première S.

<https://github.com/Granahir2/Frankgame>

### Règles 

Jeu deux joueurs (ou deux groupes jouables en classe)

Répondez à une question pour attaquer. Si la réponse est bonne, l'attaque réussit et vous régénère, tout en faisant perdre des PVs à l'ennemi.  
Si la réponse est fausse, elle vous fera perdre des points.

Le système de points dépend de votre temps de réaction

Si vous jouez sur TNI, l'écran est séparé en 4 sur la verticale. Cliquer sur une partie de l'écran répond à la question choisie.

### Programmation

Jeu très mal programmé, créé en une semaine.  
Le code pique littéralement les yeux et n'est pas optimisé (la relation front-end et back-end se fait via une seule fonction). De nombreux bugs existent si vous faites n'importe quoi.  
Préférez jouer avec le clavier plutôt que la souris.

### Staff

* Pixel-art / Graphisme : [Urufu Kaminari](https://www.youtube.com/channel/UC4XtcsTQm5A1_INqMcFmjgw)
* Programmation back-end (gestion des questions/points/joueurs, scripts E/S) : [@Granahir2](https://github.com/Granahir2/)
* Programmation font-end (affichage dans la fenêtre, animations, effets spéciaux) : [@LoganTann](<https://github.com/LoganTann>)
* Compilation du code (builds Windows) : [@LoganTann](<https://github.com/LoganTann>)

## Compiler le code (adaptez selon votre compteur et le système d'exploitation) : 

Game.exe : easing.cpp + display.cpp , librairies à inclure : SFML, standarts C++11 ou supérieurs

ed.exe : q_editor.cpp , librairies à inclure : standarts C++11 ou supérieurs
