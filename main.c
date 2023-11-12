// Authors: Deitel & Deitel - C How to Program

//-----------------------------------------------------
// includes
//-----------------------------------------------------

#include "Functions.h"

//-----------------------------------------------------
// forward declarations
//-----------------------------------------------------


//-----------------------------------------------------
// functions
//-----------------------------------------------------

// sort the cards inline, return them sorted



int main(void)
{

	srand((unsigned)time(NULL)); /* seed random-number generator */

	shuffle(&TheDeck);

	// not sure what 'deal' is even supposed to do

	deal(TheDeck.Cards, face, suit);

	// deal each player 5 cards. For every card we use up from the deck, set it's values to -1, -1 so we don't pick it again.
	// this would be a lot easier with a single array...

	do
	{
		printf("Pick starting amount of Monies:");
		scanf("%d", &PlayerHuman.Money);
		if (PlayerHuman.Money < 100)
		{
			printf("Pick something reasonable, at least 100 Monies...\n");
		}

	} while (PlayerHuman.Money < 100);

	Dealer.Money = PlayerHuman.Money;

	while (true)
	{
		bool bQuit = PlayEntireRound();
		if (bQuit)
		{
			break;
		}
	}


	printf( "Thanks for playing\n" );

	return 0;
}

