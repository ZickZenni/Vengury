#pragma once
#include <cstdint>
#include <utility>
#include <rage/vector.hpp>
#include <script/scrVector.hpp>
#include <script/scrNativeHandler.hpp>

namespace rage
{
	template <typename T>
	class atArray;

	class datBitBuffer;
	class sysMemAllocator;

	class scriptIdBase;
	class scriptId;
	class scriptHandler;
	class scriptHandlerNetComponent;
	class scriptHandlerObject;
	class scriptHandlerMgr;

	class scrProgram;
	class scrProgramTable;

	class scrThreadContext;
	class scrThread;
	class tlsContext;

	class netLoggingInterface;
	class netLogStub;

	class netPlayer;
	class netPlayerMgr;

	class netGameEvent;
	class netEventMgr;

	class netSyncTree;

	class netObject;
	class netObjectMgrBase;

	class scrNativeCallContext;
	class scrNativeRegistration;
	class scrNativeRegistrationTable;

	class fwRefAwareBase;
	class fwExtensibleBase;
	class fwEntity;
	class fwArchetype;

	class CVehicle;
	class CPed;

	using scrNativeMapping = std::pair<scrNativeHash, scrNativeHash>;
}

class GtaThread;

class CGameScriptId;
class CGameScriptHandler;
class CGameScriptHandlerNetwork;
class CGameScriptHandlerMgr;

class CEntity;
class CDynamicEntity;
class CPhysical;

class CObject;
class CPickup;

class CPedFactory;
class CVehicleFactory;

class CNetGamePlayer;
class CNetworkPlayerMgr;
class CPlayerInfo;
class CNetworkObjectMgr;

using Void = void;
using Any = std::uint32_t;
using Hash = std::uint32_t;
using Entity = std::int32_t;
using Player = std::int32_t;
using FireId = std::int32_t;
using Interior = std::int32_t;
using Ped = Entity;
using Vehicle = Entity;
using Cam = std::int32_t;
using Object = Entity;
using Pickup = Object;
using Blip = std::int32_t;
using Camera = Entity;
using ScrHandle = Entity;
using Vector3 = rage::scrVector;

using RagePlayer = std::int32_t;