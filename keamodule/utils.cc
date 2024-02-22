#include "keamodule.h"

using namespace std;
using namespace isc::data;

extern "C" {

int
assert_long_value(PyObject *value, const char *name) {
    if (value == NULL) {
        PyErr_Format(PyExc_TypeError, "cannot delete the %s attribute", name);
        return (-1);
    }
    if (!PyLong_Check(value)) {
        PyErr_Format(PyExc_TypeError, "the %s attribute value must be an int", name);
        return (-1);
    }
    return (0);
}

int
assert_bool_value(PyObject *value, const char *name) {
    if (value == NULL) {
        PyErr_Format(PyExc_TypeError, "cannot delete the %s attribute", name);
        return (-1);
    }
    if (!PyBool_Check(value)) {
        PyErr_Format(PyExc_TypeError, "the %s attribute value must be a bool", name);
        return (-1);
    }
    return (0);
}

int
assert_string_value(PyObject *value, const char *name, bool allow_none) {
    if (value == NULL) {
        PyErr_Format(PyExc_TypeError, "cannot delete the %s attribute", name);
        return (-1);
    }
    if (allow_none && value == Py_None) {
        return (0);
    }
    if (!PyUnicode_Check(value)) {
        PyErr_Format(PyExc_TypeError, "the %s attribute value must be a str", name);
        return (-1);
    }
    return (0);
}

PyObject *
element_to_object(ConstElementPtr ptr) {
    if (!ptr) {
        Py_RETURN_NONE;
    }
    switch (ptr->getType()) {
        case Element::integer:
            try {
                return (PyLong_FromLong(ptr->intValue()));
            }
            catch (const exception &e) {
                PyErr_SetString(PyExc_TypeError, e.what());
                return (0);
            }

        case Element::real:
            try {
                return (PyFloat_FromDouble(ptr->doubleValue()));
            }
            catch (const exception &e) {
                PyErr_SetString(PyExc_TypeError, e.what());
                return (0);
            }

        case Element::boolean:
            try {
                return (PyBool_FromLong(ptr->boolValue()));
            }
            catch (const exception &e) {
                PyErr_SetString(PyExc_TypeError, e.what());
                return (0);
            }

        case Element::null:
            Py_RETURN_NONE;

        case Element::string:
            try {
                return (PyUnicode_FromString(ptr->stringValue().c_str()));
            }
            catch (const exception &e) {
                PyErr_SetString(PyExc_TypeError, e.what());
                return (0);
            }

        case Element::list: {
            auto list_ptr = ptr->listValue();
            PyObject *list = PyList_New(list_ptr.size());
            if (!list) {
                return (0);
            }
            try {
                for (std::vector<ElementPtr>::const_iterator it = list_ptr.begin(); it != list_ptr.cend(); ++it) {
                    auto pos = it - list_ptr.begin();
                    PyObject *elem = element_to_object(*it);
                    if (!elem || PyList_SetItem(list, pos, elem) < 0) {
                        Py_DECREF(list);
                        return (0);
                    }
                }
            }
            catch (const exception &e) {
                PyErr_SetString(PyExc_TypeError, e.what());
                Py_DECREF(list);
                return (0);
            }
            return (list);
        }

        case Element::map: {
            auto map_ptr = ptr->mapValue();
            PyObject *dict = PyDict_New();
            if (!dict) {
                return (0);
            }
            try {
                for (std::map<std::string, ConstElementPtr>::const_iterator it = map_ptr.begin(); it != map_ptr.end(); ++it) {
                    PyObject *elem = element_to_object(it->second);
                    if (!elem || PyDict_SetItemString(dict, it->first.c_str(), elem) < 0) {
                        Py_DECREF(dict);
                        Py_XDECREF(elem);
                        return (0);
                    }
                    Py_DECREF(elem);
               }
            }
            catch (const exception &e) {
                PyErr_SetString(PyExc_TypeError, e.what());
                Py_DECREF(dict);
                return (0);
            }
            return (dict);
        }

        default:
            Py_RETURN_NONE;
    }
}

ElementPtr
object_to_element(PyObject *obj) {
    if (obj == Py_None) {
        return Element::create();
    }
    if (PyBool_Check(obj)) {
        return Element::create((bool) (obj == Py_True));
    }
    if (PyLong_Check(obj)) {
        return Element::create(PyLong_AsLong(obj));
    }
    if (PyFloat_Check(obj)) {
        return Element::create(PyFloat_AsDouble(obj));
    }
    if (PyUnicode_Check(obj)) {
        return Element::create(PyUnicode_AsUTF8(obj));
    }
    if (PyList_Check(obj)) {
        ElementPtr ptr = Element::createList();
        for (Py_ssize_t i = 0; i < PyList_Size(obj); i++) {
            ElementPtr item = object_to_element(PyList_GetItem(obj, i));
            if (!item) {
                return (0);
            }
            ptr->add(item);
        }
        return ptr;
    }
    if (PyDict_Check(obj)) {
        ElementPtr ptr = Element::createMap();
        PyObject *key;
        PyObject *value;
        Py_ssize_t pos = 0;

        while (PyDict_Next(obj, &pos, &key, &value)) {
            if (!PyUnicode_Check(key)) {
                PyErr_SetString(PyExc_TypeError, "keys must be string");
                return (0);
            }
            ElementPtr item = object_to_element(value);
            if (!item) {
                return (0);
            }
            ptr->set(PyUnicode_AsUTF8(key), item);
        }
        return ptr;
    }

    PyErr_Format(PyExc_TypeError, "unhandled type %s", Py_TYPE(obj)->tp_name);
    return (0);
}

}
