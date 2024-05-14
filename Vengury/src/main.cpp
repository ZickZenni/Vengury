#include "common.hpp"

#include "features.hpp"
#include "fiber_pool.hpp"
#include "logger.hpp"
#include "pointers.hpp"
#include "renderer.hpp"
#include "gui/gui.hpp"
#include "hooking/hooking.hpp"
#include "script/script_mgr.hpp"
#include "service/player_service.hpp"
#include "service/notification_service.hpp"
#include "service/object_service.hpp"
#include "commands/command.hpp"
#include "commands/vehicle/veh_spawn.hpp"

BOOL APIENTRY DllMain(HMODULE hmod, DWORD reason, PVOID)
{
	using namespace Vengury;
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hmod);

		g_hmodule = hmod;
		g_main_thread = CreateThread(nullptr, 0, [](PVOID) -> DWORD
		{
			while (!FindWindow(L"grcWindow", L"Grand Theft Auto V"))
				std::this_thread::sleep_for(1s);

			auto logger_instance = std::make_unique<Logger>();
			try
			{
				LOG(RAW_GREEN_TO_CONSOLE) << R"(
          _______  _        _______           _______          
|\     /|(  ____ \( (    /|(  ____ \|\     /|(  ____ )|\     /|
| )   ( || (    \/|  \  ( || (    \/| )   ( || (    )|( \   / )
| |   | || (__    |   \ | || |      | |   | || (____)| \ (_) / 
( (   ) )|  __)   | (\ \) || | ____ | |   | ||     __)  \   /  
 \ \_/ / | (      | | \   || | \_  )| |   | || (\ (      ) (   
  \   /  | (____/\| )  \  || (___) || (___) || ) \ \__   | |   
   \_/   (_______/|/    )_)(_______)(_______)|/   \__/   \_/   
                                                               )";
				auto pointers_instance = std::make_unique<Pointers>();
				LOG(INFO) << "Pointers initialized.";

				auto renderer_instance = std::make_unique<Renderer>();
				LOG(INFO) << "Renderer initialized.";

				auto fiber_pool_instance = std::make_unique<FiberPool>(10);
				LOG(INFO) << "Fiber pool initialized.";

				auto hooking_instance = std::make_unique<Hooking>();
				LOG(INFO) << "Hooking initialized.";

				g_settings.Load();
				LOG(INFO) << "Settings Loaded.";

				g_commands.push_back(new VehSpawnCommand());
				LOG(INFO) << "Commands initialized.";

				auto player_service_instance = std::make_unique<PlayerService>();
				LOG(INFO) << "Player Service initialized.";

				auto object_service_instance = std::make_unique<ObjectService>();
				LOG(INFO) << "Object Service initialized.";

				auto notification_service_instance = std::make_unique<NotificationService>();
				LOG(INFO) << "Notification Service initialized.";

				g_script_mgr.AddScript(std::make_unique<Script>(&features::ScriptFunc));
				g_script_mgr.AddScript(std::make_unique<Script>(&Gui::RunScript));

				g_script_mgr.AddScript(std::make_unique<Script>(&NotificationService::RunSubtitle));
				g_script_mgr.AddScript(std::make_unique<Script>(&NotificationService::RunMinimap));

				LOG(INFO) << "Scripts registered.";

				g_hooking->Enable();
				LOG(INFO) << "Hooking enabled.";

				while (g_running)
				{
					std::this_thread::sleep_for(500ms);
				}

				std::this_thread::sleep_for(1000ms);

				g_hooking->Disable();
				LOG(INFO) << "Hooking disabled.";

				g_script_mgr.RemoveAllScripts();
				LOG(INFO) << "Scripts unregistered.";

				object_service_instance.reset();
				LOG(INFO) << "Object Service uninitialized.";

				player_service_instance.reset();
				LOG(INFO) << "Player Service uninitialized.";

				notification_service_instance.reset();
				LOG(INFO) << "Notification Service uninitialized.";

				hooking_instance.reset();
				LOG(INFO) << "Hooking uninitialized.";

				fiber_pool_instance.reset();
				LOG(INFO) << "Fiber pool uninitialized.";

				renderer_instance.reset();
				LOG(INFO) << "Renderer uninitialized.";

				pointers_instance.reset();
				LOG(INFO) << "Pointers uninitialized.";

			}
			catch (std::exception const& ex)
			{
				LOG(WARNING) << ex.what();
				MessageBoxA(nullptr, ex.what(), nullptr, MB_OK | MB_ICONEXCLAMATION);
			}

			LOG(INFO) << "Farewell!";
			logger_instance.reset();

			CloseHandle(g_main_thread);
			FreeLibraryAndExitThread(g_hmodule, 0);
		}, nullptr, 0, &g_main_thread_id);
	}

	return true;
}
