#include "Engine/Script/Py3.hpp"
#include "ThirdParty/python/Python.h"
#include "Engine/Develop/Log.hpp"

PyObject* PyLog(PyObject* self, PyObject* args)
{
	const char* str = nullptr;
	int succ = PyArg_ParseTuple(args, "s", &str);
	if (succ) {
		Log("Script", "Py: %s", str);
	}
	Py_INCREF(Py_None);
	return Py_None;
}

PyMethodDef MyPythonMethods[] = {
	{"log", PyLog, METH_VARARGS, "Log using Engine Log"},
	{nullptr, nullptr, 0, nullptr}
};

PyModuleDef MyModule = {
	// base					name	doc		 size	method			slot	tarverse_cb  clear_cb	free_cb
	PyModuleDef_HEAD_INIT, "flare", nullptr, -1, MyPythonMethods, nullptr,  nullptr,		nullptr, nullptr
};

PyObject* MyModuleInit()
{
	return PyModule_Create(&MyModule);
}


void FTest()
{
	const wchar_t* program = L"TestPyProgram";

	Py_SetProgramName(program);
	Py_SetPath(L"./python37.zip");
	PyImport_AppendInittab("flare", &MyModuleInit);
	Py_Initialize();
	PyRun_SimpleString(R"(
from time import time, ctime
print ('Today is', ctime(time())) 
)");
}

void FClose()
{
	Py_FinalizeEx();
}

////////////////////////////////
void FLogTest()
{
	PyRun_SimpleString(R"(
import flare
flare.log(100)
)");
}

void FRunPythonTest()
{
	const char* module_name = "test";
	const char* func = "Anything";

	PyObject* py_path;
	PyObject* py_module=nullptr;
	PyObject* py_func;

	py_path = PyUnicode_DecodeFSDefault(module_name);
	py_module = PyImport_Import(py_path);
	if (py_module) {
		py_func = PyObject_GetAttrString(py_module, func);
		if (py_func) {
			if (PyCallable_Check(py_func)) {
				PyObject* args = PyTuple_New(2);
				PyObject* value = PyLong_FromLong(32);
				PyTuple_SetItem(args, 0, value);
				value = PyLong_FromLong(20);
				PyTuple_SetItem(args, 1, value);

				PyObject* ref = PyObject_CallObject(py_func, args);

				Py_DECREF(ref);
				Py_DECREF(args);
				Py_DECREF(py_func);
			}
		}
		Py_DECREF(py_module);
	}
}
