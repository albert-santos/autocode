import pandas as pd
import numpy as np


def get_formated_arrays (array, array_name):    
    number_of_BBUs = array.shape[1] - 1

    for i in range(array.shape[0]):
        print(f'int {array_name}_{i}', end='' )
        print(' [number_of_BBUs] = {', end='')
        for j in range(number_of_BBUs):
            if j != (number_of_BBUs - 1):
                print(f'{array[i,j]}, ', end='')
            else:
                print(f'{array[i,j]}', end='')
                print('};')

    print('')
    for i in range(array.shape[0]):
        print(f'map_{array_name}[{i}] = {array_name}_{i};')
        

energy_worksheets = 'Energy_worksheets/vetores-para-calculo-de-energia.xlsx'


df_SA_users_by_BBU = pd.read_excel(energy_worksheets, sheet_name="SA_Users_by_BBU")
#print(df_SA_users_by_BBU)
array_SA_users_by_BBU = df_SA_users_by_BBU.values
# get_formated_arrays(array_SA_users_by_BBU, 'SA_Users_by_BBU')

df_SA_RRH_by_BBU = pd.read_excel(energy_worksheets, sheet_name="SA_RRH_by_BBU")
#print(df_SA_RRH_by_BBU)
array_SA_RRH_by_BBU = df_SA_RRH_by_BBU.values
# get_formated_arrays(array_SA_RRH_by_BBU, 'SA_RRH_by_BBU')

df_HDSO_users_by_BBU = pd.read_excel(energy_worksheets, sheet_name="HDSO_Users_by_BBU")
#print(df_HDSO_users_by_BBU)
array_HDSO_users_by_BBU = df_HDSO_users_by_BBU.values
get_formated_arrays(array_HDSO_users_by_BBU, 'HDSO_Users_by_BBU')

df_HDSO_RRH_by_BBU = pd.read_excel(energy_worksheets, sheet_name="HDSO_RRH_by_BBU")
#print(df_HDSO_RRH_by_BBU)
array_HDSO_RRH_by_BBU = df_HDSO_RRH_by_BBU.values
# get_formated_arrays(array_HDSO_RRH_by_BBU, 'HDSO_RRH_by_BBU')




# int SA_Users_by_BBU_0 [number_of_BBUs] = {13, 11, 8, 19, 11, 18};
# int SA_Users_by_BBU_1 [number_of_BBUs] = {9, 9, 7, 12, 15, 8};   
# int SA_Users_by_BBU_2 [number_of_BBUs] = {8, 14, 5, 0, 9, 12};
# int SA_Users_by_BBU_3 [number_of_BBUs] = {9, 11, 7, 7, 0, 7};
# int SA_Users_by_BBU_4 [number_of_BBUs] = {7, 0, 9, 0, 0, 13};
# int SA_Users_by_BBU_5 [number_of_BBUs] = {0, 4, 5, 6, 6, 0};
# int SA_Users_by_BBU_6 [number_of_BBUs] = {6, 0, 0, 0, 5, 0};
# int SA_Users_by_BBU_7 [number_of_BBUs] = {0, 0, 0, 5, 0, 3};
# int SA_Users_by_BBU_8 [number_of_BBUs] = {0, 0, 0, 0, 0, 4};
# int SA_Users_by_BBU_9 [number_of_BBUs] = {0, 9, 0, 0, 0, 0};
# int SA_Users_by_BBU_10 [number_of_BBUs] = {0, 10, 7, 6, 0, 0};
# int SA_Users_by_BBU_11 [number_of_BBUs] = {7, 10, 9, 10, 5, 5};
# int SA_Users_by_BBU_12 [number_of_BBUs] = {12, 12, 14, 10, 17, 12};
# int SA_Users_by_BBU_13 [number_of_BBUs] = {13, 21, 17, 9, 15, 7};
# int SA_Users_by_BBU_14 [number_of_BBUs] = {8, 10, 13, 14, 9, 10};
# int SA_Users_by_BBU_15 [number_of_BBUs] = {7, 10, 10, 15, 0, 11};
# int SA_Users_by_BBU_16 [number_of_BBUs] = {10, 14, 9, 8, 9, 14};
# int SA_Users_by_BBU_17 [number_of_BBUs] = {15, 13, 14, 14, 13, 10};
# int SA_Users_by_BBU_18 [number_of_BBUs] = {11, 11, 19, 12, 11, 12};
# int SA_Users_by_BBU_19 [number_of_BBUs] = {19, 11, 8, 13, 8, 10};
# int SA_Users_by_BBU_20 [number_of_BBUs] = {8, 11, 14, 20, 10, 0};
# int SA_Users_by_BBU_21 [number_of_BBUs] = {10, 10, 8, 9, 21, 14};
# int SA_Users_by_BBU_22 [number_of_BBUs] = {17, 13, 19, 14, 13, 12};
# int SA_Users_by_BBU_23 [number_of_BBUs] = {11, 12, 9, 20, 26, 12};

# map_SA_Users_by_BBU[0] = SA_Users_by_BBU_0;
# map_SA_Users_by_BBU[1] = SA_Users_by_BBU_1;
# map_SA_Users_by_BBU[2] = SA_Users_by_BBU_2;
# map_SA_Users_by_BBU[3] = SA_Users_by_BBU_3;
# map_SA_Users_by_BBU[4] = SA_Users_by_BBU_4;
# map_SA_Users_by_BBU[5] = SA_Users_by_BBU_5;
# map_SA_Users_by_BBU[6] = SA_Users_by_BBU_6;
# map_SA_Users_by_BBU[7] = SA_Users_by_BBU_7;
# map_SA_Users_by_BBU[8] = SA_Users_by_BBU_8;
# map_SA_Users_by_BBU[9] = SA_Users_by_BBU_9;
# map_SA_Users_by_BBU[10] = SA_Users_by_BBU_10;
# map_SA_Users_by_BBU[11] = SA_Users_by_BBU_11;
# map_SA_Users_by_BBU[12] = SA_Users_by_BBU_12;
# map_SA_Users_by_BBU[13] = SA_Users_by_BBU_13;
# map_SA_Users_by_BBU[14] = SA_Users_by_BBU_14;
# map_SA_Users_by_BBU[15] = SA_Users_by_BBU_15;
# map_SA_Users_by_BBU[16] = SA_Users_by_BBU_16;
# map_SA_Users_by_BBU[17] = SA_Users_by_BBU_17;
# map_SA_Users_by_BBU[18] = SA_Users_by_BBU_18;
# map_SA_Users_by_BBU[19] = SA_Users_by_BBU_19;
# map_SA_Users_by_BBU[20] = SA_Users_by_BBU_20;
# map_SA_Users_by_BBU[21] = SA_Users_by_BBU_21;
# map_SA_Users_by_BBU[22] = SA_Users_by_BBU_22;
# map_SA_Users_by_BBU[23] = SA_Users_by_BBU_23;



# int SA_RRH_by_BBU_0 [number_of_BBUs] = {1, 2, 1, 1, 1, 1};
# int SA_RRH_by_BBU_1 [number_of_BBUs] = {1, 1, 1, 1, 1, 1};
# int SA_RRH_by_BBU_2 [number_of_BBUs] = {1, 1, 0, 1, 1, 1};
# int SA_RRH_by_BBU_3 [number_of_BBUs] = {1, 1, 1, 1, 0, 1};
# int SA_RRH_by_BBU_4 [number_of_BBUs] = {1, 0, 1, 0, 0, 1};
# int SA_RRH_by_BBU_5 [number_of_BBUs] = {1, 0, 1, 0, 1, 1};
# int SA_RRH_by_BBU_6 [number_of_BBUs] = {0, 1, 0, 0, 1, 0};
# int SA_RRH_by_BBU_7 [number_of_BBUs] = {1, 0, 0, 0, 0, 1};
# int SA_RRH_by_BBU_8 [number_of_BBUs] = {0, 0, 0, 0, 0, 1};
# int SA_RRH_by_BBU_9 [number_of_BBUs] = {0, 0, 1, 0, 0, 0};
# int SA_RRH_by_BBU_10 [number_of_BBUs] = {1, 1, 0, 1, 0, 0};
# int SA_RRH_by_BBU_11 [number_of_BBUs] = {1, 1, 1, 1, 1, 1};
# int SA_RRH_by_BBU_12 [number_of_BBUs] = {1, 1, 1, 1, 1, 2};
# int SA_RRH_by_BBU_13 [number_of_BBUs] = {1, 1, 1, 1, 1, 1};
# int SA_RRH_by_BBU_14 [number_of_BBUs] = {1, 2, 1, 1, 1, 1};
# int SA_RRH_by_BBU_15 [number_of_BBUs] = {1, 1, 1, 0, 1, 1};
# int SA_RRH_by_BBU_16 [number_of_BBUs] = {1, 1, 1, 2, 1, 1};
# int SA_RRH_by_BBU_17 [number_of_BBUs] = {1, 1, 1, 2, 2, 1};
# int SA_RRH_by_BBU_18 [number_of_BBUs] = {2, 1, 1, 1, 1, 1};
# int SA_RRH_by_BBU_19 [number_of_BBUs] = {1, 1, 1, 1, 1, 1};
# int SA_RRH_by_BBU_20 [number_of_BBUs] = {0, 1, 1, 1, 1, 1};
# int SA_RRH_by_BBU_21 [number_of_BBUs] = {1, 1, 1, 1, 2, 1};
# int SA_RRH_by_BBU_22 [number_of_BBUs] = {1, 1, 1, 1, 2, 2};
# int SA_RRH_by_BBU_23 [number_of_BBUs] = {1, 1, 1, 1, 1, 1};


# map_SA_RRH_by_BBU[0] = SA_RRH_by_BBU_0;
# map_SA_RRH_by_BBU[1] = SA_RRH_by_BBU_1;
# map_SA_RRH_by_BBU[2] = SA_RRH_by_BBU_2;
# map_SA_RRH_by_BBU[3] = SA_RRH_by_BBU_3;
# map_SA_RRH_by_BBU[4] = SA_RRH_by_BBU_4;
# map_SA_RRH_by_BBU[5] = SA_RRH_by_BBU_5;
# map_SA_RRH_by_BBU[6] = SA_RRH_by_BBU_6;
# map_SA_RRH_by_BBU[7] = SA_RRH_by_BBU_7;
# map_SA_RRH_by_BBU[8] = SA_RRH_by_BBU_8;
# map_SA_RRH_by_BBU[9] = SA_RRH_by_BBU_9;
# map_SA_RRH_by_BBU[10] = SA_RRH_by_BBU_10;
# map_SA_RRH_by_BBU[11] = SA_RRH_by_BBU_11;
# map_SA_RRH_by_BBU[12] = SA_RRH_by_BBU_12;
# map_SA_RRH_by_BBU[13] = SA_RRH_by_BBU_13;
# map_SA_RRH_by_BBU[14] = SA_RRH_by_BBU_14;
# map_SA_RRH_by_BBU[15] = SA_RRH_by_BBU_15;
# map_SA_RRH_by_BBU[16] = SA_RRH_by_BBU_16;
# map_SA_RRH_by_BBU[17] = SA_RRH_by_BBU_17;
# map_SA_RRH_by_BBU[18] = SA_RRH_by_BBU_18;
# map_SA_RRH_by_BBU[19] = SA_RRH_by_BBU_19;
# map_SA_RRH_by_BBU[20] = SA_RRH_by_BBU_20;
# map_SA_RRH_by_BBU[21] = SA_RRH_by_BBU_21;
# map_SA_RRH_by_BBU[22] = SA_RRH_by_BBU_22;
# map_SA_RRH_by_BBU[23] = SA_RRH_by_BBU_23;









# int HDSO_Users_by_BBU_0 [number_of_BBUs] = {11, 14, 14, 16, 14, 19};
# int HDSO_Users_by_BBU_1 [number_of_BBUs] = {8, 20, 9, 6, 9, 10};
# int HDSO_Users_by_BBU_2 [number_of_BBUs] = {13, 8, 6, 9, 8, 8};
# int HDSO_Users_by_BBU_3 [number_of_BBUs] = {7, 8, 8, 6, 7, 9};
# int HDSO_Users_by_BBU_4 [number_of_BBUs] = {12, 5, 5, 5, 5, 6};
# int HDSO_Users_by_BBU_5 [number_of_BBUs] = {1, 3, 3, 5, 6, 5};
# int HDSO_Users_by_BBU_6 [number_of_BBUs] = {3, 0, 8, 2, 2, 3};
# int HDSO_Users_by_BBU_7 [number_of_BBUs] = {1, 2, 0, 0, 0, 0};
# int HDSO_Users_by_BBU_8 [number_of_BBUs] = {2, 0, 0, 0, 0, 2};
# int HDSO_Users_by_BBU_9 [number_of_BBUs] = {3, 0, 2, 0, 0, 0};
# int HDSO_Users_by_BBU_10 [number_of_BBUs] = {3, 7, 2, 4, 5, 4};
# int HDSO_Users_by_BBU_11 [number_of_BBUs] = {10, 10, 9, 9, 8, 10};
# int HDSO_Users_by_BBU_12 [number_of_BBUs] = {14, 13, 13, 16, 15, 14};
# int HDSO_Users_by_BBU_13 [number_of_BBUs] = {16, 16, 16, 14, 13, 16};
# int HDSO_Users_by_BBU_14 [number_of_BBUs] = {12, 10, 13, 14, 11, 11};
# int HDSO_Users_by_BBU_15 [number_of_BBUs] = {9, 9, 8, 11, 13, 10};
# int HDSO_Users_by_BBU_16 [number_of_BBUs] = {10, 12, 11, 12, 10, 18};
# int HDSO_Users_by_BBU_17 [number_of_BBUs] = {17, 15, 15, 14, 13, 14};
# int HDSO_Users_by_BBU_18 [number_of_BBUs] = {12, 12, 15, 12, 15, 14};
# int HDSO_Users_by_BBU_19 [number_of_BBUs] = {17, 14, 12, 9, 10, 12};
# int HDSO_Users_by_BBU_20 [number_of_BBUs] = {12, 11, 12, 12, 12, 13};
# int HDSO_Users_by_BBU_21 [number_of_BBUs] = {15, 13, 13, 12, 16, 12};
# int HDSO_Users_by_BBU_22 [number_of_BBUs] = {15, 16, 16, 16, 15, 18};
# int HDSO_Users_by_BBU_23 [number_of_BBUs] = {14, 15, 10, 21, 9, 28};

# map_HDSO_Users_by_BBU[0] = HDSO_Users_by_BBU_0;
# map_HDSO_Users_by_BBU[1] = HDSO_Users_by_BBU_1;
# map_HDSO_Users_by_BBU[2] = HDSO_Users_by_BBU_2;
# map_HDSO_Users_by_BBU[3] = HDSO_Users_by_BBU_3;
# map_HDSO_Users_by_BBU[4] = HDSO_Users_by_BBU_4;
# map_HDSO_Users_by_BBU[5] = HDSO_Users_by_BBU_5;
# map_HDSO_Users_by_BBU[6] = HDSO_Users_by_BBU_6;
# map_HDSO_Users_by_BBU[7] = HDSO_Users_by_BBU_7;
# map_HDSO_Users_by_BBU[8] = HDSO_Users_by_BBU_8;
# map_HDSO_Users_by_BBU[9] = HDSO_Users_by_BBU_9;
# map_HDSO_Users_by_BBU[10] = HDSO_Users_by_BBU_10;
# map_HDSO_Users_by_BBU[11] = HDSO_Users_by_BBU_11;
# map_HDSO_Users_by_BBU[12] = HDSO_Users_by_BBU_12;
# map_HDSO_Users_by_BBU[13] = HDSO_Users_by_BBU_13;
# map_HDSO_Users_by_BBU[14] = HDSO_Users_by_BBU_14;
# map_HDSO_Users_by_BBU[15] = HDSO_Users_by_BBU_15;
# map_HDSO_Users_by_BBU[16] = HDSO_Users_by_BBU_16;
# map_HDSO_Users_by_BBU[17] = HDSO_Users_by_BBU_17;
# map_HDSO_Users_by_BBU[18] = HDSO_Users_by_BBU_18;
# map_HDSO_Users_by_BBU[19] = HDSO_Users_by_BBU_19;
# map_HDSO_Users_by_BBU[20] = HDSO_Users_by_BBU_20;
# map_HDSO_Users_by_BBU[21] = HDSO_Users_by_BBU_21;
# map_HDSO_Users_by_BBU[22] = HDSO_Users_by_BBU_22;
# map_HDSO_Users_by_BBU[23] = HDSO_Users_by_BBU_23;




# int HDSO_RRH_by_BBU_0 [number_of_BBUs] = {2, 1, 1, 4, 2, 2};
# int HDSO_RRH_by_BBU_1 [number_of_BBUs] = {1, 3, 2, 2, 1, 1};
# int HDSO_RRH_by_BBU_2 [number_of_BBUs] = {2, 2, 2, 3, 2, 1};
# int HDSO_RRH_by_BBU_3 [number_of_BBUs] = {2, 1, 1, 2, 2, 1};
# int HDSO_RRH_by_BBU_4 [number_of_BBUs] = {1, 2, 1, 3, 1, 1};
# int HDSO_RRH_by_BBU_5 [number_of_BBUs] = {1, 2, 1, 1, 1, 1};
# int HDSO_RRH_by_BBU_6 [number_of_BBUs] = {1, 1, 2, 1, 1, 1};
# int HDSO_RRH_by_BBU_7 [number_of_BBUs] = {1, 0, 0, 1, 0, 0};
# int HDSO_RRH_by_BBU_8 [number_of_BBUs] = {1, 0, 0, 0, 0, 1};
# int HDSO_RRH_by_BBU_9 [number_of_BBUs] = {0, 1, 1, 0, 0, 0};
# int HDSO_RRH_by_BBU_10 [number_of_BBUs] = {2, 2, 1, 1, 2, 1};
# int HDSO_RRH_by_BBU_11 [number_of_BBUs] = {2, 2, 2, 3, 1, 3};
# int HDSO_RRH_by_BBU_12 [number_of_BBUs] = {2, 1, 4, 2, 3, 1};
# int HDSO_RRH_by_BBU_13 [number_of_BBUs] = {3, 1, 1, 2, 4, 1};
# int HDSO_RRH_by_BBU_14 [number_of_BBUs] = {3, 2, 2, 1, 2, 2};
# int HDSO_RRH_by_BBU_15 [number_of_BBUs] = {2, 2, 3, 4, 1, 1};
# int HDSO_RRH_by_BBU_16 [number_of_BBUs] = {1, 1, 3, 3, 2, 2};
# int HDSO_RRH_by_BBU_17 [number_of_BBUs] = {1, 3, 2, 3, 2, 2};
# int HDSO_RRH_by_BBU_18 [number_of_BBUs] = {2, 1, 3, 1, 3, 2};
# int HDSO_RRH_by_BBU_19 [number_of_BBUs] = {2, 4, 3, 1, 1, 1};
# int HDSO_RRH_by_BBU_20 [number_of_BBUs] = {2, 3, 3, 2, 1, 2};
# int HDSO_RRH_by_BBU_21 [number_of_BBUs] = {3, 3, 2, 2, 1, 2};
# int HDSO_RRH_by_BBU_22 [number_of_BBUs] = {2, 2, 3, 1, 3, 2};
# int HDSO_RRH_by_BBU_23 [number_of_BBUs] = {3, 1, 2, 3, 1, 2};


# map_HDSO_RRH_by_BBU[0] = HDSO_RRH_by_BBU_0;
# map_HDSO_RRH_by_BBU[1] = HDSO_RRH_by_BBU_1;
# map_HDSO_RRH_by_BBU[2] = HDSO_RRH_by_BBU_2;
# map_HDSO_RRH_by_BBU[3] = HDSO_RRH_by_BBU_3;
# map_HDSO_RRH_by_BBU[4] = HDSO_RRH_by_BBU_4;
# map_HDSO_RRH_by_BBU[5] = HDSO_RRH_by_BBU_5;
# map_HDSO_RRH_by_BBU[6] = HDSO_RRH_by_BBU_6;
# map_HDSO_RRH_by_BBU[7] = HDSO_RRH_by_BBU_7;
# map_HDSO_RRH_by_BBU[8] = HDSO_RRH_by_BBU_8;
# map_HDSO_RRH_by_BBU[9] = HDSO_RRH_by_BBU_9;
# map_HDSO_RRH_by_BBU[10] = HDSO_RRH_by_BBU_10;
# map_HDSO_RRH_by_BBU[11] = HDSO_RRH_by_BBU_11;
# map_HDSO_RRH_by_BBU[12] = HDSO_RRH_by_BBU_12;
# map_HDSO_RRH_by_BBU[13] = HDSO_RRH_by_BBU_13;
# map_HDSO_RRH_by_BBU[14] = HDSO_RRH_by_BBU_14;
# map_HDSO_RRH_by_BBU[15] = HDSO_RRH_by_BBU_15;
# map_HDSO_RRH_by_BBU[16] = HDSO_RRH_by_BBU_16;
# map_HDSO_RRH_by_BBU[17] = HDSO_RRH_by_BBU_17;
# map_HDSO_RRH_by_BBU[18] = HDSO_RRH_by_BBU_18;
# map_HDSO_RRH_by_BBU[19] = HDSO_RRH_by_BBU_19;
# map_HDSO_RRH_by_BBU[20] = HDSO_RRH_by_BBU_20;
# map_HDSO_RRH_by_BBU[21] = HDSO_RRH_by_BBU_21;
# map_HDSO_RRH_by_BBU[22] = HDSO_RRH_by_BBU_22;
# map_HDSO_RRH_by_BBU[23] = HDSO_RRH_by_BBU_23;