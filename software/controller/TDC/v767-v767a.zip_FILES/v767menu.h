/*******************************************************************************/
/*                                                                             */
/*                        V767 menu definitions                                */
/*                                                                             */
/*                               C.A.E.N                                       */
/*                                                                             */
/*                 by Carlo Tintori     Viareggio, 07/97                       */
/*                                                                             */    
/*******************************************************************************/


/*******************************************************************************/
/*                      MENU FUNCTIONS DECLARATIONS                            */
/*******************************************************************************/
void board_select(), board_status(), acq_setup(), clear_data(), readout_data(),
     memory_test(), statistics(),reset_mod();
void get_base_addr(), get_geo_addr(), get_sds_addr();
void tdc_err(), tdc_id(), read_conf_rom(), get_int_vect(),
     get_int_lev(), set_brd_pos(), get_almf_lev(), set_berren();
void set_acq_mode(), set_dtrdy_mode(), ch_enable(), trg_sets(), start_sets(),
     adjust_sets(), edge_detect(), advanced_sets(), saveload_setup();
void ch1_en(), ch1_dis(), chall_en(), chall_dis(), ch16_endis();
void get_twin_width(), get_twin_offs(), get_trg_lat(), en_sub_trg(),
     en_ovl_trg();
void en_ro_str(),en_sub_str(),en_empty_str();
void get_ch_adj(), en_ind_adj(), get_glob_offs();
void rise_all(), fall_all(), both_all(), oddr_evenf(), oddf_evenr(),
     rise_start(), fall_start();
void set_lbwin(), set_lawin(), set_rejmar(), en_auto_rej(), set_dllc(), 
     reset_dll(), db_syncro(), db_hprio();
void save_user(), load_user(), load_default(), auto_load();
void mtst_write1(), mtst_writen(), mtst_writev(), mtst_read1(), abandon_test();


/*******************************************************************************/
/*                             MENU DECLARATIONS                               */
/*******************************************************************************/
/* MAIN MENU */
MENU_ENTRY main_m_def[] = { " 1 - BOARD SELECT"       , board_select       , '1',
                            " 2 - BOARD STATUS"       , board_status       , '2',
                            " 3 - ACQUISITION SETUP"  , acq_setup          , '3',
                            " 4 - CLEAR DATA"         , clear_data         , '4',
                            " 5 - READOUT DATA"       , readout_data       , '5',
                            " 6 - BOARD TEST"         , memory_test        , '6',
                            " 7 - STATISTICS"         , statistics         , '7',
                            " 8 - MANUAL CONTROLLER"  , manual_controller  , '8',
                            " 9 - RESET MODULE"       , reset_mod          , '9',
                            " 0 - EXIT"               , quit_menu          , '0',
                            EOM };
MENU *main_menu;

/* BOARD SELECT MENU */
MENU_ENTRY sel_m_def[] = {  " 1 - BASE ADDRESS"            , get_base_addr , '1',
                            " 2 - GEO ADDRESS"             , get_geo_addr  , '2',
                            " 3 - SDS ADDRESS"             , get_sds_addr  , '3',
                            " 0 - MAIN MENU"               , quit_menu     , '0',
                            EOM };
MENU *sel_menu;

/* BOARD SETUP MENU */
MENU_ENTRY bset_m_def[] = { " 1 - READ TDCs ERROR CODES"   , tdc_err       , '1',
                            " 2 - READ TDCs ID CODE"       , tdc_id        , '2',
                            " 3 - READ CONFIGURATION ROM"  , read_conf_rom , '3',
                            " 4 - INTERRUPT VECTOR"        , get_int_vect  , '4',
                            " 5 - INTERRUPT LEVEL"         , get_int_lev   , '5',
                            " 6 - ALMOST FULL LEVEL"       , get_almf_lev  , '6',
                            " 7 - BOARD POSITION"          , set_brd_pos   , '7',
                            " 8 - BERR ENABLE"             , set_berren    , '8',
                            " 0 - MAIN MENU"               , quit_menu     , '0',
                            EOM };
MENU *bset_menu;

/* ACQUISITION SETUP MENU */
MENU_ENTRY acq_m_def[] =  { " 1 - ACQUISITION MODE"       , set_acq_mode   , '1',
                            " 2 - DATA READY MODE"        , set_dtrdy_mode , '2',
                            " 3 - CHANNELS ENABLE"        , ch_enable      , '3',
                            " 4 - TRIGGER SETTINGS"       , trg_sets       , '4',
                            " 5 - START SETTINGS"         , start_sets     , '5',
                            " 6 - CHANNELS ADJUST"        , adjust_sets    , '6',
                            " 7 - EDGE DETECTION"         , edge_detect    , '7',
                            " 8 - ADVANCED SETTINGS"      , advanced_sets  , '8',
                            " 9 - SAVE/LOAD SETUP"        , saveload_setup , '9',
                            " 0 - MAIN MENU"              , quit_menu      , '0',
                            EOM };
MENU *acq_menu;

/* CHANNEL ENABLE MENU */
MENU_ENTRY chen_m_def[] =  { " 1 - ENABLE A CHANNEL"      , ch1_en         , '1',
                             " 2 - DISABLE A CHANNEL"     , ch1_dis        , '2',
                             " 3 - ENABLE ALL CHANNELS"   , chall_en       , '3',
                             " 4 - DISABLE ALL CHANNELS"  , chall_dis      , '4',
                             " 5 - WRITE ENABLE PATTERNS" , ch16_endis     , '5',
                             " 0 - ACQUISITION MENU"      , quit_menu      , '0',
                             EOM };
MENU *chen_menu;

/* TRIGGER SETTINGS MENU */
MENU_ENTRY trgs_m_def[] =  { " 1 - TRIGGER WINDOW WIDTH"  , get_twin_width , '1',
                             " 2 - TRIGGER WINDOW OFFSET" , get_twin_offs  , '2',
                             " 3 - TRIGGER LATENCY"       , get_trg_lat    , '3',
                             " 4 - SUBRACTION OF TRG TIME", en_sub_trg     , '4',
                             " 5 - OVERLAPPING TRIGGER"   , en_ovl_trg     , '5',
                             " 0 - ACQUISITION MENU"      , quit_menu      , '0',
                             EOM };
MENU *trgs_menu;

/* START SETTINGS MENU */
MENU_ENTRY strs_m_def[] =  { " 1 - READ OUT OF START TIME"   , en_ro_str     , '1',
                             " 2 - SUBRACTION OF START TIME" , en_sub_str    , '2',
                             " 3 - EMPTY START"              , en_empty_str  , '3',
                             " 0 - ACQUISITION MENU"         , quit_menu     , '0',
                             EOM };
MENU *strs_menu;

/* CHANNEL ADJUST MENU */
MENU_ENTRY adj_m_def[]   = { " 1 - CHANNELS ADJUST"        , get_ch_adj    , '1',
                             " 2 - INDIVIDUAL ADJUST"      , en_ind_adj    , '2',
                             " 3 - GLOBAL OFFSET"          , get_glob_offs , '3',
                             " 0 - ACQUISITION MENU"       , quit_menu     , '0',
                             EOM };
MENU *adj_menu;

/* EDGE DETECTION MENU */
MENU_ENTRY edge_m_def[]  = { " 1 - RISE ALL CHANNELS"           , rise_all   , '1',
                             " 2 - FALL ALL CHANNELS"           , fall_all   , '2',
                             " 3 - BOTH ALL CHANNELS AND START" , both_all   , '3',
                             " 4 - RISE ODD / FALL EVEN"        , oddr_evenf , '4',
                             " 5 - FALL ODD / RISE EVEN"        , oddf_evenr , '5',
                             " 6 - RISE START"                  , rise_start , '6',
                             " 7 - FALL START"                  , fall_start , '7',
                             " 0 - ACQUISITION MENU"            , quit_menu  , '0',
                             EOM };
MENU *edge_menu;


/* ADVANCED SETTINGS MENU */
MENU_ENTRY advs_m_def[]  = { " 1 - LOOK BACK WINDOW"            , set_lbwin  , '1',
                             " 2 - LOOK AHEAD WINDOW"           , set_lawin  , '2',
                             " 3 - REJECT OFFSET"               , set_rejmar , '3',
                             " 4 - AUTOMATIC REJECT"            , en_auto_rej, '4',
                             " 5 - DLL CURRENT"                 , set_dllc   , '5',
                             " 6 - RESET DLL"                   , reset_dll  , '6',
                             " 7 - DOUBLE SYNCRONIZER"          , db_syncro  , '7',
                             " 8 - DOUBLE HIT PRIORITY QUEUE"   , db_hprio   , '8',
                             " 0 - ACQUISITION MENU"            , quit_menu  , '0',
                             EOM };
MENU *advs_menu;


/* EDGE DETECTION MENU */
MENU_ENTRY save_m_def[]  = { " 1 - SAVE USER SETUP"        , save_user    , '1',
                             " 2 - LOAD USER SETUP"        , load_user    , '2',
                             " 3 - LOAD DEFAULT SETUP"     , load_default , '3',
                             " 4 - AUTO LOAD USER SETUP"   , auto_load    , '4',
                             " 0 - ACQUISITION MENU"       , quit_menu    , '0',
                             EOM };
MENU *save_menu;


/* MEMORY TEST */
MENU_ENTRY memt_m_def[]  = { " 1 - WRITE A WORD"             , mtst_write1  , '1',
                             " 2 - WRITE A SEQUENCE"         , mtst_writen  , '2',
                             " 3 - WRITE EVENTS"             , mtst_writev  , '3',
                             " 4 - READ A WORD"              , mtst_read1   , '4',
                             " 5 - CLEAR MEMORY"             , clear_data   , '5',
                             " 7 - ABANDON TEST MODE"        , abandon_test , '7',
                             " 0 - MAIN MENU"                , quit_menu    , '0',
                             EOM };
MENU *memt_menu;
