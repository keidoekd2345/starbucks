#include <stdio.h>
#include <vector>
#include <sstream>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <linux/cn_proc.h>
#include <linux/netlink.h>
#include <linux/connector.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <sys/types.h>  
//#include <Linux/in.h>
#include <linux/if_ether.h>
#include<unistd.h>
#include <linux/filter.h>
#include <errno.h>
using namespace std;

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_nl addr;
    int iret;
    addr.nl_family = AF_NETLINK;
    addr.nl_pid = getpid ();
    addr.nl_groups = CN_IDX_PROC;
    stringstream ss;

    sock = socket (PF_NETLINK, SOCK_DGRAM | SOCK_NONBLOCK | 
               SOCK_CLOEXEC, NETLINK_CONNECTOR);
    
    iret = bind (sock, (struct sockaddr *)&addr, sizeof addr);   
    if(iret == -1)
    {
        ss << "errno." << errno << " is "<< strerror(errno) << endl;
        cout<< ss.str();
    }

    struct iovec iov[3];
    char nlmsghdrbuf[NLMSG_LENGTH (0)];
    struct nlmsghdr *nlmsghdr = (struct nlmsghdr *)nlmsghdrbuf;
    struct cn_msg cn_msg;
    enum proc_cn_mcast_op op;

    nlmsghdr->nlmsg_len = NLMSG_LENGTH (sizeof cn_msg + sizeof op);
    nlmsghdr->nlmsg_type = NLMSG_DONE;
    nlmsghdr->nlmsg_flags = 0;
    nlmsghdr->nlmsg_seq = 0;
    nlmsghdr->nlmsg_pid = getpid ();

    iov[0].iov_base = nlmsghdrbuf;
    iov[0].iov_len = NLMSG_LENGTH (0);

    cn_msg.id.idx = CN_IDX_PROC;
    cn_msg.id.val = CN_VAL_PROC;
    cn_msg.seq = 0;
    cn_msg.ack = 0;
    cn_msg.len = sizeof op;

    iov[1].iov_base = &cn_msg;
    iov[1].iov_len = sizeof cn_msg;

    op = PROC_CN_MCAST_LISTEN;

    iov[2].iov_base = &op;
    iov[2].iov_len = sizeof op;

    iret = writev (sock, iov, 3);

    unsigned char * buf = (unsigned char *)malloc(1024*1024*8);
    while(1)
    {
        struct msghdr msghdr;
        struct sockaddr_nl addr;
        struct iovec iov[1];
        ssize_t len;
        memset(buf, 0 , 1024*1024*8);
        msghdr.msg_name = &addr;
        msghdr.msg_namelen = sizeof addr;
        msghdr.msg_iov = iov;
        msghdr.msg_iovlen = 1;
        msghdr.msg_control = NULL;
        msghdr.msg_controllen = 0;
        msghdr.msg_flags = 0;

        iov[0].iov_base = buf;
        iov[0].iov_len = 1024*1024*8;


        struct sock_filter filter[] = {
                BPF_STMT (BPF_RET|BPF_K, 0xffffffff),
        };

        struct sock_fprog fprog;
        fprog.filter = filter;
        fprog.len = sizeof filter / sizeof filter[0];

        setsockopt (sock, SOL_SOCKET, SO_ATTACH_FILTER, &fprog,
                    sizeof fprog);




        len = sizeof (msghdr);
        len = recvmsg (sock, &msghdr, 0);

        if(len == -1 )
        {
            //ss << "errno." << errno << " is "<< strerror(errno) << endl;
           // cout<< ss.str();
            sleep(1);
            continue;
        }
        else if (len ==0 )
        {
            ss << "zero data returned by recvmsg"<< endl;
            cout << ss;
            continue;
        }

        for (struct nlmsghdr *nlmsghdr = (struct nlmsghdr *)buf; NLMSG_OK (nlmsghdr, len); nlmsghdr = NLMSG_NEXT (nlmsghdr, len))
        {
            if ((nlmsghdr->nlmsg_type == NLMSG_ERROR)
                || (nlmsghdr->nlmsg_type == NLMSG_NOOP))
                    continue;

            struct cn_msg *cn_msg = (struct cn_msg *) NLMSG_DATA (nlmsghdr);
            if ((cn_msg->id.idx != CN_IDX_PROC)
                || (cn_msg->id.val != CN_VAL_PROC))
                    continue;        
            struct proc_event *ev = (struct proc_event *)cn_msg->data;

            switch (ev->what) 
            {
                case proc_event::PROC_EVENT_EXEC:
                cout << "EXEC" << endl;
                break;
                case proc_event::PROC_EVENT_NONE:
                cout << "nonoe" << endl;
                break;                
                case proc_event::PROC_EVENT_UID:
                cout << "UID" << endl;
                break;
                case proc_event::PROC_EVENT_GID:
                cout << "gid"<< endl;
                break;
                case proc_event::PROC_EVENT_SID:
                cout<<"sid"<<endl;
                break;
                case proc_event::PROC_EVENT_PTRACE:
                cout<<"ptrace"<<endl;
                break;
                case proc_event::PROC_EVENT_COMM:
                cout << "common" << endl;
                break;
                case proc_event::PROC_EVENT_FORK:
                        printf ("FORK %d/%d -> %d/%d\n",
                                ev->event_data.fork.parent_pid,
                                ev->event_data.fork.parent_tgid,
                                ev->event_data.fork.child_pid,
                                ev->event_data.fork.child_tgid);
                        break;
                default:
                        break;
            /* more message types here */
            }        
        }
    }




    while(1)
    {
        char inputstr[20];
        cin.getline(inputstr,18);
        string strcmd = inputstr;
        if(strcmd=="usage")
        {
        }
        else if(strcmd == "free")
        {

        }
        else if(strcmd == "allocate")
        {
            
        }
        else 
        {

        }
    }
    

    return 0;
}

