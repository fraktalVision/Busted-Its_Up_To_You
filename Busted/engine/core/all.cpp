#include "all.h"


///////////////////////////
////Override VSC++ new and delete garbage
///////////////////////////
//	#include <exception> // for std::bad_alloc
//	#include <new>
//	#include <cstdlib> // for malloc() and free()
//	// Visual C++ fix of operator new
//
//	void* operator new (size_t size)
//	{
//	 void *p=malloc(size); 
//	 if (p==0) // did malloc succeed?
//	  throw std::bad_alloc(); // ANSI/ISO compliant behavior
//	 return p;
//	}
//
//	void operator delete (void *p)
//	{
//	 free(p); 
//	}
///////////////////////////

#include "../tools/datadrive/datadrive.h"

//Global object render stack -- every drawable object in here...
std::vector<obj*> objStack;

//load in all INI data
DataDrive* DATA_MAN = NULL;
Text_Manager* pText_man = NULL;

//Camera to modify view
camera* Camera = NULL;
