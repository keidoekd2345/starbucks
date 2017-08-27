#ifndef _HIPS_IMGR_H_
#define _HIPS_IMGR_H_
#include <map>
#include <string>
#include <set>
#include <vector>
#include "../common/typedef.h"
#include "../common/threadobj.h"

using namespace std;
struct st_interface
{
    unsigned long st_size;
    char interfaces[0];
};

class CHips_interface
{
    public:
    friend class CHips_imgr;

    private:
    uint32 m_itype;
    vector<void *> m_vec_addr;
    set<string> m_set_users;
};
#define interface_map map<uint32, CHips_interface>

class CHips_imgr
{
    public:
    CHips_imgr();
    ~CHips_imgr();
    bool hips_imgr_registe_interface(uint32 itype, st_interface * pinterface);
    bool hips_imgr_unregiste_interface(uint32 itype);
    bool hips_imgr_query_interfce(const char * pmod_name, uint32 itype, st_interface * pinterface);
    int hips_imgr_query_all_info(char *pstr_buf, IN OUT uint32* psize_in_bytes);  
    private:
    bool map_lock();
    bool map_unlock();
    map<uint32, CHips_interface> m_interfaces;
    CThread_mutex m_interface_mutex;


};

#endif