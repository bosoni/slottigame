/*
Slotti game
(c) mjt, 2019 
[mixut@hotmail.com]

Released under MIT license.

*/
#include "include.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define DATADIR "Data"

const int RULLALKM = 3, RULLALEN = 10, IMGLKM = 5, RULLASTOPPROSENTTI = 10;

string infoText = "";

struct SRulla
{
	int id[RULLALEN];
	int index;
	int speed = 0, yOffset = 0;
} rullat[RULLALKM];

static struct sSceneGame : public ZL_Scene
{
	ZL_Surface bg[3], img[IMGLKM];
	ZL_Font fntMain;
	bool playing = false;

	const int x = 314, y = -110;

	int money = 20, bet = 1;

	sSceneGame() : ZL_Scene(SCENE_GAME) { }

	void InitRullat()
	{
		for (int q = 0; q < RULLALKM; q++)
			for (int w = 0; w < RULLALEN; w++)
			{
				rullat[q].id[w] = rand() % IMGLKM;
				rullat[q].index = rand() % 5 + 2;
			}
	}

	virtual void InitGlobal()
	{
		InitRullat();

		srand(time(NULL));

		//fntMain = ZL_Font("Data/fntMain.png");
		fntMain = ZL_Font(DATADIR"/slotgame/digitalnumbers/DigitalNumbers-Regular.ttf", 40);
		fntMain.SetColor(ZL_Color(1, 0, 0, 1));

		bg[0] = ZL_Surface(DATADIR"/slotgame/bg.png");
		bg[1] = ZL_Surface(DATADIR"/slotgame/rullas.png");

		for (int q = 0; q < IMGLKM; q++)
		{
			string s = DATADIR"/slotgame/";
			s = s + std::to_string(q + 1);
			s = s + ".png";
			img[q] = ZL_Surface(s.c_str());
		}
	}

	virtual void InitAfterTransition()
	{
		ZL_Display::sigKeyDown.connect(this, &sSceneGame::OnKeyDown);
	}

	virtual void DeInitLeave(ZL_SceneType SceneTypeTo)
	{
		ZL_Display::AllSigDisconnect(this);
	}

	virtual void OnKeyDown(ZL_KeyboardEvent& e)
	{
		if (e.key == ZLK_ESCAPE) ZL_Application::Quit();

		if (e.key == ZLK_UP || e.key == ZLK_DOWN || e.key == ZLK_SPACE)
			infoText = "";
		else return;

		if (e.key == ZLK_UP && playing == false)
			if (money > bet && bet < 10)
				bet++;
		if (e.key == ZLK_DOWN && playing == false)
			if (bet > 1)
				bet--;


		if (e.key == ZLK_SPACE && playing == false)
		{
			if (money <= 0)
			{
				money = 0;
				return;
			}

			money -= bet;
			playing = true;

			for (int q = 0; q < RULLALKM; q++)
				rullat[q].speed = 20;
		}
	}

	virtual void Draw()
	{
		ZL_Display::ClearFill(ZL_Color(0,0,0.2f));

		if (playing)
		{
			// tarkista pyöriikö joku rulla
			int isPlaying = false;
			for (int q = 0; q < RULLALKM; q++)
				if (rullat[q].speed > 0) isPlaying = true;
			playing = isPlaying;

			// rullat ei pyöri enää, joten tarkistetaan voitot
			if (playing == false)
			{
				int win = 0;
				int q = 0;
				int index;

				index = (3 + rullat[q].index) % RULLALEN;
				int idx1 = rullat[q].id[index]; q++;
				index = (3 + rullat[q].index) % RULLALEN;
				int idx2 = rullat[q].id[index]; q++;
				index = (3 + rullat[q].index) % RULLALEN;
				int idx3 = rullat[q].id[index]; q++;

				if (idx1 == idx2)
				{
					win = 2; // 2 samaa vierekkäin
					if (idx2 == idx3)
						win = 3; // kolmaskin
				}

				if (win > 0)
				{
					// VOITTOLOGIIKAT TÄNNE:
					int mult = bet * win;

					// idx on kuvan indexi (idx0 == i1.png  jne)
					// laitetaan että eri kuvilla on eri arvo
					int winmoney = mult * (idx1 + 1);

					money += winmoney;

					infoText = "You won " + to_string(winmoney);
				}

				if (money < bet)
					bet = money;
			}
		}

		// rullien rendaus
		for (int q = 0; q < RULLALKM; q++)
		{
			for (int w = 0; w < RULLALEN; w++)
			{
				int idx = (w + rullat[q].index) % RULLALEN;

				if (w == 3)
					img[rullat[q].id[idx]].Draw(-2 + x + q * 129, rullat[q].yOffset + y + w * 128); // tsekkausrivi on hieman sivussa
				else
					img[rullat[q].id[idx]].Draw(x + q * 129, rullat[q].yOffset + y + w * 128);
			}

			// rullien varjostukset
			bg[1].Draw(x + q * 130, y + 250);
		}

		// rullaus
		if (playing == true)
		{
			for (int q = 0; q < RULLALKM; q++)
			{
				rullat[q].yOffset += rullat[q].speed * (s(ZL_Application::ElapsedTicks / 10));

				if (rullat[q].yOffset >= 128)
				{
					rullat[q].yOffset -= 128;
					rullat[q].index--;
					if (rullat[q].index < 0) rullat[q].index += RULLALEN;

					rullat[q].speed -= 0.1f;

					// pysäytetäänkö rulla
					if (rand() % 100 < RULLASTOPPROSENTTI || rullat[q].speed <= 1)
					{
						rullat[q].yOffset = 0;
						rullat[q].speed = 0;
					}
				}
			}
		}

		// rendaa masiina
		bg[0].Draw(0, 0);

		if (money <= 0 && playing == false)
			infoText = "Loser!  GAME OVER!";


		const int sx = 230, sy = 100;
		if (infoText == "")
		{
			string txt = "Money:" + to_string(money);
			fntMain.Draw(sx, 768 - sy, txt.c_str());
			txt = "Bet:" + to_string(bet);
			fntMain.Draw(sx + 300, 768 - sy, txt.c_str());
		}
		else fntMain.Draw(sx, 768 - sy, infoText.c_str());

	}

} SceneGame;
