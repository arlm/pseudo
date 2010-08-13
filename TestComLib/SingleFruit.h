// Copyright (c) John Lyon-Smith 2002.  All rights reserved.

#pragma once

#include "testcomlib.h"

using namespace Pseudo;

class SingleFruit : 
	public ComUnknown,
	public ComCreateObject<SingleFruit>,
	public IFruit
{
public:
	SingleFruit();
	~SingleFruit();

BEGIN_COM_IUNKNOWN_IMPL()
	COM_IUNKNOWN_INTERFACE(IID_IFruit, IFruit)
END_COM_IUNKNOWN_IMPL()

	// Required for ComCreateObject support
	STDMETHOD(ConstructObject)();

	// IFruit
    STDMETHOD(SetFruit)(TestFruitType e);
    STDMETHOD(GetFruit)(TestFruitType* pe);

private:	
	// Helper methods
	
private:
	TestFruitType m_eFruit;
};
