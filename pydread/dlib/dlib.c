
#include "dlib.h"

D_HEADER *read_header(FILE *fp){

    D_HEADER 	*header;
    D_VAL 	*data_info;

    // S_HEADER
    ui1 	d_val;
    ui4     data_pos;
    
    // X_HEADER
    si1     cont;
    //char     mnemo_str[2];
    ui2     mnemo, field_len, *xhdr_field_buffer;
    ui1     *xhdr_temp_buffer;
    si4     precision;
    si4     nb;
    ui8     add_number; // For d file bug corrections
    ui1     val_0, val_1, val_2, val_3;
    TAG_TABLE_ENTRY *tte;


    // General
    ui1     read_tag_table;
    si4     i, j;
    ui8     tag_def_post, sz;
    ui2     counter, ntags;

    // Allocate header - parent function is responsible for deallocation
    header = (D_HEADER *) malloc(sizeof(D_HEADER));

    header->sh = (S_HEADER *) malloc(sizeof(S_HEADER));
    data_info = (D_VAL *) malloc(sizeof(D_VAL));

    header->xh = (X_HEADER *) calloc(1,sizeof(X_HEADER));

    // ----- Standard header -----
    fseek(fp,0,SEEK_SET);
	if (fread(header->sh, sizeof(S_HEADER), 1, fp) == 0){
		printf("Error reading file! Exiting...\n");
		exit(1);
	}

    // make sure sign ends with 0
    header->sh->sign[14] = 0;

	// Get data info
    d_val = header->sh->d_val;
    data_info->data_invalid = (d_val >> 7) & 0x01;
    data_info->data_packed = (d_val >> 6) & 0x01;
    data_info->block_structure = (d_val >> 5) & 0x01;
    data_info->polarity = (d_val >> 4) & 0x01;
    data_info->data_calib = (d_val >> 3) & 0x01;
    data_info->data_modified = (d_val >> 2) & 0x01;
    data_info->data_cell_size = (d_val >> 0) & 0x03;

    header->sh->data_info = data_info;

    // Fix data_org_fields -> conversion from paragraphs to bytes (see documentation)
    header->sh->data_org *= 16;
    header->sh->xhdr_org *= 16;

    //----- Extended header -----
    // Allocate temporary buffer
    xhdr_field_buffer = malloc(2 * sizeof(ui2));
    xhdr_temp_buffer = malloc(8192 * sizeof(ui1)); // NOTE: the largest value is list_len

    // Read the header fields
    if (header->sh->xhdr_org != 0){
    	fseek(fp,header->sh->xhdr_org,SEEK_SET);
    	cont = 1;        
    	while (cont){            
    		if (fread(xhdr_field_buffer,sizeof(ui2),2,fp) == 0){
    			printf("Error reading file! Exiting...\n");
				exit(1);
    		}
    		mnemo = *xhdr_field_buffer;
    		field_len = *(xhdr_field_buffer + 1);

            if ((field_len != 0) & (mnemo != EASXHDR_BLANK_SPACE_CODE)){
                if (fread(xhdr_temp_buffer,field_len,1,fp) != 1){
                    printf("Error reading file! Exiting...\n");
                    free(xhdr_field_buffer);
                    free(xhdr_temp_buffer);
                    exit(1);
                }
            }
            
    		switch (mnemo) {
    			case EASXHDR_AUTHENTICATION_KEY_CODE: // Authentication key
                    header->xh->authentication_key = *(ui4 *) xhdr_temp_buffer;
                    break;

                case EASXHDR_BLANK_SPACE_CODE: // Blank space
                    fseek(fp, field_len, SEEK_CUR);
                    break;

    			// case EASXHDR_BLOCK_VARIABLE_CODE: // Block variable list

    			case EASXHDR_CHANNEL_ATTRIBUTES_CODE: // Channel attributes
                    header->xh->channel_attributes = (si1 *) calloc(sizeof(si1), field_len);
                    memcpy(header->xh->channel_attributes, (si1 *) xhdr_temp_buffer, field_len);
                    break;

    			// case EASXHDR_CALIBRATION_INFO_CODE: // Calibration info

    			case EASXHDR_CHANNEL_NAMES_CODE: // Channel names
                    for (i=0; i<field_len; i+=4){
                        for (j=0;j<4;++j){
                            header->xh->channel_names[i/4][j] = *(xhdr_temp_buffer+i+j);
                        }
                        header->xh->channel_names[i/4][j] = 0;
                    }
                    break;

    			case EASXHDR_DISPOSE_FLAGS_CODE: // Dispose flags
                    header->xh->dispose_flags = *(ui2 *) xhdr_temp_buffer;
                    break;

    			case EASXHDR_DATA_INFO_CODE: // Data info
                    header->xh->data_info = malloc(field_len);
                    memcpy(header->xh->data_info, (si1 *) xhdr_temp_buffer, field_len);
                    break;

                // case EASXHDR_FILE_LINKS_CODE: // File links

                case EASXHDR_FREQUENCY_OF_SAMPLING_CODE: // Frequency os sampling
                    header->xh->fractional_sampling_frequency = *(sf4 *) xhdr_temp_buffer;
                    break;

                case EASXHDR_PATIENT_ID_CODE: // Patient ID
                    header->xh->patient_id_number = *(ui4 *) xhdr_temp_buffer;
                    break;

                case EASXHDR_PROJECT_NAME_CODE: // Project name
                    strncpy(header->xh->project_name, (si1 *) xhdr_temp_buffer, PROJECT_NAME_BYTES);
                    break;
                // case EASXHDR_R_BLOCK_CODE: // R block

                // case EASXHDR_SOURCE_FILE_CODE: // Source file

                case EASXHDR_TEXT_RECORD_CODE: // Text record
                    header->xh->text_record = (si1 *) calloc(sizeof(si1), field_len);
                    strcpy(header->xh->text_record, (si1 *) xhdr_temp_buffer);
                    break;

    			case EASXHDR_TIME_INFO_CODE: // Time info
                    header->xh->time_info = *(ui4 *) xhdr_temp_buffer;
                    break;

    			case EASXHDR_TAG_TABLE_CODE: // Tag table
                    header->xh->tag_table_info = *(XH_TT *) xhdr_temp_buffer;
                    break;

    			case EASXHDR_TEXT_EXTENSION_CODE: // Text extrarec
                    header->xh->text_extension_record = (si1 *) calloc(sizeof(si1), field_len);
                    strcpy(header->xh->text_extension_record, (si1 *) xhdr_temp_buffer);
                    break;

    			case EASXHDR_END_CODE: // Data 
    				cont=0;
    				data_pos = ftell(fp);                    
                    break;
                default:                    
                    break;
    		}


    	}
    }else{
    	data_pos = ftell(fp);
    }

    // Fix d-file bugs
    precision = get_prec(header->sh);
    nb = prec_bytes(precision);

    add_number=1;
    for (i=0;i<8;++i)
        add_number *= 16;
    
    header->xh->corr_tag_table_info.def_len = header->xh->tag_table_info.def_len;
    header->xh->corr_tag_table_info.list_len = header->xh->tag_table_info.list_len;
    header->xh->corr_tag_table_info.def_off = (ui8) header->xh->tag_table_info.def_off;
    header->xh->corr_tag_table_info.list_off = (ui8) header->xh->tag_table_info.list_off;
    
    while ((header->xh->corr_tag_table_info.def_off < ((ui8) header->sh->nchan * header->sh->nsamp * nb) + header->sh->data_org) && (header->xh->corr_tag_table_info.def_off > data_pos)){
        header->xh->corr_tag_table_info.def_off += add_number;
        header->xh->corr_tag_table_info.list_off += add_number;
    }

    // Are there any tags? If so, check that we can read them
    if (header->xh->corr_tag_table_info.list_len > 0){
        fseek(fp, 0, SEEK_END);
        sz = ftell(fp);
        if (sz > header->xh->corr_tag_table_info.list_off){
            read_tag_table = 1;
        }else{
            read_tag_table = 0;
            header->xh->tags = NULL;
        }
    }else{
        read_tag_table = 0;
        header->xh->tags = NULL;
    }

    // Read tags if available        
    if (read_tag_table){

        // Allocate tags
        ntags = header->xh->corr_tag_table_info.list_len/4;
        header->xh->tags = calloc(ntags,sizeof(TAG));

        // Read tag list
        fseek(fp,header->xh->corr_tag_table_info.list_off,SEEK_SET);
        free(xhdr_temp_buffer);
        xhdr_temp_buffer = malloc(header->xh->corr_tag_table_info.list_len * sizeof(ui1));
        if (fread(xhdr_temp_buffer,header->xh->corr_tag_table_info.list_len,1,fp) != 1){
            printf("Error reading file at %d\n", __LINE__);
            free(xhdr_field_buffer);
            free(xhdr_temp_buffer);
            exit(1);
        }
        for (i=0; i<header->xh->corr_tag_table_info.list_len; i+=4){
            
            val_0 = (ui1)*(xhdr_temp_buffer+i+0);
            val_1 = (ui1)*(xhdr_temp_buffer+i+1);
            val_2 = (ui1)*(xhdr_temp_buffer+i+2) % 128;
            val_3 = (ui1)*(xhdr_temp_buffer+i+3);

            header->xh->tags[i/4].tag_pos = (val_0*1) + (val_1*256) + (val_2*65536);
            header->xh->tags[i/4].tag_class = val_3 % 128;
            header->xh->tags[i/4].tag_selected = val_3 / 128;
            
        }
        // Fix the long datasets bug in D-file (positions > 2 ** 23-1)
        add_number=1;
        for (i=0;i<23;++i)
            add_number *= 2;
        if (header->xh->tags[0].tag_pos != 0){
            cont = 1;
            while (cont){
                cont = 0;
                for (i=0;i<(header->xh->corr_tag_table_info.list_len/4)-1;++i){
                    if (((si4) header->xh->tags[i+1].tag_pos - (si4) header->xh->tags[i].tag_pos) < 0){
                        for (j=i+1;j<(header->xh->corr_tag_table_info.list_len/4);++j){
                            header->xh->tags[j].tag_pos += add_number;
                        }
                    cont = 1;
                    }
                }
            }
        }

        // Read the tag table - this is stupidly done but works
        tag_def_post = header->xh->corr_tag_table_info.def_off;
        tte = (TAG_TABLE_ENTRY *) calloc(1,sizeof(TAG_TABLE_ENTRY));
        counter = 0;
        while (counter*8 < header->xh->corr_tag_table_info.def_len){ // Loop accors tag table


            fseek(fp,tag_def_post,SEEK_SET);
            if (fread(tte,sizeof(TAG_TABLE_ENTRY),1,fp) != 1){
                printf("Error reading tag_table, the end of file reached...\n");
                break;
                // printf("Error reading file at %d\n", __LINE__);
                // free(xhdr_field_buffer);
                // free(xhdr_temp_buffer);
                // exit(1);
            }
            // printf("TTE abrv=%c%c\n",tte->abrv[0],tte->abrv[1]);
            // printf("TTE n=%i\n",tte->n);
            // printf("TTE txtlen=%i\n",tte->txtlen);
            // printf("TTE txtoff=%i\n",tte->txtoff);

            if (tte->n == 32768){
                break;
            }
            tag_def_post += 8;


            fseek(fp,tte->txtoff + header->xh->corr_tag_table_info.def_off,SEEK_SET);
            if (fread(xhdr_temp_buffer,tte->txtlen,1,fp) != 1){
                break;
            }

            counter++;
        }
    }


    // Clean up
    free(xhdr_temp_buffer);
    free(xhdr_field_buffer);

    return header;
}

void free_header_struct(D_HEADER *dh){
    // free tags
    if ( dh->xh->tags != NULL)
        free(dh->xh->tags);

    if ( dh->xh->data_info != NULL)
        free(dh->xh->data_info);

    if ( dh->xh->text_extension_record != NULL)
        free(dh->xh->text_extension_record);

    if ( dh->xh->text_record != NULL)
        free(dh->xh->text_record);

    return;
}

void show_s_header(S_HEADER *sh){

    si4 i;

    printf("\n----- D-FILE STANDARD HEADER -----\n");
    printf("Sign: ");
    for (i=0;i<15;++i){
    	printf("%c",sh->sign[i]);
    };
    printf("\n");

    printf("Ftype: %c\n",sh->ftype);
    printf("Nchan: %u\n",sh->nchan);
    printf("Naux: %u\n",sh->naux);
    printf("Fsamp: %u\n",sh->fsamp);
    printf("Nsamp: %u\n",sh->nsamp);
    printf("Dval: %u\n",sh->d_val);
    printf("Unit: %u\n",sh->unit);
    printf("Zero: %u\n",sh->zero);
    printf("Data_org: %u\n",sh->data_org);
    printf("Data_xhdr_org: %i\n",sh->xhdr_org);

    printf("\n*** Data info ***\n");
    printf("Data invalid: %i\n",sh->data_info->data_invalid);
    printf("Data packed: %i\n",sh->data_info->data_packed);
    printf("Block structure: %i\n",sh->data_info->block_structure);
    printf("Polarity: %i\n",sh->data_info->polarity);
    printf("Data calibration: %i\n",sh->data_info->data_calib);
    printf("Data modified: %i\n",sh->data_info->data_modified);
    printf("Data cell size: %i\n",sh->data_info->data_cell_size);


    return;
}

void show_x_header(X_HEADER *xh){

    si4 i;

    printf("\n----- D-FILE EXTENDED HEADER -----\n");
    printf("Channel names: \n");
    for (i=0;i<512;++i){
        if (xh->channel_names[i][0] == 0)
            break;

        printf("%s",xh->channel_names[i]);
        
        printf(", ");
        if ((i+1) % 10 == 0)
            printf("\n");
    }
    printf("\n");
    printf("Authentication key: %i\n",xh->authentication_key);
    printf("Dispose flags int: %i\n", xh->dispose_flags);
    printf("Time info: %i\n",xh->time_info);
    printf("Data info: %s\n",xh->data_info);
    printf("Fractional sampling frequency: %f\n", xh->fractional_sampling_frequency);
    printf("Patient ID number: %d\n", xh->patient_id_number);
    printf("Project name:");
    for (i=0; i<PROJECT_NAME_BYTES; ++i){
        printf("%c",xh->project_name[i]);
    }
    printf("\n");
    printf("Text record: %s\n", xh->text_record);
    printf("Text extension record: %s\n", xh->text_extension_record);
    printf("Tag table: \n");
    printf("%i, ",xh->corr_tag_table_info.def_len);
    printf("%i, ",xh->corr_tag_table_info.list_len);
    printf("%llu, ",xh->corr_tag_table_info.def_off);
    printf("%llu\n",xh->corr_tag_table_info.list_off);

    if ( xh->tags != NULL ){
        printf("Tags: \n");
        for (i=0;i<512;++i){
            if (xh->tags[i].tag_pos == 0)
                break;
            
            printf("tag %i; tag pos: %u, tag class: %u, tag_selected: %u\n",i,xh->tags[i].tag_pos,xh->tags[i].tag_class,xh->tags[i].tag_selected);
        }
    }
    return;
}

si4 get_prec(S_HEADER *sh){

    if (sh->ftype == *"D"){
        if (sh->data_info->data_cell_size == 2)
            return PREC_SI2;
        else if (sh->data_info->data_cell_size == 3)
            return PREC_SI4;
        else
            return PREC_UI1;
    }else if (sh->ftype == *"R"){
        return PREC_SF4;
    }else{
        return PREC_UI1;
    }
}

si4 prec_bytes(si4 precision){
    switch (precision){
        case PREC_UI1:
            return 1;
        case PREC_SI2:
            return 2;
        case PREC_SI4:
            return 4;
        case PREC_SF4:
            return 4;
        default:
            return 4;
    }
}

void read_data(FILE *fp, D_HEADER *header, void *data_buffer, ui2 *channel_ids, ui2 n_channels_to_read, ui8 start_samp, ui8 stop_samp){

    si4     nb;
    si4     precision;
    ui8 	n_samp, first_sample;
    ui4 	i,j;
    void    *temporary_buffer;

    precision = get_prec(header->sh);    
    nb = prec_bytes(precision);

    n_samp = stop_samp - start_samp;


    // Seek to the first data sample - the block
    first_sample = header->sh->data_org + start_samp*nb*header->sh->nchan;
    fseek(fp,first_sample,SEEK_SET);

    // Read all data in given segment
    if (precision == PREC_UI1){
        temporary_buffer = malloc(sizeof(ui1)*header->sh->nchan);
        for (i=0;i<n_samp;++i){
            if (fread(temporary_buffer, nb, header->sh->nchan, fp) == 0){
                printf("Error reading data! Exiting...\n");
                exit(1);
            }

            for (j=0;j<n_channels_to_read;++j){
                memcpy((ui1*)data_buffer+(i*n_channels_to_read+j),(ui1*)temporary_buffer + channel_ids[j],nb);
            }
        }
    }
    else if (precision == PREC_SI2){
        temporary_buffer = malloc(sizeof(si2)*header->sh->nchan);
        for (i=0;i<n_samp;++i){
            if (fread(temporary_buffer, nb, header->sh->nchan, fp) == 0){
                printf("Error reading data! Exiting...\n");
                exit(1);
            }

            for (j=0;j<n_channels_to_read;++j){
                memcpy((si2*)data_buffer+(i*n_channels_to_read+j),(si2*)temporary_buffer + channel_ids[j],nb);
            }
        }
    }
    else if (precision == PREC_SI4){
        temporary_buffer = malloc(sizeof(si4)*header->sh->nchan);        
        for (i=0;i<n_samp;++i){
            if (fread(temporary_buffer, nb, header->sh->nchan, fp) == 0){                
                printf("Error reading data! Exiting...\n");
                exit(1);
            }

            for (j=0;j<n_channels_to_read;++j){
                memcpy((si4*)data_buffer+(i*n_channels_to_read+j),(si4*)temporary_buffer + channel_ids[j],nb);
            }
        }
    }
    else if (precision == PREC_SF4){
        temporary_buffer = malloc(sizeof(sf4)*header->sh->nchan);
        for (i=0;i<n_samp;++i){
            if (fread(temporary_buffer, nb, header->sh->nchan, fp) == 0){
                printf("Error reading data! Exiting...\n");
                exit(1);
            }

            for (j=0;j<n_channels_to_read;++j){
                memcpy((sf4*)data_buffer+(i*n_channels_to_read+j),(si4*)temporary_buffer + channel_ids[j],nb);
            }
        }
    }
    else{
        temporary_buffer = malloc(sizeof(si4)*header->sh->nchan);
        for (i=0;i<n_samp;++i){
            if (fread(temporary_buffer, nb, header->sh->nchan, fp) == 0){
                printf("Error reading data! Exiting...\n");
                exit(1);
            }

            for (j=0;j<n_channels_to_read;++j){
                memcpy((si4*)data_buffer+(i*n_channels_to_read+j),(si4*)temporary_buffer + channel_ids[j],nb);
            }
        }
    }

    free(temporary_buffer);
    return;
}