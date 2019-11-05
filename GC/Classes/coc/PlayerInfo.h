#ifndef __COC_PLAYERINFO_H__
#define __COC_PLAYERINFO_H__

#include "BaseDef.h"

NS_COC_BEGIN

struct PlayerInfo
{
	int			money[MAX_MONEY_NUMS];

	int			numsWorker;
	int			freeWorker;

	int			exp;

	int			cup;
};

NS_COC_END

#endif  // __COC_PLAYERINFO_H__