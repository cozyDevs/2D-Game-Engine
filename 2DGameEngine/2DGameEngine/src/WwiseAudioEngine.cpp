

#include <iostream>
#include <vector>
#include <cassert>
#include <chrono>
#include <thread>

//Wwise Sound engine
#include <AK/SoundEngine/Common/AkSoundEngine.h>

//Memory manager interface
#include <AK/SoundEngine/Common/AkMemoryMgr.h>

//Default memory manager
#include <AK/SoundEngine/Common/AkModule.h>        

//Music engine
#include <AK/MusicEngine/Common/AkMusicEngine.h>


//Streaming manager
#include <AK/SoundEngine/Common/IAkStreamMgr.h>

#include <AK/Tools/Common/AkPlatformFuncs.h> 

#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
 
//Spatial audio
#include <AK/SpatialAudio/Common/AkSpatialAudio.h>

//Communications
#include <AK/Comm/AkCommunication.h>

//Low level implementation

#include <AK/SoundEngine/Win32/AkFilePackageLowLevelIOBlocking.h>
#include <AK/SoundEngine/Win32/AkDefaultIOHookBlocking.h>

CAkDefaultIOHookBlocking g_blockingDevice;

//IDs generated by Wwise
#include <Wwise_IDs.h>

namespace AK
{
	void* dRealloc(size_t s) { return malloc(s); }
	void Free(void* p) { free(p); }
}

bool InitSoundEngine()
{
	//Initializing the memory manager
	AkMemSettings memSettings;
	AK::MemoryMgr::GetDefaultSettings(memSettings);

	if (AK::MemoryMgr::Init(&memSettings) != AK_Success)
	{
		assert(! "Could not create memory manager");
		return false;
	}


	//Creation of default streaming manager

	AkStreamMgrSettings stmSettings;
	AK::StreamMgr::GetDefaultSettings(stmSettings);

	if (!AK::StreamMgr::Create(stmSettings))
	{
		assert(!"Could not create the Streaming Manager");
		return false;
	}


	//Streaming device with default low level I/O
	AkDeviceSettings deviceSettings;
	AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);

	if (g_blockingDevice.Init(deviceSettings) != AK_Success)
	{
		assert(!"Could not create streaming device and low level I/O system");
		return false;
	}


	//Creating the sound engine using default initialization parameters

	AkInitSettings initSettings;
	AkPlatformInitSettings platformInitSettings;
	AK::SoundEngine::GetDefaultInitSettings(initSettings);
	AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);

	if (AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success)
	{
		assert(!"Could not initialize sound engine");
		return false;
	}

	//Initializing the music engine

	AkMusicSettings musicInit;
	AK::MusicEngine::GetDefaultInitSettings(musicInit);

	if (AK::MusicEngine::Init(&musicInit) != AK_Success)
	{
		assert(!"Could not initialize the music system");
		return false;
	}

	//Initializing spatial audio
	
	AkSpatialAudioInitSettings spatialSettings;

	if (AK::SpatialAudio::Init(spatialSettings) != AK_Success)
	{
		assert(!"Could not initialize spatial audio");
		return false;
	}

	//Initializing communications
#ifndef AK_OPTIMIZED

	AkCommSettings commSettings;
	AK::Comm::GetDefaultInitSettings(commSettings);

	if (AK::Comm::Init(commSettings) != AK_Success)
	{
		assert(!"Communications could not be initialized.");
		return false;
	}
	

#endif AK_OPTIMIZED


	//Setting base path for banks
	g_blockingDevice.SetBasePath(AKTEXT("C:\\Program Files (x86)\\Audiokinetic\\Wwise 2021.1.2.7629\\Cube\\Wwise-101_v2019.1\\Lesson 1\\WwiseSoundEnginePractice\\GeneratedSoundBanks\\Windows"));

	//Set the current language
	//AK::StreamMgr::SetCurrentLanguage(AKTEXT("English(US"));

	//Loading sound banks from Wwise project
	AkBankID bankID;

	AKRESULT eResult = AK::SoundEngine::LoadBank(L"Init.bnk", bankID);
	assert(eResult == AK_Success);

	eResult = AK::SoundEngine::LoadBank(L"SFX.bnk", bankID);
	assert(eResult == AK_Success);

	return true;

}



void TermSoundEngine()
{
	//Terminate the communications module
#ifndef AK_OPTIMIZED

	AK::Comm::Term();

#endif AK_OPTIMIZED

	//Terminate music engine
	AK::MusicEngine::Term();

	//Unload banks
	AK::SoundEngine::UnloadBank(L"SFX.bnk", nullptr);
	AK::SoundEngine::UnloadBank(L"Init.bnk", nullptr);

	//Terminate the sound engine
	AK::SoundEngine::Term();

	//Terminate streaming device and stream manager
	g_blockingDevice.Term();

	if (AK::IAkStreamMgr::Get())
	{
		AK::IAkStreamMgr::Get()->Destroy();
	}

	//Terminate memory manager
	AK::MemoryMgr::Term();
}

void PlaySound()
{
	//Registering game object
	AkGameObjectID gameObject = 1;
	AK::SoundEngine::RegisterGameObj(gameObject, "GameObj1");

	//Post event using its ID
	AK::SoundEngine::PostEvent(AK::EVENTS::PLAY_SFX_1, gameObject);
	AK::SoundEngine::UnregisterGameObj(gameObject);
}

void PseudoGameMainLoop()
{
	//Function to replicate game loop 
	int frameCount = 0;
	while (frameCount++ < 300)
	{
		auto time = std::chrono::milliseconds(30);
		std::this_thread::sleep_for(time);
		AK::SoundEngine::RenderAudio();
	}
}

int main()
{
	std::cout << "Initializing Wwise" << std::endl;;
	if (InitSoundEngine())
	{
		//auto time = std::chrono::seconds(20);
		//std::this_thread::sleep_for(time);

		std::cout << "Playing sound" << std::endl;
		PlaySound();
		PseudoGameMainLoop();
	}

	std::cout << "Terminating Wwise" << std::endl;
	TermSoundEngine();
}