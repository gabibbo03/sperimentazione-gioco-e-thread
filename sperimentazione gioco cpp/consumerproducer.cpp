#include<iostream>
#include<thread>
#include<mutex>

#include<condition_variable>
#include<vector>


std::mutex mtx;
std::condition_variable cv;

void producer(std::vector<int>& buff){
    
    int i = 0;
    while (true)
    {   
        std::unique_lock<std::mutex> lck(mtx);
        i++;
        std::cout<<"ho scritto : "<<i<<std::endl;
        buff.push_back(i);
        cv.notify_one();
    
    }
}

void consumer(std::vector<int>& buff){

    std::unique_lock<std::mutex> lck(mtx);
     while (true)
     {
        while (buff.empty())
        {
            cv.wait(lck);
        }
        
        int x = buff.front();
        std::cout<<"ho letto : "<< x <<std::endl;
        buff.pop_back();
     }
    
}


int main(){
    
    std::vector<int> x;
    std::thread th2(producer,std::ref(x));
    std::thread th1(consumer,std::ref(x));

     th1.join();
     th2.join();

    
    return 0;
}