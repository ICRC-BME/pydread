
// Library includes

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// Typedefs

typedef char			si1;
typedef unsigned char		ui1;
typedef short			si2;
typedef unsigned short		ui2;
typedef int			si4;
typedef unsigned int		ui4;
typedef long long int		si8;
typedef long long unsigned int	ui8;
typedef float			sf4;
typedef double			sf8;


// ----- Macros for precision -----

#define PREC_UI1    0
#define PREC_SI2    1
#define PREC_SI4    2
#define PREC_SF4    3

// ----- Header constants -----
#define EASHDR_SIGN_BYTES   15

#define EASXHDR_END_STRING   ""
#define EASXHDR_END_CODE    (ui2) 0x0000
#define EASXHDR_AUTHENTICATION_KEY_STRING   "AU"
#define EASXHDR_AUTHENTICATION_KEY_CODE    (ui2) 0x5541
#define EASXHDR_BLANK_SPACE_STRING   "BS"
#define EASXHDR_BLANK_SPACE_CODE    (ui2) 0x5342
#define EASXHDR_BLOCK_VARIABLE_STRING   "BV"
#define EASXHDR_BLOCK_VARIABLE_CODE    (ui2) 0x5642
#define EASXHDR_CHANNEL_ATTRIBUTES_STRING   "CA"
#define EASXHDR_CHANNEL_ATTRIBUTES_CODE    (ui2) 0x4143
#define EASXHDR_CALIBRATION_INFO_STRING   "CI"
#define EASXHDR_CALIBRATION_INFO_CODE    (ui2) 0x4943
#define EASXHDR_CHANNEL_NAMES_STRING   "CN"
#define EASXHDR_CHANNEL_NAMES_CODE    (ui2) 0x4E43
#define EASXHDR_DISPOSE_FLAGS_STRING   "DF"
#define EASXHDR_DISPOSE_FLAGS_CODE    (ui2) 0x4644
#define EASXHDR_DATA_INFO_STRING   "DI"
#define EASXHDR_DATA_INFO_CODE    (ui2) 0x4944
#define EASXHDR_FILE_LINKS_STRING   "FL"
#define EASXHDR_FILE_LINKS_CODE    (ui2) 0x4C46
#define EASXHDR_FREQUENCY_OF_SAMPLING_STRING   "FS"
#define EASXHDR_FREQUENCY_OF_SAMPLING_CODE    (ui2) 0x5346
#define EASXHDR_PATIENT_ID_STRING   "ID"
#define EASXHDR_PATIENT_ID_CODE    (ui2) 0x4449
#define EASXHDR_PROJECT_NAME_STRING   "PJ"
#define EASXHDR_PROJECT_NAME_CODE    (ui2) 0x4A51
#define EASXHDR_R_BLOCK_STRING   "RB"
#define EASXHDR_R_BLOCK_CODE    (ui2) 0x4252
#define EASXHDR_SOURCE_FILE_STRING   "SF"
#define EASXHDR_SOURCE_FILE_CODE    (ui2) 0x4653
#define EASXHDR_TEXT_RECORD_STRING   "TE"
#define EASXHDR_TEXT_RECORD_CODE    (ui2) 0x4554
#define EASXHDR_TIME_INFO_STRING   "TI"
#define EASXHDR_TIME_INFO_CODE    (ui2) 0x4954
#define EASXHDR_TAG_TABLE_STRING   "TT"
#define EASXHDR_TAG_TABLE_CODE    (ui2) 0x5454
#define EASXHDR_TEXT_EXTENSION_STRING   "TE"
#define EASXHDR_TEXT_EXTENSION_CODE    (ui2) 0x5854

#define PROJECT_NAME_BYTES  12


// ----- Header structures -----

// Standard header

typedef struct {
	si1 data_invalid;
	si1 data_packed;
	si1 block_structure;
	si1 polarity;
	si1 data_calib;
	si1 data_modified;
	si1 data_cell_size;

} D_VAL;

typedef struct {

    si1 sign[15];
    si1 ftype;
    ui1 nchan;
    ui1 naux;
    ui2 fsamp;
    ui4 nsamp;
    ui1 d_val;
    ui1 unit;
    ui2 zero;
    ui2 data_org;
    si2 xhdr_org;

    si4 data_pos; // TODO: remove this after we are sure we do not need it

    D_VAL 	*data_info;

} S_HEADER;

// Extended header

typedef struct {
    ui2 def_len;
    ui2 list_len;
    ui4 def_off;
    ui4 list_off;
} XH_TT;

typedef struct{
    ui2 def_len;
    ui2 list_len;
    ui8 def_off;
    ui8 list_off;
} XH_TT_LONG;

typedef struct {
    ui4 tag_pos;
    ui2 tag_class;
    ui2 tag_selected;
} TAG;

typedef struct {
    si1 abrv[2];
    ui2 n;
    ui2 txtlen;
    ui2 txtoff;
 } TAG_TABLE_ENTRY;


typedef struct {
    ui4 authentication_key;
    // ui2 *block_var_list;
    si1 *channel_attributes;
    // ui2 *calib_info;
    si1 channel_names[512][5];
    ui2 dispose_flags;
    sf4 fractional_sampling_frequency;
    ui4 patient_id_number;
    si1 project_name[PROJECT_NAME_BYTES];
    si1 *text_record;
    ui4 time_info;
    si1 *text_extension_record;
    XH_TT tag_table_info;
    XH_TT_LONG corr_tag_table_info;
    si1 *data_info;
    TAG *tags;	
} X_HEADER;

 // General header

typedef struct {
	S_HEADER *sh;
	X_HEADER *xh;
} D_HEADER;

// ----- Function declarations -----
D_HEADER *read_header(FILE *fp);
void free_header_struct(D_HEADER *dh);
void show_s_header(S_HEADER *sh);
void show_x_header(X_HEADER *xh);
si4 get_prec(S_HEADER *sh);
si4 prec_bytes(si4 precision);
void read_data(FILE *fp, D_HEADER *header, void *data_buffer, ui2 *channel_ids, ui2 n_channels_to_read, ui8 start_samp, ui8 stop_samp);
