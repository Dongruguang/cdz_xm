
extern sock_mq_pt sock_mq;
extern sock_sta_t socks[];
extern sock_hlw_opt_pt sock_hlw_opt;

extern void sock_init(void);
extern void sock_work_entry( void );

extern void sock_check_service_work( void );

extern int cat_fuse_err( int road );
extern void listen_fuse( void );

extern void wait_net_order( void );
extern void cycle_wait_ordef_req( int id );

extern int cat_sock_working( int id );
extern int get_sock_sta( int road );

extern void sock_ctrl_work( void );

//定时刷新实际已充时长
extern void utime_refresh( sock_sta_pt pssock,time_t tick  );



