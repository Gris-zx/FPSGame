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

	GuardState = EAIState::Idle;//����AIĬ��Ϊ��ɢ״̬
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();

	OriginalRotation = GetActorRotation();//���NPC��ʼ��תλ��
	
}

void AFPSAIGuard::OnPawnSeen(APawn* SeenPawn)//�������ʱ
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

	SetGuardState(EAIState::Alerted);//����ʱ����Ϊ����״̬
}

void AFPSAIGuard::OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume)//��������ʱ
{
	if (GuardState == EAIState::Alerted)//����Ѿ��������˱㲻�ں�AI�Ƿ�����������
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


	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);//ÿ�ε���ǰ�Ȱ��ϴεĶ�ʱ���������
	GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AFPSAIGuard::ResetOrientation, 3.0f);

	SetGuardState(EAIState::Suspicious);//��������ʱ����
}

void AFPSAIGuard::ResetOrientation()//����AI��λʱ
{
	if (GuardState == EAIState::Alerted)//ǰ����δ���ھ���״̬
	{
		return;
	}

	SetActorRotation(OriginalRotation);

	SetGuardState(EAIState::Idle);//�ٴ���Ϊ�й�
}

void AFPSAIGuard::OnRep_GuardState()
{
	OnStateChanged(GuardState);//������״̬

}

void AFPSAIGuard::SetGuardState(EAIState NewState)
{
	if (GuardState == NewState)//������״̬�Ƿ�ı䣬���ı���ֻ�践�ز���Ҫ�������ɶ
	{
		return;
	}

	GuardState = NewState;//��������Ϊ��״̬
	OnRep_GuardState();


}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//�������������ิ�Ʊ���,����������������������Ļ������ͻ��ˣ�
void AFPSAIGuard::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSAIGuard, GuardState);
}



