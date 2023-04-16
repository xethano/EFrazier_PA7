#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>

//-----------------------------------------------------
// types and structs
//-----------------------------------------------------

typedef struct _Card
{
	int Suit; // 0-3
	int Face; // 0-12
} Card;

typedef enum _HandRank
{
	HIGH_CARD = 0,
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

const char* HandRankString[];

typedef struct _Hand
{
	Card Cards[5];
	int SignificantFaceCards[5];
	HandRank Rank;
} Hand;

typedef struct _Player
{
	Hand Hand;
	int Money;
	int Bet;
	int RoundsPlayed;
} Player;

typedef struct _Deck
{
	Card Cards[52];
	int CardsLeft;
} Deck;

//-----------------------------------------------------
// variables come next
//-----------------------------------------------------

Player PlayerHuman;
Player Dealer;
Deck TheDeck;
int HandCardCount;
int ante;

/* initialize suit array */
const char* suit[4];

/* initialize face array */
const char* face[14];

/* initalize deck array */

void SortCardsByFaceThenSuit(Hand* pHand);
void GetFaceHistogram(Card Cards[], int Histogram[]); // Ace counts as both 0 and 13. This enables us to find straights
// used to find flushes
void GetSuitHistogram(Card Cards[], int Histogram[]);

bool IsOnePair(Card Cards[], int* pSignificantArray);

bool IsStraight(Card Cards[], int* pStartsAtFace); // don't forget A can be bottom or top

bool IsTwoPair(Card Cards[], int* pPair1Face, int* pPair2Face, int* pLoneCardFace);

bool IsThreeOfAKind(Card Cards[], int* faceof3, int* facesolo1, int* facesolo2);

bool IsFourOfAKind(Card Cards[], int* faceof4, int* facesolo1);
bool IsFullHouse(Card Cards[], int* pThreesFace, int* pTwosFace); // pass out the face # for which card has the 3s, and face # for the 2s
bool IsFlush(Card Cards[], int* pHighFace);
bool IsStraightFlush(Card Cards[], int* pStartsAtFace);
bool IsRoyalFlush(Card Cards[], int* pStartsAtFace);
void ComputeHandRank(Hand* pHand);
int CompareFaces(int sigface1, int sigface2);
int CompareHands(Player* pHuman, Player* pDealer);
void shuffle(Deck* pDeck);
void deal(const Card deck[], const char* wFace[], const char* wSuit[]);
void ShowHand( Player* pPlayer, const char* szTitle);
const char* GetRankString(HandRank rank);

bool PlayEntireRound();
Card drawCard(Deck* pDeck);
void PlayerQuitPrintStatsAndExit();
void DropDealerCards();

#endif
