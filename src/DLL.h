//
// DLL.h - DLL defines for importing/exporting.
// Copyright (C) Sebastian Nordgren 
//

#pragma once

#ifdef DD_EXPORT
#define DD_DLL __declspec(dllexport)
#else 
#define DD_DLL __declspec(dllimport)
#endif 