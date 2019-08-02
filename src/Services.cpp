//
// Services.cpp
// Copyright (C) Sebastian Nordgren 
// January 10th 2019
//

#include "PCH.h"
#include "Services.h"

std::vector<void(*)()> dd::Services::s_unregisterFuncs;