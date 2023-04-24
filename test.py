#!/usr/bin/env python3
# -*- coding: utf-8 -*-


# import sys,time

# sys.path.append('/home/jan_cimbalnik/Dropbox/Source/Python/Libraries_shared')


#import D_file_read

# file_name = '/home/jan_cimbalnik/Dropbox/Source/C/pydread/Easrec_ic_exp-032_141008-1041.d'

# file_name = "/media/jan_cimbalnik/DATADRIVE2/data-d_fnusa_oragnizace/seeg/seeg-062-170614/Easrec_sciexp-seeg062_170614-0825.d";


# file_name = "/media/jan_cimbalnik/DATADRIVE2/data-d_fnusa_oragnizace/seeg/seeg-036-150430/Easrec_ic_exp-036_150429-1404.d";


# xsh,xxh = D_file_read.read_d_header(file_name)

# ch_list = [0,1]

# odata = D_file_read.read_d_data(xsh,ch_list,0,5000)


# %%
import pydread
import numpy as np

# # file_name = '/home/jan_cimbalnik/Dropbox/Source/C/pydread/Easrec_ic_exp-032_141008-1041.d'

# file_name = "/home/patrik/Desktop/CEITEC/C_processing/icveegR4-4282-20181010-222226.d--"
# file_name = "/home/patrik/Desktop/CEITEC/C_processing/icveegR4-4282-20181010-222226.d.uti"
# file_name = "/home/patrik/Desktop/CEITEC/C_processing/icveegR4-4282-20181010-222226.d"

file_name = "../../icveegR4-9755-20220901-090202.d--"
# file_name = "/home/patrik/Desktop/icveegR4-4282-20181010-222226.d"

# %%

sh,xh = pydread.read_d_header(file_name)
nsamp = sh['nsamp']
nchan = sh['nchan']

# ch_list = [0,1]
ch_list = np.arange(nchan)

ndata = pydread.read_d_data(file_name,ch_list.tolist(),0,nsamp)
# ndata = pydread.read_d_data(file_name,ch_list,0,4)
print(ndata)

   

# %% File 2
# file_name = "/home/patrik/Desktop/CEITEC/C_processing/pro_patrika/temp_patrik/icveegR4-4282-20181010-222226.d--"

# sh,xh = pydread.read_d_header(file_name)
# nsamp = sh['nsamp']

# ch_list = [0,1]

# ndata = pydread.read_d_data(file_name,ch_list,0,nsamp)
# print(ndata)
# print(odata)





# %%
