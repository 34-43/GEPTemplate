// BgmPlayer.cpp - 배경음악 플레이어 UI
#include "systems/BgmPlayer.h"
#include "Components/TextBlock.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void UBgmPlayer::SetTrack(int32 Index)
{
	if (Index == CurrentTrackIndex && AudioComponent && AudioComponent->IsPlaying()) return;
	if (!MusicArray.IsValidIndex(Index)) return;
	USoundBase* NewTrack = MusicArray[Index];
	if (!NewTrack) return;

	UAudioComponent* PrevAudio = AudioComponent;
	AudioComponent = nullptr;
	CurrentTrackIndex = Index;

	if (TrackIndex)
	{
		FText TrackText = FText::FromString(NewTrack->GetName());
		TrackIndex->SetText(TrackText);
	}

	// 이전 오디오가 존재할 경우에만 처리
	if (PrevAudio)
	{
		if (PrevAudio->IsPlaying())
		{
			PrevAudio->FadeOut(FadeDuration, 0.f);

			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, PrevAudio, NewTrack]()
			{
				if (PrevAudio && !PrevAudio->IsPendingKill())
				{
					PrevAudio->Stop();
					if (PrevAudio->IsRegistered())
						PrevAudio->UnregisterComponent();
					PrevAudio->DestroyComponent();
				}
				PlayNewTrack(NewTrack);
			}, FadeDuration + 0.05f, false);

			return;
		}

		if (PrevAudio->IsRegistered())
			PrevAudio->UnregisterComponent();
		PrevAudio->DestroyComponent();
	}

	PlayNewTrack(NewTrack);
}

void UBgmPlayer::PlayNewTrack(USoundBase* NewTrack)
{
	if (!NewTrack)
		return;

	AudioComponent = UGameplayStatics::SpawnSound2D(
		this,
		NewTrack,
		1.0f,
		1.0f,
		0.0f,
		nullptr,
		false
	);

	if (AudioComponent)
	{
		if (!AudioComponent->IsRegistered())
			AudioComponent->RegisterComponent();

		AudioComponent->OnAudioFinished.AddDynamic(this, &UBgmPlayer::OnTrackFinished);
		AudioComponent->FadeIn(FadeDuration);
	}
}

void UBgmPlayer::OnTrackFinished()
{
	if (AudioComponent)
	{
		FTimerHandle LoopTimer;
		GetWorld()->GetTimerManager().SetTimer(LoopTimer, [this]()
		{
			if (AudioComponent && !AudioComponent->IsPlaying())
				AudioComponent->Play();
		}, 0.05f, false);
	}
}

void UBgmPlayer::NativeDestruct()
{
	if (AudioComponent)
	{
		if (AudioComponent->IsPlaying())
			AudioComponent->Stop();

		if (AudioComponent->IsRegistered())
			AudioComponent->UnregisterComponent();

		AudioComponent->DestroyComponent();
		AudioComponent = nullptr;
	}

	Super::NativeDestruct();
}
