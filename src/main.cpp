#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // lcd 16col X 2rows wit i2c

#define OkBtn 13
#define NextBtn 12
#define CancelBtn 11

//Load init State value Variable
int StateMode = EEPROM.read(0); // val 1 for wash,val 2 for rins
long StateWashTime = EEPROM.read(1); // load last status wash time in min
int StateRinsTime = EEPROM.read(2); // load last status rins time in no of times
int StateWaterLevel = EEPROM.read(3); // load last status rins time in no of times

// For Water level variable
#define TotalNoOfWaterLevel 9
#define DefaultWaterLevel 7
int WaterLevel = DefaultWaterLevel;
int WaterLevelDistanceInch;
int EachLevelDistanceInch;

// water level sensor check distance 
#define FullWaterLevelDistanceInch 8
#define EmptyWaterLevelDistanceInch 26
#define WaterDrainDelay 180000 //180000 millis = 3min

int DistanceInch;

// for check water level, ultrasonic sensor trig echo pin define
#define WaterLevelTrigPin 6
#define WaterLevelEchoPin 7

//For Water filling tub volve
#define WaterInPin 2
boolean WaterInPinStatus = false;
//For Water draining tub volve
#define WaterOutPin 3
boolean WaterOutPinStatus = false;

//for wash time variable
// default wash time 10min
const long DefaultWashTime = 600000; // 600000 millis = 10min
long WashTime = DefaultWashTime;
//#define DefaultRinsRotationTime 120000; // 120000 millis = 2min
#define DefaultRinsRotationTime 30000; // 120000 millis = 2min
long RinsRotationTime = DefaultRinsRotationTime;

// for rins time variable
#define DefaultRinsTime 2
int RinsTime = DefaultRinsTime;

//Motor Pin LOW for ON and HIGH for OFF
#define MotorPin 4
boolean MotorPinStatus = false;
//Motor Rotation Pin HIGH for one side and LOW for another side
#define MotorDirectionPin 5
boolean MotorDirectionPinStatus = false;

#define MotorRotationDelay 5000 //  5000 millis = 5sec, motor rotation time ON
#define MotorRinsRotationDelay 3000 // 3000 millis = 3sec, motor rins mod rotation time ON
#define MotorRotationRestDelay 2000 // 2000 mills = 2sec, motor rest time between another rotation ON

/*	
	int SelectedMode
	1 for set water level
	2 for set Wash time
	3 for set Rins times
	4 for Start Wash
	5 for Rins 
*/
int SelectedMode = 0;
void NextMode(){
	SelectedMode = (SelectedMode < 5)? (SelectedMode+1) : 1; 
	lcd.clear();

	switch (SelectedMode) {
		case 1:
			lcd.print("Water Level: ");
			lcd.print(WaterLevel);
			break;

		case 2:
			lcd.print("Wash Time: ");
			lcd.print((WashTime / 1000) / 60);
			lcd.print("min");
			break;

		case 3:
			lcd.print("Rins Time: ");
			lcd.print(RinsTime);
			lcd.print("tms");
			break;

		case 4: 
			lcd.print("Start Wash: ");
			lcd.print((WashTime / 1000) / 60);
			lcd.print("min");
			break;

		case 5:
			lcd.print("Start Rins: ");
			lcd.print(RinsTime);
			lcd.print("tms");
			break;
	
	}

}

void save_state_value(int sMode = 0, long sWT = StateWashTime, int sRT = StateRinsTime, int sWL = StateWaterLevel){

	StateMode = sMode;
	StateWashTime = sWT;
	StateRinsTime = sRT;
	StateWaterLevel = sWL;

	EEPROM.write(0, StateMode);
	EEPROM.write(1, StateWashTime);
	EEPROM.write(2, StateRinsTime);
	EEPROM.write(3, StateWaterLevel);

}

int calculate_water_level(){
	WaterLevelDistanceInch =  EmptyWaterLevelDistanceInch - (EachLevelDistanceInch * WaterLevel);
	return WaterLevelDistanceInch;
}

int get_current_water_level(){
	digitalWrite(WaterLevelTrigPin, LOW);
	delayMicroseconds(2);
	digitalWrite(WaterLevelTrigPin, HIGH);
	delayMicroseconds(10);
	digitalWrite(WaterLevelTrigPin, LOW);
	long Duration = pulseIn(WaterLevelEchoPin, HIGH);
	DistanceInch = Duration*0.0133/2;
	return DistanceInch;
}

void set_wash_time(){
	WashTime = (WashTime < (long (20 * 60) * 1000))? (WashTime+60000) : 60000;
}

void set_water_level(){
	WaterLevel = (WaterLevel < TotalNoOfWaterLevel)? (WaterLevel+1) : 1 ;
	calculate_water_level();
}

void set_rins_time(){
	RinsTime = (RinsTime >= 3)? 1 : (RinsTime+1);
}

void job_completed(){
	lcd.clear();
	lcd.print("**JobCompleted**");
	lcd.setCursor(0, 1);
	lcd.print("_-_-_-_-_-_-_-_-");
	while(true){
		if(digitalRead(OkBtn) or digitalRead(CancelBtn)){
			delay(500);
			break;
		}
	}
}

void fill_tub(){

	digitalWrite(WaterOutPin, HIGH);
	WaterOutPinStatus = false;
	WaterInPinStatus = true;
	get_current_water_level();
	delay(300);
	while(DistanceInch >= WaterLevelDistanceInch){
		digitalWrite(WaterInPin, LOW);
		delay(30000);
		get_current_water_level();
	}
	WaterInPinStatus = false;
	digitalWrite(WaterInPin, HIGH);

}

void wash(){
	long CurrentTime = millis();
	long t = (millis() - CurrentTime);
	while(WashTime >= t){
		// Setting motor direction
		MotorDirectionPinStatus = MotorDirectionPinStatus? false : true;
		digitalWrite(MotorDirectionPin, MotorDirectionPinStatus? LOW : HIGH);
		//Start Rotate motor
		digitalWrite(MotorPin, LOW);
		MotorPinStatus = true;
		delay(MotorRotationDelay);
		digitalWrite(MotorPin, HIGH);
		MotorPinStatus = false;
		delay(MotorRotationRestDelay);
		t = (millis() - CurrentTime);
		if(WashTime >= t) {
			save_state_value(1, (WashTime - t), RinsTime, WaterLevel);
		} else {
			save_state_value(2, 0, RinsTime, WaterLevel);
		}
	}

}

void drain_water(){

	digitalWrite(WaterInPin, HIGH);
	WaterInPinStatus = false;

	WaterOutPinStatus = true;
	digitalWrite(WaterOutPin, LOW);
		get_current_water_level();
	while(DistanceInch < EmptyWaterLevelDistanceInch){
		delay(30000);
		get_current_water_level();
		delay(2000);
	}
//	delay(WaterDrainDelay);

	digitalWrite(WaterOutPin, HIGH);
	WaterOutPinStatus = false;

}

void rins_rotation(){
	long CurrentTime = millis();
	long t = (millis() - CurrentTime);
	while(RinsRotationTime >= t){
		// Setting motor direction
		MotorDirectionPinStatus = MotorDirectionPinStatus? false : true;
		digitalWrite(MotorDirectionPin, MotorDirectionPinStatus? LOW : HIGH);
		//Start Rotate motor
		digitalWrite(MotorPin, LOW);
		MotorPinStatus = true;
		delay(MotorRinsRotationDelay);
		digitalWrite(MotorPin, HIGH);
		MotorPinStatus = false;
		delay(MotorRotationRestDelay);
		t = (millis() - CurrentTime);
	}
}

void rins(){
	for (int i = 1; i <= RinsTime; i++) {
		fill_tub();
		rins_rotation();
		if(i < RinsTime){
			save_state_value(2, 0, (RinsTime - i), WaterLevel);
		} else {
			save_state_value(0, 0, 0, WaterLevel);
		}
		drain_water();
	}
}

void start_washing(){

	if(WashTime){
		fill_tub();
		wash();
	}
	drain_water();
	rins();
	drain_water();

}

void welcome_msg(){
	lcd.setCursor(0, 0);
	lcd.print("===Welcome To===");
	lcd.setCursor(0, 1);
	lcd.print("====WashTime====");
	delay(3000);
}
void wait_loading_msg(){
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("======Wait======");
	lcd.setCursor(0, 1);
	lcd.print("==For Loading!==");
	delay(2000);
}

// the setup function runs once when you press reset or power the board
void setup() {

	Serial.begin(9600);
	lcd.init();
	lcd.backlight();
	lcd.clear();

	pinMode(OkBtn, INPUT);
	pinMode(NextBtn, INPUT);
	pinMode(CancelBtn, INPUT);

/* 	for (int i = 0; i <= 20; i++) {
		EEPROM.write(i, 0);
	}
 */	

	pinMode(WaterLevelTrigPin, OUTPUT); // Sets the trigPin as an Output
	pinMode(WaterLevelEchoPin, INPUT); // Sets the echoPin as an Input

	pinMode(WaterInPin, OUTPUT);
	digitalWrite(WaterInPin, HIGH);

	pinMode(WaterOutPin, OUTPUT);
	digitalWrite(WaterOutPin, HIGH);
	
	pinMode(MotorPin, OUTPUT);
	digitalWrite(MotorPin, HIGH);
	
	pinMode(MotorDirectionPin, OUTPUT);
	digitalWrite(MotorDirectionPin, HIGH);

	// Calculat Water Level
	int TotalWaterLevelInch =  EmptyWaterLevelDistanceInch - FullWaterLevelDistanceInch;
	EachLevelDistanceInch = TotalWaterLevelInch / TotalNoOfWaterLevel;

	calculate_water_level();
//	EEPROM.write(0, 1);
	// initialize digital pin LED_BUILTIN as an output.
	//pinMode(LED_BUILTIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {

	welcome_msg();
	wait_loading_msg();

	// for load last status if frequent power off
	if(StateMode){
		lcd.setCursor(0, 0);
		lcd.print("Load Last State!  ");
		lcd.setCursor(0, 1);
		lcd.print(" OK! OR Cancel!  ");
	}
	while (StateMode){

 		if(digitalRead(OkBtn) == 1){
			delay(500);
			lcd.clear();
			lcd.print("OK Clicked!");
			delay(2000);
			if(StateMode == 1){
				WashTime = StateWashTime;
				RinsTime = StateRinsTime;
				WaterLevel = StateWaterLevel;

				lcd.clear();
				lcd.print("Wash Running!");
				lcd.setCursor(0, 1);
				lcd.print("WashTime: ");
				lcd.print((WashTime / 1000) / 60);
				lcd.print("min");
				delay(10000);
//					start_washing();
				save_state_value(0, WashTime, RinsTime, WaterLevel);
				job_completed();
				SelectedMode = 0;
			} else if(StateMode == 2){
				RinsTime = StateRinsTime;
				WaterLevel = StateWaterLevel;
					lcd.clear();
					lcd.print("Rins Running!");
					lcd.setCursor(0, 1);
					lcd.print("Times: ");
					lcd.print(RinsTime);
					lcd.print("times");
//					delay(10000);
					rins();
					save_state_value(0, WashTime, RinsTime, WaterLevel);
					job_completed();
					SelectedMode = 0;
			}
		}
		if(digitalRead(CancelBtn) == 1){
			delay(500);
			StateMode = 0;
			lcd.clear();
			lcd.print("Cancel!");
			delay(2000);
			save_state_value(0, WashTime, RinsTime, WaterLevel);
		}
	}

	wait_loading_msg();
	NextMode();
	while (StateMode == 0){
		if(digitalRead(NextBtn) == 1){
			delay(500);
			NextMode();
		} else if(SelectedMode == 0){
			NextMode();
		}

		if(digitalRead(CancelBtn) == 1){
			delay(500);
			wait_loading_msg();
			SelectedMode = 0;
		}

		if(digitalRead(OkBtn) == 1){
			delay(500);

			lcd.clear();

			switch (SelectedMode) {

				case 1:
					set_water_level();
					delay(200);
					lcd.print("Water Level: ");
					lcd.print(WaterLevel);
				break;

				case 2:
					set_wash_time();
					delay(200);
					lcd.print("Wash Time: ");
					lcd.print((WashTime / 1000) / 60);
					lcd.print("min");
					break;

				case 3:
					set_rins_time();
					delay(200);
					lcd.print("Rins Time: ");
					lcd.print(RinsTime);
					lcd.print("tms");
					break;

				case 4: 
					save_state_value(2, WashTime, RinsTime, WaterLevel);
					lcd.clear();
					lcd.print("Wash Running!");
					lcd.setCursor(0, 1);
					lcd.print("WashTime: ");
					lcd.print((WashTime / 1000) / 60);
					lcd.print("min");
					delay(5000);
					start_washing();
					job_completed();
					SelectedMode = 0;
					break;

				case 5:
					save_state_value(2, WashTime, RinsTime, WaterLevel);
					lcd.clear();
					lcd.print("Rins Running!");
					lcd.setCursor(0, 1);
					lcd.print("Times: ");
					lcd.print(RinsTime);
					lcd.print("times");
					delay(5000);
					rins();
					job_completed();
					SelectedMode = 0;
					break;

			
			}

		}

/* 
 		if(digitalRead(CancelBtn) == 1){
			wait_loading_msg();
			SelectedMode = 0;
		} 
*/

	}

}
