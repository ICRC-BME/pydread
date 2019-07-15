#include <Python.h>

#include "dlib.c"

// This is fot python 2 only
// static char pydread_docstring[] =
//     "Module to read .d files.";

static char read_d_header_docstring[] =
    "Function to read .d file header.\n\n\
     Parameters\n\
     ----------\n\
     file_path: str\n\
         Path to .d file";

static char read_d_data_docstring[] =
    "Function to read .d file data.\n\n\
     Parameters\n\
     ----------\n\
     file_path: str\n\
        Path to .d file\n\
     channel_map: list\n\
        List of channel indices in int\n\
     start_samp: int\n\
        Start sample to read\n\
     stop_samp: int\n\
        Stop sample to read";

/* Pyhon object declaration*/
static PyObject *read_d_header(PyObject *self, PyObject *args);
static PyObject *read_d_data(PyObject *self, PyObject *args);

/* Numpy structure create functions */
// static PyObject *create_eashdr_dtype();

/* Specification of the members of the module */
static PyMethodDef module_methods[] = {
    {"read_d_header", read_d_header, METH_VARARGS, read_d_header_docstring},
    {"read_d_data", read_d_data, METH_VARARGS, read_d_data_docstring},

    //{"create_eashdr_dtype", create_eashdr_dtype, 0, NULL},
    {NULL, NULL, 0, NULL}
};

/* Definition of struct for python 3 */
static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "pydread",     /* m_name */
    "This module provides functions for reading .d files",  /* m_doc */
    -1,                  /* m_size */
    module_methods,    /* m_methods */
    NULL,                /* m_reload */
    NULL,                /* m_traverse */
    NULL,                /* m_clear */
    NULL,                /* m_free */
};

/* Module initialisation */
PyObject * PyInit_pydread(void)
{
    PyObject *m = PyModule_Create(&moduledef);

    if (m == NULL)
        return NULL;

    return m;
}

/*Helper functions*/
PyObject *map_d_standard_header(S_HEADER *sh);
PyObject *map_d_val(D_VAL *d_val);
PyObject *map_d_extended_header(D_HEADER *h);