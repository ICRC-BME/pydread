#include <Python.h>

#include "dlib.c"

// This is fot python 2 only
// static char pydread_docstring[] =
//     "Module to read .d files.";

static char read_d_header_docstring[] =
    "Function to read .d file header read_d_header(file_path).";

static char read_d_data_docstring[] =
    "Function to read .d file data read_d_data(file_path, channel_map, start_samp, stop_samp).";

/* Pyhon object declaration*/
static PyObject *read_d_header(PyObject *self, PyObject *args);
static PyObject *read_d_data(PyObject *self, PyObject *args);

/* Specification of the members of the module */
static PyMethodDef module_methods[] = {
    {"read_d_header", read_d_header, METH_VARARGS, read_d_header_docstring},
    {"read_d_data", read_d_data, METH_VARARGS, read_d_data_docstring},
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