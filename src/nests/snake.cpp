#include <common/asl.h>
#include <nests/snake.h>

using namespace albaos;
using namespace albaos::common;
using namespace albaos::drivers;


asl ASLSNAKE;
Speaker PCSPEAKER;

void printfTUI(char*, uint8_t, uint8_t, uint8_t, uint8_t);

// ported from OsakaOS to ALbaOS :)

void snakeTUI() {

	ASLSNAKE.TUI(0x00, 0x00, 0, 0, 0, 0, false);
	printfTUI(" (press ctrl+c to exit)", 0x0f, 0x00, 0, 0);
	printfTUI("SCORE = ", 0x0f, 0x00, 40, 0);

	for (int i = 0; i < 80; i++) {

		ASLSNAKE.putcharTUI(0xcd, 0x0f, 0x00, i, 1);
	}
}



void snakeInit() {

	//tail
	ASLSNAKE.putcharTUI(0xff, 0x06, 0x06, 40, 9);
    //body
	ASLSNAKE.putcharTUI(0xff, 0x0a, 0x0a, 40, 10);
	ASLSNAKE.putcharTUI(0xff, 0x0a, 0x0a, 40, 11);

	//head
	ASLSNAKE.putcharTUI(0xff, 0x02, 0x02, 40, 12);
}


void snake(char key) {

	static uint8_t x = 40, y = 12;
	static uint8_t tailX = 40, tailY = 9;
	volatile uint16_t* vidmem;


	static uint8_t bodyChar = 0;
	static uint8_t bodyFore = 0;
	static uint8_t tail = 0;
	static char lastChar = 'w';


	static bool vertical = 1;
	static bool up = 0;
	static bool right = 0;


	static uint8_t fruitX = 0;
	static uint8_t fruitY = 0;
	static bool fruit = false;


	static uint8_t score = 0;
	static bool dead = false;
	static uint16_t speed = 100;


	if (dead && key != 'r') {
			return;
	}
	if (score <= tail) {

		//remove snake body from previous tail
		ASLSNAKE.putcharTUI(0xff, 0x00, 0x00, tailX, tailY);


							   //some light green char in vidmem i guess
		vidmem = (volatile uint16_t*)0xb8000 + (80*(tailY-(1 * tailY != 25))+(tailX));
		uint16_t findU = *vidmem;

		vidmem = (volatile uint16_t*)0xb8000 + (80*(tailY+(1))+(tailX));
		uint16_t findD = *vidmem;

		vidmem = (volatile uint16_t*)0xb8000 + (80*(tailY)+(tailX-1));
		uint16_t findL = *vidmem;

		vidmem = (volatile uint16_t*)0xb8000 + (80*(tailY)+(tailX+1));
		uint16_t findR = *vidmem;


		uint16_t find = 0xffff;



		for (uint8_t i = 0; i < 4; i++) {

			switch (i) {

				case 0:
					vidmem = (volatile uint16_t*)0xb8000 + (80*(tailY-1)+(tailX));
					break;
				case 1:
					vidmem = (volatile uint16_t*)0xb8000 + (80*(tailY+1)+(tailX));
					break;
				case 2:
					vidmem = (volatile uint16_t*)0xb8000 + (80*(tailY)+(tailX-1));
					break;
				case 3:
					vidmem = (volatile uint16_t*)0xb8000 + (80*(tailY)+(tailX+1));
					break;
				default:
					break;
		}

			if ((find > *vidmem) && (*vidmem >> 12) == 0xa) {

				find = *vidmem;
			}
		}

		tailY -= (find == findU);
		tailY += (find == findD);
		tailX -= (find == findL);
		tailX += (find == findR);

		//print snake tail
		ASLSNAKE.putcharTUI(0xff, 0x06, 0x06, tailX, tailY);
	}

	tail = score;



	//print snake body
	ASLSNAKE.putcharTUI(bodyChar, bodyFore, 0x0a, x, y);


	if (lastChar != key) {

		switch (key) {

			case 'w':
				up = 1;
				vertical = 1;
				lastChar = 's';
				break;
			case 'a':
				right = 0;
				vertical = 0;
				lastChar = 'd';
				break;
			case 's':
				up = 0;
				vertical = 1;
				lastChar = 'w';
				break;
			case 'd':
				right = 1;
				vertical = 0;
				lastChar = 'a';
				break;
			case 'r':

				x = 40;
				y = 12;

				tailX = 40;
				tailY = 9;
				lastChar = 'w';

				bodyChar = 0;
				bodyFore = 0;

				vertical = 1;
				up = 0;
				right = 0;

				score = 0;
				tail = 0;
				speed = 100;

				dead = false;
				fruit = false;

				snakeTUI();
				snakeInit();
				ASLSNAKE.sleep(250);

				return;
				break;

			default:
				break;
		}
	}


	//move
	if (vertical) {
		if (up) {y--;} else {y++;}
	} else {
		if (right) {x++;} else {x--;}
	}



	//game over
	vidmem = (volatile uint16_t*)0xb8000 + (80*y+x);
	dead = ((*vidmem >> 12) == 0xa);


	if (x >= 80 || y <= 1 || y >= 25 || dead) {

		dead = true;

		ASLSNAKE.TUI(0x04, 0x04, 0, 0, 0, 0, false);
		printfTUI("-GAME OVER-", 0x0f, 0x04, 35, 10);

		printfTUI("Score was ", 0x0f, 0x04, 34, 12);
		char* foo = "   ";
		foo[0] = (score / 100) + 48;
		foo[1] = ((score / 10) % 10) + 48;
		foo[2] = (score % 10) + 48;
		printfTUI(foo, 0x0f, 0x04, 44, 12);

		printfTUI("Press 'r' to restart.", 0x0f, 0x04, 30, 13);

		PCSPEAKER.Speak(294);
		ASLSNAKE.sleep(300);

		PCSPEAKER.Speak(220);
		ASLSNAKE.sleep(300);

		PCSPEAKER.Speak(208);
		ASLSNAKE.sleep(300);

		return;
	}


	//spawn new fruit
	if (!fruit) {

		do {
			fruitX = (((uint8_t)ASLSNAKE.betterRandom()) % 78) + 2;
			fruitY = (((uint8_t)ASLSNAKE.betterRandom()) % 21) + 4;

			vidmem = (volatile uint16_t*)0xb8000 + (80*fruitY+fruitX);

		} while (*vidmem != 0x00ff);

		ASLSNAKE.putcharTUI(0xff, 0x04, 0x04, fruitX, fruitY);
	}

	fruit = true;


	//player gains point
	if (x == fruitX && y == fruitY) {

		score++;
		fruit = false;

		if (score % 5 == 0 && speed > 20) {

			speed -= 2;
		}

		PCSPEAKER.Speak(1000);
	}


	//print snake head
	ASLSNAKE.putcharTUI(0xff, 0x02, 0x02, x, y);
	bodyChar++;

	if (bodyChar == 0xff) {

		if (bodyFore >= 0x0f) {

			bodyFore = 0;
		}
		bodyFore++;
	}


	//print score
	ASLSNAKE.putcharTUI((score / 100) + 48, 0x0f, 0x00, 48, 0);
	ASLSNAKE.putcharTUI(((score / 10) % 10) + 48, 0x0f, 0x00, 49, 0);
	ASLSNAKE.putcharTUI((score % 10) + 48, 0x0f, 0x00, 50, 0);


	ASLSNAKE.sleep(speed);
}

