// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSAIGuard.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "FPSGameMode.h"
#include "FPSExtractionZone.h"
#include "Net/UnrealNetwork.h"



// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));

	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnNoiseHeard);

	GuardState = EAIState::Idle;//设置AI默认为闲散状态
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();

	OriginalRotation = GetActorRotation();//获得NPC初始旋转位置
	
}

void AFPSAIGuard::OnPawnSeen(APawn* SeenPawn)//发现玩家时
{
	if (SeenPawn == nullptr) 
	{
		return;
	}

	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12, FColor::Red, false, 10.0f);


	AFPSGameMode* GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		//GM->CompleteMission(SeenPawn, false);
		OnMissionCompleted(SeenPawn, false);

	}

	SetGuardState(EAIState::Alerted);//发现时设置为警觉状态
}

void AFPSAIGuard::OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume)//听到声音时
{
	if (GuardState == EAIState::Alerted)//如果已经被发现了便不在乎AI是否听到声音了
	{
		return;
	}

	DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Green, false, 10.0f);

	FVector Direction = Location - GetActorLocation();
	Direction.Normalize();

	FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();
	NewLookAt.Pitch = 0.0f;
	NewLookAt.Roll = 0.0f;

	SetActorRotation(NewLookAt);


	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);//每次调用前先把上次的定时器作废清除
	GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AFPSAIGuard::ResetOrientation, 3.0f);

	SetGuardState(EAIState::Suspicious);//听到声音时警觉
}

void AFPSAIGuard::ResetOrientation()//重置AI方位时
{
	if (GuardState == EAIState::Alerted)//前提是未处于警觉状态
	{
		return;
	}

	SetActorRotation(OriginalRotation);

	SetGuardState(EAIState::Idle);//再次设为闲逛
}

void AFPSAIGuard::OnRep_GuardState()
{
	OnStateChanged(GuardState);//传递新状态

}

void AFPSAIGuard::SetGuardState(EAIState NewState)
{
	if (GuardState == NewState)//看敌人状态是否改变，若改变则只需返回不需要额外多做啥
	{
		return;
	}

	GuardState = NewState;//否则设置为新状态
	OnRep_GuardState();


}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//在其生命周期类复制变量,即复制所有与服务器相连的机器（客户端）
void AFPSAIGuard::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSAIGuard, GuardState);
}



