const int my_const_int = 0x56788765;
extern unsigned char dtcm_g_c_u_data_2;

void itcm_func_1(void)

{
    dtcm_g_c_u_data_2 += dtcm_g_c_u_data_2 + my_const_int;
}

void itcm_os_entry(void)

{
    dtcm_g_c_u_data_2 = 0xAC;
    itcm_func_1();
}
