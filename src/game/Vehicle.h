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

#ifndef MANGOSSERVER_VEHICLE_H
#define MANGOSSERVER_VEHICLE_H

#include "Common.h"
#include "ObjectGuid.h"
#include "Creature.h"
#include "Unit.h"
#include "SharedDefines.h"

struct VehicleEntry;
struct VehicleSeatEntry;

struct VehicleSeat
{
    ObjectGuid passengerGuid;
    VehicleSeatEntry const* vehicleSeatEntry;
};

typedef std::map<uint32, VehicleSeat> VehicleSeatMap;

class VehicleInfo
{
    public:
        explicit VehicleInfo(VehicleEntry const* entry, Unit* vehicle);

        VehicleEntry const* GetEntry() const { return m_vehicleEntry; }
        VehicleSeatEntry const* GetSeatEntry(ObjectGuid passengerGuid);
        Unit* GetVehicle() { return m_vehicle; }

        bool AddPassenger(ObjectGuid passengerGuid, uint32 seatId = 0);
        void RelocatePassengers(float x, float y, float z, float ang);
        void RemoveAllPassengers() { for (VehicleSeatMap::iterator itr = m_vehicleSeats.begin(); itr != m_vehicleSeats.end(); ++itr) { itr->second.passengerGuid.Clear(); } }
        void RemovePassenger(ObjectGuid passengerGuid);

        void FillPassengerGuidList(std::list<ObjectGuid>& list);
        uint32 GetNextEmptyUsableSeatId();

        bool IsSeatAvailable(uint32 seatId) { return m_vehicleSeats.find(seatId) != m_vehicleSeats.end(); }
        ObjectGuid GetPassengerGuid(uint32 seatId) { return (IsSeatAvailable(seatId)) ? m_vehicleSeats[seatId].passengerGuid : ObjectGuid(); }

    private:
        VehicleEntry const* m_vehicleEntry;
        VehicleSeatMap m_vehicleSeats;
        Unit* m_vehicle;
};

#endif
