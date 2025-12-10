#pragma once

#include "GameFramework/SpectatorPawn.h"
#include "RobotSpectatorPawn.generated.h"

class AAttachablePart;
class UAttachmentPoint;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPartStateChanged, AAttachablePart*, Part);

UCLASS()
class ROBOTABUSE_API ARobotSpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "UI Events")
	FOnPartStateChanged OnPartStateChanged;
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	// ===== Input Handlers =====
    
	void OnMouseClick();

	// ===== Interaction Functions =====
    
	void HandleNewClick(AActor* Actor);
	void StopDragging();

	// ===== Update Functions =====
    
	void UpdateDraggedActor();
	void UpdateHighlights();
	
	void StartHighlightTimer();
	void StopHighlightTimer();

private:
	
	float InitialDragDistance;
	
	// Timer for hover detection
	FTimerHandle HighlightTimerHandle;
	
	UPROPERTY()
	APlayerController* CachedPC;
    
	UPROPERTY()
	AActor* DraggedActor;

	UPROPERTY()
	UObject* HoveredTarget;
};
