// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
class UBehaviorTreeComponent;
UCLASS()
class SHOOTER_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
protected:
private:
	UPROPERTY(BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
		UBlackboardComponent* BlackBoardComponent;
	UPROPERTY(BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
		UBehaviorTreeComponent* BehaviorTreeComponent;
public:
	AEnemyAIController();
	virtual void OnPossess(APawn* InPawn) override;
	FORCEINLINE UBlackboardComponent* GetBlackBoardComponent() const {return BlackBoardComponent;}
	
};
