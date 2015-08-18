#include <SPI.h>  
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h> 

Adafruit_PCD8544 display = Adafruit_PCD8544(5, 4, 3);   //инициализация экрана

#define DISPLAY_WIDTH 84
#define DISPLAY_HIGHT 48

#define pad1 1
#define pad2 2


int cursor_level = 1;
boolean pause = false;
boolean menu = true;
boolean refresh_text_menu = true;

boolean bot_enabled = false;
boolean bot_flag = true;
int bot_y = 16;
int bot_difficult = 2;




int fix_pause_counter = 0;

int ball_x = 41;
int ball_y = 23;

int naprav_x = 1;
int naprav_y = 1;
int sp_mult = 2;
boolean speed_flag = true;

int platform_1 = 16;
int platform_2 = 16;


int player1 = 0;
int player2 = 0;

unsigned long current_time;
unsigned long previous_time_main = 0;
unsigned long previous_time_speed = 0;
unsigned long previous_time_platform = 0;




void setup()   {
    
  display.begin();
  display.setContrast(60);        // контраст
  display.clearDisplay(); 
  display.setRotation(2);
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(18, INPUT_PULLUP);
  pinMode(19, INPUT_PULLUP);
  
}

void(* resetFunc) (void) = 0;

void next_level(){
ball_x = 41;
ball_y = 23;
beep_death_sound();
sp_mult = 1;
bot_flag = true;

int multiplier = 0;

if(random(2, 127) >= 63){
multiplier =  1;
}else{
multiplier = -1;
}

naprav_x = naprav_x * multiplier;

if(random(2, 127) >= 63){
multiplier =  1;
}else{
multiplier = -1;
}

naprav_y = naprav_y * multiplier;



}

void restart(){
if(digitalRead(19) == LOW){

resetFunc();
}
}

void text_display() {     //написать что-нибудь
  display.setTextSize(1);
  display.setCursor(30,2);
  display.setTextColor(BLACK); //
  display.println(player1);
  display.setCursor(46,2);
  display.setTextColor(BLACK); //
  display.println(player2);
  
}

void draw_frame(){       // рисует рамку
for (int i =0; i < DISPLAY_WIDTH; i++) {
     
     display.drawPixel(0 + i, 0, BLACK);
}

for (int i =0; i < DISPLAY_WIDTH; i++) {
     
     display.drawPixel(0 + i, 47, BLACK);
}
}

void draw_platform(const int x, const int y) {     // координата указывается верхняя левая
const unsigned int hight = 15;
const unsigned int width = 3;


    for (int b =0; b < width; b++){
  
  
    for (int i =0; i < hight; i++) {
     
     display.drawPixel(x + b, y + i, BLACK);
}
}   
}
   
void draw_ball(const int x, const int y, const int size_x, const int size_y){       //  рисует мяч
    for (int b =0; b < size_x; b++){
  
  
    for (int i =0; i < size_y; i++) {
     
     display.drawPixel(x + b, y + i, BLACK);
}
}  
}

void ball_motion(){       // отвечает за движение мяча  


         

ball_x = ball_x + naprav_x * sp_mult;      //даем пинок шару
ball_y = ball_y + naprav_y * sp_mult;

}

void ball_collide(){        //отскок мяча
if (ball_y -1 <= 0){
   naprav_y = abs(naprav_y);
   tone(2, 390, 150);
  
}

if (ball_y + 3 >= 46){
   naprav_y = -abs(naprav_y);
   tone(2, 390, 150);
  
}



// платформы

if (ball_x <= 3){

if(ball_y >= platform_1-3 && ball_y <= platform_1 + 16){
naprav_x = abs(naprav_x);
beep_collide(330);
bot_flag = true;
}
}

if (ball_x >= 77){
if(ball_y >= platform_2-3 && ball_y <= platform_2 + 16){
naprav_x = -abs(naprav_x);

beep_collide(150); 
}
}


//стены 

if (ball_x + 4 >= 83){

next_level();
player1 = player1 + 1;

}


if (ball_x +1 <= 0){

next_level();
player2 = player2 + 1;

}

}

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

void platform_motion(){
          
if( platform_1 - pad_state(pad1) > 0){

platform_1--;

}else if(platform_1 - pad_state(pad1) < 0){

platform_1++;

}



platform_1 = constrain(platform_1, 0, 32);

//bot section
int pad_state_temp = pad_state(pad2);


if(bot_enabled == true){

pad_state_temp = bot_y;

}


 
if( platform_2 - pad_state_temp > 0){

platform_2--;

}else if(platform_2 - pad_state_temp < 0){

platform_2++;

}

platform_2 = constrain(platform_2, 0, 32);
      
      

}

void clear_ball_track(const int x, const int y, const int size_x, const int size_y){            //очистка пердыдущего изображения мяча


    for (int b =0; b < size_x; b++){
  
  
    for (int i =0; i < size_y; i++) {
     
     display.drawPixel(x + b, y + i, WHITE);
}
}  




}

void beep_collide(int freq){

tone(2, freq, 280);
}

void beep_death_sound(){
tone(2, 420, 150);
delay(100);
tone(2, 400, 150);
delay(100);
tone(2, 300, 150);
delay(100);
tone(2, 180, 150);
delay(100);
tone(2, 280, 150);
delay(100);
}

void pause_game(){
if(fix_pause_counter >= 500){

if(digitalRead(18) == LOW){
delay(50);
if(digitalRead(18) == LOW){

pause = !pause;
delay(400);

}
}

}else{

fix_pause_counter++;

}
Serial.println(fix_pause_counter);
}

void speedup(){



sp_mult++;

}

void speedcontrol(){
/*
if(speed_flag == true){
 sp_mult++;
 speed_flag = false;
}else{

 sp_mult--;
 speed_flag = true;
}
*/
}

void draw_menu(int text_id){

  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(15,5);
  display.println("vs Player");
  display.setCursor(15,25);
  display.println("vs Arduino");
  display.display();

}

void menu_logic(){


if(refresh_text_menu == true){

draw_menu(1);
refresh_text_menu = false;
}

cursor_level = pad_state(pad1);

clear_ball_track(3, 7, 6, 32);


if(cursor_level >=16){

draw_ball( 3, 7, 5, 5);
cursor_level = 1;


}else{

draw_ball( 3, 27, 5, 5);
cursor_level = 2;

}


display.display();



  if(digitalRead(18) == LOW){
  delay(50);
  if(digitalRead(18) == LOW){           //кнопа ок/pause
  fix_pause_counter = 0;                 // фикс паузы сразу после выбора пунка меню
  previous_time_speed = current_time;    //фикс скорости(происходит скачок скорости мяча на 1 уровень)
    switch (cursor_level) {
    case 1:
    menu = false;
  break;
  
  case 2:
  menu = false;
    bot_enabled = true;
    break;

}






}
}




}

void bot(){
if(bot_enabled == true){
switch (bot_difficult) {


  case 1:
bot1_logic();
break;



  case 2:
bot2_logic(1);
break;



}
}
}

void bot1_logic(){

if(ball_x >= 20){

bot_y = ball_y - 8;

}else{

bot_y = 16;
}


}

void bot2_logic(int platform){





if(ball_x >= 40 && bot_flag == true && naprav_x == 1 ){

int ball_x_temp = ball_x;
int naprav_y_temp = naprav_y;

bot_y = ball_y;


for(;ball_x_temp <= 78; ball_x_temp = ball_x_temp + sp_mult){



if (bot_y -1 <= 0 || bot_y + 3 >= 46){
   naprav_y_temp = -naprav_y_temp;
  
}

bot_y  = bot_y  + naprav_y_temp * sp_mult;



}

bot_y = bot_y - 5;





Serial.println("done");
Serial.println(bot_y);
bot_flag = false;

}else if(naprav_x < 0){

bot_y = 16;

}


}

void loop() { 


current_time = millis();
restart();
 
 
if(menu == true){

menu_logic();           //работа с меню


}else{ 



 
if(pause == false){
 

 if(current_time - previous_time_main > 100) {

  previous_time_main = current_time;
  
  
  
  speedcontrol();
  ball_collide();
  ball_motion();
  display.clearDisplay();
  draw_ball(ball_x, ball_y, 4, 4);
  draw_platform(0, platform_1);
  draw_platform(81, platform_2);
  draw_frame();
  text_display();
  display.display();


}

 if(current_time - previous_time_speed > 15000) {
  previous_time_speed = current_time;
  
  
  speedup();
  

}

 if(current_time - previous_time_platform > 50) {
  previous_time_platform = current_time;
  
  bot();
  platform_motion();
  }

}else{

  display.setTextSize(2);
  display.setTextColor(BLACK);
  display.setCursor(15,20);
  display.println("Pause");
  display.display();
  

}



pause_game();





}
}




