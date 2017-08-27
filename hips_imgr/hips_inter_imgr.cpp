#include "hips_imgr.h"
#include "../common/typedef.h"
CHips_imgr g_imgr;
extern "C"
{
    __attribute ((visibility("default"))) uint32 hips_imgr_registe_interface(uint32 itype, void * psti)
    {
        return g_imgr.hips_imgr_registe_interface(itype, (struct st_interface *)psti);
    }
    
     __attribute ((visibility("default"))) uint32 hips_imgr_unregiste_interface(uint32 itype)
    {
        return g_imgr.hips_imgr_unregiste_interface(itype);
    }

    __attribute ((visibility("default"))) uint32 hips_imgr_query_interfce(const char * pmod_name, uint32 itype, void* psti)
    {
        return g_imgr.hips_imgr_query_interfce(pmod_name, itype, (struct st_interface *)psti);
    }
    __attribute ((visibility("default"))) uint32 hips_imgr_query_all_info(char * pstr_buf, uint32 * psize_in_bytes)
    {
        return g_imgr.hips_imgr_query_all_info(pstr_buf, psize_in_bytes);
    }
}