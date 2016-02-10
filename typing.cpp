#include  <iostream.h>
#include  <dos.h>
#include  <conio.h>
#define   ESC 0x1b
#define   BSPACE 0x08

const unsigned long far * const dosTime =
    (const unsigned long far * const)MK_FP( 0x40, 0x6C );

class Timer
{

public:

    Timer();
    void start();
    void stop();
    void reset();
    int status();
    double time();
    static double resolution();
private:

    static unsigned adjust;
    static unsigned calibrate();
    int running;
    struct TIME
		 {
		 unsigned long dosCount;
		 unsigned timerCount;
		 };
    TIME startTime;
    double time_;
};

inline double Timer::time()
{
    return time_/1.E6;
}

inline double Timer::resolution()
{
    return 839/1.E9;
}

unsigned Timer::adjust = calibrate();

Timer::Timer() : time_(0), running(0)
{
}

void Timer::start()
{
    if( !running )
		 {
		 outportb( 0x43, 0x34 );
		 asm jmp __1;
    __1:
		 outportb( 0x40, 0 );
		 asm jmp __2;
    __2:
		 outportb( 0x40, 0 );
		 startTime.dosCount = *dosTime;
		 startTime.timerCount = 0;
		 running = 1;
		 }
}

void Timer::stop()
{
    outportb( 0x43, 0 );
    unsigned char temp = inportb( 0x40 );

    TIME stopTime;
    stopTime.timerCount = (inportb( 0x40 ) << 8) + temp;
    stopTime.dosCount = *dosTime;

    TIME elapsedTime;
    elapsedTime.dosCount = stopTime.dosCount - startTime.dosCount;
    elapsedTime.timerCount = -( stopTime.timerCount - adjust );

    const double fudge = 83810.0/100000.0;
    time_ += ((elapsedTime.dosCount << 16) +elapsedTime.timerCount)*fudge;

    running = 0;

}

void Timer::reset()
{
    time_ = 0;
    if( running )
		 start();
}

unsigned Timer::calibrate()
{
    adjust = 0;
    unsigned long sum = 0;
    Timer w;
    for( int i = 0; i < 100; i++ )
		 {
		 w.start();
		 w.stop();
		 sum += w.time();
		 w.reset();
		 }
    return (unsigned)((sum+5)/100);
}


void main()
{
 clrscr();
 Timer t;
 char text[1000];
 int i = 0, space_count = 0, letter_count = 0;
 float duration;
 cout<<"\n program to calculate typing speed ";
 cout<<"\nHit any key to start timer...";
 if(getch())
   {
 cout<<"\nYour time has started. Start typing. Hit Esc when done........\n";
    delay(100);
    t.start();
   cout<<"start";
   }
 while(1)
 {
   text[i] = getche();
   letter_count++;
   if(text[i] == 'a')
      space_count++;
   if(text[i] == 's')
      cout<<"";
   if(text[i] == BSPACE)
      cout<<" "; // to erase previous character instead of cursoring
//over
   if(text[i] == ESC)
    {
      cout<<"";
	   break;
    }
 }

 t.stop();
 duration = t.time();
 cout<<"\nYour typing speed is :";
 cout<<"\ncharacters per minute: ";
 cout<<letter_count/duration;
 cout<<"\nwords per minute (Actual):";
 cout<<space_count/duration;
 cout<<"\nwords per minute (Average):";
 cout<<letter_count/duration/5;
 if((letter_count/duration)>3)
 cout<<"\nexcellent typing";
// else if(400>(60*letter_count/duration)<600)
// cout<<"moderate typing";
 else
 {
 cout<<"\ntry to improve your typing skills";
 cout<<"\nbetter luck next time";
 }
 getch();
}
