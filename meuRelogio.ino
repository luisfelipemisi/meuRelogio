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
#define P	14
#define O	15
#define A	16
#define L	17
#define R	18
#define C	19
#define N	20
#define F	21
#define U	22
#define T	23
#define WORKTIME 25
#define SHORTBREAK 5
#define LONGBREAK 15

	
// notes in the melody:
// change this to make the song slower or faster
int tempo = 140;

// change this to whichever pin you want to use
int buzzer = 10;

// notes of the moledy followed by the duration.
// a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
// !!negative numbers are used to represent dotted notes,
// so -4 means a dotted quarter note, that is, a quarter plus an eighteenth!!
int melody[] = {
  
  // The Lick 
  NOTE_B3,2
  
};
byte numbs[24][8] = {
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
	//null
	{0, 0, 0, 0, 0, 0, 0, 0},
	//_
	{0, 0, 0, 0, 0, d, 0, 0},
	//P
	{e, f, a, g, b, 0, 0, 0},
	//O
	{a, b, c, d, e, f, 0, 0},
	//A
	{a, b, c, 0, e, f, g, 0},
	//L
	{f, e, d, 0, 0, 0, 0, 0},
	//R
	{e, f, a, b, g, c, 0, 0},
	//C
	{d, e, f, a, 0, 0, 0, 0},
	//N
	{a, b, c, 0, e, f, 0, 0},
	//F
	{e, f, a, g, 0, 0, 0, 0},
	//U
	{0, b, c, d, e, f, 0, 0},
	//T
	{a, b, c, 0, 0, 0, 0, 0}
	
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
	alarme_hora = EEPROM.read(0) * 10 + EEPROM.read(1);
	alarme_minuto = EEPROM.read(2) * 10 + EEPROM.read(3);
	Serial.begin(115200);
}
// meter um init.
void loop()
{
	//Serial.println("ALARME: " + String(alarme_hora) + ":" + String(alarme_minuto));
	bool ctrlClick_1 = false;
	bool ctrlClick_2 = false;
	bool fg1 = false;
	bool fg2 = false;
	int botao, previous, current;
	bool ctrlAlarm = true, ctrlSecondsBlink = false;
	int num[6];
	unsigned long int timeStandBy = millis();
	int dayOfWeek = rtc.getDOWInt();
	Serial.println(dayOfWeek);
	while (1)
	{
		dayOfWeek = rtc.getDOWInt();
		dysplayRelogio(num, &previous, &current, &ctrlSecondsBlink, 5);
		if (dayOfWeek != 6 && dayOfWeek != 7  && alarme_hora == num[0] * 10 + num[1] && alarme_minuto == num[2] * 10 + num[3] && ctrlAlarm )
		{
			ctrlAlarm = false;
			activateAlarm(num, &ctrlClick_1, &ctrlClick_2, &fg1, &fg2);
			timeStandBy = millis();
			while (millis() - timeStandBy < 10000)
			{
				dysplayRelogio(num, &previous, &current, &ctrlSecondsBlink, 10);
			}
		}
		else if (alarme_minuto != num[2] * 10 + num[3] && !ctrlAlarm )
		{
			ctrlAlarm = true;
		}

		botao = button(&ctrlClick_1, &ctrlClick_2, &fg1, &fg2);

		if (botao == 2)
		{	
			comands(num, &ctrlClick_1, &ctrlClick_2, &fg1, &fg2);
		}
	}
}

void comands(int *num, bool *ctrl1, bool *ctrl2, bool *flag1, bool *flag2){
	int botao = button(ctrl1, ctrl2, flag1, flag2), funcao =0;
	unsigned long int timeToBack = millis();
	while(1){
		if(millis() - timeToBack > 60000){
			return;
		}
		botao = button(ctrl1, ctrl2, flag1, flag2);
		if (botao == 2)
		{
			funcao++;
		}

		if(funcao == 0){
			relogio(A,L,A ,10 , 8, false);
			if(botao == 1){
				alarme(num, ctrl1, ctrl2, flag1, flag2);
			}
		}else if(funcao == 1){
			relogio(C,O,N ,F , 8, false);
			if(botao == 1){
				confRelogio(num, ctrl1, ctrl2, flag1, flag2);
			}
		}else if(funcao == 2){
			relogio(P,O, nil ,nil , 8, false);
			if(botao == 1){
				pomodoro(ctrl1, ctrl2, flag1, flag2);
			}
		}else if(funcao == 3){
			return;
		}
	}
}

void pomodoro(bool *ctrl1, bool *ctrl2, bool *flag1, bool *flag2){
	int minuto, segundos = 0;
	int pomoNum[6],previous,current,funcao = 0, botao, count =0;
	bool ctrlWhile = true, ctrlFuncao = true ;
	while(ctrlWhile){

		if(!ctrlFuncao){
			previous = 1;
			//rtc.getTimeInt(pomoNum);
			current = 2;
			if(current != previous){
				segundos--;
				if(segundos < 0){
					minuto--;
					if(minuto < 0 && segundos < 0){
						ctrlFuncao = true;
						count++;
						segundos=0;
						song(ctrl1, ctrl2, flag1, flag2);
					}else{
						segundos = 59;
					}
				}
			}	
		}
		botao = button(ctrl1, ctrl2, flag1, flag2);
		if(botao == 1 || botao == 2){
			ctrlFuncao = true;
			segundos = 0;
		}
		if(ctrlFuncao){
			if( funcao == 0 && ( count == 1 || count == 3 || count == 5)){
				//mando para o shortbreak
				funcao = 1;
			}else if( funcao == 1 && ( count == 2 || count == 4 || count == 6)){
				//mando para o worktime
				funcao = 0;
			}else if( funcao == 0  && count == 7){
				//mando para o longbreak
				funcao = 2;
			}else if( funcao == 2  && count == 8){
				//mando para o longbreak
				count = 0;
				funcao = 0;
			}
		}
		while(ctrlFuncao){
			
			botao = button(ctrl1, ctrl2, flag1, flag2);

			//pomodoro configurado
			if(botao == 2){
				funcao++;
				if(funcao >3){
					funcao = 0;
				}
			}else if(botao == 1 && funcao < 3){
				//play
				ctrlFuncao = false;
			}else if(botao == 1 ){
				return;
			}

			if(funcao == 0){
				minuto = WORKTIME;
				relogio( minuto/10, minuto%10 , segundos/10 , segundos%10 , 5 , true );
			}
			else if(funcao == 1){
				minuto = SHORTBREAK;
				relogio( minuto/10, minuto%10 , segundos/10 , segundos%10 , 5 , true );
			}
			else if(funcao == 2){
				minuto = LONGBREAK;
				relogio( minuto/10, minuto%10 , segundos/10 , segundos%10 , 5 , true );
			}
			else if(funcao == 3){
				relogio( O , U , T , nil , 5 , false );
			}
			
		}
		relogio( minuto/10, minuto%10 , segundos/10 , segundos%10 , 5, true );
	}
	
}

void dysplayRelogio(int *num, int *previous, int *current, bool *ctrlSecondsBlink, int timeUpdateDisplay)
{
	*previous = num[5];
	rtc.getTimeInt(num);
	*current = num[5];
	*ctrlSecondsBlink = blinkDots(previous, current, *ctrlSecondsBlink);
	relogio(num[0], num[1], num[2], num[3], timeUpdateDisplay, *ctrlSecondsBlink);
}

bool blinkDots(int *secondsPrev, int *secondsCurr, bool ctrlSecondsBlink)
{

	if (*secondsPrev != *secondsCurr)
	{
		*secondsPrev = *secondsCurr;
		return !ctrlSecondsBlink;
	}
}

void activateAlarm(int *num,bool *ctrl1, bool *ctrl2, bool *flag1, bool *flag2)
{
	// iterate over the notes of the melody:
	unsigned long timeToReadTime = millis();
	int ant, botao = 0;
	bool ctrl = false;
	bool ctrlWhile = true;
	

	while (ctrlWhile)
	{
		ctrlWhile = song(ctrl1, ctrl2, flag1, flag2);
		timeToReadTime = millis();
		while (ctrlWhile && millis() - timeToReadTime < 300)
		{
			botao = button(ctrl1, ctrl2, flag1, flag2);
			ant = num[3];
			rtc.getTimeInt(num);
			if (ant != num[3] || botao == 1 || botao == 2)
			{
				return ;
			}
			relogio(num[0], num[1], num[2], num[3], 5, true);
		}
	}
}

bool song(bool *ctrl1, bool *ctrl2, bool *flag1, bool *flag2){
	int notes = sizeof(melody) / sizeof(melody[0]) / 2;
	int wholenote = (60000 * 4) / tempo;
	int divider = 0, noteDuration = 0;
	int ant, botao = 0;
	unsigned long int timeToReadTime = millis();
	for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2)
		{
			// calculates the duration of each note
			divider = melody[thisNote + 1];
			if (divider > 0)
			{
				// regular note, just proceed
				noteDuration = (wholenote) / divider;
			}
			else if (divider < 0)
			{
				// dotted notes are represented with negative durations!!
				noteDuration = (wholenote) / abs(divider);
				noteDuration *= 1.5; // increases the duration in half for dotted notes
			}

			// we only play the note for 90% of the duration, leaving 10% as a pause
			tone(buzzer, melody[thisNote], noteDuration * 0.9);

			// Wait for the specief duration before playing the next note.
			timeToReadTime = millis();
			while (millis() - timeToReadTime < noteDuration)
			{
				botao = button(ctrl1, ctrl2, flag1, flag2);
				if (botao == 1 || botao == 2)
				{
					return false;
				}
			}
			//delay(noteDuration);

			// stop the waveform generation before the next note.
			noTone(buzzer);
		}
		return true;
}

void alarme(int *num, bool *ctrl1, bool *ctrl2, bool *flag1, bool *flag2)
{
	int ha0, ha1, ha2, ha3, dataInt[10], diaD, diaU, mesD, mesU, anoD, anoU, dia, mes, ano, limDias, hora, minuto;
	char cc, *novaData = rtc.getDateStr(FORMAT_SHORT, FORMAT_LITTLEENDIAN, '.');
	for (int i = 0; i < 10; i++)
	{
		cc = novaData[i];
		dataInt[i] = cc - '0';
	}
	ha0 = alarme_hora / 10;
	ha1 = alarme_hora % 10;
	ha2 = alarme_minuto / 10;
	ha3 = alarme_minuto % 10;
	int controle_funcao = 1, clickButton;
	while (1)
	{
		hora = ha0 * 10 + ha1;
		minuto = ha2 * 10 + ha3;

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
		else if (clickButton == 2)
		{
			controle_funcao++;
		}
		
		if (controle_funcao == 1)
		{
			relogio(10, 10, ha2, ha3, 20, true);
		}
		else if (controle_funcao == 2)
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
	int dayOfWeek, ha0, ha1, ha2, ha3, dataInt[10], diaD, diaU, mesD, mesU, anoD, anoU, dia, mes, ano, limDias, hora, minuto;
	char cc, *novaData = rtc.getDateStr(FORMAT_SHORT, FORMAT_LITTLEENDIAN, '.');
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
	dayOfWeek = rtc.getDOWInt();

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
		else if (clickButton == 1 && controle_funcao == 6)
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
		if (i != 10)
		{
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
		if(numbs[num][i] != 0){
			digitalWrite(numbs[num][i], 1);
		}
	}
	if (blickFlag)
	{
		digitalWrite(16, 1);
	}
	else
	{
		digitalWrite(16, 0);
	}
	digitalWrite(ds, 1);
	delay(tempo);
	digitalWrite(ds, 0);
}
