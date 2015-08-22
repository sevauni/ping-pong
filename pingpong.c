#include <math.h>
#include <wiringPi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctime>   
#include "PCD8544.h"


/*test github*/


sososos

#define DISPLAY_WIDTH 84
#define DISPLAY_HIGHT 48

const int ticks_per_msec = CLOCKS_PER_SEC / 1000;   
  
#define pad1 1
#define pad2 2

using namespace std;
// pin setup
int _din = 1;
int _sclk = 0;
int _dc = 2;
int _rst = 4;
int _cs = 3;

int contrast = 60;

int cursor_level = 1;
bool my_pause = false;
bool menu = false;
bool refresh_text_menu = true;

bool bot_enabled = false;
bool bot_flag = true;
int bot_y = 16;
int bot_difficult = 2;

int fix_my_pause_counter = 0;

float ball_x = 41;
float ball_y = 23;

double naprav_x = 1;
double naprav_y = 1;

float speed = 3;
double angle;
const float pi = 3.14;

bool speed_flag = true;

int platform_1 = 16;
int platform_2 = 16;


int player1 = 0;
int player2 = 0;

unsigned long current_time;
unsigned long previous_time_main = 0;
unsigned long previous_time_speed = 0;
unsigned long previous_time_platform = 0;

long long int my_mills(){
return clock()/ticks_per_msec;
}

int min(int a,int b){
return (a < b) ? a : b;
}

int max(int a,int b){
return (a > b) ? a : b;
}

int constrain(int x,int a,int b){
return min(max(x,a),b); 
}

double good_angle(double b){
long int a = b * 1000;
a = a % int((2 * pi)*1000);

if(a > pi * 1000) a -= 2 * pi * 1000;

return double(a)/1000;
}


int Landing_point(int X, int Y, double an, bool vpravo){
  int distance_x;
  if(vpravo) distance_x = 84 - X;
  int distance_y = distance_x * tan(an) + Y;
  bool reverse;
  if((distance_y/48) % 2 != 0) reverse = true;
  distance_y = abs(distance_y % 48);
  if(reverse) distance_y = 48 - distance_y;
  return distance_y;
}

void beep_collide(int freq){

//tone(2, freq, 280);
}

void beep_death_sound(){
//tone(2, 420, 150);
delay(100);
//tone(2, 400, 150);
delay(100);
//tone(2, 300, 150);
delay(100);
//tone(2, 180, 150);
delay(100);
//tone(2, 280, 150);
delay(100);
}

void next_level(bool U_R_DED){
ball_x = 41;
ball_y = 23;
if(U_R_DED)beep_death_sound();
bot_flag = true;


speed = 3;
angle = (rand()%1000)*pi/1000;


if(angle >0 && angle < pi / 8){

angle += pi / 4;
}else if(angle > 3 * pi / 8 && angle < 5 * pi / 8) {
angle += pi / 4;
}else if(angle > 7 * pi / 8 && angle < 9 * pi / 8){
angle += pi / 4;
}else if(angle > 11 * pi / 8 && angle < 13 * pi / 8){
angle += pi / 4;
}else if(angle > 15 * pi / 8 && angle < 2 * pi){
angle += pi / 4;
}

angle = good_angle(angle);  


}

void ball_collide(){        //отскок мяча
if (ball_y -1 <= 0){
   angle = abs(good_angle(angle));
   //tone(2, 390, 150);
  
}

if (ball_y + 3 >= 46){
   angle = -abs(good_angle(angle));

   //tone(2, 390, 150);
  
}



// платформы

if (ball_x <= 3){

if(ball_y >= platform_1-3 && ball_y <= platform_1 + 16){
if(cos(angle) < 0)angle = pi - good_angle(angle);
beep_collide(330);
bot_flag = true;
}
}

if (ball_x >= 77){
if(ball_y >= platform_2-3 && ball_y <= platform_2 + 16){
if(cos(angle) > 0)angle = pi - good_angle(angle);

beep_collide(150); 
}
}


//стены 

if (ball_x + 4 >= 83){

next_level(true);
player1 = player1 + 1;

}


if (ball_x +1 <= 0){

next_level(true);
player2 = player2 + 1;

}

}
/*
int  pad_state(int pin){
static int prev;
int sval = 0;
for (int i = 0; i < 10; i++){
    sval += analogRead(pin);    // первый джойстик
  }
sval = sval / 10;
if (abs(sval - prev) > 7){
prev = sval;
}

return prev / 31;

}
*/
void platform_motion(){
 
if( digitalRead(13) != 0){

platform_1--;

}else if(digitalRead(12) != 0){

platform_1++;

}

if( digitalRead(6) != 0){

platform_2--;

}else if(digitalRead(5) != 0){

platform_2++;

}



platform_1 = constrain(platform_1, 0, 32);
platform_2 = constrain(platform_2, 0, 32);      
      

}

void restart(){
if(digitalRead(19) == LOW){

//restart here
}
}

/*
void text_display() {     //написать что-нибудь
  LCDsetTextSize(1);
  LCDsetCursor(30,2);
  LCDsetTextColor(BLACK); //
  LCDdrawstring(player1);
  LCDsetCursor(46,2);
  LCDsetTextColor(BLACK); //
  LCDdrawstring(player2);
  
}
*/

void draw_frame(){       // рисует рамку
for (int i =0; i < DISPLAY_WIDTH; i++) {
     
     LCDsetPixel(0 + i, 0, BLACK);
}

for (int i =0; i < DISPLAY_WIDTH; i++) {
     
     LCDsetPixel(0 + i, 47, BLACK);
}
}

void draw_platform(const int x, const int y) {     // координата указывается верхняя левая
const unsigned int hight = 15;
const unsigned int width = 3;


    for (int b =0; b < width; b++){
  
  
    for (int i =0; i < hight; i++) {
     
     LCDsetPixel(x + b, y + i, BLACK);
}
}   
}
   
void draw_ball(const int x, const int y, const int size_x, const int size_y){       //  рисует мяч
    for (int b =0; b < size_x; b++){
  
  
    for (int i =0; i < size_y; i++) {
     
     LCDsetPixel(x + b, y + i, BLACK);
}
}  
}

void ball_motion(){       // отвечает за движение мяча  


         

ball_x = ball_x + speed * cos(angle);      //даем пинок шару
ball_y = ball_y + speed * sin(angle);

}

void clear_ball_track(const int x, const int y, const int size_x, const int size_y){            //очистка пердыдущего изображения мяча


    for (int b =0; b < size_x; b++){
  
  
    for (int i =0; i < size_y; i++) {
     
     LCDsetPixel(x + b, y + i, WHITE);
}
}  




}

/*void my_pause_game(){
if(fix_my_pause_counter >= 500){

if(digitalRead(18) == LOW){
delay(50);
if(digitalRead(18) == LOW){

my_pause = !my_pause;
delay(400);

}
}


}else{

fix_my_pause_counter++;

}
//Serial.println(fix_my_pause_counter);
}
*/

void speedup(){

speed += 0.02;
//Serial.println(speed);

}

void draw_menu(int text_id){
/*
  LCDsetTextSize(1);
  LCDsetTextColor(BLACK);
  LCDsetCursor(15,5);
  LCDdrawstring("vs Player");
  LCDsetCursor(15,20);
  LCDdrawstring("vs Arduino");
   LCDsetCursor(15,35);
  LCDdrawstring("Bot vs Bot");
  LCDdisplay();
*/
}

void menu_logic(){

/*
if(refresh_text_menu == true){

draw_menu(1);
refresh_text_menu = false;
}

cursor_level = pad_state(pad1);

clear_ball_track(3, 7, 6, 45);


if(cursor_level >=20){

draw_ball( 3, 7, 5, 5);
cursor_level = 1;

}else if(cursor_level >=11 && cursor_level < 20){

draw_ball( 3, 22, 5, 5);
cursor_level = 2;

}else if(cursor_level <10 ){

draw_ball( 3, 37, 5, 5);
cursor_level = 2;

}


LCDdisplay();



  if(digitalRead(18) == LOW){
  delay(50);
  if(digitalRead(18) == LOW){           //кнопа ок/my_pause
  fix_my_pause_counter = 0;                 // фикс паузы сразу после выбора пунка меню
  previous_time_speed = current_time;    //фикс скорости(происходит скачок скорости мяча на 1 уровень)
    switch (cursor_level) {
    case 1:
    menu = false;
  next_level(false);
  break;
  
  case 2:
  menu = false;
    bot_enabled = true;
  next_level(false);
    break;

}






}
}



*/
}

int main(void) { 
 pinMode (6, INPUT);
 pinMode (5, INPUT); 
 pinMode (12, INPUT);
 pinMode (13, INPUT); 

  if (wiringPiSetup() == -1)
  {
	printf("wiringPi-Error\n");
    exit(1);
  }


  LCDInit(_sclk, _din, _dc, _cs, _rst, contrast);
  LCDclear();
  
  printf("AND HUNGER GAMES HAS STARTED!!!");
  printf("=====OUR FIRST PARTICIPANT======= - mr Faggot!!!\n");
  LCDshowLogo();
  delay(2000);
 while(1){ 

current_time = my_mills(); // потом
restart();
 
 
if(menu == true){

menu_logic();           //работа с меню


}else{ 


 
if(my_pause == false){
 

 if(current_time - previous_time_main > 100) {

  previous_time_main = current_time;
  
  
  speedup();
  ball_collide();
  ball_motion();
  LCDclear();
  draw_ball(ball_x, ball_y, 4, 4);
  draw_platform(0, platform_1);
  draw_platform(81, platform_2);
  draw_frame();
  //text_display();
  LCDdisplay();


}


 if(current_time - previous_time_platform > 50) {
  previous_time_platform = current_time;
  
  //bot();
  platform_motion();

  }

}else{
/*
  LCDsetTextSize(2);
  LCDsetTextColor(BLACK);
  LCDsetCursor(15,20);
  LCDdrawstring("my_pause");
  LCDdisplay();
  */

}



//my_pause_game();





}
}
}


