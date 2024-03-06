///*
//--   ----------------------------------------------------------------------
//--   Project:          DLL creation
//--
//--   Author:           Hamza Qureshi
//--
//--   File name:        DataDecryption.h
//--
//--   Rev.:             1.0
//--   Creation date:    DEC 2023
//--
//--   ---------------------------------------------------------------------
//Purpose of this module: MAIN MODULE
//
//----------Main body--------------
//
//Comments:
//
//WARININGS
//
//---------------------------------------------------------------------------
//*/
//// DataDecryption.cpp : implementation file
////
//
# pragma once
// DataDecryption.h - Contains declarations of DataDecryption.dll functions

#ifdef STC_EXPORTS
#define STC_API __declspec(dllexport)
#else
#define STC_API __declspec(dllimport)
#endif


extern "C" STC_API void writeLogEntry(const unsigned char count,const char* first, ...);

extern "C" STC_API const char * decryptRecord(const char* table_name, const char* record);

//extern "C" STC_API void MyFunction();


