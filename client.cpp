#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>




int main() {
    bool joue1 = false;
    bool game = false;
    bool bataille = false;
    bool a;
    bool partie = true;
    std::string win;
    std::vector<std::string> carte;
    std::string message_temp;
    sf::TcpSocket socket;
    std::string text_vict;
    std::string text_j1;
    std::string text_j2;
    std::string plis_tempj1;
    std::string plis_tempj2;
    sf::IpAddress ip = "127.0.0.1"; // Adresse du serveur (local)

    unsigned short port = 5325;

    // 2. Connexion au serveur
    std::cout << "Tentative de connexion au serveur..." << std::endl;
    if (socket.connect(ip, port) != sf::Socket::Done) {
        std::cerr << "Erreur : Impossible de rejoindre le serveur !" << std::endl;
        return -1;
    }

    std::cout << "Connecte !" << std::endl;

    sf::Packet carte_recue;

    if (socket.receive(carte_recue) == sf::Socket::Done) {
        sf::Uint32 size;

        // 1. On récupère d'abord la taille
        if (carte_recue >> size) {


            // 2. On boucle selon la taille reçue
            for (sf::Uint32 i = 0; i < size; ++i) {
                std::string nombre;
                carte_recue >> nombre;
                carte.push_back(nombre);
            }


        }

    }


    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Ca marche !");

    sf::Texture fondImage,cartej1, cartej2;
    if (!fondImage.loadFromFile("asset/plateau.jpeg")) {
        return -1;
    }
    sf::Sprite Background,cardj1, cardj2;
    Background.setTexture(fondImage);
    float scaleX = 1920.f / fondImage.getSize().x;
    float scaleY = 1080.f / fondImage.getSize().y;
    Background.setScale(scaleX, scaleY);



    cardj1.setPosition(600.f, 400.f);
    cardj2.setPosition(1100.f, 400.f);
    sf::Font font;

    if (!font.loadFromFile("arial.ttf")) {
        return -1;
    }
    sf::Text text;
    sf::Text textj2;
    sf::Text text3;
    sf::Text plisj1;
    sf::Text plisj2;


    text.setPosition(150.f,300.f);
    textj2.setPosition(650.f,300.f);
    text3.setPosition(1200.f,100.f);
    plisj1.setPosition(700.f,100.f);
    plisj2.setPosition(700.f,150.f);

    text.setFont(font);
    textj2.setFont(font);
    text3.setFont(font);
    plisj1.setFont(font);
    plisj2.setFont(font);

    text.setFillColor(sf::Color::Red);
    text3.setFillColor(sf::Color::Black);
    plisj1.setFillColor(sf::Color::Black);
    plisj2.setFillColor(sf::Color::Black);
    socket.setBlocking(false);

    while (window.isOpen()) {
        if (partie == false) {
            std::cout << "Tentative de connexion au serveur..." << std::endl;
            if (socket.connect(ip, port) != sf::Socket::Done) {
                std::cerr << "Erreur : Impossible de rejoindre le serveur !" << std::endl;
                return -1;
            }

            std::cout << "Connecte !" << std::endl;

            sf::Packet carte_recue;

            if (socket.receive(carte_recue) == sf::Socket::Done) {
                sf::Uint32 size;

                // 1. On récupère d'abord la taille
                if (carte_recue >> size) {


                    // 2. On boucle selon la taille reçue
                    for (sf::Uint32 i = 0; i < size; ++i) {
                        std::string nombre;
                        carte_recue >> nombre;
                        carte.push_back(nombre);
                    }


                }

            }
            partie =true;

        }
            sf::Event event;


            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }

                if (!game && event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Space) {
                        if (joue1 == false) {
                            text_j1 = carte.front();
                            sf::Packet joue;
                            joue << text_j1;
                            if (bataille == true) {
                                if (cartej1.loadFromFile("asset/Cartes/00_G.png")) {
                                    cardj1.setTexture(cartej1);
                                    cardj1.setTextureRect(sf::IntRect(0, 0, cartej1.getSize().x, cartej1.getSize().y));
                                }
                                joue1 = true;
                            } else {
                                if (cartej1.loadFromFile("asset/Cartes/" + carte.front() + ".png")) {
                                    cardj1.setTexture(cartej1);
                                    cardj1.setTextureRect(sf::IntRect(0, 0, cartej1.getSize().x, cartej1.getSize().y));
                                }
                                joue1 = true;
                            }
                            socket.send(joue);
                            carte.erase(carte.begin());
                        }
                    }
                }
            }
            sf::Packet reponse;
            if (socket.receive(reponse) == sf::Socket::Done) {
                std::string recu;
                reponse >> recu;
                if (recu == "Bataille") {
                    joue1 = false;
                    reponse >> bataille;
                    text_vict = recu;
                    sf::Uint32 plis;
                    reponse >> plis;
                    plis_tempj2 = "le joueur adverse :" + std::to_string(plis);
                    plis_tempj1 = "Vous :" + std::to_string(carte.size());
                } else if (recu == "FINI") {
                    game = true;
                    std::string message;
                    reponse >> message;
                    text_vict = "Tu as " + message + "la partie";
                    text_j1 = "";
                    text_j2 = "";
                    sf::Uint32 plis;
                    reponse >> plis;
                    plis_tempj2 = "le joueur adverse :" + std::to_string(plis);
                    plis_tempj1 = "Vous :" + std::to_string(carte.size());
                    partie = false;
                } else if (recu == "Gagne" || recu == "Perdu") {
                    text_vict = "Tu as " + recu + " ce pli !";
                    joue1 = false;
                    if (recu == "Gagne") {
                        int size;
                        reponse >> size;
                        for (int i = 0; i < size; ++i) {
                            std::string element;
                            reponse >> element;
                            carte.push_back(element);
                        }
                    }
                    sf::Uint32 plis;
                    reponse >> plis;
                    plis_tempj2 = "le joueur adverse :" + std::to_string(plis);
                    plis_tempj1 = "Vous :" + std::to_string(carte.size());
                    bataille = false;
                } else {
                    text_j2 = recu;
                    if (joue1 == true) {
                        bataille = false;
                    }
                    if (cartej2.loadFromFile("asset/Cartes/" + text_j2 + ".png")) {
                        cardj2.setTexture(cartej2);
                        // On force la mise à jour du sprite au cas où la taille change
                        cardj2.setTextureRect(sf::IntRect(0, 0, cartej2.getSize().x, cartej2.getSize().y));
                    }
                    joue1 = false;
                }
            }


            plisj1.setString(plis_tempj1);
            plisj2.setString(plis_tempj2);
            text.setString(text_j1);
            textj2.setString(text_j2);
            text3.setString(text_vict);
            window.clear(sf::Color::White);
            window.draw(Background);
            window.draw(cardj1);
            window.draw(cardj2);
            window.draw(text3);
            window.draw(plisj1);
            window.draw(plisj2);
            window.display();
        if (partie == false) {
             a = true;
        }
        while (a == true) {
            while (window.pollEvent(event)){
                std::cout <<" enter dans la boucle";
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::R) {
                        std::cout << "reconnect";
                        a = false;
                    }
                    if (event.key.code == sf::Keyboard::Q) {
                        window.close();
                    }
                }
            }
        }

    }




    return 0;
}