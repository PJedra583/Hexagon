#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>
using namespace std;
/** Sygnatury metod do obsługi planszy
 *``
 * Ciała metod dostarczone są na końcu pliku. Poniższe metody służą do obsługi planszy gry Hexagon przedstawionej jako
 * tablicy char[17][9]. Nie należy stosować przy użyciu innych tablic.
 * Więcej w dokumentacji metod na końcu plku
 */
void makePlansza(char pole[17][9]);
int countPlansza(char pole[17][9],char searching);
void showPlansza(char pole[17][9]);
void colorPlansza(char pole[17][9],sf::CircleShape hexagons[17][9]);
bool isMovePossible(char tab[17][9],char pionek);
bool inArr(int y, int x);
void rozpropaguj (char tab[17][9], int poleY, int poleX, char current, char target);
void loadPlansza (char tab[17][9]);
void saveScore(int score);
/** Sygnatury metod do obsługi gry
 *
 *  Zajmują się obsługą poszczególnych modułów oraz wyświetlaniem przy uzyciu biblioteki SFML.
 */
int menu();
int game();

/** Zmienne globalne
 *
 * Służą do komunikacji pomiędzy interfejsem menu a grą. Odpowiednie im znaczenie opisane jest poniżej
 */
/** @param rendergame - True oznacza próbe inicjalizacji rozgrywki
  *  Gra moze byc inicjowana w dwoch przypadkach: Stworzenia nowej lub załadowania istniejącej
  */
bool rendergame = false;
/** @param computermode - True Oznacza inicjalizacje rozgrywki w trybie przeciwko AI
  *  Gra moze byc inicjowana w dwoch przypadkach: Stworzenia nowej lub załadowania istniejącej
  */
bool computermode = false;
/** @param loading - True Oznacza że gra musi zostać załadowana z pliku
 *  Jedna z dwoch mozliwosci opisanych w dokumentacji bool rendergame.
 *  Sciezka pliku do załadowania przechowywana jest w zmiennej string i jest sciezka absolutna
 *  Potrzebna jest także zmienna do identyfikacji tury
 */
bool loading = false;
/** @param file_to_load - przechowuje sciezke zapisu
 *  Jest to sciezka absolutna. Może nie działać na innych komputerach
 */
string file_to_load;
/** @param turnP - True oznacza ruch gracza pierwszego
 *  Uwaga - Dziala tylko w trybie przeciwko dwom graczom. W trybie z komputerem powinna byc ustalona na True
 */
bool turnP = true;
/** @param score - przechowuje wynik rozgrywki
 *  Wynik zapisywany jest w metodzie saveScore
 */
int score = 0;
/** Metoda main - ustala przebieg programu
 *
 */
bool stop = false;
int main() {
    menu();
    while (!stop) {
        if (rendergame) {
            game();
        } else {
            menu();
        }
    }
}
/** Metoda menu - ilustruje menu
 *  Wymaga biblioteki SFML
 *  Jest oddzielnym oknem niezależnym od gry
 */
int menu() {
        ////////////Rysunek Hexagon///////////
        sf::CircleShape hexagon(50, 6);
        sf::Color OutlineColor(51, 0, 102);
        sf::Color InlineColor(255, 0, 255);
        //kolor kontur
        hexagon.setOutlineColor(OutlineColor);
        hexagon.setOutlineThickness(15);
        //kolor wypelnienia
        hexagon.setFillColor(InlineColor);
        hexagon.setPosition({50, 50});
        ///////////Font//////////////
        sf::Font font;
        if (!font.loadFromFile("C:\\Users\\Acer\\CLionProjects\\untitled\\comic.ttf")) {
            return EXIT_FAILURE;
        }
        //////////Tytuł gry/////////////
        sf::Text title(font, "Peter's Hexagon", 55);
        title.setPosition({190, 70});
        title.setFillColor(InlineColor);
        //////////Buttons////////////////
        sf::Text newgame(font, "New Game", 50);
        sf::Text loadgame(font, "Load Game", 50);
        sf::Text changemode(font, "2 Players", 50);
        sf::Text bestresults(font, "Best Results", 50);
        sf::Text quitgame(font, "Exit", 50);

        sf::RectangleShape buttonNG(sf::Vector2f(500, 70));
        sf::RectangleShape buttonLG(sf::Vector2f(500, 70));
        sf::RectangleShape buttonCM(sf::Vector2f(500, 70));
        sf::RectangleShape buttonBR(sf::Vector2f(500, 70));
        sf::RectangleShape buttonQ(sf::Vector2f(500, 70));

        newgame.setPosition({630, 255});
        buttonNG.setPosition({500, 250});
        buttonNG.setFillColor(OutlineColor);

        loadgame.setPosition({630, 355});
        buttonLG.setPosition({500, 350});
        buttonLG.setFillColor(OutlineColor);

        changemode.setPosition({640, 455});
        buttonCM.setPosition({500, 450});
        buttonCM.setFillColor(OutlineColor);

        bestresults.setPosition({615, 555});
        buttonBR.setPosition({500, 550});
        buttonBR.setFillColor(OutlineColor);

        quitgame.setPosition({690, 655});
        buttonQ.setPosition({500, 650});
        buttonQ.setFillColor(OutlineColor);


        ////////////Menu/////////////
        sf::RenderWindow menu(sf::VideoMode({1600, 900}), "HEXAGONS",
                          sf::Style::Default,
                          sf::ContextSettings(0, 0, 8));

        while (menu.isOpen()) {
            sf::Event e;
            while (menu.pollEvent(e)) {
                if (e.type == sf::Event::Closed) {
                    stop = true;
                    menu.close();
                } else if (e.type == sf::Event::MouseButtonPressed) {
                    if (e.mouseButton.button == sf::Mouse::Left) {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(menu);

                        if (buttonNG.getGlobalBounds().contains({static_cast<float>(mousePos.x),
                                                                 static_cast<float>(mousePos.y)})) {
                            rendergame = true;
                            menu.close();
                        }
                        else if (buttonQ.getGlobalBounds().contains({static_cast<float>(mousePos.x),
                                                                            static_cast<float>(mousePos.y)}))
                        {
                            stop = true;
                            menu.close();
                        }
                        else if (buttonLG.getGlobalBounds().contains({static_cast<float>(mousePos.x),
                                                                      static_cast<float>(mousePos.y)})) {

                            /** Okno wczytywania Rozgrywki - Wyswietla okno do wpisaniia nazwy zapisu
                             *  Okno nie obsługuje zmiany wpisanego tekstu. Po wciśnięciu ENTER jeśli zapis o wpisanej
                             *  nazwie istnieje zostanie załadowany natychmiastowo
                             *  Nie zaleca sie uzycia innych zapisów niz wygenerowanych poprzez okno save
                             *  String filename tworzony jest w oparciu o ścieżkę lokalnego komputera. Może nie działać
                             *  Na innych urządzeniach
                             */
                            string input_text;
                            sf::Text text(font,"");
                            sf::RenderWindow load(sf::VideoMode({300, 200}), "Load ur game",
                                                  sf::Style::Default,
                                                  sf::ContextSettings(0, 0, 8));
                            while (load.isOpen()) {
                                sf::Event e3;
                                sf::Text textsave(font, "Enter ur save name", 20);
                                sf::Text input(font, "", 20);
                                textsave.setPosition({50, 25});
                                text.setPosition({30, 100});
                                while (load.pollEvent(e3)) {
                                    if (e3.type == sf::Event::Closed) {
                                        load.close();
                                    } else if (e3.type == sf::Event::TextEntered) {
                                            input_text += e3.text.unicode;
                                    } else if (e3.type == sf::Event::KeyPressed) {
                                        if (e3.key.code == sf::Keyboard::Enter) {
                                            string filename = "C:\\Users\\Acer\\CLionProjects\\untitled\\saves\\";
                                            for (char c: input_text) {
                                                filename += c;
                                            }
                                            for (char c: ".txt") {
                                                filename += c;
                                            }
                                            fstream stream(filename.c_str()) ;
                                            if (stream) {
                                                file_to_load = filename;
                                                loading = true;
                                                rendergame = true;
                                                load.close();
                                                menu.close();
                                            } else {
                                                load.close();
                                            }
                                            load.close();
                                        }
                                    }
                                }
                                text.setString(input_text);
                                load.draw(text);
                                load.draw(textsave);
                                load.display();
                            }
                        }
                        else if (buttonCM.getGlobalBounds().contains({static_cast<float>(mousePos.x),
                                                                      static_cast<float>(mousePos.y)}))
                        {
                            if (computermode) {
                                computermode = false;
                                changemode.setString("2 Players");
                                changemode.setPosition({640, 455});
                            } else {
                                computermode = true;
                                changemode.setString("Computer");
                                changemode.setPosition({640, 455});
                            }
                        }
                        else if (buttonBR.getGlobalBounds().contains({static_cast<float>(mousePos.x),
                                                              static_cast<float>(mousePos.y)}))
                        {
                            /** Okno best results - Odczytuje i wyswietla dane z pliku BestResult.txt
                             *  Lokalizacja jest sciezka absolutna indywidualnego komputera. Moze nie dzialac na innych
                             *  urzadzeniach. Przepisuje Dane do vektora i wyświetla je w nowym oknie
                             */
                            fstream stream("C:\\Users\\Acer\\CLionProjects\\untitled\\BestResult.txt");
                            vector<int> scores;
                            int number;
                            if (stream.is_open()) {
                                while (stream >> number) {
                                    scores.push_back(number);
                                }
                                stream.close();
                            }
                            sf::RenderWindow results(sf::VideoMode({400, 600}), "Best Results",
                                                  sf::Style::Default,
                                                  sf::ContextSettings(0, 0, 8));
                            while (results.isOpen()) {
                                sf::Event e4;
                                sf::Text result(font, "Result 1: -- --" , 30);
                                if (scores.size() > 0) {
                                    result.setString("Result 1: " + to_string(scores.at(0)));
                                }
                                sf::Text result2(font, "Result 2: -- --" , 30);
                                if (scores.size() > 1) {
                                    result2.setString("Result 2: " + to_string(scores.at(1)));
                                }
                                sf::Text result3(font, "Result 3: -- --" , 30);
                                if (scores.size() > 2) {
                                    result3.setString("Result 3: " + to_string(scores.at(2)));
                                }
                                sf::Text result4(font, "Result 4: -- --" , 30);
                                if (scores.size() > 3) {
                                    result4.setString("Result 4: " + to_string(scores.at(3)));
                                }
                                sf::Text result5(font, "Result 5: -- --" , 30);
                                if (scores.size() > 4) {
                                    result5.setString("Result 5: " + to_string(scores.at(4)));
                                }

                                result.setPosition({100,50});
                                result2.setPosition({100,150});
                                result3.setPosition({100,250});
                                result4.setPosition({100,350});
                                result5.setPosition({100,450});

                                while (results.pollEvent(e4)) {
                                    if (e4.type == sf::Event::Closed) {
                                        results.close();
                                    }
                                    results.clear(OutlineColor);
                                    results.draw(result);
                                    results.draw(result2);
                                    results.draw(result3);
                                    results.draw(result4);
                                    results.draw(result5);
                                    results.display();
                                }
                            }
                        }
                        }
                    }
                }
                sf::Color bg(25, 25, 25);

                menu.clear(bg);
                menu.draw(hexagon);
                menu.draw(title);
                menu.draw(buttonNG);
                menu.draw(buttonLG);
                menu.draw(buttonCM);
                menu.draw(buttonBR);
                menu.draw(buttonQ);

                menu.draw(newgame);
                menu.draw(loadgame);
                menu.draw(changemode);
                menu.draw(bestresults);
                menu.draw(quitgame);

                menu.display();
            }
            return 0;
        }
        /** Okno z gra - Odpowiada ze rozgrywke oraz wyswietlenie okna z gra
         *  Wyswietla plansze z pomoca SFML, Okresla logiczne zasady rozgrywki, a takze implementuje Gracza AI
         */
int game() {

    /////////////FONT/////////////////
    sf::Font font;
    string input_text;
    int input_length;
    if (!font.loadFromFile("C:\\Users\\Acer\\CLionProjects\\untitled\\comic.ttf")) {
        return EXIT_FAILURE;
    }
    sf::Text text(font,"");

    sf::RenderWindow game(sf::VideoMode({1920, 1080}), "Peter's Hexagon",
                          sf::Style::Default,
                          sf::ContextSettings(0, 0, 8));
    sf::RectangleShape buttonreset(sf::Vector2f(100, 100));
    buttonreset.setPosition({1600, 800});
    buttonreset.setFillColor(sf::Color(34, 139, 34));

    sf::RectangleShape buttonsavegame(sf::Vector2f(200, 100));
    buttonsavegame.setPosition({1050, 800});
    buttonsavegame.setFillColor(sf::Color(34, 139, 34));

    sf::RectangleShape buttonMenu(sf::Vector2f(200, 100));
    buttonMenu.setPosition({1350, 800});
    buttonMenu.setFillColor(sf::Color(34, 139, 34));

    sf::Text buttonsavegameText(font, "SAVE", 40);
    sf::Text buttonresetText(font, "R", 70);
    sf::Text buttonMenuText(font, "MENU", 40);

    buttonMenuText.setPosition({1390, 825});
    buttonsavegameText.setPosition({1090, 825});
    buttonresetText.setPosition({1630, 810});




    char tab[17][9];
    if (loading) {
        loadPlansza(tab);
    } else {
        makePlansza(tab);
    }
    sf::CircleShape hexagons[17][9];
    sf::Color playerColor = sf::Color::White;
    sf::Color player2Color = sf::Color(160, 32, 240);
    sf::Color KomputerColor = sf::Color::Yellow;
    sf::Color placeColor = sf::Color::Black;

    for (int i = 0; i < 17; ++i) {
        for (int j = 0; j < 9; ++j) {
            if (tab[i][j] == 'n') {
                sf::CircleShape hexagon(25, 6);
                hexagon.setFillColor(sf::Color(sf::Color(25, 25, 112)));
                hexagon.setPosition({80 + j * (100.f / 2), 100 + i * (85.f / 2)});
                hexagons[i][j] = hexagon;
            }
            if (tab[i][j] == 'O') {
                sf::CircleShape hexagon(25, 6);
                hexagon.setFillColor(sf::Color(sf::Color::Magenta));
                hexagon.setPosition({80 + j * (100.f / 2), 100 + i * (85.f / 2)});
                hexagons[i][j] = hexagon;
            }
            if (tab[i][j] == 'P') {
                sf::CircleShape hexagon(25, 6);
                hexagon.setFillColor(sf::Color(playerColor));
                hexagon.setPosition({80 + j * (100.f / 2), 100 + i * (85.f / 2)});
                hexagons[i][j] = hexagon;
            }
            if (tab[i][j] == 'K') {
                sf::CircleShape hexagon(25, 6);
                hexagon.setFillColor(sf::Color(KomputerColor));
                hexagon.setPosition({80 + j * (100.f / 2), 100 + i * (85.f / 2)});
                hexagons[i][j] = hexagon;
            }
            if (tab[i][j] == '*') {
                sf::CircleShape hexagon(25, 6);
                hexagon.setFillColor(sf::Color(placeColor));
                hexagon.setPosition({80 + j * (100.f / 2), 100 + i * (85.f / 2)});
                hexagons[i][j] = hexagon;
            }
            if (tab[i][j] == 'J') {
                sf::CircleShape hexagon(25, 6);
                hexagon.setFillColor(sf::Color(player2Color));
                hexagon.setPosition({80 + j * (100.f / 2), 100 + i * (85.f / 2)});
                hexagons[i][j] = hexagon;
            }
        }
    }


    sf::Text communicate(font, "Wybierz swoj pionek", 55);
    communicate.setPosition({650, 250});
    communicate.setFillColor(sf::Color::White);
    //Temporary comm
    sf::Text inteligence(font, "", 55);
    inteligence.setPosition({0, 0});
    inteligence.setFillColor(sf::Color::White);


    sf::Text result2(font, "Ilosc Pionkow Gracza: " + to_string(countPlansza(tab, 'P')), 55);
    result2.setPosition({1000, 500});
    result2.setFillColor(sf::Color::White);

    sf::Text result(font, "" + to_string(countPlansza(tab, 'K')), 55);
    if (computermode) {
        result.setString("Ilosc Pionkow Komputera: " + to_string(countPlansza(tab, 'K')));
        result.setPosition({1000, 650});
        result.setFillColor(sf::Color::White);
    } else {
        result.setString("Ilosc Pionkow Gracza 2: " + to_string(countPlansza(tab, 'J')));
        result.setPosition({1000, 650});
        result.setFillColor(sf::Color::White);
    }

    bool checked = false;
    bool choosen = false;
    int choosenX = -1;
    int choosenY = -1;
    bool gameEnd = false;
    bool gameSave = false;
    //Wyswietlenie Planszy
    while (game.isOpen()) {
        sf::Event e2;
        while (game.pollEvent(e2)) {
            if (e2.type == sf::Event::Closed) {
                stop = true;
                game.close();
            } else if (e2.type == sf::Event::MouseButtonPressed) {
                if (e2.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos2 = sf::Mouse::getPosition(game);

                    if (buttonMenu.getGlobalBounds().contains({static_cast<float>(mousePos2.x),
                                                               static_cast<float>(mousePos2.y)})) {
                        rendergame = false;
                        computermode = false;
                        loading = false;
                        turnP = true;
                        rendergame = false;
                        game.close();
                    }
                }
            }
            if (!gameEnd) {
                if (e2.type == sf::Event::MouseButtonPressed) {
                    if (e2.mouseButton.button == sf::Mouse::Left) {
                        sf::Vector2i mousePos2 = sf::Mouse::getPosition(game);
                        //Obsluga przycisku reset
                        if (buttonreset.getGlobalBounds().contains({static_cast<float>(mousePos2.x),
                                                                    static_cast<float>(mousePos2.y)})) {
                            char tmp[17][9];
                            if (loading) {
                                loadPlansza(tmp);
                            } else {
                                makePlansza(tmp);
                                turnP = true;
                            }
                            for (int i = 0; i < 17; ++i) {
                                for (int j = 0; j < 9; ++j) {
                                    tab[i][j] = tmp[i][j];
                                }
                            }
                            colorPlansza(tab, hexagons);
                            //Obsluga przycisku Save Game
                        }
                        else if (buttonsavegame.getGlobalBounds().contains({static_cast<float>(mousePos2.x),
                                                                              static_cast<float>(mousePos2.y)})) {
                            //Koniec rozgrywki
                            gameSave = true;
                        }
                        for (int i = 0; i < 17; ++i) {
                            for (int j = 0; j < 9; ++j) {
                                if (hexagons[i][j].getGlobalBounds().contains(
                                        {static_cast<float>(mousePos2.x), static_cast<float>(mousePos2.y)})) {
                                    if (tab[i][j] == 'C') {
                                        choosen = false;
                                        communicate.setString("Wybierz swoj pionek");
                                        if (choosenY == i - 3 || choosenY == i + 3 || choosenX == j - 2 ||
                                            choosenX == j + 2) {
                                            //Ruch o 2 pola
                                            if (turnP) {
                                                tab[i][j] = 'P';
                                                tab[choosenY][choosenX] = '*';
                                                if (!computermode) {
                                                    if (turnP) { turnP = false; } else { turnP = true; }
                                                }
                                            } else {
                                                tab[i][j] = 'J';
                                                tab[choosenY][choosenX] = '*';
                                                if (!computermode) {
                                                    if (turnP) { turnP = false; } else { turnP = true; }
                                                }
                                            }
                                        }
                                            //ruch o jedno pole
                                        else {
                                            if (turnP) {
                                                tab[i][j] = 'P';
                                                if (!computermode) {
                                                    if (turnP) { turnP = false; } else { turnP = true; }
                                                }
                                            } else {
                                                tab[i][j] = 'J';
                                                if (!computermode) {
                                                    if (turnP) { turnP = false; } else { turnP = true; }
                                                }
                                            }
                                        }
                                        if (computermode) {
                                            // Rozpropagowanie pionkow gracza
                                            rozpropaguj(tab, i, j, 'P', 'K');
                                        } else {
                                            // odwrotnie poniewaz bool jest zmieniony wczesniej
                                            if (turnP) {
                                                //rozpropagowanie pionkow gracza 2
                                                rozpropaguj(tab, i, j, 'J', 'P');
                                            } else {
                                                //rozpropagowanie pionkow gracza
                                                rozpropaguj(tab, i, j, 'P', 'J');
                                            }
                                        }

                                        // Odkliknięcie
                                        for (int k = choosenY - 3; k <= choosenY + 3; ++k) {
                                            for (int l = choosenX - 2; l <= choosenX + 2; ++l) {
                                                //Sprawdzenie wyjscia poza zakres tablicy z wykluczenie wybranego pionka
                                                if (inArr(k, l) && !(k == choosenY && l == choosenX)) {
                                                    if (tab[k][l] == 'C') {
                                                        tab[k][l] = '*';
                                                    }
                                                }
                                            }
                                        }

                                        //Ustawienie Wyników
                                        if (computermode) {
                                            result.setString(
                                                    "Ilosc Pionkow Komputera:  " + to_string(countPlansza(tab, 'K')));
                                        } else {
                                            result.setString(
                                                    "Ilosc Pionkow Gracza 2:  " + to_string(countPlansza(tab, 'J')));
                                        }
                                        result2.setString(
                                                "Ilosc Pionkow Gracza:  " + to_string(countPlansza(tab, 'P')));



                                        //////////////////Implementacja Sztucznej Inteligencji/////////////////////
                                        bool isPossibleK = true;
                                        if (computermode) {
                                            // Sprawdzenie Możliwości ruchu Komputera
                                            isPossibleK = isMovePossible(tab, 'K');
                                            int Bestcount = 0;
                                            int BestmoveY = -1;
                                            int BestmoveX = -1;
                                            int FrommoveY = -1;
                                            int FrommoveX = -1;

                                            for (int k = 0; k < 17; ++k) {
                                                for (int l = 0; l < 9; ++l) {
                                                    //Wyszukanie dostępnych Pionków
                                                    if (tab[k][l] == 'K') {
                                                        for (int m = k - 3; m <= k + 3; ++m) {
                                                            for (int n = l - 2; n <= l + 2; ++n) {
                                                                if (inArr(m, n) && !(m == k && n == l)) {
                                                                    // Dostępny ruch
                                                                    if (tab[m][n] == '*') {
                                                                        // Obliczanie Najbardziej opłacalnego

                                                                        rozpropaguj(tab, m, n, 'T', 'P');
                                                                        int counter2 = countPlansza(tab, 'T') * 2;
                                                                        rozpropaguj(tab, m, n, 'P', 'T');

                                                                        // Dodac - Odjecie -1 przy ruchu 2 pola
                                                                        if (m == k - 3 || m == k + 3 || n == l - 2 ||
                                                                            n == l + 2) {
                                                                            counter2 -= 1;
                                                                        }
                                                                        if (counter2 >= Bestcount) {
                                                                            Bestcount = counter2;
                                                                            BestmoveY = m;
                                                                             BestmoveX = n;
                                                                            FrommoveY = k;
                                                                            FrommoveX = l;
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }

                                            //Rozpropagowanie
                                            rozpropaguj(tab, BestmoveY, BestmoveX, 'K', 'P');
                                            //Ruch Komputera
                                            if (BestmoveY == FrommoveY - 3 || BestmoveY == FrommoveY + 3 ||
                                                BestmoveX == FrommoveX - 2 || BestmoveX == FrommoveX + 2) {
                                                tab[FrommoveY][FrommoveX] = '*';
                                                tab[BestmoveY][BestmoveX] = 'K';
                                            } else {
                                                tab[BestmoveY][BestmoveX] = 'K';
                                            }
                                        }
                                        bool isPossibleP = true;
                                        if (isPossibleK && computermode) {
                                            isPossibleP = isMovePossible(tab, 'P');
                                        }
                                        if (!computermode && turnP) {
                                            isPossibleP = isMovePossible(tab, 'P');
                                        }
                                        bool isPossibleJ = true;
                                        if (!computermode && !turnP) {
                                            // Sprawdzenie Możliwości ruchu gracza
                                            isPossibleJ = isMovePossible(tab, 'J');
                                        }

                                        //////////////KONIEC GRY/////////////////

                                        if (!isPossibleP || !isPossibleJ || !isPossibleK) {
                                            gameEnd = true;
                                            int wynikK = countPlansza(tab,'K');
                                            int wynikP = countPlansza(tab,'P');
                                            int wynikJ = countPlansza(tab,'J');

                                            // Poniewaz zmieniaja sie tury tylko jeden bool moze byc mozliwy
                                            if (!isPossibleK) {
                                                wynikP += countPlansza(tab,'*');
                                            }
                                            if (!isPossibleP) {
                                                if (computermode) {
                                                    wynikK += countPlansza(tab, '*');
                                                } else {
                                                    wynikJ += countPlansza(tab, '*');
                                                }
                                            }
                                            if (!isPossibleJ) {
                                                wynikP += countPlansza(tab, '*');
                                            }
                                            if (computermode) {
                                                if (wynikP > wynikK)  {
                                                    communicate.setString("Koniec gry. Zwyciezyl gracz 1!");
                                                } else if (wynikK > wynikP) {
                                                    communicate.setString("Koniec gry. Zwyciezyl Komputer!");
                                                } else {
                                                    communicate.setString("Koniec gry. Remis.");
                                                }
                                                score = countPlansza(tab,'P') * 14;
                                            } else {
                                                if (wynikP > wynikJ)  {
                                                    communicate.setString("Koniec gry. Zwyciezyl gracz 1!");
                                                    score = countPlansza(tab,'P') * 14;
                                                } else if (wynikJ > wynikP) {
                                                    communicate.setString("Koniec gry. Zwyciezyl gracz 2!");
                                                    score = countPlansza(tab,'J') * 14;
                                                } else {
                                                    communicate.setString("Koniec gry. Remis.");
                                                    score = countPlansza(tab, 'P') * 14;
                                                }
                                            }
                                            // Do najlepszego wyniku nie wliczaja sie puste miejsca na ktore mozna sie ruszyc
                                            saveScore(score);
                                        }
                                    } else if (tab[i][j] != 'P' && turnP) {
                                        if (!choosen) {
                                            communicate.setString("To nie twoj pionek.");
                                        } else {
                                            communicate.setString("Zly ruch");
                                        }
                                    } else if (tab[i][j] != 'J' && !turnP) {
                                        if (!choosen) {
                                            communicate.setString("To nie pionek gracza 2. ");
                                        } else {
                                            communicate.setString("Zly ruch");
                                        }
                                    } else {
                                        if (!choosen) {
                                            choosen = true;
                                            choosenY = i;
                                            choosenX = j;
                                            communicate.setString("Wybierz miejsce na ktore chcesz sie ruszyc");
                                            hexagons[i][j].setFillColor(sf::Color::Red);
                                            //wykluczenie rogów - i-3 ruch w bok *2
                                            for (int k = i - 3; k <= i + 3; ++k) {
                                                for (int l = j - 2; l <= j + 2; ++l) {
                                                    //Sprawdzenie wyjscia poza zakres tablicy z wykluczenie wybranego pionka
                                                    if (inArr(k, l) && !(k == i && l == j)) {
                                                        if (tab[k][l] == '*') {
                                                            //ruch o dwa pola
                                                            if (k == i - 3 || k == i + 3 || l == j - 2 || l == j + 2) {
                                                                //bordo
                                                                tab[k][l] = 'C';
                                                                hexagons[k][l].setFillColor(sf::Color(102, 0, 0));
                                                            }
                                                                //ruch o jedno pole
                                                            else {
                                                                //losos
                                                                tab[k][l] = 'C';
                                                                hexagons[k][l].setFillColor(sf::Color(255, 51, 51));
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                            //choosen - odklikniecie
                                        else {
                                            choosen = false;
                                            communicate.setString("Wybierz swoj pionek");
                                            if (turnP) {
                                                hexagons[choosenY][choosenX].setFillColor(playerColor);
                                            } else {
                                                hexagons[choosenY][choosenX].setFillColor(player2Color);
                                            }
                                            //wykluczenie rogów - i-3 ruch w bok *2
                                            for (int k = choosenY - 3; k <= choosenY + 3; ++k) {
                                                for (int l = choosenX - 2; l <= choosenX + 2; ++l) {
                                                    //Sprawdzenie wyjscia poza zakres tablicy z wykluczenie wybranego pionka
                                                    if (inArr(k, l) && !(k == i && l == j)) {
                                                        if (tab[k][l] == 'C') {
                                                            tab[k][l] = '*';
                                                            hexagons[k][l].setFillColor(placeColor);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        //Koniec gry
                    }
                }
            }
        }
        sf::Color bgcolor(25, 25, 112);
        game.clear(bgcolor);
        // Rysowanie Planszy
        colorPlansza(tab,hexagons);
        for (int y = 0; y < 17; y++) {
            for (int x = 0; x < 9; x++) {
                game.draw(hexagons[y][x]);
            }
        }
        game.draw(communicate);
        game.draw(result);
        game.draw(result2);
        game.draw(buttonreset);
        game.draw(buttonresetText);
        game.draw(buttonsavegame);
        game.draw(buttonsavegameText);
        game.draw(buttonMenu);
        game.draw(buttonMenuText);
        game.display();
        /** Okno save game - Generuje Zapis rozgrywki
         *  Działa podobnie do okna load.
         */
        if (gameSave) {
            sf::RenderWindow save(sf::VideoMode({300, 200}), "Save ur game",
                                  sf::Style::Default,
                                  sf::ContextSettings(0, 0, 8));
            while (save.isOpen()) {
                sf::Event e3;
                sf::Text textsave(font, "Enter ur save name", 20);
                sf::Text input(font, "", 20);
                textsave.setPosition({50, 25});
                text.setPosition({30, 100});
                while (save.pollEvent(e3)) {
                    if (e3.type == sf::Event::Closed) {
                        gameSave = false;
                        save.close();
                    } else if (e3.type == sf::Event::TextEntered) {
                            input_text += e3.text.unicode;
                    } else if (e3.type == sf::Event::KeyPressed) {
                        if (e3.key.code == sf::Keyboard::Enter) {
                            string filename = "C:\\Users\\Acer\\CLionProjects\\untitled\\saves\\";
                            for (char c : input_text) {
                                filename += c;
                            }
                            for (char c : ".txt") {
                                filename += c;
                            }

                            //Stworzenie zapisu
                            fstream fileOut;
                            fileOut.open(filename,ios::out | ios::trunc);
                            if(fileOut.is_open()){
                                string s = "";
                                for (int i = 0; i < 17; ++i) {
                                    for (int j = 0; j < 9; ++j) {
                                        s += tab[i][j];
                                    }
                                }
                                if (computermode) {
                                    s += 'k';
                                }
                                else if (turnP) {
                                    s += 'p';
                                } else {
                                    s += 'j';
                                }
                                fileOut<<s;
                                fileOut.close();
                            }
                            gameSave = false;
                            save.close();
                        }
                    }

                }
                text.setString(input_text);
                save.draw(text);
                save.draw(textsave);
                save.display();
            }
        }
    }
    return 0;
}
/** Nieobslugiwana metoda showPlansza
 *
 * Uzywana w wesji alpha do Narysowania planszy w konsoli
 * @param pole jest tablica char[17][9] opisana w dokumentacji sygnator metod
 */
void showPlansza(char pole[17][9]) {
    for (int i = 0; i < 17; ++i) {
        cout<<endl;
        for (int j = 0; j < 9; ++j) {
            if (pole[i][j] == 'n') { cout<<"   ";}
            else cout<<pole[i][j]<<"  ";
        }
    }
}
/** loadPlansza - metoda służąca do załadowania planszy z pliku
 *  Załadowuje grę z pliku z zawartoscia tabeli i dodatkowym znakiem oznaczającym ruch/tryb rozgrywki
 *  Plik powinien być zapisany w jednej linii
 *  @param pole jest tablica char[17][9] opisana w dokumentacji sygnatur metod
 */
void loadPlansza(char pole[17][9]) {
    fstream stream(file_to_load);
    string tmp;
    if (stream.is_open())
    {
        getline(stream, tmp);
        stream.close();
    }
    int counter = 0;
    for (int i = 0; i < 17; ++i) {
        for (int j = 0; j < 9; ++j) {
            pole[i][j] = tmp.at(counter);
            counter++;
        }
    }
    if (tmp.at(counter) == 'j') {
        turnP = false;
        computermode = false;
    } else if (tmp.at(counter) == 'p') {
        turnP = true;
        computermode = false;
    } else if (tmp.at(counter) == 'k') {
        computermode = true;
    }
}
/** makePlansza - tworzy tablice przedstawiajaca plansze
 * Plansza przedstawiona jest na bazie szachownicy w której rogi usuwane sa na podstawie
 * Kolumn i wierszy - wiersze od 4 do 13 pozostaja niezmienione
 *
 * @param pole jest tablica char[17][9] opisana w dokumentacji sygnatur metod
 */
void makePlansza (char pole[17][9]) {
    // Wypełnienie tablicy

    // Rogi planszy
    int counter = 5;
    // n - brak mozliwosci ruchu

    for (int i = 0; i < 17; ++i) {
        if (i < 4) {counter--;} else if (i > 13) {counter++;}
        for (int j = 0; j < 9; ++j) {
            if (i % 2 == 0) {
                if (j % 2 == 0) {
                    pole[i][j] = '*';
                } else
                {
                    pole[i][j] = 'n';
                }
            } else {
                if (j % 2 == 0) {
                    pole[i][j] = 'n';
                } else
                {
                    pole[i][j] = '*';
                }
            }
            // Rogi planszy
            if (i < 4) {
                if (j < counter || j > 9-counter) {
                    pole[i][j] = 'n';
                }
            }
            //17-4
            if (i > 13) {
                if (j < counter || j > 9-counter) {
                    pole[i][j] = 'n';
                }
            }
        }
    }
    // trzy puste pola
    pole[6][4] = 'O';
    pole[9][3] = 'O';
    pole[9][5] = 'O';
    // trzy pola dla gracza(P)
    pole[4][0] = 'P';
    pole[4][8] = 'P';
    pole[16][4] = 'P';
    // trzy pola dla komputera(K)
    if (computermode) {
        pole[12][0] = 'K';
        pole[12][8] = 'K';
        pole[0][4] = 'K';
    } else {
        pole[12][0] = 'J';
        pole[12][8] = 'J';
        pole[0][4] = 'J';
    }
}
/** count plansza - Liczy ilosc pionkow  na planszy
 *
 * @param pole jest tablica char[17][9] opisana w dokumentacji sygnatur metod
 * @param searching jest Pionkiem ktory bedzie liczony
 * @return zwraca ilosc pionkow
 */
int countPlansza(char pole[17][9],char searching) {
    int counter = 0;
    for (int i = 0; i < 17; ++i) {
        for (int j = 0; j < 9; ++j) {
            if (pole[i][j] == searching) {
                counter++;
            }
        }
    }
    return counter;
}
/** colorPLansza - Koloruje plansze na odpowiedni pionkom kolor
 * Wymaga biblioteki sfml
 * @param tab jest tablica char[17][9] opisana w dokumentacji sygnatur metod
 * @param hexagons jest tablicą ktora przedstawia graficznie pola
 */
void colorPlansza(char tab[17][9],sf::CircleShape hexagons[17][9]) {
    sf::Color playerColor = sf::Color::White;
    sf::Color KomputerColor = sf::Color::Yellow;
    sf::Color placeColor = sf::Color::Black;
    sf::Color player2Color = sf::Color(160,32,240);
    for (int i = 0; i < 17; ++i) {
        for (int j = 0; j < 9; ++j) {
            if (tab[i][j] == 'n') {
                hexagons[i][j].setFillColor(sf::Color(sf::Color(25, 25, 112)));
            }
            if (tab[i][j] == 'O') {
                hexagons[i][j].setFillColor(sf::Color::Magenta);
            }
            if (tab[i][j] == 'P') {
                hexagons[i][j].setFillColor(playerColor);
            }
            if (tab[i][j] == 'K') {
                hexagons[i][j].setFillColor(KomputerColor);
            }
            if (tab[i][j] == '*') {
                hexagons[i][j].setFillColor(placeColor);
            }
            if (tab[i][j] == 'J') {
                hexagons[i][j].setFillColor(player2Color);
            }
        }
    }
}
/** inArr - sprawdza czy opuszczono zakresu tablicy
 *
 * @param y symbolizuje wiersze
 * @param x symbolizuje kolumny
 * @return zwraca true lub false zaleznie od tego czy y i x naleza do zakresu tablicy
 */
bool inArr(int y, int x) {
    return y<17 && y>-1 && x<9 && x>-1;
}
/** isMovePossible - sprawdza mozliwosc ruchu pionka
 *
 * @param tab jest tablica char[17][9] opisana w dokumentacji sygnatur metod
 * @param pionek to pionek dla ktorego sprawdzamy mozliwosc ruchu
 * @return
 */
bool isMovePossible(char tab[17][9], char pionek) {
    for (int k = 0; k < 17; ++k) {
        for (int l = 0; l < 9; ++l) {
            if (tab[k][l] == pionek) {
                for (int m = k - 3; m <= k + 3; ++m) {
                    for (int n = l - 2; n <= l + 2; ++n) {
                        if  (inArr(m,n)) {
                            if (tab[m][n] == '*') {
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}
/** rozpropaguj - zamienia pobliskie pionki zgodnie z zasadami rozgrywki
 *
 * @param tab jest tablica char[17][9] opisana w dokumentacji sygnatur metod
 * @param poleY To wiersz w ktorym lezy pionek current
 * @param poleX  to kolumna w ktorym lezy pionek current
 * @param current jest pionkiem na ktory sa zamieniane pobliskie pionki
 * @param target jest pionkiem ktory ma zostac zamieniony
 */
void rozpropaguj (char tab[17][9], int poleY, int poleX, char current, char target) {

    if (inArr(poleY - 2, poleX)) {
        if (tab[poleY - 2][poleX] == target){
            tab[poleY - 2][poleX] = current;
        }
    }
    if (inArr(poleY + 2,poleX)) {
        if (tab[poleY + 2][poleX] == target) {
            tab[poleY + 2][poleX] = current;
        }
    }

    if (inArr(poleY - 1,poleX+1)) {
        if (tab[poleY - 1][poleX + 1] == target) {
            tab[poleY - 1][poleX + 1] = current;
        }
    }
    if (inArr(poleY - 1,poleX-1)) {
        if (tab[poleY - 1][poleX - 1] == target) {
            tab[poleY - 1][poleX - 1] = current;
        }
    }
    if (inArr(poleY + 1,poleX+1)) {
        if (tab[poleY + 1][poleX + 1] == target) {
            tab[poleY + 1][poleX + 1] = current;
        }
    }
    if (inArr(poleY + 1,poleX-1)) {
        if (tab[poleY + 1][poleX - 1] == target) {
            tab[poleY + 1][poleX - 1] = current;
        }
    }
}
/** saveScore - zapisuje wynik do Pliku BestResults.txt
 * Zapisane dane sa odczytywane,sortowane, a następnie 5 najwiekszych wynikow jest zapisywanych
 * @param score to wynik ktory ma zostac zapisany
 */
void saveScore(int score) {
    fstream stream("C:\\Users\\Acer\\CLionProjects\\untitled\\BestResult.txt");
    vector<int> scores;
    scores.push_back(score);
    int number;
    if (stream.is_open()) {
        // Odczytaj liczby z pliku
        while (stream >> number) {
            scores.push_back(number);
        }
        stream.close();
    }
    sort(scores.begin(), scores.end(), [](int n, int m) {
        return n > m;
    });

    fstream streamOut("C:\\Users\\Acer\\CLionProjects\\untitled\\BestResult.txt");

    if (streamOut.is_open()) {
        for (int i=0;i<scores.size(); i++) {
            if (i<5) {
                streamOut << scores.at(i) << endl;
            }
        }
        streamOut.close();
    }
}
