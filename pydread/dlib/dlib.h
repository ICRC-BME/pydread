
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
    si2 data_xhdr_org;

    si4 data_pos;

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
    si1 tag_class_txt[64];
} TAG;

typedef struct {
    si1 abrv[2];
    ui2 n;
    ui2 txtlen;
    ui2 txtoff;
} TAG_TABLE_ENTRY;


typedef struct {
    // ui2 authentication_key;
    // ui2 *block_var_list;
    // ui2 *channel_atrib;
    // ui2 *calib_info;
    si1 channel_names[512][5];
    // ui2 dispose_flags;
    // si2 freq[2];
    ui4 time_info;
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
void show_s_header(S_HEADER *sh);
void show_x_header(X_HEADER *xh);
si4 get_prec(S_HEADER *sh);
si4 prec_bytes(si4 precision);
void read_data(FILE *fp, void *data_buffer, ui2 *channel_ids, ui2 n_channels_to_read, ui8 start_samp, ui8 stop_samp);
