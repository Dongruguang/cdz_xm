


//-------------------------------------------------------------------------

extern nwdt_pt nwdt;

extern ipool_pt ipool;
//-------------------------------------------------------------------------
extern deal_srv_cmd_t wira_deal_cmd[_srv_cmd_max];
extern char* _srv_cmd_list[_srv_cmd_max];



extern void show_send_data( char* send_buf );

extern void wira_work_data_rst( void );
extern int wira_wrok_cycle( wk_inet_pt inet );
extern int wira_deal_cmd_from_server( int cmd, uint8_t* dat );


extern void wira_event_report( void );
/*--*/extern void event_report_curr( event_pt event );
/*--*/extern void event_report_sock( event_pt event );
/*--*/extern void event_report_net( event_pt event );
/*--*/extern void event_report_record( event_pt event );
/*--*/extern void event_report_m_tenance( event_pt event );
