#include "Precomp.h"
#include "multifruit.h"

////////////////////////////////////////////////////////////////////////////////

MultiFruit::MultiFruit() 
{
	m_eFruit = Apple;
}

MultiFruit::~MultiFruit() 
{
}

////////////////////////////////////////////////////////////////////////////////

HRESULT MultiFruit::ConstructObject()
{
	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP MultiFruit::SetFruit(TestFruitType eFruit)
{
	m_eFruit = eFruit;

	return S_OK;
}

STDMETHODIMP MultiFruit::GetFruit(TestFruitType* peFruit)
{
	if (peFruit == NULL)
		return E_FAIL;
		
	*peFruit = m_eFruit;
	
	return S_OK;
}
