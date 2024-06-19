#include<iostream>
#include<thread>
#include<mutex>
#include<termios.h>
#include<unistd.h>
#include<chrono>

using namespace std::chrono_literals;

static bool finish = false;
std::mutex mtx;

class oggetto_gioco
{
    public:
   
    int x;
    int y;

    oggetto_gioco(int fx,int fy) : x(fx),y(fy) {}

    void move(int dx,int dy) {x+= dx; y+= dy;}
};


class pallina : public oggetto_gioco
{  
    
    using oggetto_gioco::oggetto_gioco;
};

class racchetta : public oggetto_gioco
{   
    public:
    using oggetto_gioco::oggetto_gioco;
    using oggetto_gioco::move;
};


void set_input_mode(bool enable) {

    static struct termios oldt, newt;
    if (enable) {
        tcgetattr(STDIN_FILENO, &oldt); // Salva le vecchie impostazioni
        newt = oldt; // Copia le vecchie impostazioni
        newt.c_lflag &= ~(ICANON | ECHO); // Disabilita l'input canonico e l'eco
        tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Applica le nuove impostazioni
    } else {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Ripristina le vecchie impostazioni
    }
}

void consumaPos(pallina& p,racchetta& r){
     
     while (true)
     {
         {
            std::lock_guard<std::mutex> lck(mtx);
            if(finish) break;

         std::cout<<"px : "<<p.x<<" - py : "<<p.y<<std::endl;

         if(p.x == r.x && p.y == r.y){
            std::cout<<"SOVRAPPOSTI !"<<std::endl;
         }

         }
         std::this_thread::sleep_for(50ms);
         
     }
     
}

void PrendiInput(pallina& p,racchetta& r){
    
    set_input_mode(true);
    char c = 0;

    while (c != '\n')
    {    
        std::cin.get(c);
        switch (c)
        {
        case 'w':
            p.move(0,1);
            break;
         case 's':
            p.move(0,-1);
            break;
         case 'a':
            p.move(-1,0);
            break;
         case 'd':
            p.move(1,0);
            break;
        case 'i':
            r.move(0,1);
            break;
        case 'k':
            r.move(0,-1);
            break;
        case '\n': 
            {
            std::lock_guard<std::mutex> lck(mtx);
            finish = true;
            }
            break;
        }
    }

    set_input_mode(false); 
}

int main(){

    
    pallina p(0,0);
    racchetta r(100,100);
    std::thread th1(consumaPos,std::ref(p),std::ref(r));
    std::thread th2(PrendiInput,std::ref(p),std::ref(r));


    th1.join();
    th2.join();
    return 0;
}