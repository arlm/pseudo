// Copyright (c) Microsoft Corporation 2008.  All rights reserved.

#pragma once

#include <unknwn.h>

//////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum
{
	Apple = 0,
	Banana,
	Pear
} TestFruitType;

//////////////////////////////////////////////////////////////////////////////////////////////////

interface __declspec(uuid("dd683bca-aef8-43a0-0099-8b1308d4b974")) IFruit : IUnknown
{
	STDMETHOD(SetFruit)(TestFruitType eFruit) = 0;
	STDMETHOD(GetFruit)(TestFruitType* peFruit) = 0;
};

#define IID_IFruit __uuidof(IFruit)

///////////////////////////////////////////////////////////////////////////////////////

class __declspec(uuid("b9dd55f6-09c2-4f48-009d-11c2ffa788f8")) CoClassMultiFruit {};
class __declspec(uuid("65d61b9c-0c9f-4dfc-00b1-4b284fc0d310")) CoClassSingleFruit {};

#define CLSID_MultiFruit __uuidof(CoClassMultiFruit)
#define CLSID_SingleFruit __uuidof(CoClassSingleFruit)
