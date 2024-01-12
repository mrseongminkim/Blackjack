#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

class Card {
  private:
    const string suit;
    const string face;
    const int value;
    Card *next;

  public:
    Card() : suit("temp"), face("temp"), value(0), next(NULL) {}
    Card(string s, string f, int v) : suit(s), face(f), value(v), next(NULL) {}
    string getSuit() const {return suit;}
    string getFace() const {return face;}
    int getValue() const {return value;}
    void setNext(Card *n) {next = n;}
    Card *getNext() {return next;}
    void printInfo() {cout << "suit : " << suit << " face : " << face << " value : " << value << endl;}
  
  //friend void Hand::printHand();
  friend class Hand;
};

class Hand {
  protected:
    Card *cards;

  public:
    Hand() : cards(NULL) {}
    void add(Card *c) {
      c->setNext(NULL);
      if (cards == NULL) {
        cards = c;
        return;
      }
      Card* temp = cards;
      Card* prev;
      while (temp->getNext() != NULL)
        temp = temp->getNext();
      temp->setNext(c);
    }
    void clear() {
      Card* temp = cards;
      while (temp != NULL) {
        Card* backup = temp->getNext();
        delete temp;
        temp = backup;
      }
      cards = NULL;
    }
    int getTotal() const {
      int total = 0;
      Card* temp = cards;
      while (temp != NULL) {
        total += temp->getValue();
        temp = temp->getNext();
      }
      return total;
    }
    bool isAce() const {
      int total = 0;
      int ace_num = 0;
      Card* temp = cards;
      while (temp != NULL) {
        if (temp->getFace() != "ACE")
          total += temp->getValue();
        else
        	ace_num++;
        temp = temp->getNext();
      }
      if (total + ace_num - 1 <= 10)
      	return true;
      return false;
    }
    void printHand() {
      bool already_changed = false;
      cout << "-------------------" << endl;
      Card* temp = cards;
      while (temp != NULL) {
        if (temp->getFace() == "ACE" && !already_changed) {
          if (isAce()) {
            int* changer = (int*)&temp->value;
            *changer = 11;
            already_changed = true;
          }
          else {
            int* changer = (int*)&temp->value;
            *changer = 1;
            already_changed = true;
          }
        }
        temp->printInfo();
        temp = temp->getNext();
      }
      cout << "-------------------" << endl;
    }
};

class Deck : public Hand {
  private:
    const string suits[4] = {"Clubs", "Diamonds", "Hearts", "Spades"};
    const string faces[13] = {"ACE", "2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King"};
    const int values[13] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10};
    Card* front_card;

  public:
    Deck() {
      cout << "\nDealing initial cards...\n" << endl;
      Card **card = createCards();
      shuffleDeck(card);
      createDeck(card);
    }

    Card **createCards() {
      static Card* card_array[52];
      Card** card_pointer = card_array;
      int count = 0;
      for (string suit : suits)
        for (int i = 0; i < 13; i++)
          card_array[count++] = new Card(suit, faces[i], values[i]);
      return card_pointer;
    }

    void shuffleDeck(Card **card) {
      srand((unsigned) time(0));
      for (int i = 0; i < 52; i++) {
        int temp_index = rand() % (52 - i) + i;
        Card* temp = card[i];
        card[i] = card[temp_index];
        card[temp_index] = temp;
      }
    }

    void createDeck(Card **card) {
      cards = card[0];
      Card* temp = cards;
      for (int i = 1; i < 52; i++) {
        temp->setNext(card[i]);
        temp = temp->getNext();
      }
      front_card = card[0];
    }

    Card *dealing() {
      Card* return_value = front_card;
      front_card = front_card->getNext();
      return return_value;
    }
    
    ~Deck() {
    	while (front_card != NULL) {
    		Card* next = front_card->getNext();
    		delete front_card;
    		front_card = next;
		}
	}
};

class GenericPlayer : public Hand {
  protected:
    string playerID;

  public:
    bool isBust() {
      if (getTotal() > 21)
        return true;
      return false;
    }
    void bust() {
      cout << "\n" << playerID << " has busted!" << endl;
    }
    ~GenericPlayer() {
    	clear();
	}
};

class Player : public GenericPlayer {
  public:
    Player() {playerID = "Player";}

    void win() const {
      cout << "\n" << playerID << " has won!\n" << "House has lost!" << endl;
      exit(EXIT_SUCCESS);
    }

    void lose() const {
      cout << "\n" << playerID << " has lost!\n" << "House has won!" << endl;
      exit(EXIT_SUCCESS);
    }

    void tie() const {
      cout << "\nTied!";
      exit(EXIT_SUCCESS);
    }

    void hit(Deck *deck) {add(deck->dealing());}
};

class House : public GenericPlayer {
  public:
    House() {playerID = "House";}

    void flipFirstCard() {
      cout << "House's first card" << endl;
      cout << "-------------------" << endl;
      cards->printInfo();
      cout << "-------------------\n" << endl;
    }
};

class Game {
  private:
    Deck *deck;
    Player *player;
    House *house;

  public:
    Game() {
      cout << "Let's play a round of BlackJack" << endl;
      player = new Player();
      house = new House();
      deck = new Deck();
    }
    
    void setTable() {
      house->clear();
      player->clear();
      house->add(deck->dealing());
      house->add(deck->dealing());
      house->flipFirstCard();
      player->add(deck->dealing());
      player->add(deck->dealing());
      cout << "Your initial cards ..." << endl;
      player->printHand();
	}

    void play() {
      setTable();
      while (player->getTotal() < 21) {
        char input;
        cout << "\nWould you like to have a card more? (y/n): ";
        cin >> input;
        switch (input) {
          case 'y':
          case 'Y':
            player->hit(deck);
            cout << "\nWhat you have ..." << endl;
            player->printHand();
            if (player->isBust()) {
              player->bust();
              player->lose();
            }
            if (player->getTotal() == 21)
              player->win();
            break;
          case 'n':
          case 'N':
            while (house->getTotal() < 17)
              house->add(deck->dealing());
            cout << "\nHouse's cards" << endl;
            house->printHand();
            cout << "\nWhat you have ..." << endl;
            player->printHand();
            if (house->isBust()) {
              house->bust();
              player->win();
            }
            if (house->getTotal() < player->getTotal())
              player->win();
            if (house->getTotal() > player->getTotal())
              player->lose();
            player->tie();
            break;
        }
      }
      cout << "!BLACK JACK!" << endl;
      player->win();
    }
    ~Game() {
    	delete deck;
    	delete player;
    	delete house;
	}
};

int main() {
  Game game;
  game.play();
  
  return 0;
}