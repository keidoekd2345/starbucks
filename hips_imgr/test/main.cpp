#include <stdio.h>
#include <vector>
#include <sstream>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "tinythread.h"
//#include "hips_memmgr.h"
//#include "threadobj.h"
//#include ".../hips_inter_mem.h"
#include "fast_mutex.h"
#include <dlfcn.h> 
#include "../../common/typedef.h"
#include "../hips_imgr.h"
using namespace tthread;
using namespace std;

fast_mutex g_outlock;

typedef uint32 (*pfunc_hips_imgr_registe_interface)(uint32 itype, void * psti);
    
typedef uint32 (*pfunc_hips_imgr_unregiste_interface)(uint32 itype);

typedef uint32 (*pfunc_hips_imgr_query_interfce)(
    const char * pmod_name, uint32 itype, void* psti);

typedef uint32 (*pfunc_hips_imgr_query_all_info)(
    char * pstr_buf, uint32 * psize_in_bytes);



pfunc_hips_imgr_registe_interface hips_imgr_registe_interface = 0;
pfunc_hips_imgr_unregiste_interface hips_imgr_unregiste_interface = 0;
pfunc_hips_imgr_query_interfce hips_imgr_query_interfce = 0;
pfunc_hips_imgr_query_all_info hips_imgr_query_all_info = 0;
void *hso = 0;

bool init_so()
{
    hso = dlopen("../libimgr.so",RTLD_NOW);
    if(hso)
    {
       hips_imgr_registe_interface = (pfunc_hips_imgr_registe_interface) dlsym(hso, "hips_imgr_registe_interface");
       hips_imgr_unregiste_interface = (pfunc_hips_imgr_unregiste_interface) dlsym(hso, "hips_imgr_unregiste_interface");
       hips_imgr_query_interfce =  (pfunc_hips_imgr_query_interfce) dlsym(hso, "hips_imgr_query_interfce");
       hips_imgr_query_all_info = (pfunc_hips_imgr_query_all_info) dlsym(hso, "hips_imgr_query_all_info");

       if(hips_imgr_registe_interface && hips_imgr_unregiste_interface
        && hips_imgr_query_interfce && hips_imgr_query_all_info)
       {
           return true;
       }
       else
       {
           dlclose(hso);
           return false;
       }
    }
    return false;
};
bool uninit_so()
{
    if(hso)
        dlclose(hso);

    return true;
}


#define ALLOCATE_SIZE 23
#define THREAD_NUMBER 10
#define RANDOM_THREAD_NUMBER 10

void out_error(unsigned int errorcode)
{
    g_outlock.lock();
    switch(errorcode)
    {

    }
    g_outlock.unlock();
}

void mod_1_test1(void)
{
    cout<<"mod_1_test1"<<endl;
}

void mod_1_test2(void)
{
    cout<<"mod_1_test2"<<endl;
}

void mod_1_test3(void)
{
    cout<<"mod_1_test3"<<endl;
}

void mod_1_test4(void)
{
    cout<<"mod_1_test4"<<endl;
}

void mod_1_test5(void)
{
    cout<<"mod_1_test5"<<endl;
}

void mod_2_test1(void)
{
    cout<<"mod_2_test1"<<endl;
}
void mod_2_test2(void)
{
    cout<<"mod_2_test2"<<endl;
}

void mod_2_test3(void)
{
    cout<<"mod_2_test3"<<endl;
}

void mod_2_test4(void)
{
    cout<<"mod_2_test4"<<endl;

}

void mod_2_test5(void)
{
    cout<<"mod_2_test5"<<endl;
}

void mod_3_test1(void)
{
    cout<<"mod_3_test1"<<endl;
}
void mod_3_test2(void)
{
    cout<<"mod_3_test2"<<endl;
}

void mod_3_test3(void)
{
    cout<<"mod_3_test3"<<endl;
}

void mod_3_test4(void)
{
    cout<<"mod_3_test4"<<endl;
}

void mod_3_test5(void)
{
    cout<<"mod_3_test5"<<endl;
}

void mod_4_test1(void)
{
    cout<<"mod_4_test1"<<endl;
}
void mod_4_test2(void)
{
    cout<<"mod_4_test2"<<endl;
}

void mod_4_test3(void)
{
    cout<<"mod_4_test3"<<endl;
}

void mod_4_test4(void)
{
    cout<<"mod_4_test4"<<endl;
}

void mod_4_test5(void)
{
    cout<<"mod_4_test/"<<endl;
}

bool g_hold_reg = false;
bool g_thread_exit = false;
void random_reg_unreg1(void *para)
{
    g_outlock.lock();
    cout<<"random 1 started"<<endl;
    g_outlock.unlock();

    unsigned long buf[sizeof(struct st_interface) + sizeof(unsigned long)*4];
    uint32 s = sizeof(struct st_interface);
    s += sizeof(unsigned long)*4;
    memset(buf, 0, sizeof(buf));
    struct st_interface *pinterface = (struct st_interface *)buf;


    pinterface->st_size = sizeof(struct st_interface) + sizeof(unsigned long)*4;
    ((void **) &pinterface->interfaces)[0] = (void *)mod_1_test1;
    ((void **) &pinterface->interfaces)[1] = (void *)mod_1_test2;
    ((void **) &pinterface->interfaces)[2] = (void *)mod_1_test3;
    ((void **) &pinterface->interfaces)[3] = (void *)mod_1_test4;
    while(!g_hold_reg)
    {
        uint32 ms= rand()%10;
        if(ms == 0) ms = 1;
        ms = ms*100000;
        this_thread::sleep_for(chrono::secodes(ms));

        hips_imgr_registe_interface(1, pinterface);

        this_thread::sleep_for(chrono::secodes(ms));

        hips_imgr_unregiste_interface(1);   
    }
    hips_imgr_registe_interface(1, pinterface);
    g_outlock.lock();
    cout<<"random 1 hold"<<endl;
    g_outlock.unlock();
    while(!g_thread_exit)
    {
        uint32 ms= rand()%10;
        this_thread::sleep_for(chrono::secodes(ms));        
    }
    g_outlock.lock();
    cout<<"random 1 exit"<<endl;
    g_outlock.unlock();


}

void random_reg_unreg2(void *para)
{
    g_outlock.lock();
    cout<<"random 2 start"<<endl;
    g_outlock.unlock();

    unsigned long buf[sizeof(struct st_interface) + sizeof(unsigned long)*4];
    uint32 s = sizeof(struct st_interface);
    s += sizeof(unsigned long)*4;
    memset(buf, 0, sizeof(buf));
    struct st_interface *pinterface = (struct st_interface *)buf;


    pinterface->st_size = sizeof(struct st_interface) + sizeof(unsigned long)*4;
    ((void **) &pinterface->interfaces)[0] = (void *)mod_2_test1;
    ((void **) &pinterface->interfaces)[1] = (void *)mod_2_test2;
    ((void **) &pinterface->interfaces)[2] = (void *)mod_2_test3;
    ((void **) &pinterface->interfaces)[3] = (void *)mod_2_test4;
    while(!g_hold_reg)
    {
        uint32 ms= rand()%10;
        if(ms == 0) ms = 1;
        ms = ms*100000;
        this_thread::sleep_for(chrono::secodes(ms));

        hips_imgr_registe_interface(2, pinterface);

        this_thread::sleep_for(chrono::secodes(ms));

        hips_imgr_unregiste_interface(2);   

    }
    hips_imgr_registe_interface(2, pinterface);
    g_outlock.lock();
    cout<<"random 2 hold"<<endl;
    g_outlock.unlock();
    while(!g_thread_exit)
    {
        uint32 ms= rand()%10;
        this_thread::sleep_for(chrono::secodes(ms));        
    }
    g_outlock.lock();
    cout<<"random 2 exit"<<endl;
    g_outlock.unlock();

}

void random_reg_unreg3(void *para)
{
    g_outlock.lock();
    cout<<"random 3 start"<<endl;
    g_outlock.unlock();    
    unsigned long buf[sizeof(struct st_interface) + sizeof(unsigned long)*4];
    uint32 s = sizeof(struct st_interface);
    s += sizeof(unsigned long)*4;
    memset(buf, 0, sizeof(buf));
    struct st_interface *pinterface = (struct st_interface *)buf;


    pinterface->st_size = sizeof(struct st_interface) + sizeof(unsigned long)*4;
    ((void **) &pinterface->interfaces)[0] = (void *)mod_3_test1;
    ((void **) &pinterface->interfaces)[1] = (void *)mod_3_test2;
    ((void **) &pinterface->interfaces)[2] = (void *)mod_3_test3;
    ((void **) &pinterface->interfaces)[3] = (void *)mod_3_test4;
    while(!g_hold_reg)
    {
        uint32 ms= rand()%10;
        if(ms == 0) ms = 1;
        ms = ms*100000;
        this_thread::sleep_for(chrono::secodes(ms));

        hips_imgr_registe_interface(3, pinterface);

        this_thread::sleep_for(chrono::secodes(ms));
        hips_imgr_unregiste_interface(3);   
    }
    hips_imgr_registe_interface(3, pinterface);
    g_outlock.lock();
    cout<<"random 3 hold"<<endl;
    g_outlock.unlock();    
    while(!g_thread_exit)
    {
        uint32 ms= rand()%10;
        ms = ms*10000;
        this_thread::sleep_for(chrono::secodes(ms));        
    }
    g_outlock.lock();
    cout<<"random 3 exit"<<endl;
    g_outlock.unlock();

}

void random_reg_unreg4(void *para)
{
    g_outlock.lock();
    cout<<"random 4 start"<<endl;
    g_outlock.unlock();        
    unsigned long buf[sizeof(struct st_interface) + sizeof(unsigned long)*4];
    uint32 s = sizeof(struct st_interface);
    s += sizeof(unsigned long)*4;
    memset(buf, 0, sizeof(buf));
    struct st_interface *pinterface = (struct st_interface *)buf;


    pinterface->st_size = sizeof(struct st_interface) + sizeof(unsigned long)*4;
    ((void **) &pinterface->interfaces)[0] = (void *)mod_4_test1;
    ((void **) &pinterface->interfaces)[1] = (void *)mod_4_test2;
    ((void **) &pinterface->interfaces)[2] = (void *)mod_4_test3;
    ((void **) &pinterface->interfaces)[3] = (void *)mod_4_test4;
    while(!g_hold_reg)
    {
        uint32 ms= rand()%10;
        if(ms == 0) ms = 1;
        ms = ms*100000;
        this_thread::sleep_for(chrono::secodes(ms));

        hips_imgr_registe_interface(4, pinterface);

        this_thread::sleep_for(chrono::secodes(ms));
        hips_imgr_unregiste_interface(4);   
    }
    hips_imgr_registe_interface(4, pinterface);
    g_outlock.lock();
    cout<<"random 4 hold"<<endl;
    g_outlock.unlock();       
    while(!g_thread_exit)
    {
        uint32 ms= rand()%100;
        this_thread::sleep_for(chrono::secodes(ms));        
    }
    g_outlock.lock();
    cout<<"random 4 exit"<<endl;
    g_outlock.unlock();

}

int main(int argc, char *argv[])
{
    //g_outlock.init();
    init_so();
    //g_hold_reg = true;
    thread *prandom1 = NULL;
    thread *prandom2 = NULL;
    thread *prandom3 = NULL;
    thread *prandom4 = NULL;
    while(1)
    {
        char inputstr[20];
        cin.getline(inputstr,18);
        string strcmd = inputstr;
        if(strcmd=="usage")
        {
            char * p  = 0;
            unsigned int size = 0;
            unsigned int raw_size = 0;
            raw_size = hips_imgr_query_all_info(0, &size);
            if(raw_size > 0)
            {
                p = new char [raw_size -2];
                size = raw_size - 2;
                raw_size = hips_imgr_query_all_info(p, &size);
                if(raw_size>0)
                {
                    delete p;
                    raw_size = 1024*1024;
                    p = new char [raw_size];
                    memset(p, 0, 1024*1024);
                    size = raw_size;
                    raw_size = hips_imgr_query_all_info(p, &size);
                    if(raw_size)
                    {
                        g_outlock.lock();
                        cout << "query usage return a wrong value while the input buffer is enought to receive all information" << endl;
                        g_outlock.unlock();
                    }
                    else
                    {
                        g_outlock.lock();
                        cout<<p<<endl;
                        g_outlock.unlock();
                        delete p;
                    }
                }
                else
                {
                    g_outlock.lock();
                    cout << "queyr usage return a wrong value while the input buffer size less than real size" << endl;
                    g_outlock.unlock();
                }
            }
            else
            {
                g_outlock.lock();
                cout << "query usage return a wrong value while the input buffer size is zero"<<endl;
                g_outlock.unlock();
            }
            
        }
        else if(strcmd == "registe")
        {
            prandom1 = new thread(random_reg_unreg1, NULL);
            prandom2 = new thread(random_reg_unreg2, NULL);
            prandom3 = new thread(random_reg_unreg3, NULL);
            prandom4 = new thread(random_reg_unreg4, NULL);
        }
        else if(strcmd == "hold")
        {
            g_hold_reg = true;
        }
        else if(strcmd == "exit")
        {
            g_thread_exit = true;
            if(prandom1)
                prandom1->join();
            
            if(prandom2)
                prandom2->join();
            
            if(prandom3)
                prandom3->join();
            
            if(prandom4)
                prandom4->join();
            
        }
        else if(strcmd == "query")
        {
            unsigned long buf[sizeof(struct st_interface) + sizeof(unsigned long)*4];
            uint32 s = sizeof(struct st_interface);
            s += sizeof(unsigned long)*4;
            for (uint32 i=1; i<=4; i++)
            {
                memset(buf, 0, sizeof(buf));
                struct st_interface *pinterface = (struct st_interface *)buf;
                pinterface->st_size = sizeof(struct st_interface) + sizeof(unsigned long)*4;
                stringstream ss ;
                ss<<i*1000;
                if(hips_imgr_query_interfce(ss.str().c_str(), i, pinterface))
                {
                    cout <<"interface " << i << ":"<< ((void **) &pinterface->interfaces)[0] << endl;
                    cout <<"interface " << i << ":"<< ((void **) &pinterface->interfaces)[1] << endl;
                    cout <<"interface " << i << ":"<< ((void **) &pinterface->interfaces)[2] << endl;
                    cout <<"interface " << i << ":"<< ((void **) &pinterface->interfaces)[3] << endl;
                    
                    ((void (*)(void)) (((void **) &pinterface->interfaces)[0]))();
                    ((void (*)(void)) (((void **) &pinterface->interfaces)[1]))();
                    ((void (*)(void)) (((void **) &pinterface->interfaces)[2]))();
                    ((void (*)(void)) (((void **) &pinterface->interfaces)[3]))();
                }
            }

        }
        else 
        {
            g_outlock.lock();
            cout << "unknow cmd, input again"<<endl;
            g_outlock.unlock();
        }
    }
    return 0;
}

