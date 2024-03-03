**Cel projektu**

Celem projektu było wykonanie komunikatora internetowego – aplikacji pozwalającej na komunikację między klientami w sieci. 

**Serwer**

Serwer został napisany w języku C++ i wykorzystuje gniazda do tworzenia i zarządzania komunikacją między serwerem a klientami. Każde nowe połączenie powoduje utworzenie nowego wątku. Serwer inicjuje bazę danych SQLite, w której przechowywane są informacje o użytkownikach tj. login, hasło oraz ich aktualny status. Komunikacja z klientami odbywa się przy pomocy struktury Packet, która oprócz danych zawiera informacje o ich rodzaju i rozmiarze. Program używa Mutex’ów do synchronizacji dostępu do współdzielonych zasobów. 
Podstawowe obsługi błędów są implementowane przy użyciu std::cerr do wyświetlania komunikatów o błędach i wyjątków dla błędów krytycznych. Poprawnie działający serwer na wyjściu wyświetla podstawowe komunikaty np. o nowych połączeniach, operacjach w bazie danych lub wiadomościach.

Instrukcja uruchomienia (podanie portu, na którym będzie działał serwer jest obowiązkowe!):
```
cd server
mkdir build
cd build
cmake ..
make
./server <port>
```
**Klient**
Program klienta to aplikacja okienkowa napisana również w języku C++ z wykorzystaniem Frameworka Qt.

Po nawiązaniu połączenia, użytkownik musi wprowadzić login i hasło, a następnie zalogować się lub zarejestrować. Informacje o rezultatach tych działań są wyświetlane w oknie informacyjnym. Program wyświetla listę dostępnych użytkowników, pobierając informacje od serwera. Po zalogowaniu, lista ta jest odświeżana. Użytkownik może rozpocząć rozmowę z innym użytkownikiem, co prowadzi do wysłania żądania rozmowy do serwera.

Po nawiązaniu rozmowy, użytkownik może wysyłać wiadomości tekstowe. Możliwe jest prowadzenie wielu konwersacji jednocześnie. Informacje o nowych wiadomościach wyświetlają się na czerwono w polu informacyjnym. Użytkownik może zakończyć rozmowę, co skutkuje wysłaniem informacji o zamknięciu rozmowy do serwera i aktualizacją listy aktywnych rozmów. Taką informację otrzymuje także odbiorca naszych wiadomości. Użytkownik ma także możliwość wylogowania się. Po zamknięciu programu lub rozłączeniu z serwerem, odpowiednie akcje są wykonywane, takie jak zamknięcie gniazda i wylogowanie z serwera.
&nbsp Niektóre przyciski są niedostępne w trakcie określonych momentów działania programu tak, aby zapobiec sytuacjom takim, jak np. żądanie ponownego zalogowania bez wcześniejszego wylogowania.

Aby uruchomić aplikację klienta należy skorzystać z programu Qt Creator i otworzyć plik client.pro, następnie ctrl+R.

**Komunikacja**

Struktura przesyłanych pomiędzy klientem i serwerem pakietów:
```
struct Packet{
packetType type;	// typ pakietu
int size;		// rozmiar pakietu
char* data;	// przesyłane dane
};
```
Struktura zawierająca wiadomość przesyłaną pomiędzy klientami:
```
struct Message{
std::string sender;
int receiver;
std::string text;
};
```
W projekcie występuje wiele typów pakietów (enum packetType). Poniższa tabela przestawia przykładowe schematy komunikacji (klient odpytuje, serwer odpowiada):

|**Klient**|**Serwer**|
| :-: | :-: |
|P\_ASK\_LOGIN\_USER – sprawdzenie istnienia użytkownika|P\_USER\_NOT\_EXIST lub P\_USER\_EXIST – odpowiedź w zależności od istnienia użytkownika|
|P\_SEND\_LOGIN, P\_SEND\_PASSWORD – dane do rejestracji|P\_SIGNUP\_SUCCESS – pomyślna rejestracja|
|P\_SEND\_LOGIN\_LOG, P\_SEND\_PASSWORD\_LOG – prześlij dane do logowania|P\_USER\_ALREADY\_LOGGED\_IN, P\_LOGIN\_SUCCES, P\_WRONG\_PASSWORD – odopowiedź zależna od tego, czy użytkownik istnieje i podał dobre hasło|
|P\_LOGOUT\_REQUEST – żądanie wylogowania|P\_LOGOUT\_CONFIRM – pomyślne wylogowanie|
|P\_USERS\_NEW\_CHAT – żądanie utworzenia czatu przez użytkownika X z użytkownikiem Y|P\_NEW\_CHAT\_REQUEST – wyślij do użytkownika Y propozycję utworzenia czatu z X|
|P\_YES, P\_NO – wyraź zgodę na utworzenie czatu|P\_YES, P\_NO – przekaż zgodę do odbiorcy|
|P\_MESSAGE\_DEST, P\_\_MESSAGE\_TEXT – prześlij dane odbiory oraz treść wiadomości|P\_MESSAGE\_SEND – wyślij wiadomość do odbiorcy|
|P\_CLOSE\_CHAT – żądanie zakończenia czatu|P\_CLOSE\_CHAT – informuj odbiorcę o zakończeniu czatu przez nadawcę|
