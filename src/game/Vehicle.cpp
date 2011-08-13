/*
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "Common.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "Vehicle.h"
#include "Unit.h"
#include "Util.h"

VehicleInfo::VehicleInfo(VehicleEntry const* entry, Unit* vehicle) :
    m_vehicleEntry(entry),
    m_vehicle(vehicle)
{
    for (uint8 i = 0; i < MAX_VEHICLE_SEAT; ++i)
    {
        uint32 seatId = entry->m_seatID[i];

        if (seatId)
        {
            VehicleSeatEntry const* vSeatEntry = sVehicleSeatStore.LookupEntry(seatId);
            MANGOS_ASSERT(vSeatEntry != NULL);

            m_vehicleSeats[seatId].passengerGuid = ObjectGuid();
            m_vehicleSeats[seatId].vehicleSeatEntry = vSeatEntry;
        }
    }
}

VehicleSeatEntry const* VehicleInfo::GetSeatEntry(ObjectGuid passengerGuid)
{
    for (VehicleSeatMap::const_iterator itr = m_vehicleSeats.begin(); itr != m_vehicleSeats.end(); ++itr)
    {
        if (itr->second.passengerGuid == passengerGuid)
            return itr->second.vehicleSeatEntry;
    }

    return NULL;
}

bool VehicleInfo::AddPassenger(ObjectGuid passengerGuid, uint32 seatId)
{
    if (seatId)
    {
        VehicleSeatMap::const_iterator itr = m_vehicleSeats.find(seatId);

        // The seat is not available
        if (itr == m_vehicleSeats.end())
            return false;

        // There is no empty seat
        if (!itr->second.passengerGuid.IsEmpty())
            return false;
    }
    else
    {
        seatId = GetNextEmptyUsableSeatId();

        // There is no empty seat
        if (!seatId)
            return false;
    }

    DEBUG_LOG("Add passenger to seatId %u", seatId);

    m_vehicleSeats[seatId].passengerGuid = passengerGuid;
    return true;
}

void VehicleInfo::RelocatePassengers(float x, float y, float z, float ang)
{
    for (VehicleSeatMap::const_iterator itr = m_vehicleSeats.begin(); itr != m_vehicleSeats.end(); ++itr)
    {
        if (Unit* pPassenger = m_vehicle->GetMap()->GetUnit(itr->second.passengerGuid))
        {
            float px = x + m_vehicle->GetPositionX();
            float py = y + m_vehicle->GetPositionY();
            float pz = z + m_vehicle->GetPositionZ();
            float po = ang + m_vehicle->GetOrientation();

            pPassenger->UpdateAllowedPositionZ(px, py, pz);
            pPassenger->UpdatePosition(px, py, pz, po);
        }
    }
}

void VehicleInfo::RemovePassenger(ObjectGuid passengerGuid)
{
    DEBUG_LOG("Remove Passenger");

    for (VehicleSeatMap::iterator itr = m_vehicleSeats.begin(); itr != m_vehicleSeats.end(); ++itr)
    {
        if (itr->second.passengerGuid == passengerGuid)
            itr->second.passengerGuid.Clear();
    }
}

void VehicleInfo::FillPassengerGuidList(std::list<ObjectGuid>& list)
{
    for (VehicleSeatMap::const_iterator itr = m_vehicleSeats.begin(); itr != m_vehicleSeats.end(); ++itr)
    {
        if (!itr->second.passengerGuid.IsEmpty())
            list.push_back(itr->second.passengerGuid);
    }
}

uint32 VehicleInfo::GetNextEmptyUsableSeatId()
{
    for (VehicleSeatMap::const_iterator itr = m_vehicleSeats.begin(); itr != m_vehicleSeats.end(); ++itr)
    {
        // The seat isn't empty
        if (!itr->second.passengerGuid.IsEmpty())
            continue;

        // The seat is usable or uncontrolled
        if (itr->second.vehicleSeatEntry->m_flags & SEAT_FLAG_USABLE || itr->second.vehicleSeatEntry->m_flags & SEAT_FLAG_UNCONTROLLED)
            return itr->first;
    }

    return 0;
}
