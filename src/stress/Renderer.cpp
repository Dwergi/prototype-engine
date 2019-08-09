#include "PCH.h"
#include "Renderer.h"

namespace ddr
{
	bool Renderer::CheckDuplicates(const dd::TypeInfo* component, ddc::DataUsage usage, ddc::DataCardinality cardinality)
	{
		for (const ddc::DataRequest* req : m_requests)
		{
			if (req->Component() == *component &&
				req->Usage() == usage &&
				req->Cardinality() == cardinality)
			{
				DD_ASSERT(false, "Duplicate DataRequest found!");
				return false;
			}
		}

		return true;
	}
}