//Programa : Relogio com modulo RTC DS1307
//Autor : FILIPEFLOP

//Carrega a biblioteca do RTC DS1307
#include <DS1307.h>
#include "pitches.h"
#include <EEPROM.h>
#define a 11
#define b 13
#define c 7
#define d 8
#define e 9
#define f 12
#define g 6
#define h1 2
#define h2 3
#define m1 4
#define m2 5
#define h 11
#define nil 12
#define dot 14

// notes in the melody:
int melody[] = {

	NOTE_C2,
	NOTE_AS4,
	NOTE_C2,
	NOTE_AS4,
	NOTE_C2,
	NOTE_AS4,
	NOTE_C2,
	NOTE_AS4,
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {

	10,
	2,
	10,
	2,
	10,
	2,
	10,
	2,

};
byte numbs[14][8] = {
	//0
	{a, b, c, d, e, f, 0, 0},
	//1
	{b, c, 0, 0, 0, 0, 0, 0},
	//2
	{a, b, g, e, d, 0, 0, 0},
	//3
	{a, b, c, d, g, 0, 0, 0},
	//4
	{b, c, g, f, 0, 0, 0, 0},
	//5
	{a, c, d, g, f, 0, 0, 0},
	//6
	{a, c, d, e, f, g, 0, 0},
	//7
	{a, b, c, 0, 0, 0, 0, 0},
	//8
	{a, b, c, d, e, f, g, 0},
	//9
	{a, b, c, d, f, g, 0, 0},
	//-
	{0, 0, 0, 0, 0, g, 0, 0},
	//h
	{f, g, c, e, 0, 0, 0, 0},
	//n
	{0, 0, 0, 0, 0, 0, 0, 0},
	//_
	{0, 0, 0, 0, 0, d, 0, 0}
};

int alarme_hora, alarme_minuto;

//Modulo RTC DS1307 ligado as portas A4 e A5 do Arduino
DS1307 rtc(A4, A5);

void setup()
{
	//Aciona o relogio
	rtc.halt(false);
	//Definicoes do pino SQW/Out
	rtc.setSQWRate(SQW_RATE_1);
	rtc.enableSQW(true);

	for (int i = 2; i < 14; i++)
	{
		pinMode(i, OUTPUT);
		digitalWrite(i, 0);
	}
	pinMode(16, OUTPUT);
	digitalWrite(16, 1);
	alarme_hora = EEPROM.read(0)*10+EEPROM.read(1);
	alarme_minuto = EEPROM.read(2)*10+EEPROM.read(3);
	Serial.begin(115200);
}
// meter um init.
void loop()
{
	Serial.println("ALARME: "+String(alarme_hora) + ":" + String(alarme_minuto));
	bool ct1 = true;
	bool ct2 = true;
	bool fg1 = false;
	bool fg2 = false;
	int botao, previous, current;
	bool ctrl= true,ctrlSecondsBlink = false,ctrlSecondsBlinkFlag = true ;
	int num[6];
	char *dayOW, dayOfWeek;
	while (1)
	{	
		previous = num[5];
		rtc.getTimeInt(num);
		current = num[5];
		ctrlSecondsBlink = blinkDots(&previous, &current, ctrlSecondsBlink);
		Serial.println(current);

		dayOW = rtc.getDOWStr(FORMAT_SHORT);
		dayOfWeek = dayOW[0];
		relogio(num[0], num[1], num[2], num[3], 5, ctrlSecondsBlink);
		if( dayOfWeek != 'S' && alarme_hora == num[0]*10+num[1] && alarme_minuto == num[2]*10+num[3] && ctrl){
			ctrl = false;
			melodia(num);
			ct1 = false;
			ct2 = false;
			delay(10000);
		}else if (alarme_minuto != num[2]*10+num[3] && !ctrl){
			ctrl = true;
		}

		botao = button(&ct1, &ct2, &fg1, &fg2);
		if (botao == 2)
		{
			ct1 = false;
			ct2 = false;
			confRelogio(num, &ct1, &ct2, &fg1, &fg2);
		}
		else if (botao == 1)
		{
			ct1 = false;
			ct2 = false;
			alarme(num, &ct1, &ct2, &fg1, &fg2);
		}
	}
}

bool blinkDots(int *secondsPrev, int *secondsCurr, bool ctrlSecondsBlink){

	if(*secondsPrev != *secondsCurr){
		*secondsPrev = *secondsCurr;
		return !ctrlSecondsBlink;
	}
}
void melodia(int *num)
{
	// iterate over the notes of the melody:
	unsigned long timeNote = millis();
	unsigned long timeToReadTime = millis();
	int ant, botao=0;
	bool ctrl = false;
	bool ct1 = false;
	bool ct2 = false;
	bool fg1 = false;
	bool fg2 = false;
	for (int thisNote = 0; thisNote < 8; )
	{
		int noteDuration = 1000 / noteDurations[thisNote];
		int pauseBetweenNotes = noteDuration * 1.30;
		if (millis() - timeNote < pauseBetweenNotes && ctrl) 
		{
			ctrl = false;
			tone(10, melody[thisNote], noteDuration);
		}
		else if(millis() - timeNote >= pauseBetweenNotes && !ctrl)
		{
			ctrl = true;
			noTone(10);
			thisNote++;
			if(thisNote == 8){
				thisNote = 0;
			}
			timeNote = millis();
		}
		if (millis() - timeToReadTime > 10) 
		{
			timeToReadTime = millis();
			botao = button(&ct1, &ct2, &fg1, &fg2);
			ant = num[3];
			rtc.getTimeInt(num);
			if(ant != num[3] || botao == 1 || botao == 2){
				thisNote = 9;
			}
			relogio(num[0], num[1], num[2], num[3], 1, true);
		}
		
	}
}

void alarme(int *num, bool *ctrl1, bool *ctrl2, bool *flag1, bool *flag2)
{
	Serial.println("alarme mode");
	int ha0, ha1, ha2, ha3, dataInt[10], diaD, diaU, mesD, mesU, anoD, anoU, dia, mes, ano, limDias, hora, minuto;
	char cc, aux[10], *novaData = rtc.getDateStr(FORMAT_SHORT, FORMAT_LITTLEENDIAN, '.');
	for (int i = 0; i < 10; i++)
	{
		cc = novaData[i];
		dataInt[i] = cc - '0';
		//Serial.println(dataInt[i]);
	}
	ha0 = alarme_hora/10;
	ha1 = alarme_hora%10;
	ha2 = alarme_minuto/10;
	ha3 = alarme_minuto%10;
	//num[0]*10 + num[1];
	//num[2]*10 + num[3];
	int controle_funcao = 1, clickButton;
	while (1)
	{

		hora = ha0 * 10 + ha1;
		minuto = ha2 * 10 + ha3;
		//Serial.println(String(dia) + ":"+ String(mes)+":"+String(ano));
		clickButton = button(ctrl1, ctrl2, flag1, flag2);
		if (clickButton == 1 && controle_funcao == 1)
		{
			if (ha3 + 1 < 10)
			{
				ha3++;
			}
			else if (ha2 + 1 <= 5)
			{
				ha2++;
				ha3 = 0;
			}
			else
			{
				ha2 = 0;
				ha3 = 0;
			}
		}else if (clickButton == 1 && controle_funcao == 2)
		{
			if (ha1 + 1 <= 9 && hora + 1 < 24)
			{
				ha1++;
			}
			else if (ha0 + 1 <= 2)
			{
				ha0++;
				ha1 = 0;
			}
			else
			{
				ha0 = 0;
				ha1 = 0;
			}
		}else if (clickButton == 2)
		{
			controle_funcao++;
		}
		if (controle_funcao== 1)
		{
			relogio(10, 10, ha2, ha3, 20, true);
		}
		else if (controle_funcao== 2)
		{
			relogio(ha0, ha1, h, nil, 20, true);
		}
		else if (controle_funcao > 2)
		{
			Serial.println("Alarm out");
			break;
		}
	}
	hora = ha0 * 10 + ha1;
	minuto = ha2 * 10 + ha3;
	alarme_hora = hora;
	alarme_minuto = minuto;
	EEPROM.write(0, ha0);
	EEPROM.write(1, ha1);
	EEPROM.write(2, ha2);
	EEPROM.write(3, ha3);
}
void confRelogio(int *num, bool *ctrl1, bool *ctrl2, bool *flag1, bool *flag2)
{
	Serial.println("config mode");
	int dayOfWeek,ha0, ha1, ha2, ha3, dataInt[10], diaD, diaU, mesD, mesU, anoD, anoU, dia, mes, ano, limDias, hora, minuto;
	char *dayOW, cc, aux[10], *novaData = rtc.getDateStr(FORMAT_SHORT, FORMAT_LITTLEENDIAN, '.');
	char dayowStr[8] = {'1', 'M', 'T','W','T','F','S','S'};
	for (int i = 0; i < 10; i++)
	{
		cc = novaData[i];
		dataInt[i] = cc - '0';
		//Serial.println(dataInt[i]);
	}
	anoU = dataInt[7];
	anoD = dataInt[6];
	mesU = dataInt[4];
	mesD = dataInt[3];
	diaU = dataInt[1];
	diaD = dataInt[0];
	limDias = 31;
	ha0 = num[0];
	ha1 = num[1];
	ha2 = num[2];
	ha3 = num[3];
	rtc.getTimeInt(num);
	dayOW = rtc.getDOWStr(FORMAT_SHORT);
	//Serial.println(dayOW);
	for(int i =1; i < 8; i++){
		if(dayOW[0] == dayowStr[i]){
			dayOfWeek = i;
		}
	}
	//num[0]*10 + num[1];
	//num[2]*10 + num[3];
	int controle_funcao = 1, clickButton;
	while (1)
	{

		dia = diaD * 10 + diaU;
		mes = mesD * 10 + mesU;
		ano = 2000 + anoD * 10 + anoU;
		hora = ha0 * 10 + ha1;
		minuto = ha2 * 10 + ha3;
		//Serial.println(String(dia) + ":"+ String(mes)+":"+String(ano));
		clickButton = button(ctrl1, ctrl2, flag1, flag2);
		if (clickButton == 1 && controle_funcao == 1)
		{
			if (ha3 + 1 < 10)
			{
				ha3++;
			}
			else if (ha2 + 1 <= 5)
			{
				ha2++;
				ha3 = 0;
			}
			else
			{
				ha2 = 0;
				ha3 = 0;
			}
		}
		else if (clickButton == 1 && controle_funcao == 2)
		{
			if (ha1 + 1 <= 9 && hora + 1 < 24)
			{
				ha1++;
			}
			else if (ha0 + 1 <= 2)
			{
				ha0++;
				ha1 = 0;
			}
			else
			{
				ha0 = 0;
				ha1 = 0;
			}
		}
		else if (clickButton == 1 && controle_funcao == 3)
		{
			if (anoU + 1 <= 9)
			{
				anoU++;
			}
			else if (anoD + 1 <= 4)
			{
				anoD++;
				anoU = 0;
			}
			else
			{
				anoD = 0;
				anoU = 1;
			}
		}
		else if (clickButton == 1 && controle_funcao == 4)
		{
			if (mesU + 1 <= 9 && mes + 1 <= 12)
			{
				mesU++;
			}
			else if (mesD + 1 < 2)
			{
				mesD++;
				mesU = 0;
			}
			else
			{
				mesD = 0;
				mesU = 1;
			}
		}
		else if (clickButton == 1 && controle_funcao == 5)
		{

			if (mes <= 7 && mes % 2 != 0)
			{
				// mês impar, dias vão até 31
				limDias = 31;
			}
			else if (mes <= 7)
			{
				// mês par
				if (ano % 4 == 0 && (ano % 400 == 0 || ano % 100 != 0))
				{

					//ano bissexto
					if (mes == 2)
					{
						Serial.println("BISSEXTO");
						//fevereiro, dias vão até 29
						limDias = 29;
					}
					else
					{
						// mês par, dias vão até 30
						limDias = 30;
					}
				}
				else
				{
					Serial.println("NAO BISSEXTO");
					// mês par, dias vão até 30
					limDias = 30;
				}
				// mês par, dias vão até 31
			}
			else if (mes % 2 == 0)
			{
				limDias = 31;
			}
			else
			{
				limDias = 30;
			}

			if (diaU + 1 < 10 && dia + 1 <= limDias)
			{
				diaU++;
			}
			else if (diaD + 1 <= 2 && limDias == 29)
			{
				diaD++;
				diaU = 0;
			}
			else if (diaD + 1 <= 3 && limDias != 29)
			{
				diaD++;
				diaU = 0;
			}
			else
			{
				diaD = 0;
				diaU = 1;
			}
		}
		if (clickButton == 1 && controle_funcao == 6)
		{
			if (dayOfWeek + 1 < 8)
			{
				dayOfWeek++;
			}
			else
			{
				dayOfWeek = 1;
			}
		}
		else if (clickButton == 2)
		{
			controle_funcao++;
		}
		if (controle_funcao == 1)
		{
			//configurar minuto
			relogio(10, 10, ha2, ha3, 10, true);
		}
		else if (controle_funcao == 2)
		{
			//configurar hora
			relogio(ha0, ha1, h, nil, 10, true);
		}
		else if (controle_funcao == 3)
		{
			//configurar ano
			relogio(2, 0, anoD, anoU, 15, false);
		}
		else if (controle_funcao == 4)
		{
			//configurar mes
			relogio(13, 13, mesD, mesU, 15, true);
		}
		else if (controle_funcao == 5)
		{
			//configurar dia
			relogio(diaD, diaU, 13, 13, 15, true);
		}
		else if (controle_funcao == 6)
		{
			//configurar dia da semana
			relogio(dayOfWeek, 13, 13, 13, 18, true);
		}
		else if (controle_funcao > 5)
		{
			Serial.println("config out");
			break;
		}
	}
	dia = diaD * 10 + diaU;
	mes = mesD * 10 + mesU;
	ano = 2000 + anoD * 10 + anoU;
	hora = ha0 * 10 + ha1;
	minuto = ha2 * 10 + ha3;
	rtc.setDOW(dayOfWeek);
	rtc.setTime(hora, minuto, 0); //Define o horario
	rtc.setDate(dia, mes, ano);	  //Define o dia, mes e ano
}

int button(bool *ctrl1, bool *ctrl2, bool *flag1, bool *flag2)
{
	int bot = analogRead(A3);
	if (bot > 400 && bot < 800 && *ctrl1)
	{
		*ctrl1 = false;
		return 1;
	}
	else if (bot < 400 && !*ctrl1)
	{
		*ctrl1 = true;
	}
	else if (bot > 800 && *ctrl2)
	{
		*ctrl2 = false;
		return 2;
	}
	else if (bot < 400 && !*ctrl2)
	{
		*ctrl2 = true;
	}
	return 0;
}

void clean()
{
	for (int i = 2; i < 14; i++)
	{
		if(i!=10){
			digitalWrite(i, 0);
		}
	}
}

void relogio(int i, int j, int k, int l, int tempo, bool blinkFlag)
{
	printNumb(i, h1, tempo, blinkFlag);
	printNumb(j, h2, tempo, blinkFlag);
	printNumb(k, m1, tempo, blinkFlag);
	printNumb(l, m2, tempo, blinkFlag);
}

void printNumb(int num, int ds, int tempo, bool blickFlag)
{
	clean();
	for (int i = 0; i < 8; i++)
	{
		digitalWrite(numbs[num][i], 1);
	}
	if(blickFlag){
		digitalWrite(16, 1);
	}else{
		digitalWrite(16, 0);
	}
	digitalWrite(ds, 1);
	delay(tempo);
	digitalWrite(ds, 0);
}
