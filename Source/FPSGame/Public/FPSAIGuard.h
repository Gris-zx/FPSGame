// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSAIGuard.generated.h"

class UPawnSensingComponent;

UENUM(BlueprintType)
enum class EAIState : uint8//ö�ٵ��˵�״̬
{
	Idle,//�й�

	Suspicious,//����

	Alerted
};

UCLASS()
class FPSGAME_API AFPSAIGuard : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSAIGuard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPawnSensingComponent* PawnSensingComp;

	UFUNCTION()
	void OnPawnSeen(APawn* SeenPawn);

	UFUNCTION()
	void OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume);

	FRotator OriginalRotation;//AI����ʼλ��

	UFUNCTION()
	void ResetOrientation();//���ø����赽��ʼλ�õĺ���

	FTimerHandle TimerHandle_ResetOrientation;//���ù�һ��ʱ��ͻ�ԭ����ʼλ��

	UPROPERTY(ReplicatedUsing=OnRep_GuardState)//�÷������Ϳͻ��˱���ͬ��
	EAIState GuardState;//��������״̬

	UFUNCTION()
	void OnRep_GuardState();//�ڿͻ����ϲٿ����ʱ�����øú���,���������Ĭ������²������OnRep

	void SetGuardState(EAIState NewState);//�ú������õ���״̬

	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void OnStateChanged(EAIState NewState);//������״̬

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void OnMissionCompleted(APawn* InstigatorPawn, bool bMissionSuccess);

};
