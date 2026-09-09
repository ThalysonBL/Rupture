// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rupture.h"
#include "Modules/ModuleManager.h"

#if WITH_EDITOR
#include "Containers/Ticker.h"
#include "Engine/Engine.h"
#include "Misc/CoreDelegates.h"
#endif

/**
 * Módulo do jogo. No editor, inicia o servidor MCP só na sessão interativa
 * (não em commandlets de cook/packaging) para não conflitar na porta 8000.
 */
class FRuptureModule : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override
	{
#if WITH_EDITOR
		if (IsRunningCommandlet())
		{
			return;
		}

		FCoreDelegates::GetOnPostEngineInit().AddLambda([]()
		{
			FTSTicker::GetCoreTicker().AddTicker(
				FTickerDelegate::CreateLambda([](float)
				{
					if (GEngine)
					{
						GEngine->Exec(nullptr, TEXT("ModelContextProtocol.StartServer"));
					}
					return false;
				}),
				1.0f);
		});
#endif
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE(FRuptureModule, Rupture, "Rupture");
