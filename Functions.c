#include "Functions.h"

const char* HandRankString[] = {
	"High Card",
	"One Pair",
	"Two Pair",
	"Three of a kind",
	"Straight",
	"Normal Flush",
	"Full house",
	"Four of a Kind",
	"Straight Flush",
	"Royal Flush"
};

/* initialize suit array */
const char* suit[4] = { "Hearts", "Diamonds", "Clubs", "Spades" };

/* initialize face array */
const char* face[14] = { "Ace", "Deuce(2)", "Three", "Four", "Five", "Six", "Seven", "Eight",
	"Nine", "Ten", "Jack", "Queen", "King", "Ace" /*high ace*/ };

int HandCardCount = 5;
int ante = 5;

void GetFaceHistogram(Card Cards[], int Histogram[]) // Ace counts as both 0 and 13. This enables us to find straights
{
	for (int i = 0; i <= 13; i++)
	{
		Histogram[i] = 0;
	}

	for (int i = 0; i < HandCardCount; i++)
	{
		Histogram[Cards[i].Face]++;
		if (Cards[i].Face == 0)
		{
			Histogram[13]++; // low ace is also a high ace
		}
	}

	// 1 pair would be a '2' in any of the histogram slots
	// 2 pair would be a '2' in two different slots
	// 4 of a kind would be a '4' in any one slot
	// 5 of a kind = not possible. dummy.
}

// used to find flushes
void GetSuitHistogram(Card Cards[], int Histogram[])
{
	for (int i = 0; i < 4; i++)
	{
		Histogram[i] = 0;
	}

	for (int i = 0; i < HandCardCount; i++)
	{
		Histogram[Cards[i].Suit]++;
	}
}

bool IsOnePair(Card Cards[], int* pSignificantArray)
{
	bool bFoundPair = false;
	int solo_index = 1; // start at index 1
	int FaceHistogram[14];
	GetFaceHistogram(Cards, FaceHistogram);
	// finds high aces, not low aces
	// if we don't test for a pair first, it could overwrite 'solos' in pSignificantArray with 5 of them. which would break.
	for (int face = 13; face >= 1; face--)
	{
		if (FaceHistogram[face] == 2)
		{
			pSignificantArray[0] = face;
			bFoundPair = true;
		}
	}
	if (!bFoundPair)
	{
		return false;
	}

	// store the face values in the significant array, highest first... Starting at index 1.
	for (int face = 13; face >= 1; face--)
	{
		if (FaceHistogram[face] == 1)
		{
			pSignificantArray[solo_index] = face;
			solo_index++;
		}
	}

	return true;
}

bool IsStraight(Card Cards[], int* pStartsAtFace) // don't forget A can be bottom or top
{
	// 1. find min face card
	// 2. see if there is a straight starting at min card
	int FaceHistogram[14];
	GetFaceHistogram(Cards, FaceHistogram);
	for (int i = 0; i < 14 - 5; i++) // check straights for low ace as well as high ace
	{
		if (FaceHistogram[i] == 0)
		{
			continue;
		}
		bool bFoundStraight = true;
		for (int j = i + 1; j < i + 5; j++)
		{
			if (FaceHistogram[j] == 0)
			{
				bFoundStraight = false;
				break;
			}
		}
		if (bFoundStraight)
		{
			*pStartsAtFace = i;
			return true;
		}
	}
	*pStartsAtFace = -1;
	return false;
}

bool IsTwoPair(Card Cards[], int* pPair1Face, int* pPair2Face, int* pLoneCardFace)
{
	int pairs[3];
	int pairs_found = 0;
	int FaceHistogram[14];
	GetFaceHistogram(Cards, FaceHistogram);
	// finds high aces, not low aces
	for (int face = 13; face >= 1; face--)
	{
		if (FaceHistogram[face] == 2)
		{
			pairs[pairs_found] = face;
			pairs_found += 1;
		}
		else if (FaceHistogram[face] == 1)
		{
			*pLoneCardFace = face;
		}
	}

	if (pairs_found == 2)
	{
		*pPair1Face = pairs[0];
		*pPair2Face = pairs[1];
		return true;
	}
	return false;

}

bool IsThreeOfAKind(Card Cards[], int* faceof3, int* facesolo1, int* facesolo2)
{
	*faceof3 = -1;
	*facesolo1 = -1;
	*facesolo2 = -1;
	int FaceHistogram[14];
	GetFaceHistogram(Cards, FaceHistogram);
	// find high aces first
	for (int face = 13; face >= 1; face--)
	{
		if (FaceHistogram[face] == 3)
		{
			*faceof3 = face;
		}
		else if (FaceHistogram[face] == 1)
		{

			if (*facesolo1 != -1)
			{
				*facesolo2 = face;
			}
			else
			{
				*facesolo1 = face;
			}

		}

	}

	if (*faceof3 != -1)
	{
		return true;
	}
	return false;
}

bool IsFourOfAKind(Card Cards[], int* faceof4, int* facesolo1)
{
	*faceof4 = -1;
	*facesolo1 = -1;
	int FaceHistogram[14];
	GetFaceHistogram(Cards, FaceHistogram);
	// find high aces
	for (int face = 13; face >= 1; face--)
	{
		if (FaceHistogram[face] == 4)
		{
			*faceof4 = face;
		}
		else if (FaceHistogram[face] == 1)
		{
			*facesolo1 = face;
		}

	}

	if (*faceof4 != -1)
	{
		return true;
	}
	return false;
}

bool IsFullHouse(Card Cards[], int* pThreesFace, int* pTwosFace) // pass out the face # for which card has the 3s, and face # for the 2s
{
	int Histogram[14];
	GetFaceHistogram(Cards, Histogram);
	// figure out if histogram has a 2 in one of the slots, and a 3 in one of the slots...
	int ThreesFace = -1;
	int TwosFace = -1;
	for (int i = 1; i < 14; i++) // ignore low ace, only look at high aces
	{
		if (Histogram[i] == 3)
		{
			ThreesFace = i;
		}
		if (Histogram[i] == 2)
		{
			TwosFace = i;
		}
	}
	if (ThreesFace == -1 || TwosFace == -1)
	{
		return false;
	}
	*pThreesFace = ThreesFace;
	*pTwosFace = TwosFace;
	return true;
}

bool IsFlush(Card Cards[], int* pHighFace)
{
	int SuitHistogram[4];
	GetSuitHistogram(Cards, SuitHistogram);
	int Histogram[14];
	GetFaceHistogram(Cards, Histogram);
	for (int i = 0; i < 4; i++)
	{
		if (SuitHistogram[i] == HandCardCount)
		{
			// finds HIGH aces, not low!
			for (int face = 13; face >= 1; face--)
			{
				if (Histogram[face] == 1)
				{
					*pHighFace = face;
					break;
				}
			}
			return true;
		}
	}
	return false;
}

bool IsStraightFlush(Card Cards[], int* pStartsAtFace)
{
	int highcard = -1;
	bool is_straight = IsStraight(Cards, pStartsAtFace);
	bool is_flush = IsFlush(Cards, &highcard);
	if (is_straight && is_flush)
	{
		return true;
	}
	return false;
}

bool IsRoyalFlush(Card Cards[], int* pStartsAtFace)
{
	bool isStraightFlush = IsStraightFlush(Cards, pStartsAtFace);
	if (!isStraightFlush) return false;
	if (*pStartsAtFace != 9) // index 9 = a '10' face card
	{
		return false;
	}
	return true;
}

void ComputeHandRank(Hand* pHand)
{
	/*
	HIGH_CARD
	ONE_PAIR, done
	TWO_PAIR, done
	THREE_OF_A_KIND, done
	STRAIGHT, done
	FLUSH, done
	FULL_HOUSE, done
	FOUR_OF_A_KIND, done
	STRAIGHT_FLUSH, done
	ROYAL_FLUSH done
	*/

	// test for highest ranks FIRST
	bool HasRoyalFlush = IsRoyalFlush(pHand->Cards, &pHand->SignificantFaceCards[0]);
	if (HasRoyalFlush)
	{
		pHand->Rank = ROYAL_FLUSH;
		return;
	}

	bool HasStraightFlush = IsStraightFlush(pHand->Cards, &pHand->SignificantFaceCards[0]);
	if (HasStraightFlush)
	{
		pHand->Rank = STRAIGHT_FLUSH;
		return;
	}

	bool HasFourOfAKind = IsFourOfAKind(pHand->Cards, &pHand->SignificantFaceCards[0], &pHand->SignificantFaceCards[1]);
	if (HasFourOfAKind)
	{
		pHand->Rank = FOUR_OF_A_KIND;
		return;
	}

	bool HasFullHouse = IsFullHouse(pHand->Cards, &pHand->SignificantFaceCards[0], &pHand->SignificantFaceCards[1]);
	if (HasFullHouse)
	{
		pHand->Rank = FULL_HOUSE;
		return;
	}

	bool HasFlush = IsFlush(pHand->Cards, &pHand->SignificantFaceCards[0]);
	if (HasFlush)
	{
		pHand->Rank = FLUSH;
		return;
	}

	bool HasStraight = IsStraight(pHand->Cards, &pHand->SignificantFaceCards[0]);
	if (HasStraight)
	{
		pHand->Rank = STRAIGHT;
		return;
	}

	bool HasThreeOfAKind = IsThreeOfAKind(pHand->Cards, &pHand->SignificantFaceCards[0], &pHand->SignificantFaceCards[1], &pHand->SignificantFaceCards[2]);
	if (HasThreeOfAKind)
	{
		pHand->Rank = THREE_OF_A_KIND;
		return;
	}

	bool HasTwoPair = IsTwoPair(pHand->Cards, &pHand->SignificantFaceCards[0], &pHand->SignificantFaceCards[1], &pHand->SignificantFaceCards[2]);
	if (HasTwoPair)
	{
		pHand->Rank = TWO_PAIR;
		return;
	}

	bool HasOnePair = IsOnePair(pHand->Cards, pHand->SignificantFaceCards);
	if (HasOnePair)
	{
		pHand->Rank = ONE_PAIR;
		return;
	}

	//at this point you have nothing. Let's arrange the SignificantFaceCards from highest to lowest
	int Histogram[14];
	GetFaceHistogram(pHand->Cards, Histogram);
	int index = 0;
	for (int i = 13; i >= 0; i--)
	{
		if (Histogram[i] > 0)
		{
			pHand->SignificantFaceCards[index] = i;
			index++;
		}
	}
	pHand->Rank = HIGH_CARD;
	return;
}

int CompareFaces(int sigface1, int sigface2)
{
	if (sigface1 == sigface2)
	{
		return 0;
	}
	else if (sigface1 > sigface2)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

int CompareHands(Player* pHuman, Player* pDealer)
{
	// compare hand ranks. If ranks are the same, then a sub-compare must be done.
	ComputeHandRank(&pHuman->Hand);
	ComputeHandRank(&pDealer->Hand);
	//1 means human wins, -1 dealer wins, 0 is draw

	if (pHuman->Hand.Rank > pDealer->Hand.Rank)
	{
		return 1;
	}
	if (pHuman->Hand.Rank < pDealer->Hand.Rank)
	{
		return -1;
	}

	// oh oh, they're the same rank. Compare significant face cards.
	switch (pDealer->Hand.Rank)
	{
	case ROYAL_FLUSH:
		// automatic draw. Can't have higher than royal flush
		return 0;
	case STRAIGHT_FLUSH:
	{
		// compare starting card of straights
		return CompareFaces(pHuman->Hand.SignificantFaceCards[0], pDealer->Hand.SignificantFaceCards[0]);
	}
	case FOUR_OF_A_KIND:
		return CompareFaces(pHuman->Hand.SignificantFaceCards[0], pDealer->Hand.SignificantFaceCards[0]);
	case FULL_HOUSE:
		return CompareFaces(pHuman->Hand.SignificantFaceCards[0], pDealer->Hand.SignificantFaceCards[0]);
	case FLUSH:
		return CompareFaces(pHuman->Hand.SignificantFaceCards[0], pDealer->Hand.SignificantFaceCards[0]);
	case STRAIGHT:
		return CompareFaces(pHuman->Hand.SignificantFaceCards[0], pDealer->Hand.SignificantFaceCards[0]);
	case THREE_OF_A_KIND:
	{
		//compares the face value of the three of a kind, then moves onto the next highest card, and so..
		//(there are only 2 loner cards if you do have a 3 of a kind)
		int i = CompareFaces(pHuman->Hand.SignificantFaceCards[0], pDealer->Hand.SignificantFaceCards[0]);
		if (i == -1 || i == 1)
		{
			return i;
		}
		i = CompareFaces(pHuman->Hand.SignificantFaceCards[1], pDealer->Hand.SignificantFaceCards[1]);
		if (i == -1 || i == 1)
		{
			return i;
		}
		i = CompareFaces(pHuman->Hand.SignificantFaceCards[2], pDealer->Hand.SignificantFaceCards[2]);
		return i;
	}
	case TWO_PAIR:
	{
		int i = CompareFaces(pHuman->Hand.SignificantFaceCards[0], pDealer->Hand.SignificantFaceCards[0]);
		if (i == -1 || i == 1)
		{
			return i;
		}
		i = CompareFaces(pHuman->Hand.SignificantFaceCards[1], pDealer->Hand.SignificantFaceCards[1]);
		if (i == -1 || i == 1)
		{
			return i;
		}
		i = CompareFaces(pHuman->Hand.SignificantFaceCards[2], pDealer->Hand.SignificantFaceCards[2]);
		return i;
	}
	case ONE_PAIR:
	{
		int i = CompareFaces(pHuman->Hand.SignificantFaceCards[0], pDealer->Hand.SignificantFaceCards[0]);
		if (i == -1 || i == 1)
		{
			return i;
		}
		i = CompareFaces(pHuman->Hand.SignificantFaceCards[1], pDealer->Hand.SignificantFaceCards[1]);
		if (i == -1 || i == 1)
		{
			return i;
		}
		i = CompareFaces(pHuman->Hand.SignificantFaceCards[2], pDealer->Hand.SignificantFaceCards[2]);
		if (i == -1 || i == 1)
		{
			return i;
		}
		i = CompareFaces(pHuman->Hand.SignificantFaceCards[3], pDealer->Hand.SignificantFaceCards[3]);
		return i;
	}
	default: // all that is left is high-card
	{
		int i = CompareFaces(pHuman->Hand.SignificantFaceCards[0], pDealer->Hand.SignificantFaceCards[0]);
		if (i == -1 || i == 1)
		{
			return i;
		}
		i = CompareFaces(pHuman->Hand.SignificantFaceCards[1], pDealer->Hand.SignificantFaceCards[1]);
		if (i == -1 || i == 1)
		{
			return i;
		}
		i = CompareFaces(pHuman->Hand.SignificantFaceCards[2], pDealer->Hand.SignificantFaceCards[2]);
		if (i == -1 || i == 1)
		{
			return i;
		}
		i = CompareFaces(pHuman->Hand.SignificantFaceCards[3], pDealer->Hand.SignificantFaceCards[3]);
		if (i == -1 || i == 1)
		{
			return i;
		}
		i = CompareFaces(pHuman->Hand.SignificantFaceCards[4], pDealer->Hand.SignificantFaceCards[4]);
		return i;
	}
	} // switch
}

void shuffle(Deck* pDeck)
{
	printf("\nShuffling the a new deck of cards...\n\n");

	for (int card = 0; card < 52; card++)
	{
		pDeck->Cards[card].Suit = card / 13;
		pDeck->Cards[card].Face = card % 13;
	}

	for (int i = 0; i < 52; i++)
	{
		int swap_with = rand() % 52;
		Card temp = pDeck->Cards[i];
		pDeck->Cards[i] = pDeck->Cards[swap_with];
		pDeck->Cards[swap_with] = temp;
	}

	pDeck->CardsLeft = 52;
}

/* deal cards in deck */
void deal(const Card deck[], const char* wFace[], const char* wSuit[])
{
	/* deal each of the 52 cards */
	for (int card = 0; card < 52; card++)
	{
		printf("%5s of %-8s%c", wFace[deck[card].Face], wSuit[deck[card].Suit], card % 2 == 0 ? '\t' : '\n');
	}
}

Card drawCard(Deck* pDeck)
{
	Card c = pDeck->Cards[pDeck->CardsLeft- 1];
	pDeck->CardsLeft--;
	if (pDeck->CardsLeft == 0)
	{
		printf("\nReshuffling deck, got too low...\n");
		shuffle(pDeck);
	}
	return c;
}

bool PlayEntireRound()
{
	printf("=======================================\n");
	printf("=                                     =\n");
	printf("= Round: %3d, Money left: %6d      =\n", PlayerHuman.RoundsPlayed, PlayerHuman.Money );
	printf("= The ante is %3d                     =\n", ante);
	printf("=======================================\n");
	printf("\n");
	PlayerHuman.RoundsPlayed++;
	char Response = ' ';
	do
	{

		printf("Do you want to (A)nte, or (Q)uit? ");
		scanf(" %c", &Response);
		Response = toupper(Response);
		if (Response == 'A')
		{
			if (PlayerHuman.Money < ante)
			{
				printf("Sorry, you don't have enough money to even ante!\n");
				Response = ' ';
			}
		}
	} while (Response != 'A' && Response != 'Q');

	printf("\n");

	if (Response == 'Q')
	{
		PlayerQuitPrintStatsAndExit();
		return true;
	}

	// now start the round. Automatic ante...
	PlayerHuman.Money -= ante;
	PlayerHuman.Bet = 0;
	Dealer.Money -= ante;

	// PlayerHuman's Bet doesn't go up here, this 'ante' is always "to the house". Doesn't get recovered upon a win or a draw!

	for (int i = 0; i < 5; i++)
	{
		PlayerHuman.Hand.Cards[i] = drawCard(&TheDeck);
	}
	for (int i = 0; i < 5; i++)
	{
		Dealer.Hand.Cards[i] = drawCard(&TheDeck);
	}

	ShowHand(&PlayerHuman, "PLAYER HAND");

	// this is necessary so user doesn't enter a garbage character accidentally and end up with a Fold
	int bet = -2;
	do
	{
		printf("Bet amount ('F' to fold, 0 to check):");
		char buf[256];
		scanf("%s", buf);
		if (buf[0] == 'F')
		{
			bet = -1;
		}
		else
		{
			bet = atoi(buf);
			if (PlayerHuman.Money - bet < 0)
			{
				printf("Nice try. We don't give out loans.\n");
				bet = -2;
			}
		}
	} while (bet == -2);

	if (bet == -1)
	{
		// player folded
		printf("Player folded.\n");
		return false; // still keep playing. Player loses their ante.
	}
	if (bet == 0)
	{
		printf("Player checked. No actual bet. Let's see what dealer does...\n");
	}

	// give the dealer a chance to: fold, call, or raise. If dealer raises you, you must bet to keep in it.
	int DealerBet = 0;
	ComputeHandRank(&Dealer.Hand);
	HandRank hr = Dealer.Hand.Rank;
	// depending on the size of YOUR bet, and the dealer's hand, the dealer will modify their bet
	if (hr == ONE_PAIR)
	{
		DealerBet = bet;
	}
	if (hr >= TWO_PAIR)
	{
		if (DealerBet < ante * 5)
			DealerBet = ante * 5;
	}
	if (hr >= THREE_OF_A_KIND)
	{
		if (DealerBet < ante * 10)
			DealerBet = ante * 10;
		if (DealerBet < bet * 2)
			DealerBet = bet * 2;
	}
	if (hr >= FLUSH)
	{
		DealerBet = bet * 3;
		if (DealerBet < 50)
		{
			DealerBet = 50;
		}
	}
	if (hr >= FULL_HOUSE)
	{
		DealerBet = Dealer.Money;
	}

	if (DealerBet == bet)
	{
		printf("Dealer calls.\n");
	}
	if (DealerBet < bet)
	{
		printf("Dealer folds. You get your ante back, plus dealer's.\n");
		PlayerHuman.Money += ante * 2;
		return false;
	}
	if (DealerBet > bet)
	{
		printf("Dealer raises his bet to %d. You must match this bet or fold. You agree? (If you have less than the amount, and agree, you will be all-in: Y or N? \n", DealerBet);
		char response;
		scanf(" %c", &response);
		response = toupper(response);
		if (response == 'N')
		{
			// player folds
			printf("You fold.\n");
			return false; // you lost just the ante, not too bad.
		}
		else
		{
			bet = DealerBet;
			if (bet > PlayerHuman.Money)
			{
				bet = PlayerHuman.Money;
			}
			printf("You agree to the raise! Your bet is now %d\n", bet);
		}
	}

	Dealer.Money -= DealerBet;
	PlayerHuman.Money -= bet;
	PlayerHuman.Bet = bet;

	bool DropCards[5] = { 0 }; // initialize all to false
	printf("Which cards do you want to get rid of? Max of 3. If you don't enter valid cards 1-5, I'll ignore them. :");
	char buf[256];
	gets(buf); // this will read in the enter. need to do it AGAIN
	gets(buf);

	int len = (int)strlen(buf);
	for (int i = 0; i < len; i++)
	{
		char ch = buf[i];
		int WhichCard = ch - '1';
		if (WhichCard >= 0 && WhichCard < HandCardCount)
		{
			DropCards[WhichCard] = true;
		}
	}

	int dropped = 0;
	for (int i = 0; i < HandCardCount; i++)
	{
		if (DropCards[i])
		{
			printf("replacing card %d...\n", i + 1);
			PlayerHuman.Hand.Cards[i] = drawCard(&TheDeck);
			dropped++;
			if (dropped == 3)
			{
				printf("dropping a max of 3, the rest are ignored.\n");
				break;
			}
		}
	}

	DropDealerCards();

	ShowHand(&PlayerHuman, "PLAYER HAND");

	int bet2 = -2;
	do
	{
		printf("Final Bet amount ('F' to fold, '0' to check):");
		char buf[256];
		scanf("%s", buf);
		if (buf[0] == 'F')
		{
			bet2 = -1;
		}
		else
		{
			bet2 = atoi(buf);
			if (PlayerHuman.Money - bet2 < 0)
			{
				printf("Nice try. We don't give out loans.\n");
				bet2 = -2;
			}
		}
	} while (bet2 == -2);

	if (bet2 == -1)
	{
		// player folded
		printf("Player folded. Ouch. Lost ante and 1st bet.\n");
		return false; // still keep playing. Player loses their ante.
	}
	if (bet == 0)
	{
		printf("Player checked 2nd bet. Let's see what dealer does...\n");
	}

	int total_bet = bet + bet2;

	// dealer should decide to fold, call, or raise

	int DealerBet2 = 0;
	ComputeHandRank(&Dealer.Hand);
	hr = Dealer.Hand.Rank;
	// depending on the size of YOUR bet, and the dealer's hand, the dealer will modify their bet
	if (hr == ONE_PAIR)
	{
		DealerBet2 = ante;
	}
	if (hr >= TWO_PAIR)
	{
		if (DealerBet2 < ante * 5)
			DealerBet2 = ante * 5;
	}
	if (hr >= THREE_OF_A_KIND)
	{
		if (DealerBet2 < ante * 10)
			DealerBet2 = ante * 10;
	}
	if (hr >= FLUSH)
	{
		DealerBet2 = bet2 * 3;
		if (DealerBet2 < 50)
		{
			DealerBet2 = 50;
		}
	}
	if (hr >= FULL_HOUSE)
	{
		DealerBet2 = Dealer.Money;
	}

	if (DealerBet2 == bet2)
	{
		printf("Dealer calls.\n");
	}
	if (DealerBet2 < bet2)
	{
		printf("Dealer folds. You get your ante back, plus dealer's.\n");
		PlayerHuman.Money += ante * 2;
		return false;
	}
	if (DealerBet2 > bet2)
	{
		printf("Dealer raises his bet to %d. You must match this bet or fold. You agree? (If you have less than the amount, and agree, you will be all-in: Y or N? \n", 
			DealerBet2);
		char response;
		scanf(" %c", &response);
		response = toupper(response);
		if (response == 'N')
		{
			// player folds
			printf("You fold.\n");
			return false; // you lost the ante + what you bet.
		}
		else
		{
			bet2 = DealerBet2;
			if (bet2 > PlayerHuman.Money)
			{
				bet2 = PlayerHuman.Money;
			}
			printf("You agree to the raise! Your bet is now %d\n", bet2);
		}
	}

	Dealer.Money -= DealerBet2;
	PlayerHuman.Money -= bet2;

	ShowHand(&Dealer, "DEALER HAND");

	printf("\nSHOWDOWN.....\n\n");

	// compare hands
	int YouWin = CompareHands(&PlayerHuman, &Dealer);
	if (YouWin == 0)
	{
		printf("You and the dealer tied! Split the pot!\n");
		PlayerHuman.Money += PlayerHuman.Bet + ante;
	}
	else if (YouWin > 0)
	{
		printf("You win! You made %d money!\n", PlayerHuman.Bet * 2 + ante * 2);
		PlayerHuman.Money += PlayerHuman.Bet * 2 + ante * 2;
	}
	else
	{
		printf("You lost.\n");
		Dealer.Money += PlayerHuman.Bet * 2 + ante * 2;
	}

	PlayerHuman.Bet = 0;
	Dealer.Bet = 0;

	return false; // = no quit

}

void ShowHand( Player * pPlayer, const char* szTitle)
{
	printf("%s:\n", szTitle);
	printf("----------\n");

	for (int card = 0; card < 5; card++)
	{
		int face_index = pPlayer->Hand.Cards[card].Face;
		int suit_index = pPlayer->Hand.Cards[card].Suit;
		printf("%d) %5s of %-8s\n", card + 1, face[face_index], suit[suit_index]);
	}
	printf("\n");

	ComputeHandRank(&pPlayer->Hand);
	const char* szRankString = HandRankString[pPlayer->Hand.Rank];
	switch (pPlayer->Hand.Rank)
	{
	case HIGH_CARD:
	{
		int FaceHistogram[14];
		GetFaceHistogram(pPlayer->Hand.Cards, FaceHistogram);
		int HighFace = 0;
		for (int i = 13; i >= 0; i--)
		{
			if (FaceHistogram[i] > 0)
			{
				HighFace = i;
				break;
			}
		}
		printf("Rank: High card = %s\n", face[HighFace]);
		break;
	}
	case ONE_PAIR:
	{
		int FaceArray[5];
		IsOnePair(pPlayer->Hand.Cards, FaceArray);
		printf("Rank: One pair in %s's, kickers %s, %s, and %s\n", face[FaceArray[0]], face[FaceArray[1]], face[FaceArray[2]], face[FaceArray[3]]);
		break;
	}
	case TWO_PAIR:
	{
		int Pair1Face, Pair2Face, LoneFace1;
		IsTwoPair(pPlayer->Hand.Cards, &Pair1Face, &Pair2Face, &LoneFace1);
		printf("Rank: Two pair in %s's and %s's, kicker %s\n", face[Pair1Face], face[Pair2Face], face[LoneFace1]);
		break;
	}
	case THREE_OF_A_KIND:
	{
		int ThreesFace, LoneFace1, LoneFace2;
		IsThreeOfAKind(pPlayer->Hand.Cards, &ThreesFace, &LoneFace1, &LoneFace2);
		printf("Rank: 3 of a kind in %s's, kickers %s and %s\n", face[ThreesFace], face[LoneFace1], face[LoneFace2]);
		break;
	}
	case STRAIGHT:
	{
		int StartFace;
		IsStraight(pPlayer->Hand.Cards, &StartFace);
		printf("Rank: Straight, starting at %s\n", face[StartFace]);
		break;
	}
	case FLUSH:
	{
		int HighFace;
		IsFlush(pPlayer->Hand.Cards, &HighFace);
		printf("Rank: Flush, high face = %s\n", face[HighFace]);
		break;
	}
	case FULL_HOUSE:
	{
		int ThreesFace, TwosFace;
		IsFullHouse(pPlayer->Hand.Cards, &ThreesFace, &TwosFace);
		printf("Rank: Full house, %s's over %s's\n", face[ThreesFace], face[TwosFace]);
		break;
	}
	case FOUR_OF_A_KIND:
	{
		int FourFace, OtherFace;
		IsFourOfAKind(pPlayer->Hand.Cards, &FourFace, &OtherFace);
		printf("Rank: 4 of a kind in %s's, with %s kicker\n", face[FourFace], face[OtherFace]);
		break;
	}
	case STRAIGHT_FLUSH:
	{
		int StartFace;
		IsStraightFlush(pPlayer->Hand.Cards, &StartFace);
		printf("Rank: Straight flush starting at %s\n", face[StartFace]);
		break;
	}
	case ROYAL_FLUSH:
	{
		printf("Rank: *** A ROYAL FLUSH!?!!\n");
		break;
	}
	}

	printf("\n");
}

void PlayerQuitPrintStatsAndExit()
{
	// do something here
}

void DropDealerCards()
{
	ComputeHandRank(&Dealer.Hand);
	HandRank hr = Dealer.Hand.Rank;
	if (hr >= STRAIGHT)
	{
		// hey, it's already good. Do we try and go better?
		printf("Dealer chooses not to discard any cards. (Hope you have a good hand!)\n");
		return;
	}

	bool DropCard[5] = { 0 };
	int DropCardCount = 0;

	if (hr == THREE_OF_A_KIND)
	{
		int target1 = 0;
		int target2 = 0;
		int faceof3s = 0;

		IsThreeOfAKind(Dealer.Hand.Cards, &faceof3s, &target1, &target2);
		for (int i = 0; i < HandCardCount; i++)
		{
			if (Dealer.Hand.Cards[i].Face == target1 || Dealer.Hand.Cards[i].Face == target2)
			{
				DropCard[i] = true;
				DropCardCount++;
			}
		}
		
		// pick the two cards that aren't part of the 3 of a kind
	}
	else if (hr == TWO_PAIR)
	{
		int target1 = 0;
		int pair1_face = 0;
		int pair2_face = 0;
		IsTwoPair(Dealer.Hand.Cards, &pair1_face, &pair2_face, &target1);
		for (int i = 0; i < HandCardCount; i++)
		{
			if (Dealer.Hand.Cards[i].Face == target1 )
			{
				DropCard[i] = true;
				DropCardCount++;
			}
		}
	}
	else if (hr == ONE_PAIR)
	{
		int face_array[5];
		IsOnePair(Dealer.Hand.Cards, face_array);
		int val1 = face_array[1];
		int val2 = face_array[2];
		int val3 = face_array[3];
		for (int i = 0; i < HandCardCount; i++)
		{
			if (Dealer.Hand.Cards[i].Face == val1 || Dealer.Hand.Cards[i].Face == val2 || Dealer.Hand.Cards[i].Face == val3)
			{
				DropCard[i] = true;
				DropCardCount++; //should only go three times
			}
		}
	}
	else
	{
		for (int i = 0; i < HandCardCount; i++)
		{
			if (Dealer.Hand.Cards[i].Face < 9 && DropCardCount < 3)
			{
				DropCard[i] = true;
				DropCardCount++;
			}
		}
		// replace anything under 10
	}

	if (DropCardCount > 0)
	{
		printf("\nDealer is dropping %d cards\n", DropCardCount);
		for (int i = 0; i < HandCardCount; i++)
		{
			if (DropCard[i])
			{
				printf("Dealer swapped a card\n");
				Dealer.Hand.Cards[i] = drawCard(&TheDeck);
			}
		}
		printf("\n");
	}
}
