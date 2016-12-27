/** ======================================================================== **\
| Version:
0.0.1

| License:
Copyright (c) 2016 Double V

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1.	The origin of this software must not be misrepresented; you must not
	claim that you wrote the original software. If you use this software
	in a product, an acknowledgement in the product documentation would be
	appreciated but is not required.
2.	Altered source versions must be plainly marked as such, and must not be
	misrepresented as being the original software.
3.	This notice may not be removed or altered from any source distribution.
\** ======================================================================== **/

// I remove this constant because the compiler automatically generates it.
// github.com/Zeex/pawn/issues/107
#if defined _inc_tdw_pickup
	#undef _inc_tdw_pickup
#endif

#if defined _TDW_PUPLIB_PICKUP
	#endinput
#endif

#define _TDW_PUPLIB_PICKUP

const
	INVALID_PICKUP_ID = -1,
	INVALID_PICKUP_MODEL = -1,
	INVALID_PICKUP_TYPE = -1;

forward OnPlayerPickupExit(playerid, pickupid);

enum
	E_TDW_PICKUP
{
	EI_PICKUP_MODEL,
	EI_PICKUP_TYPE,
	Float:EI_PICKUP_X,
	Float:EI_PICKUP_Y,
	Float:EI_PICKUP_Z,
	EI_PICKUP_VW // virtual world
};

static stock
	TDW_g_sCurrentPickupId,
	TDW_g_sPickupData[MAX_PICKUPS][E_TDW_PICKUP],
	TDW_g_sPlayerPickup[MAX_PLAYERS] = -1;

// -----------------------------------------------------------------------------
// Hooks
// -----------------------------------------------------------------------------

#if !defined npuh_CreatePickup
	native npuh_CreatePickup(model, type, Float:X, Float:Y, Float:Z,
		virtualworld = 0) = CreatePickup;
#endif

stock
	PUH@CreatePickup(model, type, Float:X, Float:Y, Float:Z, virtualworld = 0)
{
	TDW_g_sCurrentPickupId = CreatePickup(model, type, X, Y, Z, virtualworld);
	if (TDW_g_sCurrentPickupId != INVALID_PICKUP_ID) {
		TDW_g_sPickupData[TDW_g_sCurrentPickupId][EI_PICKUP_MODEL] = model;
		TDW_g_sPickupData[TDW_g_sCurrentPickupId][EI_PICKUP_TYPE] = type;
		TDW_g_sPickupData[TDW_g_sCurrentPickupId][EI_PICKUP_X] = X;
		TDW_g_sPickupData[TDW_g_sCurrentPickupId][EI_PICKUP_Y] = Y;
		TDW_g_sPickupData[TDW_g_sCurrentPickupId][EI_PICKUP_Z] = Z;
		TDW_g_sPickupData[TDW_g_sCurrentPickupId][EI_PICKUP_VW] = virtualworld;
	}
	return TDW_g_sCurrentPickupId;
}

#if defined _ALS_CreatePickup
	#undef CreatePickup
#else
	#define _ALS_CreatePickup
#endif

#define CreatePickup PUH@CreatePickup


#if !defined npuh_DestroyPickup
	native npuh_DestroyPickup(pickupid) = DestroyPickup;
#endif

stock
	PUH@DestroyPickup(pickupid)
{
	if (DestroyPickup(pickupid) != 0) {
		TDW_g_sPickupData[pickupid][EI_PICKUP_MODEL] =
		TDW_g_sPickupData[pickupid][EI_PICKUP_TYPE] =
		TDW_g_sPickupData[pickupid][EI_PICKUP_VW] = 0;
		TDW_g_sPickupData[pickupid][EI_PICKUP_X] =
		TDW_g_sPickupData[pickupid][EI_PICKUP_Y] =
		TDW_g_sPickupData[pickupid][EI_PICKUP_Z] = 0.0;
		return 1;
	}
	return 0;
}

#if defined _ALS_DestroyPickup
	#undef DestroyPickup
#else
	#define _ALS_DestroyPickup
#endif

#define DestroyPickup PUH@DestroyPickup

// -----------------------------------------------------------------------------
// Functions
// -----------------------------------------------------------------------------

stock
	IsValidPickup(pickupid)
{
	return 0 <= pickupid <= MAX_PICKUPS &&
		TDW_g_sPickupData[pickupid][EI_PICKUP_MODEL] != 0;
}


stock
	GetPickupPos(pickupid, &Float:x, &Float:y, &Float:z)
{
	if (IsValidPickup(pickupid) == 0)
		return _:(x = y = z = 0.0);
	x = TDW_g_sPickupData[pickupid][EI_PICKUP_X];
	y = TDW_g_sPickupData[pickupid][EI_PICKUP_Y];
	z = TDW_g_sPickupData[pickupid][EI_PICKUP_Z];
	return 1;
}


stock
	GetPickupModel(pickupid)
{
	if (IsValidPickup(pickupid) == 0)
		return INVALID_PICKUP_MODEL;
	return TDW_g_sPickupData[pickupid][EI_PICKUP_MODEL];
}


stock
	GetPickupType(pickupid)
{
	if (IsValidPickup(pickupid) == 0)
		return INVALID_PICKUP_TYPE;
	return TDW_g_sPickupData[pickupid][EI_PICKUP_TYPE];
}

stock
	GetPickupVirtualworld(pickupid)
{
	if (IsValidPickup(pickupid) == 0)
		return -1;
	return TDW_g_sPickupData[pickupid][EI_PICKUP_VW];
}

stock
	SetPickupPos(pickupid, Float:x, Float:y, Float:z)
{
	if (IsValidPickup(pickupid) == 0)
		return 0;
	TDW_g_sPickupData[pickupid][EI_PICKUP_X] = x;
	TDW_g_sPickupData[pickupid][EI_PICKUP_Y] = y;
	TDW_g_sPickupData[pickupid][EI_PICKUP_Z] = z;
	UpdatePickup(pickupid);
	return 1;
}

stock
	SetPickupModel(pickupid, model)
{
	if (IsValidPickup(pickupid) == 0)
		return 0;
	TDW_g_sPickupData[pickupid][EI_PICKUP_MODEL] = model;
	UpdatePickup(pickupid);
	return 1;
}

stock
	SetPickupType(pickupid, type)
{
	if (IsValidPickup(pickupid) == 0)
		return 0;
	TDW_g_sPickupData[pickupid][EI_PICKUP_TYPE] = type;
	UpdatePickup(pickupid);
	return 1;
}

stock
	SetPickupVirtualworld(pickupid, virtualworld)
{
	if (IsValidPickup(pickupid) == 0)
		return 0;
	TDW_g_sPickupData[pickupid][EI_PICKUP_VW] = virtualworld;
	UpdatePickup(pickupid);
	return 1;
}


stock
	UpdatePickup(pickupid)
{
	if (IsValidPickup(pickupid) == 0)
		return 0;
	npuh_DestroyPickup(pickupid);
	npuh_CreatePickup(TDW_g_sPickupData[pickupid][EI_PICKUP_MODEL],
		TDW_g_sPickupData[pickupid][EI_PICKUP_TYPE],
		TDW_g_sPickupData[pickupid][EI_PICKUP_X],
		TDW_g_sPickupData[pickupid][EI_PICKUP_Y],
		TDW_g_sPickupData[pickupid][EI_PICKUP_Z],
		TDW_g_sPickupData[pickupid][EI_PICKUP_VW]
	);
	return 1;
}

// -----------------------------------------------------------------------------
// Fixes
// -----------------------------------------------------------------------------

public
	OnPlayerPickUpPickup(playerid, pickupid)
{
	if (TDW_g_sPlayerPickup[playerid] != -1)
		return 1;
	TDW_g_sPlayerPickup[playerid] = SetTimerEx(!"@__UPDATE_PICKUP", 960, false,
		!"ii", playerid, pickupid
	);
	#if defined PUH@OnPlayerPickUpPickup
		return PUH@OnPlayerPickUpPickup(playerid, pickupid);
	#else
		return 1;
	#endif
}

#if defined _ALS_OnPlayerPickUpPickup
	#undef OnPlayerPickUpPickup
#else
	#define _ALS_OnPlayerPickUpPickup
#endif

#define OnPlayerPickUpPickup PUH@OnPlayerPickUpPickup

#if defined PUH@OnPlayerPickUpPickup
	forward PUH@OnPlayerPickUpPickup(playerid, pickupid);
#endif

@__UPDATE_PICKUP(playerid, pickupid);
@__UPDATE_PICKUP(playerid, pickupid)
{
	if (IsPlayerInRangeOfPoint(playerid, 1.4,
		TDW_g_sPickupData[pickupid][EI_PICKUP_X],
		TDW_g_sPickupData[pickupid][EI_PICKUP_Y],
		TDW_g_sPickupData[pickupid][EI_PICKUP_Z]) == 0) {
		CallLocalFunction(!"OnPlayerPickupExit", !"ii", playerid, pickupid);
		TDW_g_sPlayerPickup[playerid] = -1;
	} else {
		TDW_g_sPlayerPickup[playerid] = SetTimerEx(!"@__UPDATE_PICKUP", 960,
			false, !"ii", playerid, pickupid
		);
	}
	return;
}