	//Programa : Relogio com modulo RTC DS1307
	//Autor : FILIPEFLOP

	//Carrega a biblioteca do RTC DS1307
	#include <DS1307.h>
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

	byte numbs[14][8]= {
	//0 
	{a,b,c,d,e,f,0,0},
	//1 
	{b,c,0,0,0,0,0,0},
	//2 
	{a,b,g,e,d,0,0,0},
	//3 
	{a,b,c,d,g,0,0,0},
	//4 
	{b,c,g,f,0,0,0,0},
	//5 
	{a,c,d,g,f,0,0,0},
	//6 
	{a,c,d,e,f,g,0,0},
	//7 
	{a,b,c,0,0,0,0,0},
	//8 
	{a,b,c,d,e,f,g,0},
	//9 
	{a,b,c,d,f,g,0,0},
	//-
	{0,0,0,0,0,g,0,0},
	//h
	{f,g,c,e,0,0,0,0},
	//n
	{0,0,0,0,0,0,0,0},
	//_
	{0,0,0,0,0,d,0,0},
	};

	int ant, novo, bot, teste;

	//Modulo RTC DS1307 ligado as portas A4 e A5 do Arduino
	DS1307 rtc(A4, A5);

	int num[4];
	void setup()
	{
		//Aciona o relogio
		rtc.halt(false);
		// definido o pino do sensor de presença como entrada de sinal
		//As linhas abaixo setam a data e hora do modulo
		//e podem ser comentada apos a primeira utilizacao
		rtc.setDOW(THURSDAY);      //Define o dia da semana
		//rtc.setTime(16, 26, 0);     //Define o horario
		//rtc.setDate(1, 10, 2021);   //Define o dia, mes e ano

		//Definicoes do pino SQW/Out
		rtc.setSQWRate(SQW_RATE_1);
		rtc.enableSQW(true);
	
	for(int i=2;i<14;i++){
		pinMode(i,OUTPUT);
		digitalWrite(i,0);
	}
	
		
		Serial.begin(115200);
	}

	void loop()
	{   
		bool ct1 = true;
		bool ct2 = true;
		bool fg1 = false;
		bool fg2 = false;
		
		while(1){
		rtc.getTimeInt(num);
		relogio(num[0], num[1], num[2],num[3],1);
		if(button(&ct1, &ct2, &fg1, &fg2) == 2){
			confRelogio(num,&ct1, &ct2, &fg1, &fg2);
		}
		}
	}

	void confRelogio(int *num,bool *ctrl1, bool *ctrl2, bool *flag1, bool *flag2){
		Serial.println("config mode");
		int ha0,ha1,ha2,ha3,dataInt[10],diaD,diaU,mesD,mesU,anoD,anoU,dia,mes,ano,limDias,hora, minuto;
		char cc, aux[10],*novaData = rtc.getDateStr(FORMAT_SHORT,FORMAT_LITTLEENDIAN,'.');
		for(int i =0; i< 10;i++){
			cc= novaData[i];
			dataInt[i] = cc-'0';
			//Serial.println(dataInt[i]);
		}
			anoU=dataInt[7];
			anoD=dataInt[6];
			mesU=dataInt[4];
			mesD=dataInt[3];
			diaU=dataInt[1];
			diaD=dataInt[0];
			limDias = 31;
		ha0 = num[0];
		ha1 = num[1];
		ha2 = num[2];
		ha3 = num[3];
		//num[0]*10 + num[1];
		//num[2]*10 + num[3];
		int t =1, bt;
		while(1){
			
			dia = diaD*10+diaU;
			mes = mesD*10+mesU;
			ano = 2000 + anoD*10+anoU;
			hora = ha0*10+ha1;
			minuto = ha2*10+ha3;
			//Serial.println(String(dia) + ":"+ String(mes)+":"+String(ano));
			bt = button(ctrl1, ctrl2, flag1, flag2);
			if(bt == 1 && t == 1){
			if(ha3+1 < 10){
				ha3++;
			}else if(ha2+1 <= 5){
				ha2++;
				ha3=0;
			}else{
				ha2=0;
				ha3=0;
			}
			}else if(bt == 1 && t == 2){
			if(ha1+1 <= 9 && hora+1 < 24){
				ha1++;
			}else if(ha0+1 <= 2){
				ha0++;
				ha1=0;
			}else{
				ha0=0;
				ha1=0;
			}
			}else if(bt == 1 && t == 3){
				if(anoU+1 <= 9){
					anoU++;
				}else if(anoD+1 <= 4){
					anoD++;
					anoU=0;
				}else{
					anoD=0;
					anoU=1;
				}
			}else if(bt == 1 && t == 4){
				if(mesU+1 <= 9 && mes+1 <= 12){
					mesU++;
				}else if(mesD+1 < 2){
					mesD++;
					mesU=0;
				}else{
					mesD=0;
					mesU=1;
				}
			}else if(bt == 1 && t == 5){
				
				if(mes <= 7 && mes%2 !=0){
					// mês impar, dias vão até 31
					limDias = 31;
				}else if(mes <= 7 ){
					// mês par
					if (ano % 4 == 0 && (ano % 400 == 0 || ano % 100 != 0)){
						
						//ano bissexto
						if(mes == 2){
							Serial.println("BISSEXTO");
							//fevereiro, dias vão até 29
							limDias = 29;
						}else{
							// mês par, dias vão até 30
							limDias = 30;
						}
					}else{
						Serial.println("NAO BISSEXTO");
						// mês par, dias vão até 30
						limDias = 30;
					}
					// mês par, dias vão até 31
				}else if (mes%2 ==0){
					limDias = 31;
				}else{
					limDias = 30;
				}

				if(diaU+1 < 10 && dia+1 <= limDias){
					diaU++;
				}else if(diaD+1 <= 2 && limDias == 29 ){
					diaD++;
					diaU=0;
				}else if(diaD+1 <= 3 && limDias != 29){
					diaD++;
					diaU=0;
				}else{
					diaD=0;
					diaU=1;
				}

			}else if(bt == 2){
			t++;
			}
			if(t==1){
			relogio(10,10,ha2,ha3, 10);
			}else if(t==2){
			relogio(ha0,ha1,h,nil, 10);
			}else if(t==3){
			relogio(2,0,anoD,anoU, 15);
			}else if(t==4){
			relogio(13,13,mesD,mesU, 15);
			}else if(t==5){
			relogio(diaD,diaU,13,13, 15);
			}else if(t>5){
			Serial.println("config out");
			break;
			}
		}
		dia = diaD*10+diaU;
		mes = mesD*10+mesU;
		ano = 2000 + anoD*10+anoU;
		hora = ha0*10+ha1;
		minuto = ha2*10+ha3;
		rtc.setTime(hora, minuto, 0);     //Define o horario
		rtc.setDate(dia, mes, ano);   //Define o dia, mes e ano
	}

	int button(bool *ctrl1, bool *ctrl2, bool *flag1, bool *flag2){
	bot = analogRead(A3);
	if(bot > 400 && bot < 800 && *ctrl1){
		*ctrl1= false;
		return 1;
	}
	else if(bot < 400 && !*ctrl1){
		*ctrl1=true;
	}
	else if(bot > 800 && *ctrl2){
		*ctrl2 = false;
		return 2;
	}
	else if(bot < 400 && !*ctrl2){
		*ctrl2=true;
	}
	return 0;
	}
	void clean(){
	for(int i=2;i<14;i++){
		digitalWrite(i,0);
	}
	}

	void relogio(int i, int j, int k, int l, int tempo){
	printNumb(i,h1,tempo);
	printNumb(j,h2,tempo);
	printNumb(k,m1,tempo);
	printNumb(l,m2,tempo);
	}

	void printNumb(int num, int ds, int tempo){
	clean();
	for(int i=0;i<8;i++){
		digitalWrite(numbs[num][i],1);
	}
	digitalWrite(ds,1);
	delay(tempo);
	digitalWrite(ds,0);
	delay(1);
	}
