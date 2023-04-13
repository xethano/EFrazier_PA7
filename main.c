// Authors: Deitel & Deitel - C How to Program

//-----------------------------------------------------
// includes
//-----------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

//-----------------------------------------------------
// forward declarations
//-----------------------------------------------------

void shuffle(int wDeck[][13]);
void deal(const int wDeck[][13], const char* wFace[], const char* wSuit[]);

//-----------------------------------------------------
// types and structs
//-----------------------------------------------------

typedef struct _Card
{
	int Suit; // 0-3
	int Face; // 0-12
} Card;

typedef struct _Player
{
	Hand Hand;
	int Money;
	int Bet;
} Player;

typedef enum _HandRank
{
	HIGH_CARD = 1,
	ONE_PAIR,
	TWO_PAIR,
	THREE_OF_A_KIND,
	STRAIGHT,
	FLUSH,
	FULL_HOUSE,
	FOUR_OF_A_KIND,
	STRAIGHT_FLUSH,
	ROYAL_FLUSH
} HandRank;

typedef struct _Hand
{
	Card Cards[5];
	int SignificantFaceCards[5];
	HandRank Rank;
} Hand;

//-----------------------------------------------------
// variables come next
//-----------------------------------------------------

Player PlayerHuman;
Player Dealer;
int CardsLeftInDeck = 52;
int HandCardCount = 5;

//-----------------------------------------------------
// functions
//-----------------------------------------------------

// sort the cards inline, return them sorted
void SortCardsByFaceThenSuit(Hand* pHand)
{
	// use a special histogram. Array is by "Face" value of the card.
	// Each rank is stored in the slot as a power of 4: 1, 4, 16, 64
	// After we fill the histogram, we read it out by Face. This will also 
	// sort by Suit, after Rank

	int Histogram[13] = { 0 };
	for (int i = 0; i < HandCardCount; i++)
	{
		int Suit = pHand->Cards[i].Suit;
		int Face = pHand->Cards[i].Face;
		Histogram[Face] += (int)pow(4, Suit); // stores each suit as a power of 4. 1, 4, 16, 64. So, 5 = heart + spade, 80 = spade + club
	}

	int index = 0;
	for (int i = 0; i < 13; i++)
	{
		int h = Histogram[i];
		for (int s = 0; s < 4; s++)
		{
			int p = (int)pow(4, s);
			bool HasPower = ((h / p) % 4) == 1;
			if (HasPower)
			{
				pHand->Cards[index].Suit = s;
				pHand->Cards[index].Face = i;
				index++;
			}
		}
	}
}

void GetFaceHistogram(Card Cards[], int Histogram[] ) // Ace counts as both 0 and 13. This enables us to find straights
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
	for (int face = 13; face >= 1 ; face--)
	{
		if (FaceHistogram[face] == 2)
		{
			pairs[pairs_found] = face;
			pairs_found += 1;
		}
		else if(FaceHistogram[face] == 1)
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
		else if(FaceHistogram[face] == 1)
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
	//at this point you have nothing
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

	if( pHuman->Hand.Rank > pDealer->Hand.Rank)
	{
		return 1;
	}
	if(pHuman->Hand.Rank < pDealer->Hand.Rank)
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

void shuffle(Card deck[])
{
	for (int card = 0; card < 52; card++)
	{
		deck[card].Suit = card / 13;
		deck[card].Face = card % 13;
	}

	for (int i = 0; i < 52; i++)
	{
		int swap_with = rand() % 52;
		Card temp = deck[i];
		deck[i] = deck[swap_with];
		deck[swap_with] = temp;
	}

	CardsLeftInDeck = 52;
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

int main(void)
{
	/* initialize suit array */
	const char* suit[4] = { "Hearts", "Diamonds", "Clubs", "Spades" };

	/* initialize face array */
	const char* face[13] = { "Ace", "Deuce", "Three", "Four", "Five", "Six", "Seven", "Eight",
		"Nine", "Ten", "Jack", "Queen", "King" };

	/* initalize deck array */
	Card deck[52];

	srand((unsigned)time(NULL)); /* seed random-number generator */

	shuffle(deck);

	// not sure what 'deal' is even supposed to do

	deal(deck, face, suit);

	// deal each player 5 cards. For every card we use up from the deck, set it's values to -1, -1 so we don't pick it again.
	// this would be a lot easier with a single array...

	while (true)
	{
		// if there aren't enough cards left, shuffle a new deck
		if (CardsLeftInDeck < 10)
		{
			shuffle(deck);
		}

		// Ask for bet. Can quit here

		// deal the cards

		for (int i = 0; i < 5; i++)
		{
			PlayerHuman.Hand.Cards[i] = deck[CardsLeftInDeck-1];
			CardsLeftInDeck--;
		}
		for (int i = 0; i < 5; i++)
		{
			Dealer.Hand.Cards[i] = deck[CardsLeftInDeck-1];
			CardsLeftInDeck--;
		}

		// show the cards

		// bet again: quit, fold, call, raise

		// ask player for how many cards to trade in
		
		// bet again: quit, fold, call, raise

		// show all cards

		// repeat
	}



	return 0;
}

