#ifdef __cplusplus
extern "C" {
#endif
  // #include "<Python.h>"
#include "Python.h"
#include "structmember.h"
#ifdef __cplusplus
}
#endif

#include <string>
#include <fstream>
#include <vector>
#include "tx/tx.hpp"

using namespace std;

typedef struct {
  PyObject_HEAD
  tx_tool::tx *txp;
} Tx;

static void
Tx_dealloc(Tx* self) {
  if (self->txp)
    delete self->txp;
  self->ob_type->tp_free((PyObject*)self);
}

static PyObject *
Tx_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
  Tx *self;

  self = (Tx *)type->tp_alloc(type, 0);
  if (self != NULL) {
    self->txp = NULL;
  }
  return (PyObject *)self;
}

static int
Tx_init(Tx *self, PyObject *args, PyObject *kwds) {
  const char *filename;
  if (!PyArg_ParseTuple(args, "s", &filename))
    return -1;

  tx_tool::tx *txp = new tx_tool::tx;
  if (txp->read(filename) == -1){
    delete txp;
    PyErr_SetString(PyExc_IOError, txp->getErrorLog().c_str());
    return -1;
  }
  self->txp = txp;
  return 0;
}

static PyObject *
Tx_prefixSearch(Tx *self, PyObject *args, PyObject *kwds) {
  const char *head;
  if (!PyArg_ParseTuple(args, "s", &head))
    return NULL;

  size_t retLen;    
  const tx_tool::uint id = self->txp->prefixSearch(head, strlen(head), retLen);
  if (id != tx_tool::tx::NOTFOUND) {
    return Py_BuildValue("i", (int) id);
    // string retKey;
    // size_t retLen = txp->reverseLookup(id, retKey);
    // return Py_BuildValue("s#", retKey.c_str(), (int) retLen);
  } else {
    Py_INCREF(Py_None);
    return Py_None;
  }
}

static PyObject *
Tx_commonPrefixSearch(Tx *self, PyObject *args, PyObject *kwds) {
  const char *head;
  if (!PyArg_ParseTuple(args, "s", &head))
    return NULL;

  vector<string> ret;
  vector<tx_tool::uint> retID;
  tx_tool::uint retNum;

  retNum = self->txp->commonPrefixSearch(head, strlen(head), ret, retID);
  PyObject *results;
  results = PyList_New((Py_ssize_t) ret.size());
  if (!results)
    return NULL;
  for (size_t i = 0; i < ret.size(); i++){
    PyList_SetItem(results, i, PyString_FromStringAndSize(ret[i].c_str(), ret[i].size()));
  }
  return results;
}

static PyObject *
Tx_commonPrefixSearchId(Tx *self, PyObject *args, PyObject *kwds) {
  const char *head;
  if (!PyArg_ParseTuple(args, "s", &head))
    return NULL;

  vector<tx_tool::uint> retLen;
  vector<tx_tool::uint> retID;
  tx_tool::uint retNum;

  retNum = self->txp->commonPrefixSearch(head, strlen(head), retLen, retID);
  PyObject *results;
  results = PyList_New((Py_ssize_t) retLen.size());
  if (!results)
    return NULL;
  for (size_t i = 0; i < retLen.size(); i++){
    PyList_SetItem(results, i, PyInt_FromLong(retID[i]));
  }
  return results;
}


static PyObject *
Tx_predictiveSearch(Tx *self, PyObject *args, PyObject *kwds) {
  const char *head;
  if (!PyArg_ParseTuple(args, "s", &head))
    return NULL;

  vector<string> ret;
  vector<tx_tool::uint> retID;
  tx_tool::uint retNum;

  retNum = self->txp->predictiveSearch(head, strlen(head), ret, retID);
  PyObject *results;
  results = PyList_New((Py_ssize_t) ret.size());
  if (!results)
    return NULL;
  for (size_t i = 0; i < ret.size(); i++){
    PyList_SetItem(results, i, PyString_FromStringAndSize(ret[i].c_str(), ret[i].size()));
  }
  return results;
}

static PyObject *
Tx_predictiveSearchId(Tx *self, PyObject *args, PyObject *kwds) {
  const char *head;
  if (!PyArg_ParseTuple(args, "s", &head))
    return NULL;

  vector<tx_tool::uint> retLen;
  vector<tx_tool::uint> retID;
  tx_tool::uint retNum;

  retNum = self->txp->predictiveSearch(head, strlen(head), retLen, retID);
  PyObject *results;
  results = PyList_New((Py_ssize_t) retLen.size());
  if (!results)
    return NULL;
  for (size_t i = 0; i < retLen.size(); i++){
    PyList_SetItem(results, i, PyInt_FromLong(retID[i]));
  }
  return results;
}

static PyObject *
Tx_reverseLookup(Tx *self, PyObject *args, PyObject *kwds) {
  int id;
  if (!PyArg_ParseTuple(args, "i", &id))
    return NULL;

  string retKey;
  size_t retLen = self->txp->reverseLookup((tx_tool::uint) id, retKey);
  return Py_BuildValue("s#", retKey.c_str(), (int) retLen);
}

static PyObject *
Tx_keynum(Tx *self, PyObject *args, PyObject *kwds) {
    tx_tool::uint num = self->txp->getKeyNum();
    return Py_BuildValue("i", (int) num);
}

static PyMethodDef Tx_methods[] = {
    {"prefixSearch", (PyCFunction)Tx_prefixSearch, METH_VARARGS,
     "prefix search"
    },
    {"commonPrefixSearch", (PyCFunction)Tx_commonPrefixSearch, METH_VARARGS,
     "common prefix search that returns keys"
    },
    {"commonPrefixSearchId", (PyCFunction)Tx_commonPrefixSearchId, METH_VARARGS,
     "common prefix search that returns ids"
    },
    {"predictiveSearch", (PyCFunction)Tx_predictiveSearch, METH_VARARGS,
     "predictive search that returns keys"
    },
    {"predictiveSearchId", (PyCFunction)Tx_predictiveSearchId, METH_VARARGS,
     "predictive search that returns ids"
    },
    {"reverseLookup", (PyCFunction)Tx_reverseLookup, METH_VARARGS,
     "Return the key from id"
    },
    {"keynum", (PyCFunction)Tx_keynum, METH_NOARGS,
     "Return the number of keys in the trie"
    },
    {NULL}  /* Sentinel */
};

static PyTypeObject TxType = {
  PyObject_HEAD_INIT(NULL)
  0,                         /*ob_size*/
  "tx.Tx",                   /*tp_name*/
  sizeof(Tx),                /*tp_basicsize*/
  0,                         /*tp_itemsize*/
  (destructor)Tx_dealloc,    /*tp_dealloc*/
  0,                         /*tp_print*/
  0,                         /*tp_getattr*/
  0,                         /*tp_setattr*/
  0,                         /*tp_compare*/
  0,                         /*tp_repr*/
  0,                         /*tp_as_number*/
  0,                         /*tp_as_sequence*/
  0,                         /*tp_as_mapping*/
  0,                         /*tp_hash */
  0,                         /*tp_call*/
  0,                         /*tp_str*/
  0,                         /*tp_getattro*/
  0,                         /*tp_setattro*/
  0,                         /*tp_as_buffer*/
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
  "Tx objects",              /* tp_doc */
  0,		             /* tp_traverse */
  0,		             /* tp_clear */
  0,		             /* tp_richcompare */
  0,		             /* tp_weaklistoffset */
  0,	                     /* tp_iter */
  0,	                     /* tp_iternext */
  Tx_methods,                /* tp_methods */
  0,             /* tp_members */
  0,                         /* tp_getset */
  0,                         /* tp_base */
  0,                         /* tp_dict */
  0,                         /* tp_descr_get */
  0,                         /* tp_descr_set */
  0,                         /* tp_dictoffset */
  (initproc)Tx_init,      /* tp_init */
  0,                         /* tp_alloc */
  Tx_new,                 /* tp_new */
};

static PyMethodDef module_methods[] = {
    {NULL}  /* Sentinel */
};

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
inittx(void)  {
  PyObject* m;

  if (PyType_Ready(&TxType) < 0)
    return;

  m = Py_InitModule3("tx", module_methods,
		     "Python interface to tx");
  if (m == NULL)
    return;

  Py_INCREF(&TxType);
  PyModule_AddObject(m, "Tx", (PyObject *)&TxType);
}
