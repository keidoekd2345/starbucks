#include "hips_imgr.h"
#include <sstream>
#include <string>
#include <string.h>
using namespace std;
CHips_imgr::CHips_imgr()
{
    this->m_interface_mutex.init_mutex();
}
CHips_imgr::~CHips_imgr()
{
    
}

/*
 注册接口�?1?7
 itype 接口的类型��?1?7
 pinterface 输入参数，一组接口函数的结构体指针��?1?7
 return 1 成功�?1?7 0失败  丢�般失败都是指定的接口类型已经被注册导致的�?1?7
*/
bool CHips_imgr::hips_imgr_registe_interface(uint32 itype, 
                                st_interface * pinterface)
{
    map_lock();
    interface_map::iterator it  = this->m_interfaces.find(itype);
    if( it != m_interfaces.end())
    {
        map_unlock();
        return false;
    }
    else
    {
        CHips_interface inter;
        inter.m_itype = itype;
        size_t szpointer = sizeof(void*);
        for(uint32 i=0;
            i< (pinterface->st_size-sizeof(unsigned long)) / szpointer;
            i++
        )
        {
            void * addr = *((void **)(pinterface->interfaces) +i);
            inter.m_vec_addr.push_back(addr);
        }
        
        pair<interface_map::iterator, bool> ret = 
        this->m_interfaces.insert(pair<uint32, CHips_interface>(itype, inter));
        map_unlock();
        if(ret.second)
        {
           return true;
        }
        else
        {
           return false;
        }
    }
}
/*
    反组册接�?1?7
    itype 接口类型�?1?7
    return 1 成功�?1?70失败，一般失败都是由于接口类型不存在导致的��?1?7
*/
bool CHips_imgr::hips_imgr_unregiste_interface(uint32 itype)
{
    map_lock();
    interface_map::iterator it = this->m_interfaces.find(itype);
    if(it == this->m_interfaces.end())
    {
        map_unlock();
        return false;
    }
    else
    {
        this->m_interfaces.erase(it);
        map_unlock();
        return true;        
    }
}

/*
 查询接口
 pmod_name 发起查询请求的模块名
 itype 请求查询的接口类�?1?7
 pinterface 接收接口函数的结构体指针
*/
bool CHips_imgr::hips_imgr_query_interfce(const char * pmod_name, 
                        uint32 itype, st_interface * pinterface)
{
    map_lock();
    interface_map::iterator it = this->m_interfaces.find(itype);
    if(it == this->m_interfaces.end())
    {
        map_unlock();
        return false;
    }
    else
    {
        CHips_interface & interface = it->second;
        uint32 count = 0;
        if(
            (pinterface->st_size - sizeof(unsigned long)) / sizeof(void *) < 
            interface.m_vec_addr.size()
        )
        {
            map_unlock();
            return false;
        }
        for(
            vector<void *>::iterator addr_it = interface.m_vec_addr.begin();
            addr_it != interface.m_vec_addr.end() && count < (pinterface->st_size - sizeof(unsigned long))/sizeof(void *);
            addr_it ++
        )
        {
            *((void **)(pinterface->interfaces) + count) = *addr_it;
            count ++;
        }
         interface.m_set_users.insert(string(pmod_name));       
        map_unlock();
        return true;
    }
}

bool CHips_imgr::map_lock()
{
    return this->m_interface_mutex.lock_mutex();
}

bool CHips_imgr::map_unlock()
{
    return this->m_interface_mutex.unlock_mutex();
}
/*
    查询全部接口信息，包括接口的TYPE值， 每组接口的函数地坢�
    pstr_buf 接收接口信息的缓冲区
    psize_in_bytes 输入，输出， 缓冲区大�?1?7 以及实际写入缓冲区的数据长度
    return 返回 如果缓冲区长度不足，返回的是霢�要的缓冲区长度��返�?1?70 说明缓冲区足够��?1?7
*/
int CHips_imgr::hips_imgr_query_all_info(char *pstr_buf, 
        IN OUT uint32* psize_in_bytes)
{
    string info="";
    stringstream outstream;
    map_lock();
    for (interface_map::iterator it = this->m_interfaces.begin(); 
                it != this->m_interfaces.end(); it++)
    {
        CHips_interface & interface = it->second;
        outstream << "TYPE:" << dec << interface.m_itype << endl;
        for (
            vector<void *>::iterator addr_it = interface.m_vec_addr.begin();
            addr_it != interface.m_vec_addr.end();
            addr_it ++
        )
        {
            outstream << "address:" << showbase << hex << *addr_it<<endl;
        }

        outstream << "users:";

        for(
            set<string>::iterator it_user = interface.m_set_users.begin();
            it_user != interface.m_set_users.end();
            it_user ++
        )
        {
            outstream << " " << *it_user;
        }
        outstream << endl;
    }
    map_unlock();
    if ( pstr_buf == 0 )
    {
        *psize_in_bytes = 0;
        return outstream.str().length();
    }
    else
    {
        if(*psize_in_bytes - 1 >= outstream.str().length())
        {
            //strcpy_s(pstr_buf, psize_in_bytes, outstream.str().c_str());
            strncpy(pstr_buf, outstream.str().c_str(), outstream.str().length());
            *psize_in_bytes = outstream.str().length();
            return 0;
        }
        else
        {
            //strcpy_s(pstr_buf, psize_in_bytes, outstream.str().c_str());
            strncpy(pstr_buf, outstream.str().c_str(), *psize_in_bytes - 1);
            *psize_in_bytes = *psize_in_bytes - 1;
            return outstream.str().length();
        }
        

    }

}
