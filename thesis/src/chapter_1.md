# Wstęp
## Opis Problemu

Konsola Game Boy została wydana w Japonii 21 kwietnia 1989 roku i od razu odniosła ogromny sukces komercyjny. W ciągu pierwszych dwóch lat sprzedały się ponad 20 milionów egzemplarzy, co czyniło ją jedną z najlepiej sprzedających się konsoli gier wideo w historii. W 1990 roku Game Boy została wydana na zachodzie i odniosła podobny sukces komercyjny. Konsola była dostępna w kilku różnych barwach i wersjach, co dodatkowo zwiększyło jej popularność. Gry były przechowywane na tzw. "Cartridge'ach" lub "Cart'ach" które zawierały kod gry, grafiki i dźwięk a i także dodatkowe element jak dodatkowa pamięc RAM czy bateria do przechowania stanu gry pomiędzy sesjami.

Jednakże, kiedy Nintendo zaprzestała produkcji Game Boya, wiele gier zostało opracowanych tylko dla tej konsoli. Dzisiaj większość tych gier nie jest dostępna na żadnym innym platformie, a gracze muszą szukać innych rozwiązań.

W związku z brakiem dostępności oryginalnej konsoli Game Boya, gracze zaczęli poszukiwać sposobów na zagranie tych klasycznych gier. Jednym z takich rozwiązań są emulatory konsol - oprogramowanie które pozwala na symulację pracy oryginalnej konsoli wirtualnie.
## Emulatory
Emulatory to oprogramowanie które pozwala na symulację pracy oryginalnej konsoli wirtualnie. Są one zdolne do wyświetlania grafiki, generowania dźwięku i obsługi sterowników w sposób podobny do oryginalnego sprzętu.


Emulatory są stosowane głównie przez graczy, którzy chcą zagranie klasycznych gier bez potrzeby posiadania oryginalnego sprzętu. Są one również używane przez muzea i archiwalia, które pragną zachować treści konsolowej historii w formie digitalnej.


Emulatory są ważne dla archiwizacji treści z kilku powodów:

- Zachowanie historycznej wartości: Emulatory pozwalają na zachowanie historycznej wartości konsoli i gier, które mogłyby inaczej być utracone.
- Dostępność dla nowej generacji graczy: Emulatory umożliwiają nowej generacji graczom dostęp do klasycznych gier, które mogą nie być dostępne w oryginalnej formie.
- Zachowanie jakości: Emulatory pozwalają na zachowanie jakości grafiki i dźwięku oryginalnej konsoli, co jest ważne dla archiwizacji treści.
- Możliwość dostępu do gier nieopublikowanych: Emulatory umożliwiają dostęp do gier nieopublikowanych lub eksperymentalnych, które mogłyby inaczej pozostać ukryte.

Istnieją dwa rodzaje emulacji:

- Emulacja pełna: Emulacja pełna jest zdolna do symulowania pracy oryginalnej konsoli w całości, w tym wyświetlania grafiki, generowania dźwięku i obsługi sterowników.
- Emulacja częściowa: Emulacja częściowa jest zdolna do symulowania tylko części pracy oryginalnej konsoli, takiej jak grafika lub dźwięk.

## Założenia pracy 

Cel pracy to stworzenie emulacji konsoli Game Boy w języku programowania C++, wykorzystującej bibliotekę Raylib do obsługi interfejsu graficznego. Użytkownik posiada dostępne w postaci cyfrowej własne kopie gier.

Emulacja musi być implementowana w języku programowania C++ i wykorzystać bibliotekę Raylib do obsługi interfejsu graficznego. Muszą być zapewnione mechanizmy kontrolne, by użytkownik mógł sterować grą za pomocą klawiatury.
Końcowy program musi być dostępny do uruchomienia na różnych platformach, takich jak Windows, macOS i Linux.
Emulator ma mieć wsparcie dla podstawowego rodzaju cart'a - zawierający jedynie kod gry ROM Only.
Dostępne w postaci cyfrowej własne rzuty gier Game Boy będą używane do testowania emulacji. Muszą być zapewnione możliwości odtwarzania i grania gier Game Boy z własnymi rzutami gier.

Plan realizacji pracy obejmuje zrozumienie architektury konsoli Game Boy, implementację emulacji w języku programowania C++ oraz wykorzystanie biblioteki Raylib do obsługi interfejsu graficznego. Emulacja musi być zweryfikowana poprzez porównanie wyników z testami tworzynymi przez wierną społeczność dokumentującą konsolę Game Boy.
## Grupa docelowa

Grupa docelowa dla emulatora Game Boy to gracze, którzy chcą ponownie grać w swoje ulubione gry z lat 80. i 90., ale nie mają dostępu do oryginalnej konsoli lub posiadają sposób na skopiowanie danych ze swoich kartridge'ów bez dostępu do konsoli.

Oni są osobami, które chcą odtworzyć swoje dzieciństwo i ponownie grać w gry, które lubili w młodości. Grupa ta obejmuje również graczy, którzy zakupili cyfrowe gry homebrew i chcieli je zagrać, które bez oryginalnej konsoli i narzędzi do tworzenia własnych kaset z grą.

Emulator Game Boya jest dla nich sposobem na grę w ich ulubione gry bez potrzeby posiadania oryginalnej konsoli lub posiadania specjalistycznych narzędzi.
