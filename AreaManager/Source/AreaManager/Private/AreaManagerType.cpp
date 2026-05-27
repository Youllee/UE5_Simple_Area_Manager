#include "AreaManagerType.h"
#include "AreaManagerAreaActor.h"

bool FAreaManagerAreaData::IsValid() const
{
	return ::IsValid(Area.Get());
}
