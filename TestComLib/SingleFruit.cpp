#include "Precomp.h"
#include "SingleFruit.h"

////////////////////////////////////////////////////////////////////////////////

SingleFruit::SingleFruit() 
{
	m_eFruit = Apple;
}

SingleFruit::~SingleFruit() 
{
}

////////////////////////////////////////////////////////////////////////////////

HRESULT SingleFruit::ConstructObject()
{
	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP SingleFruit::SetFruit(TestFruitType eFruit)
{
	m_eFruit = eFruit;

	return S_OK;
}

STDMETHODIMP SingleFruit::GetFruit(TestFruitType* peFruit)
{
	if (peFruit == NULL)
		return E_FAIL;
		
	*peFruit = m_eFruit;
	
	return S_OK;
}
