#include <chrono>
#include <iostream>
#include <random>
#include <SFML/Network.hpp>

int main() {
    sf::TcpListener server;
    std::vector<std::string> j1 ;
    std::vector<std::string> j2 ;
    bool bataille = false;
    std::vector<int> plis;
    //std::vector<std::string> carte = {"01_S","02_S", "03_S","04_S", "05_S", "06_S","07_S","08_S","09_S","10_S","01_H","02_H", "03_H","04_H", "05_H", "06_H","07_H","08_H","09_H","10_H"};
    std::vector<std::string> carte = {"01_S","02_S","03_S","04_S"};
    std::vector<sf::TcpSocket*> clients;
    std::string joue_j1 = "";
    std::string joue_j2 = "";
    std::vector<std::string> temp_j1 ;
    std::vector<std::string> temp_j2 ;
    bool partie;
    bool c = true;
    while (true) {
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::shuffle(carte.begin(), carte.end(), std::default_random_engine(seed));
        for (std::string element : carte) {
            if (c == false) {

                j1.push_back(element);

            }
            else {
                j2.push_back(element);

            }
            c=!c;
        }
        if (server.listen(5325) != sf::Socket::Done) {
            std::cerr << "Error listening for connections" << std::endl;
        }


        while (clients.size() < 2) {
            sf::TcpSocket* client = new sf::TcpSocket;
            std::cout << "Tentative de connexion..." << std::endl;
            if (server.accept(*client) == sf::Socket::Done) {
                std::cout << "Joueur " << clients.size() + 1 << " connecte !" << std::endl;
                clients.push_back(client);
            } else {
                delete client;
            }
        }


        sf::Packet carte_j1;
        sf::Packet carte_j2;
        carte_j1 << static_cast<sf::Uint32>(j1.size());
        carte_j2 << static_cast<sf::Uint32>(j2.size());
        for (std::string element : j1) {
            carte_j1 << element;
        }
        for (std::string element : j2) {
            carte_j2 << element;
        }


        clients[0]->send(carte_j1);
        clients[1]->send(carte_j2);


        std::string cartej1="";
        std::string cartej2="";
        sf::SocketSelector joueur;
        for (auto client : clients) {
            joueur.add(*client);
        }
        partie = true;
        while (partie){
            if ((j1.size() == 0 && j2.size() == 4) || (j2.size() == 0 && j1.size() == 4)) {
                sf::Packet victj1, victj2;
                victj1 <<"FINI";
                victj2 <<"FINI";
                if (j1.size() == 0) {
                    victj2 <<"Gagne";
                    victj1 <<"Perdu";
                    victj1 << static_cast<sf::Uint32>(j2.size());
                    victj2 << static_cast<sf::Uint32>(0);

                }
                else {
                    victj1 <<"Gagne";
                    victj2 <<"Perdu";
                    victj1 << static_cast<sf::Uint32>(0);
                    victj2 << static_cast<sf::Uint32>(j1.size());
                }
                clients[0]->send(victj1);
                clients[1]->send(victj2);
                partie =false;


            }
            else if (bataille == true) {
                if (joueur.wait(sf::milliseconds(100))) {
                    for (int i=0; i<2; ++i) {
                        if (joueur.isReady(*clients[i])) {
                            sf::Packet joue;
                            if (clients[i]->receive(joue) == sf::Socket::Done) {
                                std::string carte;
                                joue >> carte;
                                if (i == 0) {
                                    cartej1 = "00_G";

                                }
                                else {
                                    cartej2 = "00_G";

                                }
                                std::cout << "J" << i + 1 << " joue " << carte << std::endl;
                                sf::Packet carte_autre;
                                carte_autre << "00_G";
                                clients[(i==0)?1:0]->send(carte_autre);

                            }
                        }
                    }
                }
            }
            else {
                if (joueur.wait(sf::milliseconds(100))) {
                    for (int i=0; i<2; ++i) {
                        if (joueur.isReady(*clients[i])) {
                            sf::Packet joue;
                            if (clients[i]->receive(joue) == sf::Socket::Done) {
                                std::string carte;
                                joue >> carte;
                                if (i == 0) {
                                    cartej1 = carte;
                                    j1.erase(j1.begin());

                                }
                                else {
                                    cartej2 = carte;
                                    j2.erase(j2.begin());
                                }
                                std::cout << "J" << i + 1 << " joue " << carte << std::endl;
                                sf::Packet carte_autre;
                                carte_autre << carte;
                                clients[(i==0)?1:0]->send(carte_autre);

                            }
                        }
                    }
                }
            }
            if (cartej1 !="" && cartej2 != "") {
                std::cout << cartej1 << cartej2 << std::endl;
                sf::Packet victj1, victj2;
                int v1 = std::stoi(cartej1.substr(0,2));
                int v2 = std::stoi(cartej2.substr(0,2));
                if (v1 == v2 && bataille == false ) {
                    temp_j1.push_back(cartej1);
                    temp_j2.push_back(cartej2);
                    victj1 <<"Bataille" << true;
                    victj2 <<"Bataille" << true;

                    bataille =true;
                }
                else if (bataille == true) {
                    temp_j1.push_back(j1.front());
                    temp_j2.push_back(j2.front());
                    j1.erase(j1.begin());
                    j2.erase(j2.begin());
                    bataille = false;
                }

                else if (v1 > v2) {
                    if (temp_j1.size() != 0 && temp_j2.size() != 0) {
                        temp_j1.push_back(cartej1);
                        temp_j2.push_back(cartej2);
                        int size = temp_j1.size() + temp_j2.size();
                        victj1 << "Gagne" << size;
                        victj2 << "Perdu";
                        for (std::string element : temp_j1) {
                            j1.push_back(element);
                            victj1 << element;
                        }
                        for (std::string element : temp_j2) {
                            j1.push_back(element);
                            victj1 << element;
                        }
                        temp_j1.clear();
                        temp_j2.clear();
                    }
                    else {
                        int size = 2;
                        victj1 << "Gagne" << size << cartej1 << cartej2;
                        victj2 << "Perdu";
                        j1.push_back(cartej1);
                        j1.push_back(cartej2);
                    }

                }
                else {
                    if (temp_j1.size() != 0 && temp_j2.size() != 0) {
                        temp_j1.push_back(cartej1);
                        temp_j2.push_back(cartej2);
                        int size = temp_j1.size() + temp_j2.size();
                        victj1 << "Perdu";
                        victj2 << "Gagne" << size;
                        for (std::string element : temp_j1) {
                            j2.push_back(element);
                            victj2 << element;
                        }
                        for (std::string element : temp_j2) {
                            j2.push_back(element);
                            victj2 << element;
                        }
                        temp_j1.clear();
                        temp_j2.clear();
                    }
                    else {
                        int size = 2;
                        victj1 << "Perdu";
                        victj2 << "Gagne" << size << cartej1 << cartej2;
                        j2.push_back(cartej2);
                        j2.push_back(cartej1);
                    }

                }
                victj1 << static_cast<sf::Uint32>(j2.size());
                victj2 << static_cast<sf::Uint32>(j1.size());
                clients[0]->send(victj1);
                clients[1]->send(victj2);
                cartej1 ="";
                cartej2 ="";
            }
        }

        // ... (Fin de la boucle while(partie))

        std::cout << "Partie terminee. Nettoyage..." << std::endl;

        // On laisse un peu de temps aux clients pour afficher le message de fin
        sf::sleep(sf::seconds(2));

        // Nettoyage impératif pour recommencer une partie propre
        for (auto client : clients) {
            client->disconnect(); // On coupe la connexion
            delete client;        // On libère la mémoire
        }
        clients.clear();
        joueur.clear();
        j1.clear();
        j2.clear();
        temp_j1.clear();
        temp_j2.clear();
        cartej1 = "";
        cartej2 = "";
        bataille = false;

        std::cout << "Pret pour une nouvelle session !" << std::endl;

    }

    return 0;
}
