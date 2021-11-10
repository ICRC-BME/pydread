#include "pydread.h"

#include <numpy/arrayobject.h>
#include <numpy/npy_math.h>



static PyObject *read_d_header(PyObject *self, PyObject *args){

	si1    *py_file_path;
	D_HEADER 	*header;
	FILE 	*fp;

	PyObject	*sh, *xh, *out_tuple;
    

	// --- Parse the input --- 
    if (!PyArg_ParseTuple(args,"s",
                          &py_file_path)){
        return NULL;
    }

    fp = fopen(py_file_path,"rb");
    header = read_header(fp);

    sh = map_d_standard_header(header->sh);
    xh = map_d_extended_header(header);

    out_tuple = PyTuple_New(2);
    PyTuple_SetItem(out_tuple, 0, sh);
    PyTuple_SetItem(out_tuple, 1, xh);

    // !!!!!!!!!!!!!!!!!!!!!!!!
    // TODO - free the header structures!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!

    fclose(fp);

    return out_tuple; 

}
static PyObject *read_d_data(PyObject *self, PyObject *args){

	si1    *py_file_path;
	PyObject *py_channel_map;
	ui8		py_start_samp;
	ui8 	py_stop_samp;

	ui2 	i,*channel_map,n_channels;
    si4     precision;
	PyObject *temp_item, *py_array_out;
	FILE *fp;
	D_HEADER *header;
	void *numpy_arr_data;

	if (!PyArg_ParseTuple(args,"sO!kk",
                          &py_file_path,
                          &PyList_Type, &py_channel_map,
                          &py_start_samp,
                          &py_stop_samp)){
        return NULL;
    }

    // Initialize numpy
	import_array();

    // Convert python list to C array
    n_channels = (ui2) PyList_Size(py_channel_map);
    channel_map = malloc(sizeof(ui2)*n_channels);

    for (i=0;i<n_channels;++i){
    	temp_item = PyList_GetItem(py_channel_map,i);
    	channel_map[i] = PyLong_AsLong(temp_item);
    }

    // Open the file
    fp = fopen(py_file_path,"rb");

    // Get the header
    header = read_header(fp);

    // Get the precision
    precision = get_prec(header->sh);

    // Allocate create numpy array with specified dtype
    npy_intp dims[2] = {py_stop_samp-py_start_samp,n_channels};

	if ( precision == PREC_UI1 ){
		py_array_out = PyArray_SimpleNew(2, dims, NPY_UINT8);
    	numpy_arr_data = PyArray_GETPTR2(py_array_out, 0, 0);
	} else if (precision == PREC_SI2){
		py_array_out = PyArray_SimpleNew(2, dims, NPY_INT16);
    	numpy_arr_data = PyArray_GETPTR2(py_array_out, 0, 0);
	} else if (precision == PREC_SI4){
		py_array_out = PyArray_SimpleNew(2, dims, NPY_INT32);
    	numpy_arr_data = PyArray_GETPTR2(py_array_out, 0, 0);
	} else if (precision == PREC_SF4){
		py_array_out = PyArray_SimpleNew(2, dims, NPY_FLOAT32);
    	numpy_arr_data = PyArray_GETPTR2(py_array_out, 0, 0);
	} else{
        py_array_out = PyArray_SimpleNew(2, dims, NPY_INT32);
        numpy_arr_data = PyArray_GETPTR2(py_array_out, 0, 0);
    }
        
	read_data(fp, header, numpy_arr_data, channel_map, n_channels, py_start_samp, py_stop_samp);

	fclose(fp);
	free(header);

	return py_array_out;
}


PyObject *map_d_standard_header(S_HEADER *sh){

	PyObject *sh_dict;//, *d_val;

	// Create output dictionary   
    sh_dict = PyDict_New();
    
    
    // Insert entries into dictionary
    PyDict_SetItemString(sh_dict,"sign",Py_BuildValue("s", sh->sign));
    PyDict_SetItemString(sh_dict,"ftype",Py_BuildValue("b", sh->ftype));
    PyDict_SetItemString(sh_dict,"nchan",Py_BuildValue("B", sh->nchan));
    PyDict_SetItemString(sh_dict,"naux",Py_BuildValue("B", sh->naux));
    PyDict_SetItemString(sh_dict,"fsamp",Py_BuildValue("H", sh->fsamp));
    PyDict_SetItemString(sh_dict,"nsamp",Py_BuildValue("I", sh->nsamp));
    PyDict_SetItemString(sh_dict,"d_val",Py_BuildValue("B", sh->d_val));
    PyDict_SetItemString(sh_dict,"unit",Py_BuildValue("B", sh->unit));
    PyDict_SetItemString(sh_dict,"zero",Py_BuildValue("H", sh->zero));
    PyDict_SetItemString(sh_dict,"data_offset",Py_BuildValue("H", sh->data_org));
    PyDict_SetItemString(sh_dict,"xhdr_offset",Py_BuildValue("h", sh->xhdr_org));
    // Map dval
    //d_val = map_d_val(sh->data_info);
    PyDict_SetItemString(sh_dict,"data_info",map_d_val(sh->data_info));

    return sh_dict;

}

PyObject *map_d_val(D_VAL *d_val){

    PyObject *d_val_dict;

    // Create output dictionary   
    d_val_dict = PyDict_New();

    // Insert entries into dictionary

    PyDict_SetItemString(d_val_dict,"data_invalid",Py_BuildValue("b", d_val->data_invalid));
    PyDict_SetItemString(d_val_dict,"data_packed",Py_BuildValue("b", d_val->data_packed));
    PyDict_SetItemString(d_val_dict,"block_structure",Py_BuildValue("b", d_val->block_structure));
    PyDict_SetItemString(d_val_dict,"polarity",Py_BuildValue("b", d_val->polarity));
    PyDict_SetItemString(d_val_dict,"data_calib",Py_BuildValue("b", d_val->data_calib));
    PyDict_SetItemString(d_val_dict,"data_modified",Py_BuildValue("b", d_val->data_modified));
    PyDict_SetItemString(d_val_dict,"data_cell_size",Py_BuildValue("b", d_val->data_cell_size));

    return d_val_dict;

}

PyObject *map_d_extended_header(D_HEADER *h){

    PyObject *xh_dict;
    PyObject *channel_list, *tag_list, *tag;

    si4     i;

    xh_dict = PyDict_New();

    PyDict_SetItemString(xh_dict,"time_info",Py_BuildValue("I", h->xh->time_info));
    PyDict_SetItemString(xh_dict,"data_info",Py_BuildValue("s", h->xh->data_info));
    PyDict_SetItemString(xh_dict,"patient_id",Py_BuildValue("I", h->xh->patient_id_number));
    PyDict_SetItemString(xh_dict,"fractional_sampling_frequency",Py_BuildValue("f", h->xh->fractional_sampling_frequency));

    if (h->xh->project_name[0])
        PyDict_SetItemString(xh_dict, "project_name",
            Py_BuildValue("s", h->xh->project_name));
    else
        PyDict_SetItemString(xh_dict, "project_name", Py_None);

    // Channels
    channel_list = PyList_New(0); // Number of channels could be passed but this hardly matters since it is done only once.
    for (i=0;i<h->sh->nchan;++i){
        PyList_Append(channel_list,Py_BuildValue("s",h->xh->channel_names[i]));
    }
    PyDict_SetItemString(xh_dict,"channel_names",channel_list);

    // Tags
    if (h->xh->tags != NULL){
        tag_list = PyList_New(h->xh->corr_tag_table_info.list_len/4);
        if (h->xh->corr_tag_table_info.list_len > 0){
            for (i=0;i<h->xh->corr_tag_table_info.list_len/4;++i){
                tag = PyList_New(3);
                PyList_SET_ITEM(tag,0,Py_BuildValue("I",h->xh->tags[i].tag_pos));
                PyList_SET_ITEM(tag,1,Py_BuildValue("H",h->xh->tags[i].tag_class));
                PyList_SET_ITEM(tag,2,Py_BuildValue("H",h->xh->tags[i].tag_selected));
         
                PyList_SET_ITEM(tag_list,i,tag);
            }
        }
        PyDict_SetItemString(xh_dict,"tags",tag_list);
    }

    return xh_dict;

}

// static PyObject *create_eashdr_dtype()
// {
//     import_array();

//     // Numpy array out
//     PyObject    *op;
//     PyArray_Descr    *descr;

//     // Build dictionary

//     op = Py_BuildValue("[(s, s, i),\
//                          (s, s, i),\
//                          (s, s, i),\
//                          (s, s, i),\
//                          (s, s, i),\
//                          (s, s, i)]",

//                        "sign", "S", EASHDR_SIGN_BYTES,
//                        "ftype", "S", 1,
//                        "nchan", "u1", 1,
//                        "naux", "u1", 1,
//                        "fsamp", "u2", 1,
//                        "nasamp", "u4", 1,
//                        "d_val", "b", 1,
//                        "unit", "u1", 1,
//                        "zero", "i2", 1,
//                        "data_org", "u2", 1,
//                        "data_xhdr_org", "i2", 1);

//     PyArray_DescrConverter(op, &descr);
//     Py_DECREF(op);

//     PyDict_SetItemString(sh_dict,"data_info",map_d_val(sh->data_info));

//     return (PyObject *) descr;
// }