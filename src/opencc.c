/* Opencc module using dictionary interface */

#include "Python.h"
#include <opencc.h>

typedef struct {
    PyObject_HEAD
    opencc_t *opencc;
} openccobject;

static PyTypeObject Opencctype;

static PyObject *
new_opencc_object(char *config)
{
    openccobject *dp;

    dp = PyObject_New(openccobject, &Opencctype);
    if (dp == NULL)
        return NULL;

    if (!(dp->opencc = opencc_open(config))) {
        PyErr_SetString(PyExc_IOError, "Cannot open config file.");
        Py_DECREF(dp);
        return NULL;
    }

    return (PyObject *)dp;
}

/* Methods */

void
_opencc_close(openccobject* self)
{
    if (self->opencc) {
        opencc_close(self->opencc);
        self->opencc = NULL;
    }
}

static void
opencc_dealloc(openccobject* self)
{
    _opencc_close(self);
    PyObject_Del(self);
}

static PyObject *
opencc__close(register openccobject *dp, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":close"))
        return NULL;
    _opencc_close(dp);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
opencc__convert(register openccobject *dp, PyObject *args)
{
    PyObject *str;
    PyObject *ret;
    PyObject *tmp;

    int  is_unicode = 0;
    char *out = NULL;

    if (!PyArg_ParseTuple(args, "O:convert", &str))
        return NULL;

    if(!dp->opencc){
        PyErr_SetString(PyExc_ValueError, "ValueError: I/O operation on closed file.");
        return NULL;
    }

    if (PyString_Check(str)){
        tmp = str;
    }else if (PyUnicode_Check(str)){
        is_unicode = 1;
        tmp = PyUnicode_AsUTF8String(str);
    }else{
        PyErr_SetString(PyExc_TypeError, "TypeError: must be string or buffer.");
        return NULL;
    }

    out = opencc_convert_utf8(dp->opencc,
                PyString_AsString(tmp),
                PyString_Size(tmp));

    ret = PyString_FromString(out);
    PyMem_Free(out);

    if(is_unicode){
        Py_DECREF(tmp);
        tmp = PyString_AsDecodedObject(ret, "utf8", NULL);
        Py_DECREF(ret);
        ret = tmp;
    }

    return ret;
}

static PyObject *
opencc__set_conversion_mode(register openccobject *dp, PyObject *args)
{
    long conversion_mode;

    if (!PyArg_ParseTuple(args, "l:set_conversion_mode", &conversion_mode))
        return NULL;

    if(conversion_mode < 0 || conversion_mode > 2){
        PyErr_SetString(PyExc_ValueError, "ValueError: conversion mode must be in [0,1,2].");
        return NULL;
    }

    opencc_set_conversion_mode(dp->opencc, conversion_mode);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef opencc_methods[] = {
    {"close", (PyCFunction)opencc__close, METH_VARARGS,
     "close()\nClose the handler."},
    {"set_conversion_mode", (PyCFunction)opencc__set_conversion_mode, METH_VARARGS,
     "set_conversion_mode(conversion_mode)\nset conversion mode."},
    {"convert", (PyCFunction)opencc__convert, METH_VARARGS,
     "convert(text) -> value\n"
     "Return conversion."},
    {NULL, NULL} /* sentinel */
};

static PyObject *
opencc_getattr(openccobject *dp, char *name)
{
    return Py_FindMethod(opencc_methods, (PyObject *)dp, name);
}

static PyTypeObject Opencctype = {
    PyObject_HEAD_INIT(NULL)
    0,
    "opencc.OpenCC",
    sizeof(openccobject),
    0,
    (destructor)opencc_dealloc,  /*tp_dealloc*/
    0,                          /*tp_print*/
    (getattrfunc)opencc_getattr, /*tp_getattr*/
    0,                          /*tp_setattr*/
    0,                          /*tp_compare*/
    0,                          /*tp_repr*/
    0,                          /*tp_as_number*/
    0,                          /*tp_as_sequence*/
    0,                          /*tp_as_mapping*/
};

/* ----------------------------------------------------------------- */
/* opencc module                                                      */
/* ----------------------------------------------------------------- */

static PyObject *
opencc_new(PyObject *self, PyObject *args)
{
    char *config_file = "zht2zhs.ini";

    if (!PyArg_ParseTuple(args, "s:open", &config_file))
        return NULL;

    if (config_file == NULL || *config_file == 0){
        config_file = "zht2zhs.ini";
    }

    return new_opencc_object(config_file);
}

static PyMethodDef openccmodule_methods[] = {
    { "new", (PyCFunction)opencc_new, METH_VARARGS,
      "new(config_file) -> mapping\n"
      "Return a opencc object."},
    { "OpenCC", (PyCFunction)opencc_new, METH_VARARGS,
      "OpenCC(config_file) -> mapping\n"
      "Return a opencc object."},
    { 0, 0 },
};

PyMODINIT_FUNC
initopencc(void) {
    PyObject *m, *d, *s;

    Opencctype.ob_type = &PyType_Type;

    m = Py_InitModule("opencc", openccmodule_methods);
    if (m == NULL)
        return;

    d = PyModule_GetDict(m);
    s = PyString_FromString("OpenCC Python binding");
    if (s != NULL) {
        PyDict_SetItemString(d, "__doc__", s);
        Py_DECREF(s);
    }

    s = PyString_FromString("0.4.2");
    if (s != NULL) {
        PyDict_SetItemString(d, "__version__", s);
        Py_DECREF(s);
    }

    // Default conversion mode.
    PyDict_SetItem(d, Py_BuildValue("s", "CONVERSION_FAST"),
        PyLong_FromLong((long)0));

    // Only converts text into segments.
    PyDict_SetItem(d, Py_BuildValue("s", "CONVERSION_SEGMENT_ONLY"),
        PyLong_FromLong((long)1));

    // List all candidates of every segment.
    PyDict_SetItem(d, Py_BuildValue("s", "CONVERSION_LIST_CANDIDATES"),
        PyLong_FromLong((long)2));
}
