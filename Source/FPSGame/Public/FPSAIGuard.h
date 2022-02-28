// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSAIGuard.generated.h"

class UPawnSensingComponent;

UENUM(BlueprintType)
enum class EAIState : uint8//枚举敌人的状态
{
	Idle,//闲逛

	Suspicious,//怀疑

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

	FRotator OriginalRotation;//AI的起始位置

	UFUNCTION()
	void ResetOrientation();//设置个重设到初始位置的函数

	FTimerHandle TimerHandle_ResetOrientation;//设置过一段时间就还原到初始位置

	UPROPERTY(ReplicatedUsing=OnRep_GuardState)//让服务器和客户端保持同步
	EAIState GuardState;//敌人守卫状态

	UFUNCTION()
	void OnRep_GuardState();//在客户端上操控玩家时，调用该函数,因服务器在默认情况下不会调用OnRep

	void SetGuardState(EAIState NewState);//用函数设置敌人状态

	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void OnStateChanged(EAIState NewState);//传递新状态

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void OnMissionCompleted(APawn* InstigatorPawn, bool bMissionSuccess);

};
